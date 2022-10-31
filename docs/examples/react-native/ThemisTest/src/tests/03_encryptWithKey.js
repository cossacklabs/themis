import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { secureCellSealWithSymmetricKeyEncrypt64, symmetricKey64 } from 'react-native-themis';
import { styles } from "../styles";


export async function testEncryptWithKey(key) {
    try {
        const key64 = await symmetricKey64();
        const message = 'Hello, World!';
        const encrypted = await secureCellSealWithSymmetricKeyEncrypt64(key64, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptWithKeyFailEmptyKey(key) {
    try {
        const key64 = '';
        const message = 'Hello, World!';
        const encrypted = await secureCellSealWithSymmetricKeyEncrypt64(key64, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.resolved }>Test passed with exception: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptWithKeyFailEmptyText(key) {
    try {
        const key64 = await symmetricKey64();
        const message = '';
        const encrypted = await secureCellSealWithSymmetricKeyEncrypt64(key64, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because plaintext is empty</Text>
            <Text style={ styles.rejected }>Test failed: encrypted value is { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because plaintext is empty</Text>
            <Text style={ styles.resolved }>Test passed with exception: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testEncryptWithWrongKey(key) {
    const key64 = 'Hello, World!';
    try {
        const message = 'Hello, World!';
        const encrypted = await secureCellSealWithSymmetricKeyEncrypt64(key64, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key64 is not base64 encoded</Text>
            <Text style={ styles.text }>key64 = { key64 }</Text>
            <Text style={ styles.rejected }>Test failed with result: { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyEncrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key64 is not base64 encoded</Text>
            <Text style={ styles.text }>key64 = { key64 }</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}
