var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "fs", "./parse"], factory);
    }
})(function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.readFile = void 0;
    var fs_1 = __importDefault(require("fs"));
    var parse_1 = require("./parse");
    function readFile(aFile, callback) {
        fs_1.default.readFile(aFile, function (err, contents) {
            if (err) {
                return callback(err);
            }
            var results;
            try {
                results = (0, parse_1.parse)(contents, aFile);
            }
            catch (error) {
                return callback(error instanceof Error
                    ? error
                    : new Error("failed to read file " + aFile));
            }
            callback(null, results);
        });
    }
    exports.readFile = readFile;
});
