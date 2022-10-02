var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "bplist-parser", "plist"], factory);
    }
})(function (require, exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.parse = void 0;
    var bplist_parser_1 = __importDefault(require("bplist-parser"));
    var plist_1 = __importDefault(require("plist"));
    /**
     * Detects the format of the given string or buffer, then attempts to parse the
     * payload using the appropriate tooling.
     */
    function parse(aStringOrBuffer, aFile) {
        var firstByte = aStringOrBuffer[0];
        var results;
        try {
            if (firstByte === 60 || firstByte === "<") {
                results = plist_1.default.parse(aStringOrBuffer.toString());
            }
            else if (firstByte === 98) {
                results = bplist_parser_1.default.parseBuffer(aStringOrBuffer)[0];
            }
            else if (aFile) {
                throw new Error("Unable to determine format for '" + aFile + "'");
            }
            else {
                throw new Error("Unable to determine format for plist aStringOrBuffer");
            }
        }
        catch (error) {
            throw error instanceof Error ? error : new Error("error parsing " + aFile);
        }
        return results;
    }
    exports.parse = parse;
});
