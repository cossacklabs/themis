/// <reference types="node" />
import { PathOrFileDescriptor, WriteFileOptions } from "fs";
import { callbackFn, PlistJsObj } from "./types";
export declare function writeBinaryFile(aFile: PathOrFileDescriptor, anObject: PlistJsObj, callback: callbackFn<void>): void;
export declare function writeBinaryFile(aFile: PathOrFileDescriptor, anObject: PlistJsObj, options: WriteFileOptions, callback: callbackFn<void>): void;
