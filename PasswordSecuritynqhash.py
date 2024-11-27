import ctypes
from ctypes import c_uint8, c_size_t, POINTER

# Enum-like class to represent the return status of the hash computation function
class ComputeStatus(ctypes.c_int):
    HASH_SUCCESS = 0
    HASH_BUSY = 1
    HASH_ERROR = 2

# Load the NeuralLeadQHash shared library
hash_lib = ctypes.CDLL(r"C:\path\to\NeuralLeadQHash.dll")

# Define the argument types and return type for the hashing function
hash_lib.DirectComputeHashPointer.argtypes = [POINTER(c_uint8), c_size_t, POINTER(c_uint8)]
hash_lib.DirectComputeHashPointer.restype = ComputeStatus

def compute_hash(data):
    """Compute hash using NeuralLeadQHash."""
    length = len(data)
    data_array = (c_uint8 * length)(*data)
    output_array = (c_uint8 * 32)()

    status = hash_lib.DirectComputeHashPointer(data_array, length, output_array)
    return (status, bytes(output_array))

def AskandStorePassword():
    """Prompts user to store a hashed password."""
    NewPassword = input("Write your new password: ")
    EncodedNewPassword = NewPassword.encode('utf-8')
    (status, HashedOutput) = compute_hash(EncodedNewPassword)

    with open("password.db", "w", encoding="utf-8") as file:
        file.write(HashedOutput.hex())

def RetrieveAndCheckPassword():
    """Retrieves stored hashed password and verifies user input."""
    with open("password.db", "r", encoding="utf-8") as file:
        HashedPasswordFromDB = file.read().strip()

    PasswordToBeChecked = input("Type your password to login: ")
    EncodedPasswordToBeChecked = PasswordToBeChecked.encode('utf-8')
    (statusCheck, HashedPasswordOutput) = compute_hash(EncodedPasswordToBeChecked)
    
    if HashedPasswordFromDB == HashedPasswordOutput.hex():
        print("Your password is correct.")
    else:
        print("Your password is not correct.")

if __name__ == '__main__':
    AskandStorePassword()
    RetrieveAndCheckPassword()
