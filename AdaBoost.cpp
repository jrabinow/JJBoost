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

#include "AdaBoost.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "readSampleDataFile.h"

struct SampleElement {
    int sampleIndex;
    double sampleValue;

    bool operator<(const SampleElement& comparisonElement) const { return sampleValue < comparisonElement.sampleValue; }
};

void AdaBoost::DecisionStump::set(const int featureIndex,
                        const double threshold,
                        const double outputLarger,
                        const double outputSmaller,
                        const double error)
{
    featureIndex_ = featureIndex;
    threshold_ = threshold;
    outputLarger_ = outputLarger;
    outputSmaller_ = outputSmaller;
    error_ = error;
}

double AdaBoost::DecisionStump::evaluate(const double featureValue) const {
    if (featureValue > threshold_) return outputLarger_;
    else return outputSmaller_;
}

double AdaBoost::DecisionStump::evaluate(const std::vector<double>& featureVector) const {
    if (featureVector[featureIndex_] > threshold_) return outputLarger_;
    else return outputSmaller_;
}

void AdaBoost::setBoostingType(const int boostingType) {
    if (boostingType < 0) {
        std::cerr << "error: invalid type of boosting" << std::endl;
        exit(1);
    }
    boostingType_ = boostingType;
}

void AdaBoost::setTrainingSamples(const std::string& trainingDataFilename) {
    readSampleDataFile(trainingDataFilename, samples_, labels_);
    sampleTotal_ = static_cast<int>(samples_.size());
    if (sampleTotal_ == 0) {
        std::cerr << "error: no training sample" << std::endl;
        exit(1);
    }
    featureTotal_ = static_cast<int>(samples_[0].size());
    initializeWeights();
    sortSampleIndices();
    weakClassifiers_.clear();
#ifdef EARLY_TERMINATION
    positives = negatives = 0;
#endif
}

void AdaBoost::train(const int roundTotal, const bool verbose) {
    for (int roundCount = 0; roundCount < roundTotal; ++roundCount) {
        trainRound();

        if (verbose) {
            std::cout << "Round " << roundCount << ": " << std::endl;
            std::cout << "feature = " << weakClassifiers_[roundCount].featureIndex() << ", ";
            std::cout << "threshold = " << weakClassifiers_[roundCount].threshold() << ", ";
            std::cout << "output = [ " << weakClassifiers_[roundCount].outputLarger() << ", ";
            std::cout << weakClassifiers_[roundCount].outputSmaller() << "], ";
            std::cout << "error = " << weakClassifiers_[roundCount].error() << std::endl;
        }
    }
#ifndef EARLY_TERMINATION
    int positives = 0, negatives = 0;
#endif
    for(int sampleIndex = 0; sampleIndex < sampleTotal_; sampleIndex++)
	    if(labels_[sampleIndex] > 0)
		positives++;
	    else
		negatives++;
    // Prediction test
    if (verbose) {
        int positiveCorrectTotal = 0;
        int negativeCorrectTotal = 0;
        for (int sampleIndex = 0; sampleIndex < sampleTotal_; ++sampleIndex) {
            double score = this->predict(samples_[sampleIndex]);

            if (labels_[sampleIndex] && score > 0) {
                ++positiveCorrectTotal;
            } else if( ! labels_[sampleIndex] && score <= 0) {
                ++negativeCorrectTotal;
            }
        }

        std::cout << std::endl;
        std::cout << "Training set" << std::endl;
        std::cout << "  positive: " << static_cast<double>(positiveCorrectTotal)/positives;
        std::cout << " (" << positiveCorrectTotal << " / " << positives << "), ";
        std::cout << "negative: " << static_cast<double>(negativeCorrectTotal)/negatives;
        std::cout << " (" << negativeCorrectTotal << " / " << negatives << ")" << std::endl;;
    }
}

double AdaBoost::predict(const std::vector<double>& featureVector) const {
    double score = 0.0;
    for (int classifierIndex = 0; classifierIndex < static_cast<int>(weakClassifiers_.size()); ++classifierIndex) {
        score += weakClassifiers_[classifierIndex].evaluate(featureVector);
#ifdef EARLY_TERMINATION
	if(score > 1.0 - (double) positives / (double) sampleTotal_ ||
			score < -1.0 + (double) negatives / (double) sampleTotal_) {
		break;
	}
#endif
    }
    return score;
}


void AdaBoost::initializeWeights() {
    double initialWeight = 1.0/sampleTotal_;

    weights_.resize(sampleTotal_);
    for (int i = 0; i < sampleTotal_; ++i) weights_[i] = initialWeight;

    rev_distr_.resize(sampleTotal_);
    for (int i = 0; i < sampleTotal_; ++i) rev_distr_[i] = 1.0/initialWeight;

#ifdef MADABOOST
    madaboostEvalValues_.resize(sampleTotal_);
    for (int i = 0; i < sampleTotal_; ++i) madaboostEvalValues_[i] = 1.0;
#elif defined (ETABOOST)
    etaboostEvalValues_.resize(sampleTotal_);
    for (int i = 0; i < sampleTotal_; ++i) etaboostEvalValues_[i] = 1.0;
#endif
}

void AdaBoost::sortSampleIndices() {
    sortedSampleIndices_.resize(featureTotal_);
    for (int d = 0; d < featureTotal_; ++d) {
        std::vector<SampleElement> featureElements(sampleTotal_);
        for (int sampleIndex = 0; sampleIndex < sampleTotal_; ++sampleIndex) {
            featureElements[sampleIndex].sampleIndex = sampleIndex;
            featureElements[sampleIndex].sampleValue = samples_[sampleIndex][d];
        }
        std::sort(featureElements.begin(), featureElements.end());

        sortedSampleIndices_[d].resize(sampleTotal_);
        for (int i = 0; i < sampleTotal_; ++i) {
            sortedSampleIndices_[d][i] = featureElements[i].sampleIndex;
        }
    }
}

void AdaBoost::trainRound() {
    calcWeightSum();

    DecisionStump bestClassifier;
    for (int featureIndex = 0; featureIndex < featureTotal_; ++featureIndex) {
        DecisionStump optimalClassifier = learnOptimalClassifier(featureIndex);
        if (optimalClassifier.featureIndex() < 0) continue;

        if (bestClassifier.error() < 0 || optimalClassifier.error() < bestClassifier.error()) {
            bestClassifier = optimalClassifier;
        }
    }

    updateWeight(bestClassifier);

    weakClassifiers_.push_back(bestClassifier);
}

void AdaBoost::calcWeightSum() {
    weightSum_ = 0;
    weightLabelSum_ = 0;
    positiveWeightSum_ = 0;
    negativeWeightSum_ = 0;

    for (int sampleIndex = 0; sampleIndex < sampleTotal_; ++sampleIndex) {
        weightSum_ += weights_[sampleIndex];
        if (labels_[sampleIndex]) {
            weightLabelSum_ += weights_[sampleIndex];
            positiveWeightSum_ += weights_[sampleIndex];
        } else {
            weightLabelSum_ -= weights_[sampleIndex];
            negativeWeightSum_ += weights_[sampleIndex];
        }
    }
}

AdaBoost::DecisionStump AdaBoost::learnOptimalClassifier(const int featureIndex) {
    const double epsilonValue = 1e-6;

    double weightSumLarger = weightSum_;
    double weightLabelSumLarger = weightLabelSum_;
    double positiveWeightSumLarger = positiveWeightSum_;
    double negativeWeightSumLarger = negativeWeightSum_;
    double positiveWeightSumLargerRev = positiveWeightSum_;
    double negativeWeightSumLargerRev = negativeWeightSum_;

    double rev_distrSum = 0;
    for (int i = 0; i < sampleTotal_; ++i) {
        rev_distrSum += rev_distr_[i];
    }

    DecisionStump optimalClassifier;
    for (int sortIndex = 0; sortIndex < sampleTotal_ - 1; ++sortIndex) {
        int sampleIndex = sortedSampleIndices_[featureIndex][sortIndex];
        double threshold = samples_[sampleIndex][featureIndex];
        double sampleWeight = weights_[sampleIndex];
        double rev_distr = rev_distr_[sampleIndex]/rev_distrSum;


        weightSumLarger -= sampleWeight;
        if (labels_[sampleIndex]) {
            weightLabelSumLarger -= sampleWeight;
            positiveWeightSumLarger -= sampleWeight;
        } else {
            weightLabelSumLarger += sampleWeight;
            negativeWeightSumLarger -= sampleWeight;
        }
        while (sortIndex < sampleTotal_ - 1
               && samples_[sampleIndex][featureIndex] == samples_[sortedSampleIndices_[featureIndex][sortIndex + 1]][featureIndex])
        {
            ++sortIndex;
            sampleIndex = sortedSampleIndices_[featureIndex][sortIndex];
            sampleWeight = weights_[sampleIndex];
            weightSumLarger -= sampleWeight;

            if (labels_[sampleIndex]) {
                weightLabelSumLarger -= sampleWeight;
                positiveWeightSumLarger -= sampleWeight;
                positiveWeightSumLargerRev = positiveWeightSumLarger * rev_distr;
            } else {
                weightLabelSumLarger += sampleWeight;
                negativeWeightSumLarger -= sampleWeight;
                negativeWeightSumLargerRev = negativeWeightSumLarger * rev_distr;
            }
        }
        if (sortIndex >= sampleTotal_ - 1) break;

        if (fabs(weightSumLarger) < epsilonValue || fabs(weightSum_ - weightSumLarger) < epsilonValue) continue;

        double outputLarger, outputSmaller;
        computeClassifierOutputs(weightSumLarger, weightLabelSumLarger, positiveWeightSumLarger, negativeWeightSumLarger,
                                 positiveWeightSumLargerRev, negativeWeightSumLargerRev, outputLarger, outputSmaller);
        
        double error = computeError(positiveWeightSumLarger, negativeWeightSumLarger, outputLarger, outputSmaller);


        if (optimalClassifier.error() < 0 || error < optimalClassifier.error()) {
            double classifierThreshold = (threshold + samples_[sortedSampleIndices_[featureIndex][sortIndex + 1]][featureIndex])/2.0;

            if (boostingType_ == 0) {
                double classifierWeight = log((1.0 - error)/error)/2.0;
                outputLarger *= classifierWeight;
                outputSmaller *= classifierWeight;
            }

            optimalClassifier.set(featureIndex, classifierThreshold, outputLarger, outputSmaller, error);
        }
    }

    return optimalClassifier;
}

void AdaBoost::computeClassifierOutputs(const double weightSumLarger,
                                        const double weightLabelSumLarger,
                                        const double positiveWeightSumLarger,
                                        const double negativeWeightSumLarger,
                                        const double positiveWeightSumLargerRev,
                                        const double negativeWeightSumLargerRev,
                                        double &outputLarger,
                                        double &outputSmaller) const
{
    if (boostingType_ == 0) {
        // Discrete AdaBoost
        if (weightLabelSumLarger > 0) {
            outputLarger = 1.0;
            outputSmaller = -1.0;
        } else {
            outputLarger = -1.0;
            outputSmaller = 1.0;
        }
    } else if (boostingType_ == 1) {
        // Real AdaBoost
        const double epsilonReal = 0.0001;
        outputLarger = log((positiveWeightSumLarger + epsilonReal)/(negativeWeightSumLarger + epsilonReal))/2.0;
        outputSmaller = log((positiveWeightSum_ - positiveWeightSumLarger + epsilonReal)
                            /(negativeWeightSum_ - negativeWeightSumLarger + epsilonReal))/2.0;
    } 
    // modest adaboost
    else if (boostingType_ == 3) {
        outputLarger = (positiveWeightSumLarger * (1 - positiveWeightSumLargerRev)) - (negativeWeightSumLarger * (1 - negativeWeightSumLargerRev));
        outputSmaller = ((positiveWeightSum_-positiveWeightSumLarger) * (1 - (positiveWeightSum_ - positiveWeightSumLargerRev ))) 
        - ((negativeWeightSum_ - negativeWeightSumLarger) * (1 - (negativeWeightSum_-negativeWeightSumLargerRev)));
    } else {
        // Gentle AdaBoost
        outputLarger = weightLabelSumLarger/weightSumLarger;
        outputSmaller = (weightLabelSum_ - weightLabelSumLarger)/(weightSum_ - weightSumLarger);
    }
}

double AdaBoost::computeError(const double positiveWeightSumLarger,
                              const double negativeWeightSumLarger,
                              const double outputLarger,
                              const double outputSmaller) const
{
    double error = 0.0;
    if (boostingType_ == 0) {
        // Discrete AdaBoost
        error = positiveWeightSumLarger*(1.0 - outputLarger)/2.0
                + (positiveWeightSum_ - positiveWeightSumLarger)*(1.0 - outputSmaller)/2.0
                + negativeWeightSumLarger*(-1.0 - outputLarger)/-2.0
                + (negativeWeightSum_ - negativeWeightSumLarger)*(-1.0 - outputSmaller)/-2.0;
    } else if (boostingType_ == 1) {
        // Real AdaBoost
        error = positiveWeightSumLarger*exp(-outputLarger)
                + (positiveWeightSum_ - positiveWeightSumLarger)*exp(-outputSmaller)
                + negativeWeightSumLarger*exp(outputLarger)
                + (negativeWeightSum_ - negativeWeightSumLarger)*exp(outputSmaller);
        // modest adaboost
    } else if (boostingType_ == 3) {
        error = positiveWeightSumLarger*(1.0 - outputLarger)*(1.0 - outputLarger)
                + (positiveWeightSum_ - positiveWeightSumLarger)*(1.0 - outputSmaller)*(1.0 - outputSmaller)
                + negativeWeightSumLarger*(-1.0 - outputLarger)*(-1.0 - outputLarger)
                + (negativeWeightSum_ - negativeWeightSumLarger)*(-1.0 - outputSmaller)*(-1.0 - outputSmaller);
    } else {
        // Gentle AdaBoost
        error = positiveWeightSumLarger*(1.0 - outputLarger)*(1.0 - outputLarger)
                + (positiveWeightSum_ - positiveWeightSumLarger)*(1.0 - outputSmaller)*(1.0 - outputSmaller)
                + negativeWeightSumLarger*(-1.0 - outputLarger)*(-1.0 - outputLarger)
                + (negativeWeightSum_ - negativeWeightSumLarger)*(-1.0 - outputSmaller)*(-1.0 - outputSmaller);
    }

    return error;
}

void AdaBoost::updateWeight(const AdaBoost::DecisionStump& bestClassifier) {
    double updatedWeightSum = 0.0;
    for (int sampleIndex = 0; sampleIndex < sampleTotal_; ++sampleIndex) {
        int labelInteger;
        if (labels_[sampleIndex]) labelInteger = 1;
        else labelInteger = -1;
#ifdef MADABOOST
        madaboostEvalValues_[sampleIndex] *= exp(-1.0*labelInteger*bestClassifier.evaluate(samples_[sampleIndex]));
        if (madaboostEvalValues_[sampleIndex] < 1){
            weights_[sampleIndex] = (1.0 / sampleTotal_) * madaboostEvalValues_[sampleIndex];
        } else {
            weights_[sampleIndex] = 1.0 / sampleTotal_;
        }
#elif defined (LOGITBOOST)
        weights_[sampleIndex] *= log(1+exp(2*(-1.0*labelInteger*bestClassifier.evaluate(samples_[sampleIndex]))));

#elif defined (ETABOOST)
        double n = .5;
         etaboostEvalValues_[sampleIndex] *= exp(-1.0*labelInteger*bestClassifier.evaluate(samples_[sampleIndex]));
        if (etaboostEvalValues_[sampleIndex] < 1){
            weights_[sampleIndex] = (1/pow(n,2))*(((1-n)*(exp(etaboostEvalValues_[sampleIndex])-1)*n) + 
                ((2*n - 1) *(log(1+((exp(etaboostEvalValues_[sampleIndex])-1)*n)))));
        } else {
            weights_[sampleIndex] = 1.0 / sampleTotal_;
        }           
#else
        weights_[sampleIndex] *= exp(-1.0*labelInteger*bestClassifier.evaluate(samples_[sampleIndex]));
#endif
        updatedWeightSum += weights_[sampleIndex];
    }

    for (int sampleIndex = 0; sampleIndex < sampleTotal_; ++sampleIndex) {
        weights_[sampleIndex] /= updatedWeightSum;
    }
}


void AdaBoost::writeFile(const std::string filename) const {
    std::ofstream outputModelStream(filename.c_str(), std::ios_base::out);
    if (outputModelStream.fail()) {
        std::cerr << "error: can't open file (" << filename << ")" << std::endl;
        exit(1);
    }

    int roundTotal = static_cast<int>(weakClassifiers_.size());
#ifdef EARLY_TERMINATION
    outputModelStream << roundTotal << " " << positives << " " << negatives << std::endl;
#else
    outputModelStream << roundTotal << std::endl;
#endif
    for (int roundIndex = 0; roundIndex < roundTotal; ++roundIndex) {
        outputModelStream << weakClassifiers_[roundIndex].featureIndex() << " ";
        outputModelStream << weakClassifiers_[roundIndex].threshold() << " ";
        outputModelStream << weakClassifiers_[roundIndex].outputLarger() << " ";
        outputModelStream << weakClassifiers_[roundIndex].outputSmaller() << std::endl;
    }

    outputModelStream.close();
}

void AdaBoost::readFile(const std::string filename) {
    std::ifstream inputModelStream(filename.c_str(), std::ios_base::in);
    if (inputModelStream.fail()) {
        std::cerr << "error: can't open file (" << filename << ")" << std::endl;
        exit(1);
    }

    int roundTotal;
#ifdef EARLY_TERMINATION
    inputModelStream >> roundTotal;
    inputModelStream >> positives;
    inputModelStream >> negatives;
    sampleTotal_ = positives + negatives;
#else
    inputModelStream >> roundTotal;
#endif
    weakClassifiers_.resize(roundTotal);
    for (int roundIndex = 0; roundIndex < roundTotal; ++roundIndex) {
        int featureIndex;
        double threshold, outputLarger, outputSmaller;
        inputModelStream >> featureIndex;
        inputModelStream >> threshold;
        inputModelStream >> outputLarger;
        inputModelStream >> outputSmaller;

        weakClassifiers_[roundIndex].set(featureIndex, threshold, outputLarger, outputSmaller);
    }

    inputModelStream.close();
}
