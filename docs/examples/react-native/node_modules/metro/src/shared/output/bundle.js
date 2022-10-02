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

const Server = require("../../Server");

const relativizeSourceMapInline = require("../../lib/relativizeSourceMap");

const writeFile = require("./writeFile");

function buildBundle(packagerClient, requestOptions) {
  return packagerClient.build({
    ...Server.DEFAULT_BUNDLE_OPTIONS,
    ...requestOptions,
    bundleType: "bundle"
  });
}

function relativateSerializedMap(map, sourceMapSourcesRoot) {
  const sourceMap = JSON.parse(map);
  relativizeSourceMapInline(sourceMap, sourceMapSourcesRoot);
  return JSON.stringify(sourceMap);
}

async function saveBundleAndMap(bundle, options, log) {
  const {
    bundleOutput,
    bundleEncoding: encoding,
    sourcemapOutput,
    sourcemapSourcesRoot
  } = options;
  const writeFns = [];
  writeFns.push(async () => {
    log("Writing bundle output to:", bundleOutput);
    await writeFile(bundleOutput, bundle.code, encoding);
    log("Done writing bundle output");
  });

  if (sourcemapOutput) {
    let { map } = bundle;

    if (sourcemapSourcesRoot !== undefined) {
      log("start relativating source map");
      map = relativateSerializedMap(map, sourcemapSourcesRoot);
      log("finished relativating");
    }

    writeFns.push(async () => {
      log("Writing sourcemap output to:", sourcemapOutput);
      await writeFile(sourcemapOutput, map, null);
      log("Done writing sourcemap output");
    });
  } // Wait until everything is written to disk.

  await Promise.all(writeFns.map(cb => cb()));
}

exports.build = buildBundle;
exports.save = saveBundleAndMap;
exports.formatName = "bundle";
