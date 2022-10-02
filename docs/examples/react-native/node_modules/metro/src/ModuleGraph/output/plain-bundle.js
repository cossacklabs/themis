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

const meta = require("../../shared/output/meta");

const { getModuleCodeAndMap, concat } = require("./util");

const { BundleBuilder } = require("metro-source-map");

function asPlainBundle({
  dependencyMapReservedName,
  filename,
  globalPrefix,
  idsForPath,
  modules,
  requireCalls,
  sourceMapPath,
  enableIDInlining
}) {
  const builder = new BundleBuilder(filename);

  const modIdForPath = x => idsForPath(x).moduleId;

  for (const module of concat(modules, requireCalls)) {
    const { moduleCode, moduleMap } = getModuleCodeAndMap(
      module,
      modIdForPath,
      {
        dependencyMapReservedName,
        enableIDInlining,
        globalPrefix
      }
    );
    builder.append(moduleCode + "\n", moduleMap);
  }

  if (sourceMapPath) {
    builder.append(`//# sourceMappingURL=${sourceMapPath}`);
  }

  const code = builder.getCode();
  const map = builder.getMap();
  return {
    code,
    extraFiles: [[`${filename}.meta`, meta(code)]],
    map
  };
}

module.exports = asPlainBundle;
