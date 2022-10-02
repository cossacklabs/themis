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

const url = require("url");

const { isJsModule, wrapModule } = require("./helpers/js");

const { addParamsToDefineCall } = require("metro-transform-plugins");

function generateModules(sourceModules, graph, options) {
  const modules = [];

  for (const module of sourceModules) {
    if (isJsModule(module)) {
      // Construct a bundle URL for this specific module only
      const getURL = extension => {
        options.clientUrl.pathname = path.relative(
          options.projectRoot,
          path.join(
            path.dirname(module.path),
            path.basename(module.path, path.extname(module.path)) +
              "." +
              extension
          )
        );
        return url.format(options.clientUrl);
      };

      const sourceMappingURL = getURL("map");
      const sourceURL = getURL("bundle");
      const code =
        prepareModule(module, graph, options) +
        `\n//# sourceMappingURL=${sourceMappingURL}\n` +
        `//# sourceURL=${sourceURL}\n`;
      modules.push({
        module: [options.createModuleId(module.path), code],
        sourceMappingURL,
        sourceURL
      });
    }
  }

  return modules;
}

function prepareModule(module, graph, options) {
  const code = wrapModule(module, { ...options, dev: true });
  const inverseDependencies = getInverseDependencies(module.path, graph); // Transform the inverse dependency paths to ids.

  const inverseDependenciesById = Object.create(null);
  Object.keys(inverseDependencies).forEach(path => {
    inverseDependenciesById[options.createModuleId(path)] = inverseDependencies[
      path
    ].map(options.createModuleId);
  });
  return addParamsToDefineCall(code, inverseDependenciesById);
}
/**
 * Instead of adding the whole inverseDependncies object into each changed
 * module (which can be really huge if the dependency graph is big), we only
 * add the needed inverseDependencies for each changed module (we do this by
 * traversing upwards the dependency graph).
 */

function getInverseDependencies(path, graph, inverseDependencies = {}) {
  // Dependency alredy traversed.
  if (path in inverseDependencies) {
    return inverseDependencies;
  }

  const module = graph.dependencies.get(path);

  if (!module) {
    return inverseDependencies;
  }

  inverseDependencies[path] = [];

  for (const inverse of module.inverseDependencies) {
    inverseDependencies[path].push(inverse);
    getInverseDependencies(inverse, graph, inverseDependencies);
  }

  return inverseDependencies;
}

function hmrJSBundle(delta, graph, options) {
  return {
    added: generateModules(delta.added.values(), graph, options),
    modified: generateModules(delta.modified.values(), graph, options),
    deleted: [...delta.deleted].map(path => options.createModuleId(path))
  };
}

module.exports = hmrJSBundle;
