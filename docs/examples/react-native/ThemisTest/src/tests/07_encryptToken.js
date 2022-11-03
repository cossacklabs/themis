import React from "react";
import { View, Text } from "react-native";
import { secureCellTokenProtectEncrypt64, symmetricKey64 } from "react-native-themis";
import { styles } from "../styles";

export async function testEncryptToken(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "Hello, Themis!";
        const context = "context";
        const encrypted = await secureCellTokenProtectEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Token: { encrypted.token64 }</Text>
            <Text style={ styles.resolved }>Encrypted: { encrypted.encrypted64 }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptTokenFailEmptyKey(key) {
    try {
        const key64 = "";
        const message = "Hello, World!";
        const context = "context";
        const encrypted = await secureCellTokenProtectEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
            <Text style={ styles.rejected }>Token: { encrypted.token64 }</Text>
            <Text style={ styles.rejected }>Encrypted { encrypted.encrypted64 }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptTokenFailEmptyMessage(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "";
        const context = "context";
        const encrypted = await secureCellTokenProtectEncrypt64(key64, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because message is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
            <Text style={ styles.rejected }>Token: { encrypted.token64 }</Text>
            <Text style={ styles.rejected }>Encrypted { encrypted.encrypted64 }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because message is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptTokenWithContextEmpty(key) {
    try {
        const key64 = await symmetricKey64();
        const message = "Hello, World!";
        const encrypted = await secureCellTokenProtectEncrypt64(key64, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result without context</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Token: { encrypted.token64 }</Text>
            <Text style={ styles.resolved }>Encrypted: { encrypted.encrypted64 }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

