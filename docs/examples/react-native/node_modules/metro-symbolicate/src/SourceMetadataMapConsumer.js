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

const vlq = require("vlq");

const { normalizeSourcePath } = require("metro-source-map");

const METADATA_FIELD_FUNCTIONS = 0;

/**
 * Consumes the `x_facebook_sources` metadata field from a source map and
 * exposes various queries on it.
 *
 * By default, source names are normalized using the same logic that the
 * `source-map@0.5.6` package uses internally. This is crucial for keeping the
 * sources list in sync with a `SourceMapConsumer` instance.

 * If you're using this with a different source map reader (e.g. one that
 * doesn't normalize source names at all), you can switch out the normalization
 * function in the constructor, e.g.
 *
 *     new SourceMetadataMapConsumer(map, source => source) // Don't normalize
 */
class SourceMetadataMapConsumer {
  constructor(map, normalizeSourceFn = normalizeSourcePath) {
    this._sourceMap = map;
    this._decodedFunctionMapCache = new Map();
    this._normalizeSource = normalizeSourceFn;
  }

  /**
   * Retrieves a human-readable name for the function enclosing a particular
   * source location.
   *
   * When used with the `source-map` package, you'll first use
   * `SourceMapConsumer#originalPositionFor` to retrieve a source location,
   * then pass that location to `functionNameFor`.
   */
  functionNameFor({ line, column, source }) {
    if (source && line != null && column != null) {
      const mappings = this._getFunctionMappings(source);

      if (mappings) {
        const mapping = findEnclosingMapping(mappings, {
          line,
          column
        });

        if (mapping) {
          return mapping.name;
        }
      }
    }

    return null;
  }
  /**
   * Returns this map's source metadata as a new array with the same order as
   * `sources`.
   *
   * This array can be used as the `x_facebook_sources` field of a map whose
   * `sources` field is the array that was passed into this method.
   */

  toArray(sources) {
    const metadataBySource = this._getMetadataBySource();

    const encoded = [];

    for (const source of sources) {
      encoded.push(metadataBySource[source] || null);
    }

    return encoded;
  }
  /**
   * Prepares and caches a lookup table of metadata by source name.
   */

  _getMetadataBySource() {
    if (!this._metadataBySource) {
      this._metadataBySource = this._getMetadataObjectsBySourceNames(
        this._sourceMap
      );
    }

    return this._metadataBySource;
  }
  /**
   * Decodes the function name mappings for the given source if needed, and
   * retrieves a sorted, searchable array of mappings.
   */

  _getFunctionMappings(source) {
    if (this._decodedFunctionMapCache.has(source)) {
      return this._decodedFunctionMapCache.get(source);
    }

    let parsedFunctionMap = null;

    const metadataBySource = this._getMetadataBySource(); // $FlowFixMe[method-unbinding] added when improving typing for this parameters

    if (Object.prototype.hasOwnProperty.call(metadataBySource, source)) {
      const metadata = metadataBySource[source] || [];
      parsedFunctionMap = decodeFunctionMap(metadata[METADATA_FIELD_FUNCTIONS]);
    }

    this._decodedFunctionMapCache.set(source, parsedFunctionMap);

    return parsedFunctionMap;
  }
  /**
   * Collects source metadata from the given map using the current source name
   * normalization function. Handles both index maps (with sections) and plain
   * maps.
   *
   * NOTE: If any sources are repeated in the map (which shouldn't happen in
   * Metro, but is technically possible because of index maps) we only keep the
   * metadata from the last occurrence of any given source.
   */

  _getMetadataObjectsBySourceNames(map) {
    // eslint-disable-next-line lint/strictly-null
    if (map.mappings === undefined) {
      const indexMap = map;
      return Object.assign(
        {},
        ...indexMap.sections.map(section =>
          this._getMetadataObjectsBySourceNames(section.map)
        )
      );
    }

    if ("x_facebook_sources" in map) {
      const basicMap = map;
      return (basicMap.x_facebook_sources || []).reduce(
        (acc, metadata, index) => {
          let source = basicMap.sources[index];

          if (source != null) {
            source = this._normalizeSource(source, basicMap);
            acc[source] = metadata;
          }

          return acc;
        },
        {}
      );
    }

    return {};
  }
}

function decodeFunctionMap(functionMap) {
  if (!functionMap) {
    return [];
  }

  const parsed = [];
  let line = 1;
  let nameIndex = 0;

  for (const lineMappings of functionMap.mappings.split(";")) {
    let column = 0;

    for (const mapping of lineMappings.split(",")) {
      const [columnDelta, nameDelta, lineDelta = 0] = vlq.decode(mapping);
      line += lineDelta;
      nameIndex += nameDelta;
      column += columnDelta;
      parsed.push({
        line,
        column,
        name: functionMap.names[nameIndex]
      });
    }
  }

  return parsed;
}

function findEnclosingMapping(mappings, target) {
  let first = 0;
  let it = 0;
  let count = mappings.length;
  let step;

  while (count > 0) {
    it = first;
    step = Math.floor(count / 2);
    it += step;

    if (comparePositions(target, mappings[it]) >= 0) {
      first = ++it;
      count -= step + 1;
    } else {
      count = step;
    }
  }

  return first ? mappings[first - 1] : null;
}

function comparePositions(a, b) {
  if (a.line === b.line) {
    return a.column - b.column;
  }

  return a.line - b.line;
}

module.exports = SourceMetadataMapConsumer;
