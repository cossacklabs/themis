import React from "react";
import { Text, View } from "react-native";
import { secureMessageSign64, keyPair64 } from "react-native-themis";
import { styles } from "../styles";

export async function testSignMessage(key) {
    try {
        const keys = await keyPair64(); //KEYTYPE_EC is default now
        const privateKey64 = keys.private64;
        const publicKey64 = keys.public64;
        const message = "Hello, Themis!";
        const signed = await secureMessageSign64(message, privateKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>Wait for signed message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Signed message: { signed }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>Wait for signed message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testSignMessageWithoutPublicKey(key) {
    try {
        const keys = await keyPair64(); //KEYTYPE_EC is default now
        const privateKey64 = keys.private64;
        const message = "Hello, Themis!";
        const signed = await secureMessageSign64(message, privateKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without public key</Text>
            <Text style={ styles.text }>Wait for signed message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Signed message: { signed }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without public key</Text>
            <Text style={ styles.text }>Wait for signed message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testSignMessageWithoutPrivateKey(key) {
    try {
        const message = "Hello, Themis!";
        const signed = await secureMessageSign64(message, undefined);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without private key</Text>
            <Text style={ styles.text }>Wait for error</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Signed message: { signed }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without private key</Text>
            <Text style={ styles.text }>Wait for error</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testSignMessageWithoutMsg(key) {
    try {
        const keys = await keyPair64(); //KEYTYPE_EC is default now
        const privateKey64 = keys.private64;
        const message = "";
        const signed = await secureMessageSign64(message, privateKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without message</Text>
            <Text style={ styles.text }>Wait for error</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Signed message: { signed }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64 without message</Text>
            <Text style={ styles.text }>Wait for error</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testSignMessageIncorrectPrivateKey(key) {
    try {
        const privateKey64 = 'UkVDMgAAC0LB0EAAOMHtC77gM48DE7EVUpYBA5qgQ5sOYJAM9x5gXJeGM4U';
        const message = "Hello, Themis!";
        const signed = await secureMessageSign64(message, privateKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect private key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Signed message: { signed }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect private key</Text>
            <Text style={ styles.resolved }>Test passed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testSignMessageIncorrectPublicKey(key) {
    try {
        const privateKey64 = 'UkVDMgAAAC3OLjUFAHjE6+a5HnVK/+nb6UndRG1YvoySGfxlMPBwsplmZlSj';
        const publicKey64 = 'VUVDMgAAAC0TExZJA/tLPUTZfrfoRbEYuc6bhMVkOijXmAGsdrS0IKNQ26E2';
        const message = "Hello, Themis!";
        const signed = await secureMessageSign64(message, privateKey64, publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>A signed message is expected because the public key doesn't matter</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Signed message: { signed }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageSign64</Text>
            <Text style={ styles.text }>A signed message is expected because the public key doesn't matter</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}
