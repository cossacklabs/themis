import React from "react";
import { View, Text } from "react-native";
import { secureCellTokenProtectDecrypt64 } from "react-native-themis";
import { styles } from "../styles";

export async function testDecryptToken(key) {
    try {
        const key64 = "qFDGQv2fYu8A16oUrxK3Z/qTJeV0bX5KYLU7Cos8PDY=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptTokenAnotherKey(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenEmptyKey(key) {
    try {
        const key64 = "";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenWrongKey(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoIIUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of wrong key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of wrong key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenEmptyEncrypted(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty encrypted data</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty encrypted data</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenAnotherEncrypted(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "Is81i1G5QyKtJwZbJio=";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another encrypted data</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another encrypted data</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenEmptyToken(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty token</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty token</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenWrongToken(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADQAAAMCrR8pnMLsiCL7skawCKqXgrlX39ujiKG0UaGg";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect token</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect token</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenAnotherToken(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADgAAADqDui/CHeZPlI4Xm+Td97MM/15mDtVosvIcdqg=";
        const context = "context";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another token</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of another token</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenEmptyContext(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADgAAADqDui/CHeZPlI4Xm+Td97MM/15mDtVosvIcdqg=";
        const context = "";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty context</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptTokenOtherContext(key) {
    try {
        const key64 = "pn2TzOgO2hVyNwdX0ExoI/IUkIvNWc3TLF/KeFqdZJ8=";
        const encrypted64 = "vFjYswo6NfJ3fXq3uw==";
        const token64 = "AAEBQAwAAAAQAAAADgAAADqDui/CHeZPlI4Xm+Td97MM/15mDtVosvIcdqg=";
        const context = "ctx";
        const decrypted = await secureCellTokenProtectDecrypt64(key64, encrypted64, token64, context);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of other context</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellTokenProtectDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of other context</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

