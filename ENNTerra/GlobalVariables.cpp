#include "GlobalVariables.h"
namespace GlobalVariables
{
	int  GUARDIAN_TO_PREDATOR = 80;
	int  PREDATOR_TO_GATHERER = 80;
	int  GATHERER_TO_GUARDIAN = 80;
	int GATHERER_FOOD_BUSH = 5;
	int GATHERER_FOOD_PREDATOR = 2;
	int GATHERER_FOOD_GUARDIAN = 2;
	int PREDATOR_FOOD_GUARDIAN = 3;
	int PREDATOR_FOOD_GATHERER = 5;
	int GUARDIAN_FOOD_PREDATOR = 4;
	int GUARDIAN_FOOD_GATHERER = 3;
	int GUARDIAN_FOOD_PET = 2;
	int initializationNumberOfOrganisms = 5;
	int numberOfPredators = initializationNumberOfOrganisms / 3;
	int numberOfGuardians = initializationNumberOfOrganisms / 3;
	int numberOfGatherers = initializationNumberOfOrganisms / 3;
	int energyLostPerSecond = 1;
	int moistureLostPerSecond = 1;
	int intervalBetweenGenerations = 8;
	int numberOfChildrenPredator = 1;
	int numberOfChildrenGuardian = 1;
	int numberOfChildrenGatherer = 1;
}
