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

const { Logger } = require("metro-core");

const JestWorker = require("jest-worker").default;

class WorkerFarm {
  constructor(config, transformerConfig) {
    this._config = config;
    this._transformerConfig = transformerConfig;

    if (this._config.maxWorkers > 1) {
      const worker = this._makeFarm(
        this._config.transformer.workerPath,
        ["transform"],
        this._config.maxWorkers
      );

      worker.getStdout().on("data", chunk => {
        this._config.reporter.update({
          type: "worker_stdout_chunk",
          chunk: chunk.toString("utf8")
        });
      });
      worker.getStderr().on("data", chunk => {
        this._config.reporter.update({
          type: "worker_stderr_chunk",
          chunk: chunk.toString("utf8")
        });
      });
      this._worker = worker;
    } else {
      // eslint-disable-next-line no-useless-call
      this._worker = require.call(null, this._config.transformer.workerPath);
    }
  }

  async kill() {
    if (this._worker && typeof this._worker.end === "function") {
      await this._worker.end();
    }
  }

  async transform(filename, options) {
    try {
      const data = await this._worker.transform(
        filename,
        options,
        this._config.projectRoot,
        this._transformerConfig
      );
      Logger.log(data.transformFileStartLogEntry);
      Logger.log(data.transformFileEndLogEntry);
      return {
        result: data.result,
        sha1: data.sha1
      };
    } catch (err) {
      if (err.loc) {
        throw this._formatBabelError(err, filename);
      } else {
        throw this._formatGenericError(err, filename);
      }
    }
  }

  _makeFarm(workerPath, exposedMethods, numWorkers) {
    const env = {
      ...process.env,
      // Force color to print syntax highlighted code frames.
      FORCE_COLOR: 1
    };
    return new JestWorker(workerPath, {
      computeWorkerKey: this._config.stickyWorkers // $FlowFixMe[method-unbinding] added when improving typing for this parameters
        ? this._computeWorkerKey
        : undefined,
      exposedMethods,
      forkOptions: {
        env
      },
      numWorkers
    });
  }

  _computeWorkerKey(method, filename) {
    // Only when transforming a file we want to stick to the same worker; and
    // we'll shard by file path. If not; we return null, which tells the worker
    // to pick the first available one.
    if (method === "transform") {
      return filename;
    }

    return null;
  }

  _formatGenericError(err, filename) {
    const error = new TransformError(`${filename}: ${err.message}`);
    return Object.assign(error, {
      stack: (err.stack || "")
        .split("\n")
        .slice(0, -1)
        .join("\n"),
      lineNumber: 0
    });
  }

  _formatBabelError(err, filename) {
    const error = new TransformError(
      `${err.type || "Error"}${
        err.message.includes(filename) ? "" : " in " + filename
      }: ${err.message}`
    ); // $FlowExpectedError: TODO(t67543470): Change this to properly extend the error.

    return Object.assign(error, {
      stack: err.stack,
      snippet: err.codeFrame,
      lineNumber: err.loc.line,
      column: err.loc.column,
      filename
    });
  }
}

class TransformError extends SyntaxError {
  type = "TransformError";

  constructor(message) {
    super(message);
    Error.captureStackTrace && Error.captureStackTrace(this, TransformError);
  }
}

module.exports = WorkerFarm;
