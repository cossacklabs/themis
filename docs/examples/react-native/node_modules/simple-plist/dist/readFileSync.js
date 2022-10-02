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
    exports.readFileSync = void 0;
    var fs_1 = __importDefault(require("fs"));
    var parse_1 = require("./parse");
    function readFileSync(aFile) {
        var contents = fs_1.default.readFileSync(aFile);
        if (contents.length === 0) {
            return {};
        }
        return (0, parse_1.parse)(contents, aFile);
    }
    exports.readFileSync = readFileSync;
});
