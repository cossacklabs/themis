import React from "react";
import { View, Text } from "react-native";
import { secureCellSealWithPassphraseEncrypt64 } from "react-native-themis";
import { styles } from "../styles";

export async function testEncryptWithPassphrase(key) {
    try {
        const passphrase = "passphrase";
        const message = "Hello, World!";
        const encrypted = await secureCellSealWithPassphraseEncrypt64(passphrase, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptWithPassphraseFailEmptyPassphrase(key) {
    try {
        const passphrase = "";
        const message = "Hello, World!";
        const encrypted = await secureCellSealWithPassphraseEncrypt64(passphrase, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because passphrase is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because passphrase is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptWithPassphraseFailEmptyMessage(key) {
    try {
        const passphrase = "passphrase";
        const message = "";
        const encrypted = await secureCellSealWithPassphraseEncrypt64(passphrase, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because message is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because message is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptWithPassphraseSuccessWithContext(key) {
    try {
        const passphrase = "passphrase";
        const message = "Hello, World!";
        const context = "context";
        const encrypted = await secureCellSealWithPassphraseEncrypt64(passphrase, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

