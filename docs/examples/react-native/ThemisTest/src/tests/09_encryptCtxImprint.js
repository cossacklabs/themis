import React from "react";
import { View, Text } from "react-native";
import { secureCellContextImprintEncrypt64, symmetricKey64 } from "react-native-themis";
import { styles } from "../styles";

export async function testEncryptCtxImprint(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "Hello, Themis!";
        const context = "context";
        const encrypted = await secureCellContextImprintEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Encrypted: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptCtxImprintFailEmptyKey(key) {
    try {
        const key64 = "";
        const message = "Hello, Themis!";
        const context = "context";
        const encrypted = await secureCellContextImprintEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Encrypted: { encrypted }</Text>
        </View>;
        return Promise.rejected(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptCtxImprintFailEmptyMessage(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "";
        const context = "context";
        const encrypted = await secureCellContextImprintEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty data to encrypt</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Encrypted: { encrypted }</Text>
        </View>;
        return Promise.rejected(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty data to encrypt</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptCtxImprintFailEmptyContext(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "Hello, Themis!";
        const context = "";
        const encrypted = await secureCellContextImprintEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Encrypted: { encrypted }</Text>
        </View>;
        return Promise.rejected(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptCtxImprintFailWrongKey(key) {
    try {
        const key64 = "D3X03AlUEb85VMLhjONrBGTLaaAvGwScoNXtyOJh9s=";
        const message = "Hello, Themis!";
        const context = "context";
        const encrypted = await secureCellContextImprintEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of wrong key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Encrypted: { encrypted }</Text>
        </View>;
        return Promise.rejected(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellContextImprintEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of wrong key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

