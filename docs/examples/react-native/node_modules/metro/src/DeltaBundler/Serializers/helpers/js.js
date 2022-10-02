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

const invariant = require("invariant");

const path = require("path");

const { addParamsToDefineCall } = require("metro-transform-plugins");

function wrapModule(module, options) {
  const output = getJsOutput(module);

  if (output.type.startsWith("js/script")) {
    return output.data.code;
  }

  const moduleId = options.createModuleId(module.path);
  const params = [
    moduleId,
    Array.from(module.dependencies.values()).map(dependency =>
      options.createModuleId(dependency.absolutePath)
    )
  ];

  if (options.dev) {
    // Add the relative path of the module to make debugging easier.
    // This is mapped to `module.verboseName` in `require.js`.
    params.push(path.relative(options.projectRoot, module.path));
  }

  return addParamsToDefineCall(output.data.code, ...params);
}

function getJsOutput(module) {
  var _module$path, _module$path2;

  const jsModules = module.output.filter(({ type }) => type.startsWith("js/"));
  invariant(
    jsModules.length === 1,
    `Modules must have exactly one JS output, but ${
      (_module$path = module.path) !== null && _module$path !== void 0
        ? _module$path
        : "unknown module"
    } has ${jsModules.length} JS outputs.`
  );
  const jsOutput = jsModules[0];
  invariant(
    Number.isFinite(jsOutput.data.lineCount),
    `JS output must populate lineCount, but ${
      (_module$path2 = module.path) !== null && _module$path2 !== void 0
        ? _module$path2
        : "unknown module"
    } has ${jsOutput.type} output with lineCount '${jsOutput.data.lineCount}'`
  );
  return jsOutput;
}

function isJsModule(module) {
  return module.output.filter(isJsOutput).length > 0;
}

function isJsOutput(output) {
  return output.type.startsWith("js/");
}

module.exports = {
  getJsOutput,
  isJsModule,
  wrapModule
};
