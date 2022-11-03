import React from "react";
import { Text, View } from "react-native";
import { secureMessageEncrypt64, keyPair64 } from "react-native-themis";
import { styles } from "../styles";

export async function testEncryptMessage(key) {
    try {
        const alice = await keyPair64();
        const booob = await keyPair64();
        const message = "Hello, Bob. Best wishes, Alice.";
        const encrypted = await secureMessageEncrypt64(message, alice.private64, booob.public64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Encrypted message: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptMessageWithOnePair(key) {
    try {
        const alice = await keyPair64();
        const message = "Hello, Alice. Best wishes, Alice.";
        const encrypted = await secureMessageEncrypt64(message, alice.private64, alice.public64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Encrypted message: { encrypted }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for encrypted message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testEncryptMessageWithEmptyPublicKey(key) {
    try {
        const alice = await keyPair64();
        const message = "Hello, Alice. Best wishes, Alice.";
        const encrypted = await secureMessageEncrypt64(message, alice.private64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty public key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Encrypted message: { encrypted }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageEncrypt64</Text>
            <Text style={ styles.text }>Wait for error because of empty public key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}
