import os

from ellipticcurve.ecdsa import Ecdsa
from ellipticcurve.privateKey import PrivateKey


# Generate keys
print('{0} Generate private/public keys {0}'.format('='*30))
privKey = PrivateKey()
pubKey = privKey.publicKey()

print('privKey={}'.format(privKey.secret))
print('pubKey=({}, {})'.format(pubKey.point.x, pubKey.point.y))

# Generate N bytes random message
print('{0} Generate random message {0}'.format('='*30))
message = os.urandom(32).hex()

print(f'message={message}')

# Generate Signature
print('{0} Sign message {0}'.format('='*30))
signature = Ecdsa.sign(message, privKey)

print('signature.r={}'.format(signature.r))
print('signature.s={}'.format(signature.s))

# Verification
print('{0} Verify message {0}'.format('='*30))
v = Ecdsa.verify(message, signature, pubKey)
print('verify={}'.format(v))
