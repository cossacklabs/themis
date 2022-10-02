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

var _invariant = _interopRequireDefault(require("invariant"));

var _nullthrows = _interopRequireDefault(require("nullthrows"));

function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : { default: obj };
}

function reverseDependencyMapReferences({ types: t }) {
  return {
    visitor: {
      CallExpression(path, state) {
        const { node } = path;

        if (node.callee.name === `${state.opts.globalPrefix}__d`) {
          // $FlowFixMe Flow error uncovered by typing Babel more strictly
          const lastArg = node.arguments[0].params.slice(-1)[0]; // $FlowFixMe Flow error uncovered by typing Babel more strictly

          const depMapName = lastArg && lastArg.name;

          if (depMapName == null) {
            return;
          }

          const body = path.get("arguments.0.body");
          (0, _invariant.default)(
            !Array.isArray(body),
            "meetro: Expected `body` to be a single path."
          );
          const scope = body.scope;
          const binding = (0, _nullthrows.default)(
            scope.getBinding(depMapName)
          );
          binding.referencePaths.forEach(({ parentPath }) => {
            const memberNode =
              parentPath === null || parentPath === void 0
                ? void 0
                : parentPath.node;

            if (
              memberNode != null &&
              memberNode.type === "MemberExpression" &&
              memberNode.property.type === "NumericLiteral"
            ) {
              const numericLiteral = t.numericLiteral(
                state.opts.dependencyIds[memberNode.property.value]
              );
              (0, _nullthrows.default)(parentPath).replaceWith(numericLiteral);
            }
          });
        }
      }
    }
  };
}

module.exports = reverseDependencyMapReferences;
