/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 * @flow strict-local
 */

import React, { useState, useEffect } from 'react';
import type { Node } from 'react';
import {
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  useColorScheme,
  View,
} from 'react-native';

import {
  Colors,
} from 'react-native/Libraries/NewAppScreen';

import { styles } from './src/styles';

import { testKeyPair } from './src/tests/01_keypair';
import { testSymmetricKey } from './src/tests/02_key';
import {
  testEncryptWithKey,
  testEncryptWithKeyFailEmptyKey,
  testEncryptWithKeyFailEmptyText,
  testEncryptWithWrongKey
} from './src/tests/03_encryptWithKey';

import {
  testDecryptWithKey,
  testDecryptWithKeyFailEmptyKey,
  testDecryptWithKeyFailEmptyEncrypted,
  testDecryptWithWrongKey,
} from './src/tests/04_decryptWithKey';

import {
  testEncryptWithPassphrase,
  testEncryptWithPassphraseFailEmptyPassphrase,
  testEncryptWithPassphraseFailEmptyMessage,
  testEncryptWithPassphraseSuccessWithContext,
} from './src/tests/05_encryptWithPassphrase';

import {
  testDecryptWithPassphrase,
  testDecryptWithPassphraseWithoutContext,
  testDecryptWithPassphraseWithWrongContext,
  testDecryptWithPassphraseWithWrongPassphrase,
  testDecryptWithPassphraseWithWrongMessage,
} from './src/tests/06_decryptWithPassphrase';

import {
  testEncryptToken,
  testEncryptTokenFailEmptyKey,
  testEncryptTokenFailEmptyMessage,
  testEncryptTokenWithContextEmpty,
} from './src/tests/07_encryptToken';

import {
  testDecryptToken,
  testDecryptTokenAnotherKey,
  testDecryptTokenEmptyKey,
  testDecryptTokenWrongKey,
  testDecryptTokenEmptyEncrypted,
  testDecryptTokenAnotherEncrypted,
  testDecryptTokenEmptyToken,
  testDecryptTokenWrongToken,
  testDecryptTokenAnotherToken,
  testDecryptTokenEmptyContext,
  testDecryptTokenOtherContext,
} from './src/tests/08_decryptToken';

import {
  testEncryptCtxImprint,
  testEncryptCtxImprintFailEmptyKey,
  testEncryptCtxImprintFailEmptyMessage,
  testEncryptCtxImprintFailEmptyContext,
  testEncryptCtxImprintFailWrongKey,
} from './src/tests/09_encryptCtxImprint';

import {
  testDecryptCtxImprint,
  testDecryptCtxImprintAnotherKey,
  testDecryptCtxImprintAnotherContext,
  testDecryptCtxImprintEmptyKey,
  testDecryptCtxImprintEmptyMessage,
  testDecryptCtxImprintEmptyContext,
  testDecryptCtxImprintWrongKey,
  testDecryptCtxImprintIncorrectKey,
} from './src/tests/10_decryptCtxImprint';

import {
  testSignMessage,
  testSignMessageWithoutPublicKey,
  testSignMessageWithoutPrivateKey,
  testSignMessageWithoutMsg,
  testSignMessageIncorrectPrivateKey,
  testSignMessageIncorrectPublicKey,
} from './src/tests/11_signMessage';

import {
  testVerifyMessage,
  testVerifyMessageWithoutPrivateKey,
  testVerifyMessageWithIncorrectPublicKey,
  testVerifyMessageWithEmptyPublicKey,
  testVerifyMessageWithEmptyMessage,
} from './src/tests/12_verifyMessage';

import {
  testEncryptMessage,
  testEncryptMessageWithOnePair,
  testEncryptMessageWithEmptyPublicKey,
} from './src/tests/13_encryptMessage';

import {
  testDecryptMessage,
  testDecryptMessageIncorrectKeys,
  testDecryptMessageNotBase64Params,
} from './src/tests/14_decryptMessage';

import {
  testBase64_1,
  testBase64_2,
  testBase64_3,
  testBase64_4,
  testBase64_5,
  testBase64_6,
} from './src/tests/00_base64';

const Section = ({ children, title }): Node => {
  const isDarkMode = useColorScheme() === 'dark';
  return (
    <View style={ styles.sectionContainer }>
      <Text
        style={ [
          styles.sectionTitle,
          {
            color: isDarkMode ? Colors.white : Colors.black,
          },
        ] }>
        { title }
      </Text>
      <Text
        style={ [
          styles.sectionDescription,
          {
            color: isDarkMode ? Colors.light : Colors.dark,
          },
        ] }>
        { children }
      </Text>
    </View>
  );
};

const App: () => Node = () => {
  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  const [collectedResults, setCollectedResults] = useState({});

  const [resolved, setResolved] = useState(0);
  const [rejected, setRejected] = useState(0);

  const tests = {
    "91_testBase64_1": { execute: testBase64_1 },
    "92_testBase64_2": { execute: testBase64_2 },
    "93_testBase64_3": { execute: testBase64_3 },
    "94_testBase64_4": { execute: testBase64_4 },
    "95_testBase64_5": { execute: testBase64_5 },
    "96_testBase64_6": { execute: testBase64_6 },
    "01_testKeyPair": { execute: testKeyPair },
    "02_testSymmetricKey": { execute: testSymmetricKey },
    "03_testEncryptWithKey": { execute: testEncryptWithKey },
    "04_testEncryptWithKeyFailEmptyKey": { execute: testEncryptWithKeyFailEmptyKey },
    "05_testEncryptWithKeyFailEmptyText": { execute: testEncryptWithKeyFailEmptyText },
    "06_testEncryptWithWrongKey": { execute: testEncryptWithWrongKey },
    "07_testDecryptWithKey": { execute: testDecryptWithKey },
    "08_testDecryptWithKeyFailEmptyKey": { execute: testDecryptWithKeyFailEmptyKey },
    "09_testDecryptWithKeyFailEmptyEncrypted": { execute: testDecryptWithKeyFailEmptyEncrypted },
    "10_testDecryptWithWrongKey": { execute: testDecryptWithWrongKey },
    "11_testEncryptWithPassphrase": { execute: testEncryptWithPassphrase },
    "12_testEncryptWithPassphraseFailEmptyPassphrase": { execute: testEncryptWithPassphraseFailEmptyPassphrase },
    "13_testEncryptWithPassphraseFailEmptyMessage": { execute: testEncryptWithPassphraseFailEmptyMessage },
    "14_testEncryptWithPassphraseSuccessWithContext": { execute: testEncryptWithPassphraseSuccessWithContext },
    "15_testDecryptWithPassphrase": { execute: testDecryptWithPassphrase },
    "16_testDecryptWithPassphraseWithoutContext": { execute: testDecryptWithPassphraseWithoutContext },
    "17_testDecryptWithPassphraseWithWrongContext": { execute: testDecryptWithPassphraseWithWrongContext },
    "18_testDecryptWithPassphraseWithWrongPassphrase": { execute: testDecryptWithPassphraseWithWrongPassphrase },
    "19_testDecryptWithPassphraseWithWrongMessage": { execute: testDecryptWithPassphraseWithWrongMessage },
    "20_testEncryptToken": { execute: testEncryptToken },
    "21_testEncryptTokenFailEmptyKey": { execute: testEncryptTokenFailEmptyKey },
    "22_testEncryptTokenFailEmptyMessage": { execute: testEncryptTokenFailEmptyMessage },
    "23_testEncryptTokenWithContextEmpty": { execute: testEncryptTokenWithContextEmpty },
    "24_testDecryptToken": { execute: testDecryptToken },
    "25_testDecryptTokenAnotherKey": { execute: testDecryptTokenAnotherKey },
    "26_testDecryptTokenEmptyKey": { execute: testDecryptTokenEmptyKey },
    "27_testDecryptTokenWrongKey": { execute: testDecryptTokenWrongKey },
    "28_testDecryptTokenEmptyEncrypted": { execute: testDecryptTokenEmptyEncrypted },
    "29_testDecryptTokenAnotherEncrypted": { execute: testDecryptTokenAnotherEncrypted },
    "30_testDecryptTokenEmptyToken": { execute: testDecryptTokenEmptyToken },
    "31_testDecryptTokenWrongToken": { execute: testDecryptTokenWrongToken },
    "32_testDecryptTokenAnotherToken": { execute: testDecryptTokenAnotherToken },
    "33_testDecryptTokenEmptyContext": { execute: testDecryptTokenEmptyContext },
    "34_testDecryptTokenOtherContext": { execute: testDecryptTokenOtherContext },
    "35_testEncryptCtxImprint": { execute: testEncryptCtxImprint },
    "36_testEncryptCtxImprintFailEmptyKey": { execute: testEncryptCtxImprintFailEmptyKey },
    "37_testEncryptCtxImprintFailEmptyMessage": { execute: testEncryptCtxImprintFailEmptyMessage },
    "38_testEncryptCtxImprintFailEmptyContext": { execute: testEncryptCtxImprintFailEmptyContext },
    "39_testEncryptCtxImprintFailWrongKey": { execute: testEncryptCtxImprintFailWrongKey },
    "40_testDecryptCtxImprint": { execute: testDecryptCtxImprint },
    "41_testDecryptCtxImprintAnotherKey": { execute: testDecryptCtxImprintAnotherKey },
    "42_testDecryptCtxImprintAnotherContext": { execute: testDecryptCtxImprintAnotherContext },
    "43_testDecryptCtxImprintEmptyKey": { execute: testDecryptCtxImprintEmptyKey },
    "44_testDecryptCtxImprintEmptyMessage": { execute: testDecryptCtxImprintEmptyMessage },
    "45_testDecryptCtxImprintEmptyContext": { execute: testDecryptCtxImprintEmptyContext },
    "46_testDecryptCtxImprintWrongKey": { execute: testDecryptCtxImprintWrongKey },
    "47_testDecryptCtxImprintIncorrectKey": { execute: testDecryptCtxImprintIncorrectKey },
    "49_testSignMessage": { execute: testSignMessage },
    "50_testSignMessageWithoutPublicKey": { execute: testSignMessageWithoutPublicKey },
    "51_testSignMessageWithoutPrivateKey": { execute: testSignMessageWithoutPrivateKey },
    "52_testSignMessageWithoutMsg": { execute: testSignMessageWithoutMsg },
    "53_testSignMessageIncorrectPrivateKey": { execute: testSignMessageIncorrectPrivateKey },
    "54_testSignMessageIncorrectPublicKey": { execute: testSignMessageIncorrectPublicKey },
    "55_testVerifyMessage": { execute: testVerifyMessage },
    "56_testVerifyMessageWithoutPrivateKey": { execute: testVerifyMessageWithoutPrivateKey },
    "57_testVerifyMessageWithIncorrectPublicKey": { execute: testVerifyMessageWithIncorrectPublicKey },
    "58_testVerifyMessageWithEmptyPublicKey": { execute: testVerifyMessageWithEmptyPublicKey },
    "59_testVerifyMessageWithEmptyMessage": { execute: testVerifyMessageWithEmptyMessage },
    "60_testEncryptMessage": { execute: testEncryptMessage },
    "61_testEncryptMessageWithOnePair": { execute: testEncryptMessageWithOnePair },
    "62_testEncryptMessageWithEmptyPublicKey": { execute: testEncryptMessageWithEmptyPublicKey },
    "63_testDecryptMessage": { execute: testDecryptMessage },
    "64_testDecryptMessageIncorrectKeys": { execute: testDecryptMessageIncorrectKeys },
    "65_testDecryptMessageNotBase64Params": { execute: testDecryptMessageNotBase64Params },
  };

  useEffect(() => {

    for (const [key, value] of Object.entries(tests)) {
      value.execute(key).then(result => {
        setResolved(resolved => resolved + 1);
        if (result && result.type && result.type.render) {
          setCollectedResults(collectedResults => ({ ...collectedResults, [key]: result }));
        } else {
          setCollectedResults(collectedResults => ({ ...collectedResults, [key]: <Text key={ key }>{ result }</Text> }));
        }
      }).catch(error => {
        console.log(key, "===> ", error);
        setRejected(rejected => rejected + 1);
        if (error && error.type && error.type.render) {
          setCollectedResults(collectedResults => ({ ...collectedResults, [key]: error }));
        } else {
          setCollectedResults(collectedResults => ({ ...collectedResults, [key]: <Text style={ styles.rejected } key={ key }>{ error.message }</Text> }));
        }
      });
    }
  }, []);

  const display = (tests) => {
    const keys = Object.keys(tests).sort();
    return keys.map(key => tests[key]);
  };

  return (
    <SafeAreaView style={ backgroundStyle }>
      <StatusBar barStyle={ isDarkMode ? 'light-content' : 'dark-content' } />
      <ScrollView
        contentInsetAdjustmentBehavior="automatic"
        style={ backgroundStyle }>
        <View
          style={ {
            backgroundColor: isDarkMode ? Colors.black : Colors.white,
          } }>
          <Section title="Status of tests">
            <View style={ styles.row }>
              <View style={ styles.col50 }>
                <Text style={ styles.resolved }>
                  Resolved: { resolved }
                </Text>
              </View>
              <View style={ styles.col50 }>
                <Text style={ styles.rejected }>
                  Rejected: { rejected }
                </Text>
              </View>
            </View>
          </Section>
          <View style={ { paddingHorizontal: 24 } }>
            { display(collectedResults) }
          </View>
        </View>
      </ScrollView >
    </SafeAreaView >
  );
};

export default App;
