// AAEBQQwAAAAQAAAADQAAABYAAACPVwlzd/3Geyfk2yhqfdmVR6S8f3fQw+LLGGsmQA0DABAASEG+Op/mSVBIQW62iV9smQwaW4uz2jlj0u7Tins=
import React from "react";
import { View, Text, StyleSheet } from "react-native";
import { secureCellSealWithPassphraseDecrypt64 } from "react-native-themis";
import { styles } from "../styles";

export async function testDecryptWithPassphrase(key) {
    try {
        const passphrase = "passphrase";
        const context = "context";
        const message = "AAEBQQwAAAAQAAAADQAAABYAAACPVwlzd/3Geyfk2yhqfdmVR6S8f3fQw+LLGGsmQA0DABAASEG+Op/mSVBIQW62iV9smQwaW4uz2jlj0u7Tins=";
        const decrypted = await secureCellSealWithPassphraseDecrypt64(passphrase, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptWithPassphraseWithoutContext(key) {
    try {
        const passphrase = "passphrase";
        const message = "AAEBQQwAAAAQAAAADQAAABYAAACPVwlzd/3Geyfk2yhqfdmVR6S8f3fQw+LLGGsmQA0DABAASEG+Op/mSVBIQW62iV9smQwaW4uz2jlj0u7Tins=";
        const decrypted = await secureCellSealWithPassphraseDecrypt64(passphrase, message);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because context is empty</Text>
            <Text style={ styles.rejected }>Test failed with result: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because context is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptWithPassphraseWithWrongContext(key) {
    try {
        const passphrase = "passphrase";
        const message = "AAEBQQwAAAAQAAAADQAAABYAAACPVwlzd/3Geyfk2yhqfdmVR6S8f3fQw+LLGGsmQA0DABAASEG+Op/mSVBIQW62iV9smQwaW4uz2jlj0u7Tins=";
        const context = "wrong context";
        const decrypted = await secureCellSealWithPassphraseDecrypt64(passphrase, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because context is wrong</Text>
            <Text style={ styles.rejected }>Test failed with result: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because context is wrong</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptWithPassphraseWithWrongPassphrase(key) {
    try {
        const passphrase = "wrong passphrase";
        const context = "context";
        const message = "AAEBQQwAAAAQAAAADQAAABYAAACPVwlzd/3Geyfk2yhqfdmVR6S8f3fQw+LLGGsmQA0DABAASEG+Op/mSVBIQW62iV9smQwaW4uz2jlj0u7Tins=";
        const decrypted = await secureCellSealWithPassphraseDecrypt64(passphrase, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because passphrase is wrong</Text>
            <Text style={ styles.rejected }>Test failed with result: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because passphrase is wrong</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptWithPassphraseWithWrongMessage(key) {
    try {
        const passphrase = "passphrase";
        const context = "context";
        const message = "AAEBQQwAAAAQAAAADgAAABYAAABb1YeBXQmdO3k1pf7HAVmBdAtX1OF7YBHzmOvnQA0DABAAVRwnkFzhoq+ERkAIwg3VVQEMnwVU22Pw38VAoQ8L=";
        const decrypted = await secureCellSealWithPassphraseDecrypt64(passphrase, message, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because message is wrong</Text>
            <Text style={ styles.rejected }>Test failed with result: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithPassphraseDecrypt64</Text>
            <Text style={ styles.text }>Wait exception because message is wrong</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}


