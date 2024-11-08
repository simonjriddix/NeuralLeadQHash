/*
Copyright 2024 SimonJRiddix & NeuralLead

NeuralLead Quantum Ai Hash

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/

// First Edition with Italian Comments

#if !defined(_NLQHASH_HEADER)
#define _NLQHASH_HEADER

#include <cstdint>
#include <vector>
#include <cstddef>

#if defined(NO_DLL)
#define DLL_API_NLHASH 
#define DLL_API_VAR_NLHASH 
#elif defined(USE_DLL_API_NLHASH)
#if defined(DLL_API_EXPORT_NLHASH)
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#define DLL_API_NLHASH extern "C" __declspec(dllexport)
#elif __GNUC__ >= 4
#define DLL_API_NLHASH __attribute__((visibility("default")))
#else

#endif
#define DLL_API_VAR_NLHASH DLL_API_NLHASH 
#else
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#define DLL_API_NLHASH extern "C" __declspec(dllimport)
#elif __GNUC__ >= 4
#define DLL_API_NLHASH __attribute__((visibility("default")))
#else

#endif
#define DLL_API_VAR_NLHASH extern DLL_API_NLHASH
#endif
#else
#define DLL_API_NLHASH extern
#define DLL_API_VAR_NLHASH extern
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define IS_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define IS_BIG_ENDIAN
#error("In First Realease only little endian supported")
#else
    // Se __BYTE_ORDER__ non è definito, usiamo un controllo manuale
constexpr uint16_t test_value = 0x1;
#define IS_LITTLE_ENDIAN (*(reinterpret_cast<const uint8_t*>(&test_value)) == 0x1)
#endif

#define WORKINK_CHAR uint8_t
enum ComputeStatus { HASH_SUCCESS = 0, HASH_BUSY, HASH_ERROR };
#define NLHASH_ISIZE 64
//#define NLHASH_PAVL (2*NLHASH_ISIZE)-8-1 // Is Padding 64 bytes=119, 80 bytes=151
#define USE_NLHASH true

// Espansione delle costanti pseudo-casuali per una maggiore sicurezza
const uint32_t roundKeys[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define accumulated_seed 46  /* Seed di partenza fisso */

#define NL_OUTPUTS 5
#define WORKINK_CHAR uint8_t
DLL_API_NLHASH void neuralNetwork(float inputs[32], float outputs[8]);

// Funzione di mescolamento avanzato con integrazione Quantum++
void compress(uint32_t state[8], const uint8_t* block, uint32_t& quantum_mix, bool useNL);

// Funzione hash principale DirectComputeHash migliorata e ottimizzata
DLL_API_NLHASH ComputeStatus DirectComputeHash(const uint8_t* data, size_t length, uint8_t*& output);

DLL_API_NLHASH void setMaximumCPUThreads(int maxThreads);
DLL_API_NLHASH void setMaximumParallelNeuralNetworks(int maxParallelNueralLeadHashNeuralNetworks);

DLL_API_NLHASH int getCPUThreads();
DLL_API_NLHASH int getParallelNeuralNetworks();

void NeuralLeadOutputToInts(std::vector<float>& outputNL, uint32_t* outputs);
void InputsIntToNeuralLead(uint32_t inputs[8], float inputNL[32]);
int is_little_endian();
void convert_to_big_endian(union uint32_to_uint8* u);

#define normalize_uint8_t(t) (t / static_cast<float>(UINT8_MAX))
#define denormalize_uint8_t(f) (static_cast<uint32_t>(f * static_cast<float>(UINT8_MAX)))
#define normalize_pos_uint8_t normalize_uint8t

#define uint32Touint8(into, byto) \
byto[0] = (into >> 24) & 0xFF; \
byto[1] = (into >> 16) & 0xFF; \
byto[2] = (into >> 8) & 0xFF; \
byto[3] = into & 0xFF;

union uint32_to_uint8
{
    uint32_t integer;
    uint8_t bytes[4];
};

#endif