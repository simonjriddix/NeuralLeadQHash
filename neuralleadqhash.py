import ctypes
from ctypes import c_uint8, c_size_t, POINTER, Structure

class ComputeStatus(ctypes.c_int):
    HASH_SUCCESS = 0
    HASH_BUSY    = 1
    HASH_ERROR   = 2

# Path to library dll/so/dylib
hash_lib = ctypes.CDLL("/path/to/libneuralleadqhash.so")
hash_lib.DirectComputeHashPointer.argtypes = [POINTER(c_uint8), c_size_t, POINTER(c_uint8)]
hash_lib.DirectComputeHashPointer.restype = ComputeStatus


def compute_hash(data):
    # Prepara l'input
    length = len(data)
    data_array = (c_uint8 * length)(*data)  # Crea un array ctypes
    output_array = (c_uint8 * 32)()         # Array per l'output

    # Chiama la funzione
    status = hash_lib.DirectComputeHashPointer(data_array, length, output_array)

    return (status, bytes(output_array))    # Restituisci l'output come array

if __name__ == '__main__':
    
    inputhash="neurallead4research hash test"
    print('hash test for ' + inputhash)  # Print Input
    
    data =inputhash.encode('utf-8')      # Convert string to bytes
    (status, output)= compute_hash(data) # Compute hash
    print("Output:", output.hex())       # Print hexadecimal output
