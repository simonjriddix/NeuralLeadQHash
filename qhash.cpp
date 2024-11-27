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

// Quantum & Neural Network functions

#include <cstring>
#include <omp.h>

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <limits>
#include <cmath>

#include "qhash.hpp"

#include <qpp/qpp.h>

using namespace qpp;

#define NUM_QUBITS 2  // Working qubits

// Rotation functions
#define rotl(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define rotr(x, b) (uint32_t)(((x) >> (b)) | ((x) << (32 - (b))))

// Normalization functions
#define normalize_uint32_t(t) (static_cast<double>(t) / static_cast<double>(UINT32_MAX))
#define denormalize_uint32_t(f) (static_cast<uint32_t>(f * static_cast<double>(UINT32_MAX)))

// Improved mixing function
void inline improvedMix(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
{
    a = rotl(a, 13) ^ b;
    b = rotr(b, 7) + c;
    c = rotl(c, 17) ^ d;
    d = rotr(d, 11) + a;
}

// Additional mixing step between blocks
void mixBetweenBlocks(uint32_t state[8])
{
    improvedMix(state[0], state[1], state[2], state[3]);
    improvedMix(state[1], state[2], state[3], state[4]);
    improvedMix(state[2], state[3], state[4], state[5]);
    improvedMix(state[3], state[4], state[5], state[6]);
    improvedMix(state[4], state[5], state[6], state[7]);
}

// Improved compression function
void compress(uint32_t state[8], const uint8_t* block, uint32_t& quantum_mix, bool useNL)
{
    thread_local const Gates& insta = Gates::get_thread_local_instance();

    thread_local auto& prng = RandomDevices::get_thread_local_instance();
    prng.get_prng().seed(accumulated_seed);

    uint32_t w[64];

    // Message expansion
    for (int i = 0; i < 16; ++i)
    {
        int pos = i * 4;
        w[i] = (block[pos] << 24) | (block[pos + 1] << 16) | (block[pos + 2] << 8) | block[pos + 3];
    }

    for (int i = 16; i < 64; ++i)
    {
        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];

    for (int round = 0; round < 4; ++round)
    {
        // Improved round function
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);

        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        for (int pii = 0; pii < 16; pii++)
        {
            auto ri = pii * round;
            uint32_t temp1 = (h + S1 + ch + roundKeys[ri] + w[ri]) ^ quantum_mix;
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        
        // Additional mixing
        improvedMix(a, b, c, d);
        improvedMix(e, f, g, h);
    }

    if (useNL)  // NeuralLead Update
    {
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;

        float nn_inputs[32];
        float nn_outputs[8];

        InputsIntToNeuralLead(state, nn_inputs);

        neuralNetwork(nn_inputs, nn_outputs);

        // Mescolamento intermedio dello stato con rete neurallead prima della compressione quantistica
        for (int i = 1; i < 10; ++i)
        {
            state[i % 8] ^= static_cast<uint32_t>(nn_outputs[i % NL_OUTPUTS] * 1000.0f) ^ roundKeys[i] ^ state[(i - 1) % 8];
        }

        a = state[0], b = state[1], c = state[2], d = state[3];
        e = state[4], f = state[5], g = state[6], h = state[7];
    }

    {
        // Quantum algo

        // Improved round function
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);

        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        ket qubits = 00_ket;  // Stato iniziale |00>

        // Quantum operations

        qubits = kron(insta.H, insta.H) * qubits;

        // Apply controlled rotations
        qubits = apply(qubits, insta.RZ(normalize_uint32_t(a ^ e)), { 0 });
        qubits = apply(qubits, insta.RX(normalize_uint32_t(b ^ f)), { 1 });
        qubits = apply(qubits, insta.RY(normalize_uint32_t(c ^ g)), { 0 });
        qubits = apply(qubits, insta.RZ(normalize_uint32_t(d ^ h)), { 1 });

        // Apply CNOT gates
        qubits = apply(qubits, insta.CNOT, { 0, 1 });

        // Measure all qubits
        auto [m, probs, states] = measure(qubits, insta.Id(1 << NUM_QUBITS));

        quantum_mix ^= denormalize_uint32_t(probs[0]);

        auto dx = denormalize_uint32_t(states[0].x().real());
        auto dx_i = denormalize_uint32_t(states[0].x().imag());
        quantum_mix ^= dx;
        quantum_mix ^= dx_i;

        auto dy = denormalize_uint32_t(states[0].y().real());
        auto dy_i = denormalize_uint32_t(states[0].y().imag());
        quantum_mix ^= dy;
        quantum_mix ^= dy_i;

        uint32_t temp1 = (h + S1 + ch + roundKeys[63] + w[63]) ^ quantum_mix;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Additional mixing
    improvedMix(a, b, c, d);
    improvedMix(e, f, g, h);

    // Final state update
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

// Main hash function
DLL_API_NLHASH ComputeStatus DirectComputeHash(const uint8_t* data, size_t length, uint8_t*& hash_output)
{
    // Initial state
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // Pre-processing
    size_t padded_length = ((length + 9 + 63) / 64) * 64;
    std::vector<uint8_t> padded_data(padded_length, 0);
    memcpy(padded_data.data(), data, length);
    //padded_data[length -1] &= 0x80;

    uint64_t bit_len = length * 8;
    for (int i = 0; i < 8; ++i)
    {
        padded_data[padded_length - 1 - i] = static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF);
    }

    uint32_t quantum_mix = 0;

    // Process each block
    for (size_t i = 0; i < padded_length; i += 64)
    {        
        bool useNL = ((padded_data[i] + length) % 5 == 0);
        compress(state, &padded_data[i], quantum_mix, useNL);
        mixBetweenBlocks(state);  // Additional mixing between blocks
    }

    // Produce final hash
    for (int i = 0; i < 8; ++i)
    {
        int pos = i * 4;
        hash_output[pos] = (state[i] >> 24) & 0xFF;
        hash_output[pos + 1] = (state[i] >> 16) & 0xFF;
        hash_output[pos + 2] = (state[i] >> 8) & 0xFF;
        hash_output[pos + 3] = state[i] & 0xFF;
    }

    return ComputeStatus::HASH_SUCCESS;
}

// Main hash function with output pointer
DLL_API_NLHASH ComputeStatus DirectComputeHashPointer(const uint8_t* data, size_t length, uint8_t* hash_output)
{
    // Initial state
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // Pre-processing
    size_t padded_length = ((length + 9 + 63) / 64) * 64;
    std::vector<uint8_t> padded_data(padded_length, 0);
    memcpy(padded_data.data(), data, length);
    //padded_data[length -1] &= 0x80;

    uint64_t bit_len = length * 8;
    for (int i = 0; i < 8; ++i)
    {
        padded_data[padded_length - 1 - i] = static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF);
    }

    uint32_t quantum_mix = 0;

    // Process each block
    for (size_t i = 0; i < padded_length; i += 64)
    {
        bool useNL = ((padded_data[i] + length) % 5 == 0);
        compress(state, &padded_data[i], quantum_mix, useNL);
        mixBetweenBlocks(state);  // Additional mixing between blocks
    }

    // Produce final hash
    for (int i = 0; i < 8; ++i)
    {
        int pos = i * 4;
        hash_output[pos] = (state[i] >> 24) & 0xFF;
        hash_output[pos + 1] = (state[i] >> 16) & 0xFF;
        hash_output[pos + 2] = (state[i] >> 8) & 0xFF;
        hash_output[pos + 3] = state[i] & 0xFF;
    }

    return ComputeStatus::HASH_SUCCESS;
}
