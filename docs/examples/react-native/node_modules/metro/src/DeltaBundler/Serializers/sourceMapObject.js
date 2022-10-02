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

const {
  sourceMapGenerator,
  sourceMapGeneratorNonBlocking
} = require("./sourceMapGenerator");

function sourceMapObject(modules, options) {
  const generator = sourceMapGenerator(modules, options);
  return generator.toMap(undefined, {
    excludeSource: options.excludeSource
  });
}

async function sourceMapObjectNonBlocking(modules, options) {
  const generator = await sourceMapGeneratorNonBlocking(modules, options);
  return generator.toMap(undefined, {
    excludeSource: options.excludeSource
  });
}

module.exports = {
  sourceMapObject,
  sourceMapObjectNonBlocking
};
