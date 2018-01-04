#include "myIncrementalGA.h"
#include <ga/garandom.h>
#include <ga/GA1DArrayGenome.h>

namespace ThGkatz {

	GAParameterList& myIncrementalGA::registerDefaultParameters(GAParameterList& p) {
		GAGeneticAlgorithm::registerDefaultParameters(p);

		p.add(gaNnOffspring, gaSNnOffspring,
			GAParameter::INT, &gaDefNumOff);

		p.set(gaNscoreFrequency, &gaDefScoreFrequency2);

		return p;
	}

	myIncrementalGA::myIncrementalGA(const GAGenome& gen) : GAGeneticAlgorithm(gen) {
		child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->individual(0).clone(GAGenome::ATTRIBUTES));
		child2 = dynamic_cast<GA1DArrayGenome<float>*>(pop->individual(0).clone(GAGenome::ATTRIBUTES));
		child1->geneticAlgorithm(*this);
		child2->geneticAlgorithm(*this);

		rs = WORST;
		rf = 0;

		noffspr = gaDefNumOff;
		params.add(gaNnOffspring, gaSNnOffspring, GAParameter::INT, &noffspr);

		stats.scoreFrequency(gaDefScoreFrequency2);
		params.set(gaNscoreFrequency, &gaDefScoreFrequency2);
	}

	myIncrementalGA::myIncrementalGA(const GAPopulation& pop) : GAGeneticAlgorithm(pop) {
		child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop.individual(0).clone(GAGenome::ATTRIBUTES));
		child2 = dynamic_cast<GA1DArrayGenome<float>*>(pop.individual(0).clone(GAGenome::ATTRIBUTES));
		child1->geneticAlgorithm(*this);
		child2->geneticAlgorithm(*this);

		rs = WORST;
		rf = 0;

		noffspr = gaDefNumOff;
		params.add(gaNnOffspring, gaSNnOffspring, GAParameter::INT, &noffspr);

		stats.scoreFrequency(gaDefScoreFrequency2);
		params.set(gaNscoreFrequency, &gaDefScoreFrequency2);
	}

	myIncrementalGA::myIncrementalGA(const myIncrementalGA& ga) : GAGeneticAlgorithm(ga) {
		child1 = (GA1DArrayGenome<float> *)0;
		child2 = (GA1DArrayGenome<float> *)0;
		copy(ga);
	}

	myIncrementalGA& myIncrementalGA::operator=(const myIncrementalGA& ga) {
		if (&ga != this) copy(ga);
		return *this;
	}

	myIncrementalGA::~myIncrementalGA() {
		delete child1;
		delete child2;
	}

	void myIncrementalGA::copy(const GAGeneticAlgorithm & g) {
		GAGeneticAlgorithm::copy(g);
		const myIncrementalGA& ga = DYN_CAST(const myIncrementalGA&, g);

		rs = ga.rs;
		rf = ga.rf;
		noffspr = ga.noffspr;

		if (child1) child1->copy(*ga.child1);
		else child1 = dynamic_cast<GA1DArrayGenome<float>*>(ga.child1->clone());
		if (child2) child2->copy(*ga.child2);
		else child2 = dynamic_cast<GA1DArrayGenome<float>*>(ga.child2->clone());
		child1->geneticAlgorithm(*this);
		child2->geneticAlgorithm(*this);
	}

	void myIncrementalGA::initialize(unsigned int seed) {
		GARandomSeed(seed);

		pop->initialize();
		pop->evaluate(gaTrue);

		stats.reset(*pop);

		if (!scross) GAErr(GA_LOC, className(), "initialize", gaErrNoSexualMating);
	}

	int myIncrementalGA::setptr(const char* name, const void* value) {
		int status = GAGeneticAlgorithm::setptr(name, value);

		if (strcmp(name, gaNnOffspring) == 0 ||
			strcmp(name, gaSNnOffspring) == 0) {
			nOffspring(*((int*)value));
			status = 0;
		}
		return status;
	}

	int myIncrementalGA::get(const char* name, void* value) const {
		int status = GAGeneticAlgorithm::get(name, value);

		if (strcmp(name, gaNnOffspring) == 0 ||
			strcmp(name, gaSNnOffspring) == 0) {
			*((int*)value) = noffspr;
			status = 0;
		}
		return status;
	}

	void myIncrementalGA::objectiveFunction(GAGenome::Evaluator f) {
		GAGeneticAlgorithm::objectiveFunction(f);
		child1->evaluator(f);
		child2->evaluator(f);
	}

	void myIncrementalGA::objectiveData(const GAEvalData& v) {
		GAGeneticAlgorithm::objectiveData(v);
		child1->evalData(v);
		child2->evalData(v);
	}

	int myIncrementalGA::nOffspring(unsigned int x) {
		if (x != 1 && x != 2) {
			GAErr(GA_LOC, className(), "numCrossStrategy", gaErrBadCS);
			noffspr = 1;
		}
		else {
			noffspr = x;
		}
		params.set(gaNnOffspring, x);
		return noffspr;
	}

	myIncrementalGA::ReplacementScheme myIncrementalGA::replacement(myIncrementalGA::ReplacementScheme n, ReplacementFunction f) {
		switch (n) {
		case BEST:
		case WORST:
		case RANDOM:
		case PARENT:
			rs = n;
			break;
		case CUSTOM:
			if (f) { rs = n; rf = f; }
			else GAErr(GA_LOC, className(), "replacement", gaErrNeedRS);
			break;
		default:
			GAErr(GA_LOC, className(), "replacement", gaErrBadRS);
			break;
		}
		return rs;
	}

	void myIncrementalGA::step() {
		int mut, c1, c2;
		GAGenome *mom, *dad;          // tmp holders for selected genomes

		mom = &(pop->select());
		dad = &(pop->select());

	
		stats.numsel += 2;		// keep track of the number of selections
		
		if (noffspr == 1) {
			c1 = 0;
			if (GAFlipCoin(pCrossover())) {
				stats.numcro += (*scross)(*mom, *dad, child1, (GAGenome*)0);
				c1 = 1;
			}
			else {
				if (GARandomBit())
					child1->copy(*mom);
				else
					child1->copy(*dad);
			}
			
			stats.nummut += (mut = child1->mutate(pMutation()));
			if (mut > 0) c1 = 1;

			stats.numeval += c1;
			child1->gene(child1->size() - 1, 1000);
			child2->gene(child2->size() - 1, 1000);
			if (rs == PARENT)
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, mom));
			else if (rs == CUSTOM)
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, &(rf(*child1, *pop))));
			else
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, rs));

			stats.numrep += 1;
		}
		else {
			c1 = c2 = 0;
			if (GAFlipCoin(pCrossover())) {
				stats.numcro += (*scross)(*mom, *dad, child1, child2);
				c1 = c2 = 1;
			}
			else {
				child1->copy(*mom);
				child2->copy(*dad);
			}
			stats.nummut += (mut = child1->mutate(pMutation()));
			if (mut > 0) c1 = 1;
			stats.nummut += (mut = child2->mutate(pMutation()));
			if (mut > 0) c2 = 1;

			stats.numeval += c1 + c2;
			child1->gene(child1->size() - 1, 1000);
			child2->gene(child2->size() - 1, 1000);
			if (rs == PARENT) {
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, mom));
				if (mom == dad)		// this is a possibility, if so do worst
					child2 = dynamic_cast<GA1DArrayGenome<float>*> (pop->replace(child2, GAPopulation::WORST));
				else
					child2 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child2, dad));
			}
			else if (rs == CUSTOM) {
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, &(rf(*child1, *pop))));
				child2 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child2, &(rf(*child2, *pop))));
			}
			else {
				child1 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child1, rs));
				child2 = dynamic_cast<GA1DArrayGenome<float>*>(pop->replace(child2, rs));
			}

			stats.numrep += 2;
		}

		pop->evaluate(gaTrue);        // allow pop-based evaluators to do their thing
		stats.update(*pop);		// update the statistics for this generation
	}
}