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
    exports.writeBinaryFileSync = void 0;
    var bplist_creator_1 = __importDefault(require("bplist-creator"));
    var fs_1 = __importDefault(require("fs"));
    function writeBinaryFileSync(aFile, anObject, options) {
        return fs_1.default.writeFileSync(aFile, (0, bplist_creator_1.default)(anObject), options);
    }
    exports.writeBinaryFileSync = writeBinaryFileSync;
});
