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

const { isJsModule, wrapModule } = require("./js");

function processModules(
  modules,
  { filter = () => true, createModuleId, dev, projectRoot }
) {
  return [...modules]
    .filter(isJsModule)
    .filter(filter)
    .map(module => [
      module,
      wrapModule(module, {
        createModuleId,
        dev,
        projectRoot
      })
    ]);
}

module.exports = processModules;
