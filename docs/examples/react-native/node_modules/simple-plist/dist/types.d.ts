/// <reference types="node" />
export declare type callbackFn<T> = (error: Error | null, result?: T) => void;
export declare type StringOrBuffer = string | Buffer;
export declare type PlistJsObj = Record<any, any> | any[];
