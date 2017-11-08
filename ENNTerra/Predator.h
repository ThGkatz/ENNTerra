#pragma once
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "Organism.h"
#include "Gatherer.h"

#ifndef PREDATOR_H
#define PREDATOR_H
namespace ThGkatz
{
	class Predator : public Organism
	{
	public:
		//default constructor
		Predator();
		Predator(b2World&, sf::Vector2i);
		//deconstructor
		~Predator();
		void checkCollision(Entity*);
		Gatherer* getClosestGatherer();
		void setClosestGatherer(Gatherer*);
	private:
		//the predator can 'sense' the closest alive gatherer 
		Gatherer* closestGatherer;
	private:
		sf::ConvexShape* createShape(sf::Vector2i);
		//virtual method. check Organism abstract class
		void createBrainInput();
		//virtual method. check Organism abstract class
		void createNeuralInputs();
	};
}

#endif // !PREDATOR_H
