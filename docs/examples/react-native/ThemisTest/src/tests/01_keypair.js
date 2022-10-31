import React from 'react';
import { View, Text } from 'react-native';
import { keyPair64, KEYTYPE_EC } from "react-native-themis";
import { styles } from "../styles";

// Async Themis keyPair64 example. It resolves with asymmetric keypair anyway.
// Always return base64 encoded strings

export async function testKeyPair(key) {
    try {
        pair = await keyPair64(KEYTYPE_EC);
        const component = <View key={ key }>
            <Text style={ styles.title }>Key pair</Text>
            <Text style={ styles.text }>Private</Text>
            <Text style={ styles.resolved }> { pair.private64 }</Text>
            <Text style={ styles.text }>Public</Text>
            <Text style={ styles.resolved }> { pair.public64 }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>Key pair</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}


