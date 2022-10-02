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

const { compile } = require("metro-hermes-compiler");

function wrapModule(module, options) {
  const output = getBytecodeOutput(module);

  if (output.type.startsWith("bytecode/script")) {
    return [output.data.bytecode];
  }

  const params = [
    options.createModuleId(module.path),
    "[" +
      Array.from(module.dependencies.values())
        .map(dependency => options.createModuleId(dependency.absolutePath))
        .join(",") +
      "]"
  ];

  if (options.dev) {
    // Add the relative path of the module to make debugging easier.
    // This is mapped to `module.verboseName` in `require.js`.
    params.push(
      JSON.stringify(path.relative(options.projectRoot, module.path))
    );
  }

  const headerCode = `globalThis.$$METRO_D=[${params.join(",")}];`;
  return [
    compile(headerCode, {
      sourceURL: module.path + "-virtual.js"
    }).bytecode,
    output.data.bytecode
  ];
}

function getBytecodeOutput(module) {
  const output = module.output
    .filter(({ type }) => type.startsWith("bytecode/"))
    .map(output =>
      output.data.bytecode instanceof Buffer
        ? output // Re-create buffers after losing the Buffer instance when sending data over workers.
        : {
            ...output,
            data: {
              ...output.data,
              bytecode: Buffer.from(output.data.bytecode.data)
            }
          }
    );
  invariant(
    output.length === 1,
    `Modules must have exactly one bytecode output, but ${module.path} has ${output.length} bytecode outputs.`
  );
  return output[0];
}

function isBytecodeModule(module) {
  return (
    module.output.filter(({ type }) => type.startsWith("bytecode/")).length > 0
  );
}

module.exports = {
  getBytecodeOutput,
  isBytecodeModule,
  wrapModule
};
