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

const path = require("path");

const { getAssetData } = require("../../Assets");

const { getJsOutput, isJsModule } = require("./helpers/js");

async function getAssets(dependencies, options) {
  const promises = [];
  const { processModuleFilter } = options;

  for (const module of dependencies.values()) {
    if (
      isJsModule(module) &&
      processModuleFilter(module) &&
      getJsOutput(module).type === "js/module/asset" &&
      path.relative(options.projectRoot, module.path) !== "package.json"
    ) {
      promises.push(
        getAssetData(
          module.path,
          path.relative(options.projectRoot, module.path),
          options.assetPlugins,
          options.platform,
          options.publicPath
        )
      );
    }
  }

  return await Promise.all(promises);
}

module.exports = getAssets;
