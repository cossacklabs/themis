/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 *
 */
"use strict";

var _template = _interopRequireDefault(require("@babel/template"));

var _traverse = _interopRequireDefault(require("@babel/traverse"));

var t = _interopRequireWildcard(require("@babel/types"));

var _invariant = _interopRequireDefault(require("invariant"));

function _getRequireWildcardCache(nodeInterop) {
  if (typeof WeakMap !== "function") return null;
  var cacheBabelInterop = new WeakMap();
  var cacheNodeInterop = new WeakMap();
  return (_getRequireWildcardCache = function(nodeInterop) {
    return nodeInterop ? cacheNodeInterop : cacheBabelInterop;
  })(nodeInterop);
}

function _interopRequireWildcard(obj, nodeInterop) {
  if (!nodeInterop && obj && obj.__esModule) {
    return obj;
  }
  if (obj === null || (typeof obj !== "object" && typeof obj !== "function")) {
    return { default: obj };
  }
  var cache = _getRequireWildcardCache(nodeInterop);
  if (cache && cache.has(obj)) {
    return cache.get(obj);
  }
  var newObj = {};
  var hasPropertyDescriptor =
    Object.defineProperty && Object.getOwnPropertyDescriptor;
  for (var key in obj) {
    if (key !== "default" && Object.prototype.hasOwnProperty.call(obj, key)) {
      var desc = hasPropertyDescriptor
        ? Object.getOwnPropertyDescriptor(obj, key)
        : null;
      if (desc && (desc.get || desc.set)) {
        Object.defineProperty(newObj, key, desc);
      } else {
        newObj[key] = obj[key];
      }
    }
  }
  newObj.default = obj;
  if (cache) {
    cache.set(obj, newObj);
  }
  return newObj;
}

function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : { default: obj };
}

const WRAP_NAME = "$$_REQUIRE"; // note: babel will prefix this with _
// Check first the `global` variable as the global object. This way serializers
// can create a local variable called global to fake it as a global object
// without having to pollute the window object on web.

const IIFE_PARAM = _template.default.expression(
  "typeof globalThis !== 'undefined' ? globalThis : typeof global !== 'undefined' ? global : typeof window !== 'undefined' ? window : this"
);

function wrapModule(
  fileAst,
  importDefaultName,
  importAllName,
  dependencyMapName,
  globalPrefix
) {
  const params = buildParameters(
    importDefaultName,
    importAllName,
    dependencyMapName
  );
  const factory = functionFromProgram(fileAst.program, params);
  const def = t.callExpression(t.identifier(`${globalPrefix}__d`), [factory]);
  const ast = t.file(t.program([t.expressionStatement(def)]));
  const requireName = renameRequires(ast);
  return {
    ast,
    requireName
  };
}

function wrapPolyfill(fileAst) {
  const factory = functionFromProgram(fileAst.program, ["global"]);
  const iife = t.callExpression(factory, [IIFE_PARAM()]);
  return t.file(t.program([t.expressionStatement(iife)]));
}

function jsonToCommonJS(source) {
  return `module.exports = ${source};`;
}

function wrapJson(source, globalPrefix) {
  // Unused parameters; remember that's wrapping JSON.
  const moduleFactoryParameters = buildParameters(
    "_importDefaultUnused",
    "_importAllUnused",
    "_dependencyMapUnused"
  );
  return [
    `${globalPrefix}__d(function(${moduleFactoryParameters.join(", ")}) {`,
    `  ${jsonToCommonJS(source)}`,
    "});"
  ].join("\n");
}

function functionFromProgram(program, parameters) {
  return t.functionExpression(
    undefined,
    parameters.map(makeIdentifier),
    t.blockStatement(program.body, program.directives)
  );
}

function makeIdentifier(name) {
  return t.identifier(name);
}

function buildParameters(importDefaultName, importAllName, dependencyMapName) {
  return [
    "global",
    "require",
    importDefaultName,
    importAllName,
    "module",
    "exports",
    dependencyMapName
  ];
} // Renaming requires should ideally only be done when generating for the target
// that expects the custom require name in the optimize step.
// This visitor currently renames all `require` references even if the module
// contains a custom `require` declaration. This should be fixed by only renaming
// if the `require` symbol hasn't been redeclared.

function renameRequires(ast) {
  let newRequireName = WRAP_NAME;
  (0, _traverse.default)(ast, {
    Program(path) {
      const body = path.get("body.0.expression.arguments.0.body");
      (0, _invariant.default)(
        !Array.isArray(body),
        "metro: Expected `body` to be a single path."
      );
      newRequireName = body.scope.generateUid(WRAP_NAME);
      body.scope.rename("require", newRequireName);
    }
  });
  return newRequireName;
}

module.exports = {
  WRAP_NAME,
  wrapJson,
  jsonToCommonJS,
  wrapModule,
  wrapPolyfill
};
