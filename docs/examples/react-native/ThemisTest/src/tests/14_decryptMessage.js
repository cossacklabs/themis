import React from "react";
import { Text, View } from "react-native";
import { secureMessageDecrypt64, secureMessageEncrypt64, keyPair64 } from "react-native-themis";
import { styles } from "../styles";

export async function testDecryptMessage(key) {
    try {
        const alice = await keyPair64();
        const bob = await keyPair64();
        const message = "Hello, Bob. Best wishes, Alice.";
        const encrypted = await secureMessageEncrypt64(message, alice.private64, bob.public64);
        const decrypted = await secureMessageDecrypt64(encrypted, bob.private64, alice.public64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Decrypted message: { decrypted }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for decrypted message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testDecryptMessageIncorrectKeys(key) {
    try {
        const alice = await keyPair64();
        const bob = await keyPair64();
        const carol = await keyPair64();
        const message = "Hello, Bob. Best wishes, Alice.";
        const encrypted = await secureMessageEncrypt64(message, alice.private64, bob.public64);
        const decrypted = await secureMessageDecrypt64(encrypted, carol.private64, bob.public64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted message: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testDecryptMessageNotBase64Params(key) {
    try {
        const alicePrivateKey = "U29tZSB0ZXh0IGZvciBhIGtleSB0aGF0IGlzIG5vdCBiYXNlNjQ=";
        const bobPublicKey = "U29tZSB0ZXh0IGZvciBhIGtleSB0aGF0IGlzIG5vdCBiYXNlNjQ=";
        const message = "Hello, Bob. Best wishes, Alice.";
        const decrypted = await secureMessageDecrypt64(message, alicePrivateKey, bobPublicKey);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect message</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Decrypted message: { decrypted }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageDecrypt64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect message</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}
