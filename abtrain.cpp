/*
Copyright (c) 2013, Koichiro Yamaguchi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include "AdaBoost.h"

struct ParameterABTrain {
    bool verbose;
    std::string trainingDataFilename;
    std::string outputModelFilename;
    int boostingType;
    int roundTotal;
};

// Prototype declaration
void exitWithUsage();
ParameterABTrain parseCommandline(int argc, char* argv[]);

void exitWithUsage() {
    std::cerr << "usage: abtrain [options] training_set_file [model_file]" << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << "   -t: type of boosting (0:discrete, 1:real, 2:gentle) [default:2]" << std::endl;
    std::cerr << "   -r: the number of rounds [default:100]" << std::endl;
    std::cerr << "   -v: verbose" << std::endl;

    exit(1);
}

ParameterABTrain parseCommandline(int argc, char* argv[]) {
    ParameterABTrain parameters;
    parameters.verbose = false;
    parameters.boostingType = 2;
    parameters.roundTotal = 100;
    int c;

    // Options
    while((c = getopt(argc, argv, "vt:r:")) != -1) {
        switch (c) {
            case 'v':
                parameters.verbose = true;
                break;
            case 't':
            {

                int boostingType = atoi(optarg);
                if (boostingType < 0) {
                    std::cerr << "error: invalid type of boosting" << std::endl;
                    exitWithUsage();
                }
                parameters.boostingType = boostingType;
                break;
            }
            case 'r':
            {
                int roundTotal = atoi(optarg);
                if (roundTotal < 0) {
                    std::cerr << "error: negative number of rounds" << std::endl;
                    exitWithUsage();
                }
                parameters.roundTotal = roundTotal;
                break;
            }
            default:
                std::cerr << "error: Undefined option" << std::endl;
                exitWithUsage();
                break;
        }
    }

    // Training data file
    if (argc - optind < 1)
	    exitWithUsage();
    parameters.trainingDataFilename = argv[optind];

    // Model file
    ++optind;
    if (argc - optind < 1)
	    parameters.outputModelFilename = parameters.trainingDataFilename + ".model";
    else parameters.outputModelFilename = argv[optind];

    return parameters;
}

int main(int argc, char* argv[]) {
    ParameterABTrain parameters = parseCommandline(argc, argv);

    if (parameters.verbose) {
        std::string boostingTypeName[4] = {"discrete", "real", "gentle", "modest"};
        std::cerr << std::endl;
        std::cerr << "Traing data:  " << parameters.trainingDataFilename << std::endl;
        std::cerr << "Output model: " << parameters.outputModelFilename << std::endl;
        std::cerr << "   Type:      " << boostingTypeName[parameters.boostingType] << std::endl;
        std::cerr << "   #rounds:   " << parameters.roundTotal << std::endl;
        std::cerr << std::endl;
    }

    AdaBoost adaBoost;
    adaBoost.setBoostingType(parameters.boostingType);
    adaBoost.setTrainingSamples(parameters.trainingDataFilename);
    adaBoost.train(parameters.roundTotal, parameters.verbose);

    adaBoost.writeFile(parameters.outputModelFilename);
}
