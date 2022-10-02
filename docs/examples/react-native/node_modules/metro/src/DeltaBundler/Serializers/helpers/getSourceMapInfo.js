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

const { getJsOutput } = require("./js");

function getSourceMapInfo(module, options) {
  return {
    ...getJsOutput(module).data,
    path: module.path,
    source: options.excludeSource ? "" : getModuleSource(module)
  };
}

function getModuleSource(module) {
  if (getJsOutput(module).type === "js/module/asset") {
    return "";
  }

  return module.getSource().toString();
}

module.exports = getSourceMapInfo;
