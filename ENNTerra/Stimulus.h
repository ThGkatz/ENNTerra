#pragma once
#include "Organism.h"

#ifndef STIMULUS_H
#define STIMULUS_H

namespace ThGkatz
{
	enum EntityTypes { RIVER, ROCK, BUSH, VIGATHERER, GUARDIAN, PREDATOR, NEARGATHERER};
	struct Stimulus
	{
		float distance;
		float angle;
		EntityTypes type;
	};
}
#endif // !STIMULUS_H
