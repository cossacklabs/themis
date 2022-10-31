import React from "react";
import { View, Text } from "react-native";
import { secureCellContextImprintDecrypt64 } from "react-native-themis";
import { styles } from "../styles";

export async function testDecryptCtxImprint(key) {
    try {
        const key64 = "kXrZjQ07hQLNPc0YnUCh+B1ev7W2rhGPj28g2DHo8CU=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptCtxImprintAnotherKey(key) {
    /* Warning: In Context Imprint mode, Secure Cell cannot validate correctness of the decrypted data.
    * If an incorrect secret or context is used, or if the data has been corrupted,
    * Secure Cell will return garbage output without returning an error.
    * https://docs.cossacklabs.com/themis/languages/javascript/#secure-cell-context-imprint
    */

    try {
        const key64 = "iyzMsEu6jF5+rlLzo78dGCZYijIIrAcBbI7HV/acGRY=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.warning }>Warning: In Context Imprint mode, Secure Cell cannot validate correctness of the decrypted data.</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptCtxImprintAnotherContext(key) {
    /* Warning: In Context Imprint mode, Secure Cell cannot validate correctness of the decrypted data.
    * If an incorrect secret or context is used, or if the data has been corrupted,
    * Secure Cell will return garbage output without returning an error.
    * https://docs.cossacklabs.com/themis/languages/javascript/#secure-cell-context-imprint
    */

    try {
        const key64 = "kXrZjQ07hQLNPc0YnUCh+B1ev7W2rhGPj28g2DHo8CU=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "ctxImprintMode";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.warning }>Warning: In Context Imprint mode, Secure Cell cannot validate correctness of the decrypted data.</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}


export async function testDecryptCtxImprintEmptyKey(key) {
    try {
        const key64 = "";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptCtxImprintEmptyMessage(key) {
    try {
        const key64 = "kXrZjQ07hQLNPc0YnUCh+B1ev7W2rhGPj28g2DHo8CU=";
        const encrypted64 = "";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty data to decrypt</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty data to decrypt</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptCtxImprintEmptyContext(key) {
    try {
        const key64 = "kXrZjQ07hQLNPc0YnUCh+B1ev7W2rhGPj28g2DHo8CU=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptCtxImprintWrongKey(key) {
    try {
        const key64 = "ELDWR0fMD96G8ZsywvBKox4O0nO/nPz/sdRS0DFDkvI=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.warning }>Warning: In Context Imprint mode, Secure Cell cannot validate correctness of the decrypted data.</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptCtxImprintIncorrectKey(key) {
    try {
        const key64 = "krZjQ07hQLNPc0YnUCh+B1ev7W2rhGPj28g2DHo8CU=";
        const encrypted64 = "VKigwpiUfelWi7AsuXM=";
        const context = "context";
        const decrypted = await secureCellContextImprintDecrypt64(key64, encrypted64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of broken key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of broken key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

