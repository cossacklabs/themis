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

const DeltaCalculator = require("./DeltaBundler/DeltaCalculator");

/**
 * `DeltaBundler` uses the `DeltaTransformer` to build bundle deltas. This
 * module handles all the transformer instances so it can support multiple
 * concurrent clients requesting their own deltas. This is done through the
 * `clientId` param (which maps a client to a specific delta transformer).
 */
class DeltaBundler {
  _deltaCalculators = new Map();

  constructor(bundler) {
    this._bundler = bundler;
  }

  end() {
    this._deltaCalculators.forEach(deltaCalculator => deltaCalculator.end());

    this._deltaCalculators = new Map();
  }

  async getDependencies(entryPoints, options) {
    const depGraph = await this._bundler.getDependencyGraph();
    const deltaCalculator = new DeltaCalculator(entryPoints, depGraph, options);
    await deltaCalculator.getDelta({
      reset: true,
      shallow: options.shallow
    });
    const graph = deltaCalculator.getGraph();
    deltaCalculator.end();
    return graph.dependencies;
  } // Note: the graph returned by this function needs to be ended when finished
  // so that we don't leak graphs that are not reachable.
  // To get just the dependencies, use getDependencies which will not leak graphs.

  async buildGraph(entryPoints, options) {
    const depGraph = await this._bundler.getDependencyGraph();
    const deltaCalculator = new DeltaCalculator(entryPoints, depGraph, options);
    await deltaCalculator.getDelta({
      reset: true,
      shallow: options.shallow
    });
    const graph = deltaCalculator.getGraph();

    this._deltaCalculators.set(graph, deltaCalculator);

    return graph;
  }

  async getDelta(graph, { reset, shallow }) {
    const deltaCalculator = this._deltaCalculators.get(graph);

    if (!deltaCalculator) {
      throw new Error("Graph not found");
    }

    return await deltaCalculator.getDelta({
      reset,
      shallow
    });
  }

  listen(graph, callback) {
    const deltaCalculator = this._deltaCalculators.get(graph);

    if (!deltaCalculator) {
      throw new Error("Graph not found");
    }

    deltaCalculator.on("change", callback);
    return () => {
      deltaCalculator.removeListener("change", callback);
    };
  }

  endGraph(graph) {
    const deltaCalculator = this._deltaCalculators.get(graph);

    if (!deltaCalculator) {
      throw new Error("Graph not found");
    }

    deltaCalculator.end();

    this._deltaCalculators.delete(graph);
  }
}

module.exports = DeltaBundler;
