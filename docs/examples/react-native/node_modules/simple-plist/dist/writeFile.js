var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "fs", "plist"], factory);
    }
})(function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.writeFile = void 0;
    var fs_1 = __importDefault(require("fs"));
    var plist_1 = __importDefault(require("plist"));
    function writeFile(aFile, anObject, options, callback) {
        if (typeof options === "function" && callback === undefined) {
            fs_1.default.writeFile(aFile, plist_1.default.build(anObject), options);
        }
        else if (typeof options === "object" && typeof callback === "function") {
            fs_1.default.writeFile(aFile, plist_1.default.build(anObject), options, callback);
        }
        else {
            throw new Error("Invalid parameters passed to writeFile");
        }
    }
    exports.writeFile = writeFile;
});
