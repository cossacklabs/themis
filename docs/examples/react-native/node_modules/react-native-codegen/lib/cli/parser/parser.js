/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 *
 * @format
 */
'use strict';

const FlowParser = require('../../parsers/flow');

function parseFiles(files) {
  files.forEach(filename => {
    console.log(
      filename,
      JSON.stringify(FlowParser.parseFile(filename), null, 2),
    );
  });
}

module.exports = parseFiles;
