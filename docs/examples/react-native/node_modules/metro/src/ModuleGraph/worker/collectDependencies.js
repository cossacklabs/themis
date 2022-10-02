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

const invariant = require("invariant");

const nullthrows = require("nullthrows");

const generate = require("@babel/generator").default;

const template = require("@babel/template").default;

const traverse = require("@babel/traverse").default;

const types = require("@babel/types");

const { isImport } = types;

/**
 * Transform all the calls to `require()` and `import()` in a file into ID-
 * independent code, and return the list of dependencies. For example, a call
 * like `require('Foo')` could be transformed to `require(_depMap[3], 'Foo')`
 * where `_depMap` is provided by the outer scope. As such, we don't need to
 * know the actual module ID.
 *
 * The second argument is only provided for debugging purposes.
 */
function collectDependencies(ast, options) {
  var _options$dependencyRe, _options$dependencyTr;

  const visited = new WeakSet();
  const state = {
    asyncRequireModulePathStringLiteral: null,
    dependencyCalls: new Set(),
    dependencyRegistry:
      (_options$dependencyRe = options.dependencyRegistry) !== null &&
      _options$dependencyRe !== void 0
        ? _options$dependencyRe
        : new DefaultModuleDependencyRegistry(),
    dependencyTransformer:
      (_options$dependencyTr = options.dependencyTransformer) !== null &&
      _options$dependencyTr !== void 0
        ? _options$dependencyTr
        : DefaultDependencyTransformer,
    dependencyMapIdentifier: null,
    dynamicRequires: options.dynamicRequires,
    keepRequireNames: options.keepRequireNames,
    allowOptionalDependencies: options.allowOptionalDependencies
  };
  const visitor = {
    CallExpression(path, state) {
      if (visited.has(path.node)) {
        return;
      }

      const callee = path.node.callee;
      const name = callee.type === "Identifier" ? callee.name : null;

      if (isImport(callee)) {
        processImportCall(path, state, {
          asyncType: "async"
        });
        return;
      }

      if (name === "__prefetchImport" && !path.scope.getBinding(name)) {
        processImportCall(path, state, {
          asyncType: "prefetch"
        });
        return;
      }

      if (name === "__jsResource" && !path.scope.getBinding(name)) {
        processImportCall(path, state, {
          asyncType: "async",
          jsResource: true
        });
        return;
      }

      if (
        name === "__conditionallySplitJSResource" &&
        !path.scope.getBinding(name)
      ) {
        const args = path.get("arguments");
        invariant(Array.isArray(args), "Expected arguments to be an array");
        processImportCall(path, state, {
          asyncType: "async",
          jsResource: true,
          splitCondition: args[1]
        });
        return;
      }

      if (
        name != null &&
        state.dependencyCalls.has(name) &&
        !path.scope.getBinding(name)
      ) {
        processRequireCall(path, state);
        visited.add(path.node);
      }
    },

    ImportDeclaration: collectImports,
    ExportNamedDeclaration: collectImports,
    ExportAllDeclaration: collectImports,

    Program(path, state) {
      state.asyncRequireModulePathStringLiteral = types.stringLiteral(
        options.asyncRequireModulePath
      );

      if (options.dependencyMapName != null) {
        state.dependencyMapIdentifier = types.identifier(
          options.dependencyMapName
        );
      } else {
        state.dependencyMapIdentifier = path.scope.generateUidIdentifier(
          "dependencyMap"
        );
      }

      state.dependencyCalls = new Set(["require", ...options.inlineableCalls]);
    }
  };
  traverse(ast, visitor, null, state);
  const collectedDependencies = state.dependencyRegistry.getDependencies(); // Compute the list of dependencies.

  const dependencies = new Array(collectedDependencies.length);

  for (const { index, name, ...dependencyData } of collectedDependencies) {
    dependencies[index] = {
      name,
      data: dependencyData
    };
  }

  return {
    ast,
    dependencies,
    dependencyMapName: nullthrows(state.dependencyMapIdentifier).name
  };
}

function collectImports(path, state) {
  if (path.node.source) {
    registerDependency(
      state,
      {
        name: path.node.source.value,
        asyncType: null,
        optional: false
      },
      path
    );
  }
}

function processImportCall(path, state, options) {
  const name = getModuleNameFromCallArgs(path);

  if (name == null) {
    throw new InvalidRequireCallError(path);
  }

  const dep = registerDependency(
    state,
    {
      name,
      asyncType: options.asyncType,
      splitCondition: options.splitCondition,
      optional: isOptionalDependency(name, path, state)
    },
    path
  );
  const transformer = state.dependencyTransformer;

  if (options.jsResource) {
    transformer.transformJSResource(path, dep, state);
  } else if (options.asyncType === "async") {
    transformer.transformImportCall(path, dep, state);
  } else {
    transformer.transformPrefetch(path, dep, state);
  }
}

function processRequireCall(path, state) {
  const name = getModuleNameFromCallArgs(path);
  const transformer = state.dependencyTransformer;

  if (name == null) {
    if (state.dynamicRequires === "reject") {
      throw new InvalidRequireCallError(path);
    }

    transformer.transformIllegalDynamicRequire(path, state);
    return;
  }

  const dep = registerDependency(
    state,
    {
      name,
      asyncType: null,
      optional: isOptionalDependency(name, path, state)
    },
    path
  );
  transformer.transformSyncRequire(path, dep, state);
}

function getNearestLocFromPath(path) {
  var _current;

  let current = path;

  while (current && !current.node.loc) {
    current = current.parentPath;
  }

  return (_current = current) === null || _current === void 0
    ? void 0
    : _current.node.loc;
}

function registerDependency(state, qualifier, path) {
  const dependency = state.dependencyRegistry.registerDependency(qualifier);
  const loc = getNearestLocFromPath(path);

  if (loc != null) {
    dependency.locs.push(loc);
  }

  return dependency;
}

function isOptionalDependency(name, path, state) {
  var _state$asyncRequireMo;

  const { allowOptionalDependencies } = state; // The async require module is a 'built-in'. Resolving should never fail -> treat it as non-optional.

  if (
    name ===
    ((_state$asyncRequireMo = state.asyncRequireModulePathStringLiteral) ===
      null || _state$asyncRequireMo === void 0
      ? void 0
      : _state$asyncRequireMo.value)
  ) {
    return false;
  }

  const isExcluded = () =>
    Array.isArray(allowOptionalDependencies.exclude) &&
    allowOptionalDependencies.exclude.includes(name);

  if (!allowOptionalDependencies || isExcluded()) {
    return false;
  } // Valid statement stack for single-level try-block: expressionStatement -> blockStatement -> tryStatement

  let sCount = 0;
  let p = path;

  while (p && sCount < 3) {
    if (p.isStatement()) {
      if (p.node.type === "BlockStatement") {
        // A single-level should have the tryStatement immediately followed BlockStatement
        // with the key 'block' to distinguish from the finally block, which has key = 'finalizer'
        return (
          p.parentPath != null &&
          p.parentPath.node.type === "TryStatement" &&
          p.key === "block"
        );
      }

      sCount += 1;
    }

    p = p.parentPath;
  }

  return false;
}

function getModuleNameFromCallArgs(path) {
  const expectedCount =
    path.node.callee.name === "__conditionallySplitJSResource" ? 2 : 1;
  const args = path.get("arguments");

  if (!Array.isArray(args) || args.length !== expectedCount) {
    throw new InvalidRequireCallError(path);
  }

  const result = args[0].evaluate();

  if (result.confident && typeof result.value === "string") {
    return result.value;
  }

  return null;
}

collectDependencies.getModuleNameFromCallArgs = getModuleNameFromCallArgs;

class InvalidRequireCallError extends Error {
  constructor({ node }) {
    const line = node.loc && node.loc.start && node.loc.start.line;
    super(
      `Invalid call at line ${line || "<unknown>"}: ${generate(node).code}`
    );
  }
}

collectDependencies.InvalidRequireCallError = InvalidRequireCallError;
/**
 * Produces a Babel template that will throw at runtime when the require call
 * is reached. This makes dynamic require errors catchable by libraries that
 * want to use them.
 */

const dynamicRequireErrorTemplate = template.statement(`
  (function(line) {
    throw new Error(
      'Dynamic require defined at line ' + line + '; not supported by Metro',
    );
  })(LINE)
`);
/**
 * Produces a Babel template that transforms an "import(...)" call into a
 * "require(...)" call to the asyncRequire specified.
 */

const makeAsyncRequireTemplate = template.statement(`
  require(ASYNC_REQUIRE_MODULE_PATH)(MODULE_ID, MODULE_NAME)
`);
const makeAsyncPrefetchTemplate = template.statement(`
  require(ASYNC_REQUIRE_MODULE_PATH).prefetch(MODULE_ID, MODULE_NAME)
`);
const makeJSResourceTemplate = template.statement(`
  require(ASYNC_REQUIRE_MODULE_PATH).resource(MODULE_ID, MODULE_NAME)
`);
const DefaultDependencyTransformer = {
  transformSyncRequire(path, dependency, state) {
    const moduleIDExpression = createModuleIDExpression(dependency, state);
    path.node.arguments = state.keepRequireNames
      ? [moduleIDExpression, types.stringLiteral(dependency.name)]
      : [moduleIDExpression];
  },

  transformImportCall(path, dependency, state) {
    path.replaceWith(
      makeAsyncRequireTemplate({
        ASYNC_REQUIRE_MODULE_PATH: nullthrows(
          state.asyncRequireModulePathStringLiteral
        ),
        MODULE_ID: createModuleIDExpression(dependency, state),
        MODULE_NAME: createModuleNameLiteral(dependency)
      })
    );
  },

  transformJSResource(path, dependency, state) {
    path.replaceWith(
      makeJSResourceTemplate({
        ASYNC_REQUIRE_MODULE_PATH: nullthrows(
          state.asyncRequireModulePathStringLiteral
        ),
        MODULE_ID: createModuleIDExpression(dependency, state),
        MODULE_NAME: createModuleNameLiteral(dependency)
      })
    );
  },

  transformPrefetch(path, dependency, state) {
    path.replaceWith(
      makeAsyncPrefetchTemplate({
        ASYNC_REQUIRE_MODULE_PATH: nullthrows(
          state.asyncRequireModulePathStringLiteral
        ),
        MODULE_ID: createModuleIDExpression(dependency, state),
        MODULE_NAME: createModuleNameLiteral(dependency)
      })
    );
  },

  transformIllegalDynamicRequire(path, state) {
    var _path$node$loc$start$, _path$node$loc;

    path.replaceWith(
      dynamicRequireErrorTemplate({
        LINE: types.numericLiteral(
          (_path$node$loc$start$ =
            (_path$node$loc = path.node.loc) === null ||
            _path$node$loc === void 0
              ? void 0
              : _path$node$loc.start.line) !== null &&
            _path$node$loc$start$ !== void 0
            ? _path$node$loc$start$
            : 0
        )
      })
    );
  }
};

function createModuleIDExpression(dependency, state) {
  return types.memberExpression(
    nullthrows(state.dependencyMapIdentifier),
    types.numericLiteral(dependency.index),
    true
  );
}

function createModuleNameLiteral(dependency) {
  return types.stringLiteral(dependency.name);
}

class DefaultModuleDependencyRegistry {
  _dependencies = new Map();

  registerDependency(qualifier) {
    let dependency = this._dependencies.get(qualifier.name);

    if (dependency == null) {
      const newDependency = {
        name: qualifier.name,
        asyncType: qualifier.asyncType,
        locs: [],
        index: this._dependencies.size
      };

      if (qualifier.optional) {
        newDependency.isOptional = true;
      }

      dependency = newDependency;

      this._dependencies.set(qualifier.name, dependency);
    } else {
      const original = dependency;
      dependency = collapseDependencies(original, qualifier);

      if (original !== dependency) {
        this._dependencies.set(qualifier.name, dependency);
      }
    }

    return dependency;
  }

  getDependencies() {
    return Array.from(this._dependencies.values());
  }
}

function collapseDependencies(dependency, qualifier) {
  let collapsed = dependency; // A previously optionally required dependency was required non-optionaly.
  // Mark it non optional for the whole module

  if (collapsed.isOptional && !qualifier.optional) {
    collapsed = { ...dependency, isOptional: false };
  } // A previously asynchronously (or prefetch) required module was required synchronously.
  // Make the dependency sync.

  if (collapsed.asyncType != null && qualifier.asyncType == null) {
    collapsed = { ...dependency, asyncType: null };
  } // A prefetched dependency was required async in the module. Mark it as async.

  if (collapsed.asyncType === "prefetch" && qualifier.asyncType === "async") {
    collapsed = { ...dependency, asyncType: "async" };
  }

  return collapsed;
}

module.exports = collectDependencies;
