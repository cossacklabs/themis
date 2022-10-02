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

const getAppendScripts = require("../../lib/getAppendScripts");

const getTransitiveDependencies = require("./helpers/getTransitiveDependencies");

const nullthrows = require("nullthrows");

const path = require("path");

const { createRamBundleGroups } = require("../../Bundler/util");

const { isJsModule, wrapModule } = require("./helpers/js");

const { sourceMapObject } = require("./sourceMapObject");

async function getRamBundleInfo(entryPoint, pre, graph, options) {
  let modules = [...pre, ...graph.dependencies.values()];
  modules = modules.concat(
    getAppendScripts(entryPoint, modules, graph.importBundleNames, options)
  );
  modules.forEach(module => options.createModuleId(module.path));
  const ramModules = modules
    .filter(isJsModule)
    .filter(options.processModuleFilter)
    .map(module => ({
      id: options.createModuleId(module.path),
      code: wrapModule(module, options),
      map: sourceMapObject([module], {
        excludeSource: options.excludeSource,
        processModuleFilter: options.processModuleFilter
      }),
      name: path.basename(module.path),
      sourcePath: module.path,
      source: module.getSource().toString(),
      type: nullthrows(module.output.find(({ type }) => type.startsWith("js")))
        .type
    }));
  const { preloadedModules, ramGroups } = await _getRamOptions(
    entryPoint,
    {
      dev: options.dev,
      platform: options.platform
    },
    filePath => getTransitiveDependencies(filePath, graph),
    options.getTransformOptions
  );
  const startupModules = [];
  const lazyModules = [];
  ramModules.forEach(module => {
    if (preloadedModules.hasOwnProperty(module.sourcePath)) {
      startupModules.push(module);
      return;
    }

    if (module.type.startsWith("js/script")) {
      startupModules.push(module);
      return;
    }

    if (module.type.startsWith("js/module")) {
      lazyModules.push(module);
    }
  });
  const groups = createRamBundleGroups(
    ramGroups,
    lazyModules,
    (module, dependenciesByPath) => {
      const deps = getTransitiveDependencies(module.sourcePath, graph);
      const output = new Set();

      for (const dependency of deps) {
        const module = dependenciesByPath.get(dependency);

        if (module) {
          output.add(module.id);
        }
      }

      return output;
    }
  );
  return {
    getDependencies: filePath => getTransitiveDependencies(filePath, graph),
    groups,
    lazyModules,
    startupModules
  };
}
/**
 * Returns the options needed to create a RAM bundle.
 */

async function _getRamOptions(
  entryFile,
  options,
  getDependencies,
  getTransformOptions
) {
  if (getTransformOptions == null) {
    return {
      preloadedModules: {},
      ramGroups: []
    };
  }

  const { preloadedModules, ramGroups } = await getTransformOptions(
    [entryFile],
    {
      dev: options.dev,
      hot: true,
      platform: options.platform
    },
    /* $FlowFixMe(>=0.99.0 site=react_native_fb) This comment suppresses an
     * error found when Flow v0.99 was deployed. To see the error, delete this
     * comment and run Flow. */
    async x => Array.from(getDependencies)
  );
  return {
    preloadedModules: preloadedModules || {},
    ramGroups: ramGroups || []
  };
}

module.exports = getRamBundleInfo;
