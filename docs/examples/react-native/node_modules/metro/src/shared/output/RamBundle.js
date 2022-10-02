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

const asAssets = require("./RamBundle/as-assets");

const asIndexedFile = require("./RamBundle/as-indexed-file").save;

async function build(packagerClient, requestOptions) {
  const options = {
    ...Server.DEFAULT_BUNDLE_OPTIONS,
    ...requestOptions,
    bundleType: "ram"
  };
  return await packagerClient.getRamBundleInfo(options);
}

function save(bundle, options, log) {
  // We fork here depending on the platform: while Android is pretty good at
  // loading individual assets, iOS has a large overhead when reading hundreds
  // of assets from disk.
  return options.platform === "android" && !(options.indexedRamBundle === true)
    ? asAssets(bundle, options, log)
    : asIndexedFile(bundle, options, log);
}

exports.build = build;
exports.save = save;
exports.formatName = "bundle";
