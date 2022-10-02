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

const fs = require("fs-extra");

exports.watchFile = async function(filename, callback) {
  fs.watchFile(filename, () => {
    callback();
  });
  await callback();
};

exports.makeAsyncCommand = command => argv => {
  Promise.resolve(command(argv)).catch(error => {
    console.error(error.stack);
    process.exitCode = 1;
  });
};
