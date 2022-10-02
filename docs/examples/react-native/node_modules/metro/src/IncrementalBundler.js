/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 *
 * @format
 */
"use strict";

const Bundler = require("./Bundler");

const DeltaBundler = require("./DeltaBundler");

const ResourceNotFoundError = require("./IncrementalBundler/ResourceNotFoundError");

const crypto = require("crypto");

const fs = require("fs");

const getGraphId = require("./lib/getGraphId");

const getPrependedScripts = require("./lib/getPrependedScripts");

const path = require("path");

const transformHelpers = require("./lib/transformHelpers");

function createRevisionId() {
  return crypto.randomBytes(8).toString("hex");
}

function revisionIdFromString(str) {
  return str;
}

class IncrementalBundler {
  _revisionsById = new Map();
  _revisionsByGraphId = new Map();
  static revisionIdFromString = revisionIdFromString;

  constructor(config, options) {
    this._config = config;
    this._bundler = new Bundler(config, options);
    this._deltaBundler = new DeltaBundler(this._bundler);
  }

  end() {
    this._deltaBundler.end();

    this._bundler.end();
  }

  getBundler() {
    return this._bundler;
  }

  getDeltaBundler() {
    return this._deltaBundler;
  }

  getRevision(revisionId) {
    return this._revisionsById.get(revisionId);
  }

  getRevisionByGraphId(graphId) {
    return this._revisionsByGraphId.get(graphId);
  }

  async buildGraphForEntries(
    entryFiles,
    transformOptions,
    otherOptions = {
      onProgress: null,
      shallow: false
    }
  ) {
    const absoluteEntryFiles = await this._getAbsoluteEntryFiles(entryFiles);
    const graph = await this._deltaBundler.buildGraph(absoluteEntryFiles, {
      resolve: await transformHelpers.getResolveDependencyFn(
        this._bundler,
        transformOptions.platform
      ),
      transform: await transformHelpers.getTransformFn(
        absoluteEntryFiles,
        this._bundler,
        this._deltaBundler,
        this._config,
        transformOptions
      ),
      transformOptions,
      onProgress: otherOptions.onProgress,
      experimentalImportBundleSupport: this._config.transformer
        .experimentalImportBundleSupport,
      shallow: otherOptions.shallow
    });

    this._config.serializer.experimentalSerializerHook(graph, {
      added: graph.dependencies,
      modified: new Map(),
      deleted: new Set(),
      reset: true
    });

    return graph;
  }

  async getDependencies(
    entryFiles,
    transformOptions,
    otherOptions = {
      onProgress: null,
      shallow: false
    }
  ) {
    const absoluteEntryFiles = await this._getAbsoluteEntryFiles(entryFiles);
    const dependencies = await this._deltaBundler.getDependencies(
      absoluteEntryFiles,
      {
        resolve: await transformHelpers.getResolveDependencyFn(
          this._bundler,
          transformOptions.platform
        ),
        transform: await transformHelpers.getTransformFn(
          absoluteEntryFiles,
          this._bundler,
          this._deltaBundler,
          this._config,
          transformOptions
        ),
        transformOptions,
        onProgress: otherOptions.onProgress,
        experimentalImportBundleSupport: this._config.transformer
          .experimentalImportBundleSupport,
        shallow: otherOptions.shallow
      }
    );
    return dependencies;
  }

  async buildGraph(
    entryFile,
    transformOptions,
    otherOptions = {
      onProgress: null,
      shallow: false
    }
  ) {
    const graph = await this.buildGraphForEntries(
      [entryFile],
      transformOptions,
      otherOptions
    );
    const { type: _, ...transformOptionsWithoutType } = transformOptions;
    const prepend = await getPrependedScripts(
      this._config,
      transformOptionsWithoutType,
      this._bundler,
      this._deltaBundler
    );
    return {
      prepend,
      graph
    };
  } // TODO T34760750 (alexkirsz) Eventually, I'd like to get to a point where
  // this class exposes only initializeGraph and updateGraph.

  async initializeGraph(
    entryFile,
    transformOptions,
    otherOptions = {
      onProgress: null,
      shallow: false
    }
  ) {
    const graphId = getGraphId(entryFile, transformOptions, {
      shallow: otherOptions.shallow,
      experimentalImportBundleSupport: this._config.transformer
        .experimentalImportBundleSupport
    });
    const revisionId = createRevisionId();

    const revisionPromise = (async () => {
      const { graph, prepend } = await this.buildGraph(
        entryFile,
        transformOptions,
        otherOptions
      );
      return {
        id: revisionId,
        date: new Date(),
        graphId,
        graph,
        prepend
      };
    })();

    this._revisionsById.set(revisionId, revisionPromise);

    this._revisionsByGraphId.set(graphId, revisionPromise);

    try {
      const revision = await revisionPromise;
      const delta = {
        added: revision.graph.dependencies,
        modified: new Map(),
        deleted: new Set(),
        reset: true
      };
      return {
        revision,
        delta
      };
    } catch (err) {
      // Evict a bad revision from the cache since otherwise
      // we'll keep getting it even after the build is fixed.
      this._revisionsById.delete(revisionId);

      this._revisionsByGraphId.delete(graphId);

      throw err;
    }
  }

  async updateGraph(revision, reset) {
    const delta = await this._deltaBundler.getDelta(revision.graph, {
      reset,
      shallow: false
    });

    this._config.serializer.experimentalSerializerHook(revision.graph, delta);

    if (
      delta.added.size > 0 ||
      delta.modified.size > 0 ||
      delta.deleted.size > 0
    ) {
      this._revisionsById.delete(revision.id);

      revision = {
        ...revision,
        // Generate a new revision id, to be used to verify the next incremental
        // request.
        id: crypto.randomBytes(8).toString("hex"),
        date: new Date()
      };
      const revisionPromise = Promise.resolve(revision);

      this._revisionsById.set(revision.id, revisionPromise);

      this._revisionsByGraphId.set(revision.graphId, revisionPromise);
    }

    return {
      revision,
      delta
    };
  }

  async endGraph(graphId) {
    const revPromise = this._revisionsByGraphId.get(graphId);

    if (!revPromise) {
      return;
    }

    const revision = await revPromise;

    this._deltaBundler.endGraph(revision.graph);

    this._revisionsByGraphId.delete(graphId);

    this._revisionsById.delete(revision.id);
  }

  async _getAbsoluteEntryFiles(entryFiles) {
    const absoluteEntryFiles = entryFiles.map(entryFile =>
      path.resolve(this._config.projectRoot, entryFile)
    );
    await Promise.all(
      absoluteEntryFiles.map(
        entryFile =>
          new Promise((resolve, reject) => {
            // This should throw an error if the file doesn't exist.
            // Using this instead of fs.exists to account for SimLinks.
            fs.realpath(entryFile, err => {
              if (err) {
                reject(new ResourceNotFoundError(entryFile));
              } else {
                resolve();
              }
            });
          })
      )
    );
    return absoluteEntryFiles;
  }
}

module.exports = IncrementalBundler;
