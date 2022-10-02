/// <reference types="node" />
import { PathOrFileDescriptor, WriteFileOptions } from "fs";
import { callbackFn, PlistJsObj } from "./types";
export declare function writeFile(aFile: PathOrFileDescriptor, anObject: PlistJsObj, options: callbackFn<void>): void;
export declare function writeFile(aFile: PathOrFileDescriptor, anObject: PlistJsObj, options: WriteFileOptions, callback: callbackFn<void>): void;
