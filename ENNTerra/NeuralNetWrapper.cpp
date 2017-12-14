#include "NeuralNetWrapper.h"
#include <array>
#include<iostream>

namespace ThGkatz {
	NeuralNetWrapper::NeuralNetWrapper() {
		//will see
	}

	NeuralNetWrapper::NeuralNetWrapper(unsigned int numOfInputs, unsigned int numOfOutputs, unsigned int numOfLayers) {
		num_of_inputs = numOfInputs;
		num_of_outputs = numOfOutputs;
		num_of_layers = numOfLayers;

		//dynamically create an array of neuron numbers per layer of the neural net
		float neuronNumberFix = 2;
		unsigned int* neuronsPerLayer;
		neuronsPerLayer = new unsigned int[numOfLayers];
		neuronsPerLayer[0] = numOfInputs;
		for (unsigned int i=1;i<numOfLayers -1;i++)
		{
			neuronsPerLayer[i] = numOfInputs*neuronNumberFix;
			neuronNumberFix -= 0.5;
		}
		neuronsPerLayer[numOfLayers -1] = numOfOutputs;
		//the neural net is created
		net.create_standard_array(numOfLayers , neuronsPerLayer);

		net.set_activation_function_hidden(FANN::ELLIOT_SYMMETRIC);
		net.set_activation_function_output(FANN::ELLIOT_SYMMETRIC);
		net.set_activation_steepness_hidden(1.0);
		net.set_activation_steepness_output(1.0);
	}

	NeuralNetWrapper::~NeuralNetWrapper() {
		net.destroy();
	}

	const unsigned int NeuralNetWrapper::getNumberOfInputs() const {
		return num_of_inputs;
	}

	const unsigned int NeuralNetWrapper::getNumberOfLayers() const {
		return num_of_layers;
	}

	const unsigned int NeuralNetWrapper::getNumberOfOutputs()const {
		return num_of_outputs;
	}

	void NeuralNetWrapper::getWeights(fann_connection* myConnection) {
		net.get_connection_array(myConnection);
	}

	const unsigned int NeuralNetWrapper::getWeightsLength()const {
		
		return (static_cast<FANN::neural_net>(net).get_total_connections());
	}

	void NeuralNetWrapper::setNumOfInputs(unsigned int numberOfInputs) {
		num_of_inputs = numberOfInputs;
	}

	void NeuralNetWrapper::setNumOfLayers(unsigned int numberOfLayers) {
		num_of_layers= numberOfLayers;
	}

	void NeuralNetWrapper::setNumOfOutputs(unsigned int numberOOutputs) {
		num_of_outputs = numberOOutputs;
	}

	void NeuralNetWrapper::setWeights(fann_connection* myConnection) {
		unsigned int numberOfConnections = net.get_total_connections();
		net.set_weight_array(myConnection, numberOfConnections);
	}

	fann_type* NeuralNetWrapper::run(fann_type* inputArray) {
		return net.run(inputArray);
	}

}