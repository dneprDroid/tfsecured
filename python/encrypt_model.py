import base64
import hashlib
from Crypto import Random
from Crypto.Cipher import AES
import tensorflow as tf


class AESCipher(object):
    
    def __init__(self, key):
        self.bs = 32
        self.key = hashlib.sha256(key.encode()).digest()

def encrypt(self, raw):
    raw = self._pad(raw)
    iv = Random.new().read(AES.block_size)
    cipher = AES.new(self.key, AES.MODE_CBC, iv)
    return base64.b64encode(iv + cipher.encrypt(raw))
    
    def decrypt(self, enc):
        enc = base64.b64decode(enc)
        iv = enc[:AES.block_size]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc[AES.block_size:])).decode('utf-8')
    
    def _pad(self, s):
        return s + (self.bs - len(s) % self.bs) * chr(self.bs - len(s) % self.bs)
    
    @staticmethod
    def _unpad(s):
        return s[:-ord(s[len(s)-1:])]


path = '/Users/useruser/Desktop/TFSecured/python/models/saved_model.pb'
encrypted_map = {}

def encrypt_tensor(sess, graph_def, op):
    tensor_type = op.op_def.name
    tensor_id = op.name
    if tensor_type == 'MatMul' :
        var_index = 1 #TODO:
        input_var = op.inputs[var_index]
        input_var_data = input_var.eval(session=sess)
        
        print('1) input_var_data(%s): %s' % (input_var.name, input_var_data))
        input_var_data_flatten = input_var_data.flatten()
        for i in range(len(input_var_data_flatten)):
            input_var_data_flatten[i] = 66 #TODO:
        input_var_data = input_var_data_flatten.reshape(input_var_data.shape)
        print('2) input_var_data: %s' % input_var_data)
            
        new_tensor = tf.Variable(input_var_data)
        print('Updated: %s' % op.inputs[var_index].eval(session=sess))


def load_graph(path, prefix):
    with tf.gfile.GFile(path, 'rb') as f:
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name=None)
        return graph

graph_def = load_graph(path, 'prefix')
sess = tf.Session(graph=graph_def)
for op in graph_def.get_operations():
    tensor_type = op.op_def.name
    tensor_id = op.name
    
    print('tensor_type: %s' % tensor_type)
    print('tensor_id: %s'   % tensor_id)
    encrypt_tensor(sess, graph_def, op)
    print('-----------------\n')

#print('encrypted_map: %s' % encrypted_map)
#tf.import_graph_def(tf.GraphDef(), name='', input_map=encrypted_map)

