/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 * @flow strict-local
 */

import React, {useState, useEffect} from 'react';
import type {ReactNode} from 'react';
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

import { Buffer } from 'buffer';

import {
  keyPair64,
  symmetricKey64,
  secureCellSealWithSymmetricKeyEncrypt64,
  secureCellSealWithSymmetricKeyDecrypt64,
  secureCellSealWithPassphraseEncrypt64,
  secureCellSealWithPassphraseDecrypt64,
  secureCellTokenProtectEncrypt64,
  secureCellTokenProtectDecrypt64,
  secureCellContextImprintEncrypt64,
  secureCellContextImprintDecrypt64,
  secureMessageSign64,
  secureMessageVerify64,
  secureMessageEncrypt64,
  secureMessageDecrypt64,
  string64,
  comparatorInit64,
  comparatorBegin,
  comparatorProceed64,
  KEYTYPE_EC,
  KEYTYPE_RSA,
  COMPARATOR_NOT_READY,
  COMPARATOR_NOT_MATCH,
  COMPARATOR_MATCH,
  COMPARATOR_ERROR
} from 'react-native-themis';

type SectionProps = {
  children: ReactNode;
  title: string;
};

const Section: React.FC<SectionProps> = ({ children, title }) => {
  const isDarkMode = useColorScheme() === 'dark';
  return (
    <View style={styles.sectionContainer}>
      <Text
        style={[
          styles.sectionTitle,
          {
            color: isDarkMode ? Colors.white : Colors.black,
          },
        ]}>
        {title}
      </Text>
      <Text
        style={[
          styles.sectionDescription,
          {
            color: isDarkMode ? Colors.light : Colors.dark,
          },
        ]}>
        {children}
      </Text>
    </View>
  );
};

const App: React.FC = () => {
  const isDarkMode = useColorScheme() === 'dark';

  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  const [privateKey, setPrivateKey] = useState<string>('');
  const [publicKey, setPublicKey] = useState<string>('');
  const [masterKey, setMasterKey] = useState<string>('');
  const [encryptedWithKey, setEncryptedWithKey] = useState<string>('');
  const [encryptedWithPassphrase, setEncryptedWithPassphrase] = useState<string>('');
  const [encryptedWithTokenProtect, setEncryptedWithTokenProtect] = useState<string>('');
  const [tokenProtect, setTokenProtect] = useState<string>('');
  const [encryptedWithContextImprint, setEncryptedWithContextImprint] = useState<string>('');
  const [signedSecureMessage, setSignedSecureMessage] = useState<string>('');
  const [encryptedSecureMessage, setEncryptedSecureMessage] = useState<string>('');

  const plaintext = "Hello, Themis!";
  const context = "Themis context";
  const passphrase = "Passphrase for Themis!";

  useEffect(() => {

    // Async Themis keyPair64 example. It resolves with asymmetric key pair anyway.
    // Always return base64 encoded strings
    keyPair64(KEYTYPE_EC)
      .then((pair: any) => {
        console.log("pair private", pair.private64);
        console.log("pair public", pair.public64);
        setPrivateKey(pair.private64);
        setPublicKey(pair.public64);
      });

    // Symmetric key => promise => encryption => promise => decryption
    symmetricKey64()
      .then((key64) => {
        secureCellSealWithSymmetricKeyEncrypt64(key64, plaintext)
          .then((encrypted64) => {
            secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64)
              .then((decrypted) => {
                console.log("Decrypted with the key:", decrypted);
              })
              .catch((error: any) => {
                console.log(error);
              });
          })
          .catch((error: any) => {
            console.log(error);
          });
      });

    // the same, but with await
    (async () => {
      const key64 = await symmetricKey64();
      setMasterKey(key64);
      const encrypted64 = await secureCellSealWithSymmetricKeyEncrypt64(key64, plaintext, context);
      setEncryptedWithKey(encrypted64);
      const decrypted = await secureCellSealWithSymmetricKeyDecrypt64(key64, encrypted64, context);
      console.log("Async decrypted:", decrypted);
    })();


    // secure seal with passphrase encrypt and decrypt
    secureCellSealWithPassphraseEncrypt64(passphrase, plaintext, context)
      .then((encrypted64) => {
        setEncryptedWithPassphrase(encrypted64);
        secureCellSealWithPassphraseDecrypt64(passphrase, encrypted64, context)
          .then((decrypted) => {
            console.log("Decrypted with the passphrase:", decrypted);
          })
          .catch((error: any) => {
            console.log(error);
          });
      })
      .catch((error: any) => {
        console.log(error);
      });

    // token protect
    symmetricKey64()
      .then((key64) => {
        secureCellTokenProtectEncrypt64(key64, plaintext, context)
          .then((encrypted: any) => {
            setEncryptedWithTokenProtect(encrypted.encrypted64);
            setTokenProtect(encrypted.token64);
            secureCellTokenProtectDecrypt64(key64, encrypted.encrypted64, encrypted.token64, context)
              .then((decrypted) => {
                console.log("Decrypted with token protect:", decrypted);
              })
              .catch((error: any) => {
                console.log(error);
              });
          })
          .catch((error: any) => {
            console.log(error);
          });
      });

    // context imprint

    symmetricKey64()
      .then((key64) => {
        secureCellContextImprintEncrypt64(key64, plaintext, context)
          .then((encrypted64: any) => {
            setEncryptedWithContextImprint(encrypted64);
            secureCellContextImprintDecrypt64(key64, encrypted64, context)
              .then((decrypted) => {
                console.log("Decrypted with context imprint:", decrypted);
              })
              .catch((error: any) => {
                console.log(error);
              });
          })
          .catch((error: any) => {
            console.log(error);
          });
      });


    keyPair64(KEYTYPE_EC)
      .then((pair: any) => {
        console.log("pair private", pair.private64);
        console.log("pair public", pair.public64);
        secureMessageSign64(plaintext, pair.private64, "")
          .then((signed64: any) => {
            setSignedSecureMessage(signed64);
            secureMessageVerify64(signed64, "", pair.public64)
              .then((verified) => {
                console.log("Secure Message verified text:", verified);
              })
              .catch((error: any) => {
                console.log(error);
              });
          })
          .catch((error: any) => {
            console.log(error);
          });
      });

    (async () => {
      const aliceKeyPair = await keyPair64(KEYTYPE_RSA);
      const bobKeyPair = await keyPair64(KEYTYPE_RSA);
      const encrypted64 = await secureMessageEncrypt64(plaintext, aliceKeyPair.private64, bobKeyPair.public64);
      setEncryptedSecureMessage(encrypted64);
      const decrypted = await secureMessageDecrypt64(encrypted64, bobKeyPair.private64, aliceKeyPair.public64);
      console.log("Decrypted secure message:", decrypted);
    })();

    const lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam aliquam dictum tellus, eget malesuada lectus scelerisque non. Curabitur at nibh vestibulum, sagittis felis non, convallis magna. Vestibulum fringilla a urna quis facilisis. Etiam rhoncus hendrerit diam, at elementum nisi condimentum dignissim. Nam eleifend libero accumsan pharetra cursus. Phasellus eget nulla pellentesque, elementum dui eu, malesuada mi. Quisque dolor augue, mattis ut mauris id, sollicitudin fermentum elit. Suspendisse nulla velit, tincidunt a viverra interdum, cursus vitae nunc. Proin turpis ante, consectetur nec nisl consequat, rhoncus congue risus. Sed mattis tempus mi quis hendrerit. Nullam sit amet arcu dapibus, dignissim risus nec, tempus metus. Morbi condimentum sagittis metus eu eleifend. Duis scelerisque eu tellus non porta. Duis viverra tincidunt congue.";
    const lorem64 = string64(lorem);

    async function proceedCompare(data64: string, serverID: string, clientID: string): Promise<number> {
      console.log("Before server part of comparator", serverID, data64);
      const serverResult = await comparatorProceed64(serverID, data64);
      console.log("Server proceeded with result:", serverResult);
      console.log("Before client part of comparator", clientID, serverResult.data64);
      const clientResult = await comparatorProceed64(clientID, serverResult.data64);
      console.log("Client proceeded with result:", clientResult);
      if (clientResult.status == COMPARATOR_NOT_READY ||
        serverResult.status == COMPARATOR_NOT_READY) {
        return Promise.resolve(proceedCompare(clientResult.data64, serverID, clientID));
      } else {
        if (clientResult.status == COMPARATOR_MATCH) {
          return Promise.resolve(COMPARATOR_MATCH);
        } else {
          return Promise.resolve(COMPARATOR_NOT_MATCH);
        }
      }
    }

    (async () => {
      try {
        const server = await comparatorInit64(lorem64);
        const client = await comparatorInit64(lorem64);
        const data = await comparatorBegin(client);
        const result = await proceedCompare(data, server, client);
        console.log("Comparator done with status:", result);
      } catch (error) {
        console.log(error);
      }

    })();

    // New Test from Java
    const javaTest64 = 'AAEBQQwAAAAQAAAAEwAAABYAAAADLKbW7aho9FeDMqy0iRukfGpZAGnEcYqpfAX2QA0DABAAk2Vk0xoTQzLLUoujO2L39JcNyl6AMSHF3o/V9itzchX/7PA=';
    secureCellSealWithPassphraseDecrypt64('a password', javaTest64, 'Java context')
      .then((decrypted) => {
        console.log("Decrypted64 with passphrase from Java:", decrypted);
      });

    // Test with symmetric keys from Java
    const javaSymKey = 'Z7BY52XyuM0ss1Ma/O+4Fy9mal5lvMDRyK2nZpuA4U0=';
    const javaEnc = 'AAEBQAwAAAAQAAAAEwAAAFQFGDh5JAJFNzoXDi3SGSWqNfccYlWc/RiBf3QL8YtaT3gRlj8whlx2umdrsFE1';
    secureCellSealWithSymmetricKeyDecrypt64(javaSymKey, javaEnc, 'Java context')
      .then((decrypted) => {
        console.log("Decrypted64 with symmetric key from Java:", decrypted);
      });

    // Test from Python
    const buffp = 'AAEBQQwAAAAQAAAAEAAAABYAAACqaCdlWERyzPeFEWJbPP+fqksXKvYAUVWSb4caQA0DABAApYwgn2Kt+WKXtP3X3lL0lJ5gA4+b+vo7VWiJjmtf4d8=';
    secureCellSealWithPassphraseDecrypt64('a password', buffp, 'Python context')
      .then((decrypted) => {
        console.log("Decrypted64 with passphrase from Python:", decrypted);
      });

    // Test with symmetric keys from Python
    const pySymKey = 'pGFN54NKRpF53bpf5YtO5PmDVT9N/Ep9Hm0N0w8UXnU=';
    const pyEnc = 'AAEBQAwAAAAQAAAAEAAAAEIBP7ow0hZg7j1mv0P+S9mYC+H0AJ172CiBOTj1Sqlxzz9wboZCtTnnNwi9';
    secureCellSealWithSymmetricKeyDecrypt64(pySymKey, pyEnc, 'Python context')
      .then((decrypted) => {
        console.log("Decrypted64 with symmetric key from Python:", decrypted);
      });

    // Test from Obj-C
    const buff = 'AAEBQQwAAAAQAAAAFAAAABYAAAASUGtcrR36rVjhVPkbJRNFOXfP5DrmL0g41K3kQA0DABAAwDRJ9q4LtOtf2D2jRkZcIgy8rQU61NHu69wFdvKAfNPL1OdU';
    secureCellSealWithPassphraseDecrypt64('test', buff, 'test')
      .then((decrypted) => {
        console.log("Decrypted64 with passphrase from ObjC:", decrypted);
      });

    // Test with symmetric keys from Obj-C
    const objcSymKey = 'B+L00zvIOBh/qSTI0hAE2S2unSHhS+0EHspVCToi3oA=';
    const objcEnc = 'AAEBQAwAAAAQAAAAEAAAACd7hM2MWqiWu5SDNtzvgjcvN3PBY+VBg9kJQB8R1cwcXfOy8sY75+3pRCe0';
    secureCellSealWithSymmetricKeyDecrypt64(objcSymKey, objcEnc, 'test')
      .then((decrypted) => {
        console.log("Decrypted64 with symmetric key from ObjC:", decrypted);
      });


  }, []);

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
          <Section title="Asymmetric Keys">
            <View>
              <Text>Private key:</Text>
              <Text numberOfLines={ 3 } style={ styles.blob }>{ privateKey }</Text>
            </View>
            <View style={ { paddingTop: 10 } }>
              <Text>Public key:</Text>
              <Text numberOfLines={ 3 } style={ styles.blob }>{ publicKey }</Text>
            </View>
          </Section>
          <Section title="Symmetric Key">
            <View>
              <Text>Symmetric key:</Text>
              <Text numberOfLines={ 3 } style={ styles.blob }>
                { masterKey }
              </Text>
            </View>
          </Section>
          <Section title="Encrypted with the key">
            <Text numberOfLines={ 3 } style={ styles.blob }>
              { encryptedWithKey }
            </Text>
          </Section>
          <Section title="Encrypted with the passphrase">
            <Text numberOfLines={ 3 } style={ styles.blob }>
              { encryptedWithPassphrase }
            </Text>
          </Section>
          <Section title="Encrypted with token protect">
            <View>
              <Text>Encrypted with the same length:</Text>
              <Text numberOfLines={ 3 } style={ styles.blob }>
                { encryptedWithTokenProtect }
              </Text>
            </View>
            <View style={ { paddingTop: 10 } }>
              <Text>Token</Text>
              <Text numberOfLines={ 3 } style={ styles.blob }>
                { tokenProtect }
              </Text>
            </View>
          </Section>
          <Section title="Encrypted with context imprint">
            <Text numberOfLines={ 3 } style={ styles.blob }>
              { encryptedWithContextImprint }
            </Text>
          </Section>
          <Section title="Signed secure message">
            <Text numberOfLines={ 3 } style={ styles.blob }>
              { signedSecureMessage }
            </Text>
          </Section>
          <Section title="Encrypted secure message">
            <Text numberOfLines={ 3 } style={ styles.blob }>
              { encryptedSecureMessage }
            </Text>
          </Section>
        </View>
      </ScrollView>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  sectionContainer: {
    marginTop: 32,
    paddingHorizontal: 24,
  },
  sectionTitle: {
    fontSize: 24,
    fontWeight: '600',
  },
  sectionDescription: {
    marginTop: 8,
    fontSize: 18,
    fontWeight: '400',
  },
  highlight: {
    fontWeight: '700',
  },
  blob: {
    fontFamily: "Courier",
    backgroundColor: 'lightgray',
    borderRadius: 5,
    padding: 5,
  },
});

export default App;
