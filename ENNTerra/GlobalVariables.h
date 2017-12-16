#pragma once
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

namespace GlobalVariables {
	extern int GUARDIAN_TO_PREDATOR;
	extern int PREDATOR_TO_GATHERER;
	extern int GATHERER_TO_GUARDIAN;
	extern int GATHERER_FOOD_BUSH;
	extern int GATHERER_FOOD_PREDATOR;
	extern int GATHERER_FOOD_GUARDIAN;
	extern int PREDATOR_FOOD_GUARDIAN;
	extern int PREDATOR_FOOD_GATHERER;
	extern int GUARDIAN_FOOD_PREDATOR;
	extern int GUARDIAN_FOOD_GATHERER;
	extern int GUARDIAN_FOOD_PET;
	extern int initializationNumberOfOrganisms;
	extern int numberOfPredators;
	extern int numberOfGuardians;
	extern int numberOfGatherers;
	extern int energyLostPerSecond;
	extern int moistureLostPerSecond;
	extern int intervalBetweenGenerations;
	extern int numberOfChildrenPredator;
	extern int numberOfChildrenGuardian;
	extern int numberOfChildrenGatherer;
}


#endif // !GLOBALVARIABLES_H