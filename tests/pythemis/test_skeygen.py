# coding: utf-8
import unittest
from pythemis.skeygen import GenerateKeyPair, KEY_PAIR_TYPE
from pythemis.skeygen import GenerateSymmetricKey
from pythemis.exception import ThemisError


class GenerateKeyPairTest(unittest.TestCase):
    def test_generator(self):
        ec_keypair = GenerateKeyPair(KEY_PAIR_TYPE.EC)
        self.assertTrue(ec_keypair.export_private_key())
        self.assertTrue(ec_keypair.export_public_key())

        rsa_keypair = GenerateKeyPair(KEY_PAIR_TYPE.RSA)
        self.assertTrue(rsa_keypair.export_private_key())
        self.assertTrue(rsa_keypair.export_public_key())

        self.assertNotEqual(rsa_keypair.export_private_key(),
                            ec_keypair.export_private_key())

        self.assertNotEqual(rsa_keypair.export_public_key(),
                            ec_keypair.export_public_key())

        with self.assertRaises(ThemisError):
            GenerateKeyPair("incorrect algorithm")


class GenerateSymmetricKeyTest(unittest.TestCase):
    def test_generator(self):
        default_length = 32
        key = GenerateSymmetricKey()
        self.assertEqual(len(key), default_length)
