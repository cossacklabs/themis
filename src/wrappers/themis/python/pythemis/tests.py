# coding: utf-8
import unittest

from .ssession import SSession, MemoryTransport, SimpleMemoryTransport
from .smessage import SMessage, ssign, sverify
from .skeygen import GenerateKeyPair, KEY_PAIR_TYPE
from .scell import SCellSeal, SCellContextImprint, SCellTokenProtect
from .scomparator import SComparator, SCOMPARATOR_CODES
from .exception import ThemisError


class SCellSealTest(unittest.TestCase):
    def test_encrypt_decrypt(self):
        data = b'some data'
        key = b'some key'
        scell1 = SCellSeal(key)
        scell2 = SCellSeal(key)
        self.assertEqual(data, scell2.decrypt(scell1.encrypt(data)))

    def test_encrypt_decrypt_context(self):
        data = b'some data'
        key = b'some key'
        context = b'some context'
        scell1 = SCellSeal(key)
        scell2 = SCellSeal(key)
        self.assertEqual(data,
                         scell2.decrypt(
                             scell1.encrypt(data, context), context))

        encrypted = scell1.encrypt(data, context)
        incorrect_context = context + b'some another data'
        with self.assertRaises(ThemisError):
            scell2.decrypt(encrypted, incorrect_context)


class SCellContextImprintTest(unittest.TestCase):
    def test_encrypt_decrypt(self):
        data = b'some data'
        key = b'some key'
        context = b'some context'
        scell1 = SCellContextImprint(key)
        scell2 = SCellContextImprint(key)
        self.assertEqual(data,
                         scell2.decrypt(
                             scell1.encrypt(data, context), context))

        encrypted = scell1.encrypt(data, context)
        incorrect_context = context + b'some another data'
        decrypted = scell2.decrypt(encrypted, incorrect_context)
        self.assertNotEqual(data, decrypted)


class SCellTokenProtectTest(unittest.TestCase):
    def test_encrypt_decrypt(self):
        data = b'some data'
        key = b'some key'
        scell1 = SCellTokenProtect(key)
        scell2 = SCellTokenProtect(key)
        encrypted, token = scell1.encrypt(data)
        self.assertEqual(data, scell2.decrypt(encrypted, token))

    def test_encrypt_decrypt_context(self):
        data = b'some data'
        key = b'some key'
        context = b'some context'
        scell1 = SCellTokenProtect(key)
        scell2 = SCellTokenProtect(key)
        encrypted, token = scell1.encrypt(data, context)
        self.assertEqual(data, scell2.decrypt(encrypted, token, context))

        encrypted, token = scell1.encrypt(data, context)
        incorrect_context = context + b'some another data'
        with self.assertRaises(ThemisError):
            scell2.decrypt(encrypted, token, incorrect_context)


class SComparatorTest(unittest.TestCase):
    def test_comparator(self):
        shared_secret = b"shared secret"
        comparator1 = SComparator(shared_secret)
        comparator2 = SComparator(shared_secret)

        data1 = comparator1.begin_compare()
        data2 = None
        while not (comparator1.is_compared() and comparator2.is_compared()):
            if not comparator2.is_compared():
                data2 = comparator2.proceed_compare(data1)

            if not comparator1.is_compared():
                data1 = comparator1.proceed_compare(data2)

        self.assertTrue(comparator1.is_equal())
        self.assertTrue(comparator2.is_equal())


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


class SMessageTest(unittest.TestCase):
    def _test_wrap_unwrap(self, algorithm):
        keypair1 = GenerateKeyPair(algorithm)
        keypair2 = GenerateKeyPair(algorithm)

        smessage1 = SMessage(keypair1.export_private_key(),
                             keypair2.export_public_key())
        smessage2 = SMessage(keypair2.export_private_key(),
                             keypair1.export_public_key())

        data = b'some test data'

        encrypted_data = smessage1.wrap(data)
        self.assertEqual(data, smessage2.unwrap(encrypted_data))

    def test_wrap_unwrap(self):
        self._test_wrap_unwrap(KEY_PAIR_TYPE.EC)
        self._test_wrap_unwrap(KEY_PAIR_TYPE.RSA)

    def _test_sign_verify(self, algorithm):
        keypair = GenerateKeyPair(algorithm)

        data = b'some test data'

        signed_data = ssign(keypair.export_private_key(), data)

        self.assertEqual(data,
                         sverify(keypair.export_public_key(), signed_data))

        corrupted_signed_data = b'bla bla' + signed_data
        with self.assertRaises(ThemisError):
            sverify(keypair.export_public_key(), corrupted_signed_data)

        corrupted_signed_data = signed_data + b'bla bla'
        self.assertEqual(
            data, sverify(keypair.export_public_key(), corrupted_signed_data))

    def test_sign_verify(self):
        self._test_sign_verify(KEY_PAIR_TYPE.EC)
        self._test_sign_verify(KEY_PAIR_TYPE.RSA)


class SSessionTest(unittest.TestCase):
    def _test_ssession(self, algorithm):
        keypair1 = GenerateKeyPair(algorithm)
        keypair2 = GenerateKeyPair(algorithm)
        user_id1 = b'user_id1'
        user_id2 = b'user_id2'

        client_session = SSession(
            user_id1, keypair1.export_private_key(),
            SimpleMemoryTransport(user_id2, keypair2.export_public_key()))
        server_session = SSession(
            user_id2, keypair2.export_private_key(),
            SimpleMemoryTransport(user_id1, keypair1.export_public_key()))

        client_data = client_session.connect_request()
        server_data = None

        while not (client_session.is_established() and server_session.is_established()):
            if not server_session.is_established():
                server_data = server_session.unwrap(client_data)

            if not client_session.is_established():
                client_data = client_session.unwrap(server_data)

        data = b'some test data'

        self.assertEqual(data, client_session.unwrap(server_session.wrap(data)))
        self.assertEqual(data, server_session.unwrap(client_session.wrap(data)))

    def test_ssession(self):
        self._test_ssession(KEY_PAIR_TYPE.EC)
        #self._test_ssession(KEY_PAIR_TYPE.RSA)



if __name__ == '__main__':
    unittest.main()