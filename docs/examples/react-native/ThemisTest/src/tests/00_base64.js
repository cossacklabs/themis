import React from "react";
import { View, Text } from "react-native";
import { Buffer } from "buffer";
import { styles } from "../styles";

const regex = /^([0-9a-zA-Z+/]{4})*(([0-9a-zA-Z+/]{2}==)|([0-9a-zA-Z+/]{3}=))?$/;

export async function testBase64_1(key) {
    const b64value = "aGVsbG8gd29ybGQ=";
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("utf8");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 1</Text>
            <Text style={ styles.resolved }>Test passed with result: { value }</Text>
        </View>;
        return Promise.resolve(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 1</Text>
        <Text style={ styles.rejected }>Test failed: value is not base64 encoded</Text>
    </View>;
    return Promise.reject(component);
}

export async function testBase64_2(key) {
    const b64value = "aGVsbG8gd2ybGQ=";
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("utf8");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 2</Text>
            <Text style={ styles.rejected }>Test rejected with result: { value }</Text>
        </View>;
        return Promise.reject(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 2</Text>
        <Text style={ styles.resolved }>Test passed value is not base64 encoded</Text>
    </View>;
    return Promise.resolve(component);
}

export async function testBase64_3(key) {
    const b64value = "hello, world!=";
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("utf8");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 3</Text>
            <Text style={ styles.rejected }>Test failed with result: { value }</Text>
        </View>;
        return Promise.reject(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 3</Text>
        <Text style={ styles.resolved }>Test passed: value is not base64 encoded</Text>
    </View>;
    return Promise.resolve(component);
}

export async function testBase64_4(key) {
    const b64value = "hello, world!";
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("utf8");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 4</Text>
            <Text style={ styles.rejected }>Test failed with result: { value }</Text>
        </View>;
        return Promise.reject(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 4</Text>
        <Text style={ styles.resolved }>Test passed: value is not base64 encoded</Text>
    </View>;
    return Promise.resolve(component);
}
//bY2cqbOEaOI8Yzou5RszLkQ5zChShaMP6y43LsSAD1w=

export async function testBase64_5(key) {
    const b64value = "bY2cqbOEaOI8Yzou5RszLkQ5zChShaMP6y43LsSAD1w="; // 32 bytes key in base64
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("hex");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 5</Text>
            <Text style={ styles.resolved }>Test passed with result: { value }</Text>
        </View>;
        return Promise.resolve(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 5</Text>
        <Text style={ styles.rejected }>Test failed: value is not base64 encoded</Text>
    </View>;
    return Promise.reject(component);
}

export async function testBase64_6(key) {
    const b64value = "UkVDMgAAAC2nkbvHAOx8Dy+f4wnJcetfpa1979hRg/VRIfoN2CoLsBhJZ7X";
    const result = regex.test(b64value);
    if (result) {
        const value = Buffer.from(b64value, "base64").toString("hex");
        const component = <View key={ key }>
            <Text style={ styles.title }>Base64 decode test 6</Text>
            <Text style={ styles.rejected }>Test failed with result: { value }</Text>
        </View>;
        return Promise.reject(component);
    }
    const component = <View key={ key }>
        <Text style={ styles.title }>Base64 decode test 6</Text>
        <Text style={ styles.resolved }>Test passed: value is not base64 encoded</Text>
    </View>;
    return Promise.resolve(component);
}

