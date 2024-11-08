# NeuralLeadQHash

[NeuralLeadQHash](https://www.neurallead.com)

## What is NeuralLeadQHash?

[NeuralLeadQHash](https://www.neurallead.com) is an advanced hybrid hashing algorithm crafted by NeuralLead, uniquely blending quantum computing techniques with neural network architecture to elevate cryptographic security. Built on the NeuralLead Core technology, this algorithm simulates a quantum processing environment, enhancing both security and efficiency in the hashing process. NeuralLeadQHash leverages simulated qubits alongside neural networks to generate a high-entropy data mix, offering robust resistance against cryptographic attacks.

For more information, as well as an immediately usable binary version of the NeuralLeadQHash algorithm, visit [https://www.neurallead.com](https://www.neurallead.com) or read the [modern whitepaper](https://www.neurallead.com/NeuralLeadCoin-NLEAD-Modern-White-Paper.pdf).

## License

NeuralLeadQHash is released under the terms of the MIT license. See [LICENSE](LICENSE). 


## Algorithm - NeuralLeadQHash

NeuralLeadQHash uses a new open-source hashing algorithm based on Quantum and Artificial Intelligence algorithms. The code is available on GitHub, along with precompiled and optimized libraries, in the [repository link](https://github.com/simonjriddix/neuralleadqhash).

For more technical information, visit the [tech information page](https://dev.to/teammanagerneurallead/from-neuralleadqhash-to-research-funding-the-neurallead-coin-revolution-4gpg).



## Manual Build

*NeuralLeadQHash*:

### For Linux:

To install `git`:

    sudo apt update && sudo apt upgrade -y
    sudo apt-get install git
    

Clone the repositories:
    
    cd
    git clone --recursive https://github.com/OPM/eigen3
    
and follow instructions in [https://github.com/OPM/eigen3/blob/master/INSTALL](https://github.com/OPM/eigen3/blob/master/INSTALL)

    cd 
    git clone --recursive https://github.com/simonjriddix/quantumpp
    
and follow instructions in [https://github.com/simonjriddix/quantumpp/blob/main/INSTALL.md](https://github.com/simonjriddix/quantumpp/blob/main/INSTALL.md)

    git clone https://github.com/simonjriddix/neuralleadqhash
    make -j`nproc`


### For Windows:

install eigen3 

and follow instructions in [https://github.com/OPM/eigen3/blob/master/INSTALL](https://github.com/OPM/eigen3/blob/master/INSTALL)

install quantumpp 

and follow instructions in [https://github.com/simonjriddix/quantumpp/blob/main/INSTALL.md](https://github.com/simonjriddix/quantumpp/blob/main/INSTALL.md)

open `neuralleadqhash.vcproj` in Visual Studio 22

    


Wait for the operation to complete. The process may take several minutes depending on your hardware’s performance.

### Python version

Inside python code use **NeuralLeadQHash.dll** file for windows and **NeuralLeadQHash.so** for Linux.

Put **NeuralLeadQHash.dll** or **NeuralLeadQHash.so** (depends on the operating system i.e., Windows or Linux) in same directory or specify absolute path.
