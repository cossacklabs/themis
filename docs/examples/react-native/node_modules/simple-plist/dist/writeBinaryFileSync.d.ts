/// <reference types="node" />
import { PathOrFileDescriptor, WriteFileOptions } from "fs";
import { PlistJsObj } from "./types";
export declare function writeBinaryFileSync(aFile: PathOrFileDescriptor, anObject: PlistJsObj, options?: WriteFileOptions): void;
