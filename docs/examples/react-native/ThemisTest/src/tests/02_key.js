import React from 'react';
import { View, Text } from 'react-native';
import { symmetricKey64 } from "react-native-themis";
import { styles } from "../styles";

// Async Themis symmetricKey64 example. It resolves with symmetric key anyway.
// Always return base64 encoded strings

export async function testSymmetricKey(key) {
    try {
        const key64 = await symmetricKey64();
        const component = <View key={ key }>
            <Text style={ styles.title }>Symmetric key</Text>
            <Text style={ styles.text }>Key</Text>
            <Text style={ styles.resolved }> { key64 }</Text>
        </View>;
        return Promise.resolve(component);
    } catch (e) {
        const component = <View key={ key }>
            <Text style={ styles.title }>Symmetric key</Text>
            <Text style={ styles.rejected }>Test failed with error: { e.message }</Text>
        </View>;
        return Promise.reject(component);
    }
}