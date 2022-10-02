var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "bplist-creator", "bplist-parser", "./parse", "./readFile", "./readFileSync", "./stringify", "./writeBinaryFile", "./writeBinaryFileSync", "./writeFile", "./writeFileSync", "./parse", "./readFile", "./readFileSync", "./stringify", "./writeBinaryFile", "./writeBinaryFileSync", "./writeFile", "./writeFileSync"], factory);
    }
})(function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.writeFileSync = exports.writeFile = exports.writeBinaryFileSync = exports.writeBinaryFile = exports.stringify = exports.readFileSync = exports.readFile = exports.parse = void 0;
    var bplist_creator_1 = __importDefault(require("bplist-creator"));
    var bplist_parser_1 = __importDefault(require("bplist-parser"));
    var parse_1 = require("./parse");
    var readFile_1 = require("./readFile");
    var readFileSync_1 = require("./readFileSync");
    var stringify_1 = require("./stringify");
    var writeBinaryFile_1 = require("./writeBinaryFile");
    var writeBinaryFileSync_1 = require("./writeBinaryFileSync");
    var writeFile_1 = require("./writeFile");
    var writeFileSync_1 = require("./writeFileSync");
    // "modern" named exports
    var parse_2 = require("./parse");
    Object.defineProperty(exports, "parse", { enumerable: true, get: function () { return parse_2.parse; } });
    var readFile_2 = require("./readFile");
    Object.defineProperty(exports, "readFile", { enumerable: true, get: function () { return readFile_2.readFile; } });
    var readFileSync_2 = require("./readFileSync");
    Object.defineProperty(exports, "readFileSync", { enumerable: true, get: function () { return readFileSync_2.readFileSync; } });
    var stringify_2 = require("./stringify");
    Object.defineProperty(exports, "stringify", { enumerable: true, get: function () { return stringify_2.stringify; } });
    var writeBinaryFile_2 = require("./writeBinaryFile");
    Object.defineProperty(exports, "writeBinaryFile", { enumerable: true, get: function () { return writeBinaryFile_2.writeBinaryFile; } });
    var writeBinaryFileSync_2 = require("./writeBinaryFileSync");
    Object.defineProperty(exports, "writeBinaryFileSync", { enumerable: true, get: function () { return writeBinaryFileSync_2.writeBinaryFileSync; } });
    var writeFile_2 = require("./writeFile");
    Object.defineProperty(exports, "writeFile", { enumerable: true, get: function () { return writeFile_2.writeFile; } });
    var writeFileSync_2 = require("./writeFileSync");
    Object.defineProperty(exports, "writeFileSync", { enumerable: true, get: function () { return writeFileSync_2.writeFileSync; } });
    // preserve backwards compatibility
    module.exports = {
        bplistCreator: bplist_creator_1.default,
        bplistParser: bplist_parser_1.default,
        parse: parse_1.parse,
        readFile: readFile_1.readFile,
        readFileSync: readFileSync_1.readFileSync,
        stringify: stringify_1.stringify,
        writeBinaryFile: writeBinaryFile_1.writeBinaryFile,
        writeBinaryFileSync: writeBinaryFileSync_1.writeBinaryFileSync,
        writeFile: writeFile_1.writeFile,
        writeFileSync: writeFileSync_1.writeFileSync,
    };
});
