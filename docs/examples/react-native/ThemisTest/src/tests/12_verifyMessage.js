import React from "react";
import { Text, View } from "react-native";
import { secureMessageVerify64 } from "react-native-themis";
import { styles } from "../styles";

export async function testVerifyMessage(key) {
    try {
        const privateKey64 = "UkVDMgAAAC36wNC7AAyZVdHhdB6ODitCszw7xZQy0FoPRLrNwXJ9KyIy6i1g";
        const publicKey64 = "VUVDMgAAAC1wW3YTAsPE7kDJjwDXIBdXZwddni/PqwGk15BNDKuXJw2iHDoR";
        const signed64 = "ICYEJg4AAABHAAAASGVsbG8sIFRoZW1pcyEwRQIgR/brzPiPPQRM2OxqJu+QlCLUATWhzCcUp50qsuARissCIQD0zrHKcZNn8Ao88JQsCmES2eoDZ8wisyjfg1DrrDeT3A==";
        const verified = await secureMessageVerify64(signed64, "", publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for verified message</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Verified message: { verified }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for verified message</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testVerifyMessageWithoutPrivateKey(key) {
    try {
        const publicKey64 = "VUVDMgAAAC1wW3YTAsPE7kDJjwDXIBdXZwddni/PqwGk15BNDKuXJw2iHDoR";
        const signed64 = "ICYEJg4AAABHAAAASGVsbG8sIFRoZW1pcyEwRQIgR/brzPiPPQRM2OxqJu+QlCLUATWhzCcUp50qsuARissCIQD0zrHKcZNn8Ao88JQsCmES2eoDZ8wisyjfg1DrrDeT3A==";
        const verified = await secureMessageVerify64(signed64, "", publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for verified message without private key</Text>
            <Text style={ styles.resolved }>Test passed with result</Text>
            <Text style={ styles.resolved }>Verified message: { verified }</Text>
        </View>;
        return Promise.resolve(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for verified message without private key</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}

export async function testVerifyMessageWithIncorrectPublicKey(key) {
    try {
        const publicKey64 = "VUVDMgAAAC1VlAYyAh742Ur7CbzIZH0LXpo1iplumKuNDMYhcqnaAU8YnoLL";
        const signed64 = "ICYEJg4AAABHAAAASGVsbG8sIFRoZW1pcyEwRQIgR/brzPiPPQRM2OxqJu+QlCLUATWhzCcUp50qsuARissCIQD0zrHKcZNn8Ao88JQsCmES2eoDZ8wisyjfg1DrrDeT3A==";
        const verified = await secureMessageVerify64(signed64, "", publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect public key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Verified message: { verified }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of incorrect public key</Text>
            <Text style={ styles.resolved }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testVerifyMessageWithEmptyPublicKey(key) {
    try {
        const publicKey64 = "";
        const signed64 = "ICYEJg4AAABHAAAASGVsbG8sIFRoZW1pcyEwRQIgR/brzPiPPQRM2OxqJu+QlCLUATWhzCcUp50qsuARissCIQD0zrHKcZNn8Ao88JQsCmES2eoDZ8wisyjfg1DrrDeT3A==";
        const verified = await secureMessageVerify64(signed64, "", publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of empty public key</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Verified message: { verified }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of empty public key</Text>
            <Text style={ styles.resolved }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}

export async function testVerifyMessageWithEmptyMessage(key) {
    try {
        const publicKey64 = "VUVDMgAAAC1VlAYyAh742Ur7CbzIZH0LXpo1iplumKuNDMYhcqnaAU8YnoLL";
        const signed64 = "";
        const verified = await secureMessageVerify64(signed64, "", publicKey64);
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of empty message</Text>
            <Text style={ styles.rejected }>Test failed with result</Text>
            <Text style={ styles.rejected }>Verified message: { verified }</Text>
        </View>;
        return Promise.reject(component);
    }
    catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>secureMessageVerify64</Text>
            <Text style={ styles.text }>Wait for error because of empty message</Text>
            <Text style={ styles.resolved }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.resolve(component);
    }
}
