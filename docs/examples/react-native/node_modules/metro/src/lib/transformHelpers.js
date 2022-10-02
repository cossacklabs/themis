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

const baseIgnoredInlineRequires = ["React", "react", "react-native"];

async function calcTransformerOptions(
  entryFiles,
  bundler,
  deltaBundler,
  config,
  options
) {
  const baseOptions = {
    customTransformOptions: options.customTransformOptions,
    dev: options.dev,
    hot: options.hot,
    inlineRequires: false,
    inlinePlatform: true,
    minify: options.minify,
    platform: options.platform,
    runtimeBytecodeVersion: options.runtimeBytecodeVersion,
    unstable_transformProfile: options.unstable_transformProfile
  }; // When we're processing scripts, we don't need to calculate any
  // inlineRequires information, since scripts by definition don't have
  // requires().

  if (options.type === "script") {
    return { ...baseOptions, type: "script" };
  }

  const getDependencies = async path => {
    const dependencies = await deltaBundler.getDependencies([path], {
      resolve: await getResolveDependencyFn(bundler, options.platform),
      transform: await getTransformFn([path], bundler, deltaBundler, config, {
        ...options,
        minify: false
      }),
      transformOptions: options,
      onProgress: null,
      experimentalImportBundleSupport:
        config.transformer.experimentalImportBundleSupport,
      shallow: false
    });
    return Array.from(dependencies.keys());
  };

  const { transform } = await config.transformer.getTransformOptions(
    entryFiles,
    {
      dev: options.dev,
      hot: options.hot,
      platform: options.platform
    },
    getDependencies
  );
  return {
    ...baseOptions,
    inlineRequires: transform.inlineRequires || false,
    experimentalImportSupport: transform.experimentalImportSupport || false,
    unstable_disableES6Transforms:
      transform.unstable_disableES6Transforms || false,
    nonInlinedRequires:
      transform.nonInlinedRequires || baseIgnoredInlineRequires,
    type: "module"
  };
}

function removeInlineRequiresBlockListFromOptions(path, inlineRequires) {
  if (typeof inlineRequires === "object") {
    return !(path in inlineRequires.blockList);
  }

  return inlineRequires;
}

async function getTransformFn(
  entryFiles,
  bundler,
  deltaBundler,
  config,
  options
) {
  const { inlineRequires, ...transformOptions } = await calcTransformerOptions(
    entryFiles,
    bundler,
    deltaBundler,
    config,
    options
  );
  return async path => {
    return await bundler.transformFile(path, {
      ...transformOptions,
      type: getType(transformOptions.type, path, config.resolver.assetExts),
      inlineRequires: removeInlineRequiresBlockListFromOptions(
        path,
        inlineRequires
      )
    });
  };
}

function getType(type, filePath, assetExts) {
  if (type === "script") {
    return type;
  }

  if (assetExts.indexOf(path.extname(filePath).slice(1)) !== -1) {
    return "asset";
  }

  return "module";
}

async function getResolveDependencyFn(bundler, platform) {
  const dependencyGraph = await await bundler.getDependencyGraph();
  return (
    from,
    to // $FlowFixMe[incompatible-call]
  ) => dependencyGraph.resolveDependency(from, to, platform);
}

module.exports = {
  getTransformFn,
  getResolveDependencyFn
};
