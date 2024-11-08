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

#define _MAINAPP 
#include <AcceleratedDistributedNetwork.hpp>
#include <Loss/cross-entropy.h>
#include "qhash.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <shared_mutex>
//#include <syncstream>
#include <fstream>
#include <csignal>
#include <filesystem>
#include <map>
#include <string>

#include <algorithm> // shuffle vector
//#include <random>
#if defined(_WIN32) || defined(_WIN64)
#include <synchapi.h>
#endif

#include <omp.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <limits>

#if defined(_WIN32) || !defined(_WIN64) || defined(__MINGW32__)
#define Sleep(t) std::this_thread::sleep_for(std::chrono::milliseconds(t));

std::shared_mutex nLock;
#else
mutable std::shared_mutex nLock;
#endif

#define WriteLock std::unique_lock wlock(nLock);
#define ReadLock  std::shared_lock rlock(nLock);

#define LOAD_DIRECT 1

int32_t MAX_NLNN = 1;
int32_t MAX_CPU_THREADS_NLNN = 0;

using namespace Coscienza;

extern void py_add_net(Coscienza::AcceleratedDistributedNetwork* net);
int Init_nlhash(AcceleratedDistributedNetwork*& hellonetwork, bool OnGPU);

class HashNetControl
{
public:
    AcceleratedDistributedNetwork* hellonetwork = nullptr;
    Coscienza::Generators::FullValue* gen = nullptr;
    bool Ready = true;

    HashNetControl(bool OnGPU = false);
    ~HashNetControl();

private:

};

HashNetControl::HashNetControl(bool OnGPU)
{
    Init_nlhash(hellonetwork, OnGPU);
    gen = hellonetwork->AddFullValueSpikeGenerator(hellonetwork->GetGroup(0));
    hellonetwork->setThreads(MAX_CPU_THREADS_NLNN);
}

HashNetControl::~HashNetControl()
{
    //delete hellonetwork;
}

std::vector<HashNetControl> NetworksControl;

DLL_API_NLHASH int getCPUThreads()
{
    WriteLock;

    int n = MAX_CPU_THREADS_NLNN;
    return n;
}

DLL_API_NLHASH int getParallelNeuralNetworks()
{
    WriteLock;
    
    int n = MAX_NLNN;    
    return n;
}

DLL_API_NLHASH void setMaximumCPUThreads(int maxThreads)
{
    if (maxThreads < 0)
        return;

    WriteLock;
    
    //MAX_CPU_THREADS_NLNN = maxThreads;
    
    for (int n = 0; n < NetworksControl.size(); n++)
    {
        while (NetworksControl[n].hellonetwork->getSimulationState() == State::Running)
        {
            continue;
        }
        
        //NetworksControl[n].hellonetwork->setThreads(maxThreads);
        std::cout << "NeuralLeadHash Neural Network with index " << std::to_string(n) << " set to " << NetworksControl[n].hellonetwork->getThreads() << " Threads" << std::endl;
    }
}

DLL_API_NLHASH void setMaximumParallelNeuralNetworks(int maxParallelNueralLeadHashNeuralNetworks)
{
    if (maxParallelNueralLeadHashNeuralNetworks < 1)
        return;

    WriteLock;

    //MAX_NLNN = maxParallelNueralLeadHashNeuralNetworks;
}

bool _onGPU = false;
HashNetControl& getNetworkControl()
{
    WriteLock;

    //research:
    {
        while (true)
        {
            for (int n = 0; n < NetworksControl.size(); n++)
            {
                //if (NetworksControl[n].hellonetwork->getSimulationState() != State::Running)
                if (NetworksControl[n].Ready)
                {
                    NetworksControl[n].Ready = false;
                    return NetworksControl[n];
                }
            }

            if (NetworksControl.size() < MAX_NLNN)
            {
                //Sleep(1);
                break;
            }
        }
    }


    /*if (NetworksControl.size() >= MAX_NLNN) // Second check
    {
        goto research;
    }*/
    
    if (MAX_CPU_THREADS_NLNN == 0 && NetworksControl.size() == 0)
    {
        if (_onGPU)
        {
            MAX_NLNN = 20;
            MAX_CPU_THREADS_NLNN = 0;
        }
        else
        {
            MAX_NLNN = 1;
            MAX_CPU_THREADS_NLNN = //AcceleratedDistributedNetwork::getMaximumThreads() / MAX_NLNN;
                omp_get_max_threads() / MAX_NLNN;
            if (MAX_CPU_THREADS_NLNN < 1)
                MAX_CPU_THREADS_NLNN = 1;
        }
    }

    HashNetControl& nctrl = NetworksControl.emplace_back(_onGPU);
    nctrl.Ready = false;

    return nctrl;
}

// Funzione per determinare se il sistema è little-endian o big-endian
int is_little_endian()
{
    uint32_t test = 1;
    return *((uint8_t*)&test) == 1;
}

// Funzione per convertire un intero a 32 bit in un array di 4 byte in big-endian
void convert_to_big_endian(union uint32_to_uint8* u)
{
    uint8_t temp;
    // Scambia i byte per ottenere l'ordine big-endian
    temp = u->bytes[0];
    u->bytes[0] = u->bytes[3];
    u->bytes[3] = temp;

    temp = u->bytes[1];
    u->bytes[1] = u->bytes[2];
    u->bytes[2] = temp;
}

void NeuralLeadOutputToInts(std::vector<float>& outputNL, uint32_t* outputs)
{
    for (int p = 0; p < outputNL.size(); p++)
    {
        outputs[p] = denormalize_uint8_t(outputNL[p]);
    }
}

void InputsIntToNeuralLead(uint32_t inputs[8], float inputNL[32])
{
    union uint32_to_uint8 u;

    for (int p = 0; p < 8; p++)
    {
        u.integer = UINT32_MAX - inputs[p];
#if defined(IS_LITTLE_ENDIAN)
        convert_to_big_endian(&u);
#endif
        int pos = p * 4;
        inputNL[pos] = u.bytes[0] + 1.0f;
        inputNL[pos + 1] = u.bytes[1] + 1.0f;
        inputNL[pos + 2] = u.bytes[2] + 1.0f;
        inputNL[pos + 3] = u.bytes[3] + 1.0f;
    }
}

void neuralNetwork(float inputs[32], float outputs[8])
{
    auto& netCtrl = getNetworkControl();

    //netCtrl.gen->SetValues(_inputs);
    netCtrl.hellonetwork->SetFullValueInputs(0, inputs, 32); // 4 * 8

    netCtrl.hellonetwork->Run();

    auto& neuralleadhashOutput = netCtrl.hellonetwork->GetOutput(2);
    memcpy(outputs, neuralleadhashOutput.data(), NL_OUTPUTS * sizeof(float));

    netCtrl.Ready = true;
}

int Init_nlhash(AcceleratedDistributedNetwork*& hellonetwork, bool OnGPU)
{
    if (hellonetwork != nullptr)
    {
        //			std::cout << "NeuralLeadHash already initialzed" << std::endl;
        return 0; // Zero neuralnetworks initialized
    }

#if defined(LOAD_DIRECT)
    try
    {
        hellonetwork = AcceleratedDistributedNetwork::Load("neuralleadqhash")[0];
    }
    catch (const std::exception&)
    {
        std::cout << "Cannot load neurallead network. Missing files?" << std::endl;
        std::runtime_error("Cannot load neurallead network. Missing files?");
        return 0;
    }
    hellonetwork->UseGPU = hellonetwork->UseCUDA = hellonetwork->OnTargetDevice = OnGPU;
#else

    hellonetwork = new AcceleratedDistributedNetwork("neuralleadqhash");
    hellonetwork->UseGPU = hellonetwork->UseCUDA = hellonetwork->OnTargetDevice = OnGPU;

    LearningRuleContainer* lea = new LearningRuleContainer();

    auto actReLu = new Activations::ReLu();
    auto actSigmoid = new Activations::Sigmoid();
    auto actEXN = new Activations::EvenXorNot();

    auto neuronGen = new Coscienza::Neurons::GeneratorNeuron();
    auto neuronRelu = new Coscienza::Neurons::TraditionalNeuron(actReLu);
    auto neuronSigmoid = new Coscienza::Neurons::TraditionalNeuron(actSigmoid);
    auto neuronEXN = new Coscienza::Neurons::TraditionalNeuron(actEXN);

    bool biases = false;

    Grid* grid_InputToV1 = new Grid(8 * 4, 1, 1);
    Grid* grid_mant = new Grid(81, 1, 1);
    Grid* grid_oo = new Grid(NL_OUTPUTS, 1, 1);

    Coscienza::Group* groupInputs = new Coscienza::Group("InputTo", GroupTypes::Input, grid_InputToV1, neuronGen, 0.0f);
    groupInputs->UseBias = biases;
    hellonetwork->AddGroup(groupInputs);

    Coscienza::Group* groupS1 = new Coscienza::Group("s1", GroupTypes::Normal, grid_mant, neuronEXN, 0.0f);
    groupS1->UseBias = biases;
    groupS1->BiasInitMin = 0.08f;
    groupS1->BiasInitMax = 0.1f;
    hellonetwork->AddGroup(groupS1);

    Coscienza::Group* groupOutput = new Coscienza::Group("to", GroupTypes::Normal, grid_oo, neuronSigmoid, 0.0f);
    groupOutput->UseBias = biases;
    groupOutput->BiasInitMin = -0.28f;
    groupOutput->BiasInitMax = 0.3f;
    hellonetwork->AddGroup(groupOutput);

    // FtRy£
    float prob = .35f;
    float a = 50.05f * groupS1->CountNeurons();
    float z = 80.0f / a;
    hellonetwork->Connect(groupInputs, groupS1, "full", "pure", lea, new RangeWeight(-10.0f, -z, z, 10.0f), NULL, 1.f, new RangeDelay(0));
    hellonetwork->Connect(groupInputs, groupS1, "random", "pure", lea, new RangeWeight(-11.0f, -z / prob, z / prob, 12.0f), NULL, prob, new RangeDelay(0));
    hellonetwork->Connect(groupS1, groupOutput, "random", "pure", lea, new RangeWeight(-11.0f, -0.315f, 0.315f, 12.0f), NULL, .71f, new RangeDelay(0));

    hellonetwork->setAutoSave(0);
    hellonetwork->SetTimeSteps(1.0f); // Forward Euler
    hellonetwork->setRunDurationMs(1);
    hellonetwork->SetLearningInterval(1);
    hellonetwork->SetMicrogliaClean(0, false);

    float rewardlevel = 1.0;
    float punishlevel = 1.0;
    Dopamine::SetParameters(rewardlevel, punishlevel);
#endif

    hellonetwork->DisableLearning();

    hellonetwork->Setup();

#if !defined(LOAD_DIRECT)
    hellonetwork->Save("neuralleadhash", true);
#endif

    //gen = hellonetwork->AddFullValueSpikeGenerator(hellonetwork->GetGroup(0));

    return 1; // One neuralnetwork initialized
}
