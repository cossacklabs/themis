import React from "react";
import { View, Text } from "react-native";
import { secureCellSealWithSymmetricKeyDecrypt64 } from "react-native-themis";
import { styles } from "../styles";

export async function testDecryptWithKey(key) {
    try {
        const key64 = "CiHPUks38qU5BYoiQY+tNWX+EcExjG8ckySRvsGPDf0=";
        // const message = "Hello, Themis!";
        const encrypted64 = "AAEBQAwAAAAQAAAADgAAAIe+NTvInzZLUobUZR1bxh23A+Jd5Hxdv0OkwJxVplXnsGMxR0EdCTATew==";
        const decrypted = await secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.resolved }>Test passed with result: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted result</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptWithKeyFailEmptyKey(key) {
    try {
        const key64 = "";
        const encrypted64 = "AAEBQAwAAAAQAAAADgAAAIe+NTvInzZLUobUZR1bxh23A+Jd5Hxdv0OkwJxVplXnsGMxR0EdCTATew==";
        const decrypted = await secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.rejected }>Test failed: decrypted value is { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptWithKeyFailEmptyEncrypted(key) {
    try {
        const key64 = "CiHPUks38qU5BYoiQY+tNWX+EcExjG8ckySRvsGPDf0=";
        const encrypted64 = "";
        const decrypted = await secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because encrypted is empty</Text>
            <Text style={ styles.rejected }>Test failed: decrypted value is { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because encrypted is empty</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptWithWrongKey(key) {
    try {
        const key64 = "m2kTB09QqLjBOv71wr9EhoqDdveHX1CAwTimZPcC8fA=";
        const encrypted64 = "AAEBQAwAAAAQAAAADgAAAIe+NTvInzZLUobUZR1bxh23A+Jd5Hxdv0OkwJxVplXnsGMxR0EdCTATew==";
        const decrypted = await secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is wrong</Text>
            <Text style={ styles.rejected }>Test failed: decrypted value is { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureCellSealWithSymmetricKeyDecrypt64</Text>
            <Text style={ styles.text }>Wait for exception because key is wrong</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}
