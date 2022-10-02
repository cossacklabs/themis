var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "bplist-creator", "fs"], factory);
    }
})(function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.writeBinaryFile = void 0;
    var bplist_creator_1 = __importDefault(require("bplist-creator"));
    var fs_1 = __importDefault(require("fs"));
    function writeBinaryFile(aFile, anObject, options, callback) {
        if (typeof options === "function" && callback === undefined) {
            fs_1.default.writeFile(aFile, (0, bplist_creator_1.default)(anObject), options);
        }
        else if (typeof options === "object" && typeof callback === "function") {
            fs_1.default.writeFile(aFile, (0, bplist_creator_1.default)(anObject), options, callback);
        }
        else {
            throw new Error("Invalid parameters passed to writeBinaryFile");
        }
    }
    exports.writeBinaryFile = writeBinaryFile;
});
