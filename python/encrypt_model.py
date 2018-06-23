import base64
import hashlib
import os
import sys
import string
import random

try:
    from Crypto import Random
    from Crypto.Cipher import AES
except:
    raise Exception('Install Crypto! \n pip install pycrypto ')
try:
    import tensorflow as tf
except:
    raise Exception('Install Tensorflow!')


class AESCipher(object):
    
    def __init__(self, _key):
        self.bs = 32
        self.key = hashlib.sha256(_key.encode()).digest()
    
    def encrypt(self, raw):
        raw = self._pad(raw)
        iv = Random.new().read(AES.block_size)
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
#        return base64.b64encode(iv + cipher.encrypt(raw))
        return (iv + cipher.encrypt(raw))

    
    def decrypt(self, enc):
#        enc = base64.b64decode(enc)
        iv = enc[:AES.block_size]
        print('Iv: %s' % iv)
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc[AES.block_size:]))#.decode('utf-8')
    
    def _pad(self, s):
        return s + (self.bs - len(s) % self.bs) * chr(self.bs - len(s) % self.bs)
    
    @staticmethod
    def _unpad(s):
        return s[:-ord(s[len(s)-1:])]

###############  Util Methods ###############

def load_graph(path):
    with tf.gfile.GFile(path, 'rb') as f:
        if not tf.gfile.Exists(path):
            raise Exception('File doesn\'t exist at path: %s' % path)
        
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())
        f.close()
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name=None)
        return graph_def

def generate_output_path(input_path, suffix):
    filename, file_extension = os.path.splitext(input_path)
    return filename + suffix + file_extension

def random_string(size=30, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(size))

def read_arg(index, default=None, err_msg=None):
    def print_error():
        if err_msg is not None:
            raise Exception(err_msg)
        else:
            raise Exception('Not found arg with index %s' % index)
    if len(sys.argv) <= index:
        if default is not None:
            return default
        print_error()
    return sys.argv[index]

#############################################

def main():
    USAGE = 'python encrypt_model.py <INPUT_PB_MODEL> <OUTPUT_PB_MODEL> <KEY>'
    print('\nUSAGE: %s\n' % USAGE)

    # Args:

    INPUT_PATH      = read_arg(1, default='demo/models/saved_model.pb')
    default_out     = generate_output_path(INPUT_PATH, '-encrypted')
    OUTPUT_PATH     = read_arg(2, default=default_out)
    KEY             = read_arg(3, default=random_string())




    graph_def = load_graph(INPUT_PATH)

    cipher = AESCipher(KEY)

    nodes_binary_str = graph_def.SerializeToString()



    nodes_binary_str = cipher.encrypt(nodes_binary_str)

    with tf.gfile.GFile(OUTPUT_PATH, 'wb') as f:
        f.write(nodes_binary_str);
        f.close()
    print('Saved with key="%s" to %s' % (KEY, OUTPUT_PATH))


if __name__ == "__main__":
    main()
