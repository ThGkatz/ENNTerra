#include "floatfann.h"
#include "fann_cpp.h"
#include <vector>


#ifndef NEURALNETWRAPPER_H
#define NEURALNETWRAPPER_H

namespace ThGkatz
{
	class NeuralNetWrapper
	{
	public:
		NeuralNetWrapper();
		NeuralNetWrapper(unsigned int, unsigned int, unsigned int);
		~NeuralNetWrapper();
		fann_type* run(fann_type*); //fann_type is float since we have included floatFann library.
		//setters
		void setNumOfInputs(unsigned int);
		void setNumOfLayers(unsigned int);
		void setNumOfOutputs(unsigned int);
		//getters
		const unsigned int getNumberOfInputs() const ;
		const unsigned int getNumberOfLayers() const ;
		const unsigned int getNumberOfOutputs() const ;
	private:
		FANN::neural_net net;
		unsigned int num_of_inputs;
		unsigned int num_of_layers;
		unsigned int num_of_outputs;

	};
}

#endif // !NEURALNETWRAPPER
