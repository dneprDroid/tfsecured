import base64
import hashlib
import os
import sys
import string
import random
import argparse

try:
    from Crypto import Random
    from Crypto.Cipher import AES
except:
    raise Exception('Install Crypto! \n pip install pycrypto')
try:
    import tensorflow as tf
except:
    raise Exception('Install Tensorflow!')


class AESCipher(object):

    def __init__(self, _key):
        self.bs = 16
        self.key = hashlib.sha256(_key.encode()).digest()

    def encrypt(self, raw):
        raw = self._pad(raw)
        iv = Random.new().read(AES.block_size)
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return iv + cipher.encrypt(raw)

    def decrypt(self, enc):
        iv = enc[:AES.block_size]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc[AES.block_size:]))  # .decode('utf-8')

    def _pad(self, s):
        return s + str.encode((self.bs - len(s) % self.bs) * chr(self.bs - len(s) % self.bs))

    @staticmethod
    def _unpad(s):
        return s[:-ord(s[len(s) - 1:])]

def load_graph(path):
    if not tf.gfile.Exists(path):
        raise Exception('File doesn\'t exist at path: %s' % path)
    with tf.gfile.GFile(path, 'rb') as f:
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())
        f.close()
        tf.import_graph_def(graph_def, name=None)
        return graph_def

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input-path', type=str, default='demo/models/saved_model.pb')
    parser.add_argument('-o', '--output-path', type=str, default='demo/models/saved_model-encrypted.pb')
    parser.add_argument('-k', '--key', type=str)

    args, _ = parser.parse_known_args()

    graph_def = load_graph(args.input_path)

    cipher = AESCipher(args.key)

    nodes_binary_str = graph_def.SerializeToString()

    nodes_binary_str = cipher.encrypt(nodes_binary_str)

    with tf.gfile.GFile(args.output_path, 'wb') as f:
        f.write(nodes_binary_str)
        f.close()
    print('Saved with key="%s" to %s' % (args.key, args.output_path))


if __name__ == "__main__":
    main()
