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

const DependencyGraph = require("./node-haste/DependencyGraph");

const Transformer = require("./DeltaBundler/Transformer");

class Bundler {
  constructor(config, options) {
    this._depGraphPromise = DependencyGraph.load(config, options);

    this._depGraphPromise
      .then(dependencyGraph => {
        this._transformer = new Transformer(
          config, // $FlowFixMe[method-unbinding] added when improving typing for this parameters
          dependencyGraph.getSha1.bind(dependencyGraph)
        );
      })
      .catch(error => {
        console.error("Failed to construct transformer: ", error);
      });
  }

  async end() {
    const dependencyGraph = await this._depGraphPromise;

    this._transformer.end();

    dependencyGraph.getWatcher().end();
  }

  async getDependencyGraph() {
    return await this._depGraphPromise;
  }

  async transformFile(filePath, transformOptions) {
    // We need to be sure that the DependencyGraph has been initialized.
    // TODO: Remove this ugly hack!
    await this._depGraphPromise;
    return this._transformer.transformFile(filePath, transformOptions);
  }
}

module.exports = Bundler;
