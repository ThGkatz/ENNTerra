#pragma once
#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "Organism.h"
#include "Predator.h"
#include "River.h"
#include "Gatherer.h"
#include <vector>

#ifndef GUARDIAN_H
#define GUARDIAN_H

namespace ThGkatz
{
	class Guardian : public Organism
	{
	public:
		//default constructor
		Guardian();
		Guardian(b2World&, sf::Vector2i);
		//deconstructor
		~Guardian();
		//checks the exact type of the collided Entity and proceeds accordingly 
		void checkCollision(Entity*);
		//while being near a gatherer , replenish one point of energy per second
		void feed();
		//adds a gatherer to the vector of them being near the guardian
		void gathererEnter(Entity*);
		//after a collision has ended with the roundSensor and a gatherer 
		//this method is called to substract it from the vector
		void gathererLost(Entity*);
		/*this method returns true if there are gatherers in the proximity of the 
		round sensor of a guardian . */
		bool isNearGatherers();
		/*the basic method for updating the physical properties of the guardian ,
		as well as handling the feed function and the parameters miosture and energy .
		the deathclock is ticking in this function*/
		void update();
		std::vector<Entity*> getNearGatherers();


	private://attributes
		//a round fixture that senses gatherers near the guardian
		b2FixtureDef roundSensor;
		//the gatherers that are near the guardian
		std::vector<Entity*> nearGatherers;


	private://methods
		//creates and returns the triangular shape in SFML graphics
		sf::ConvexShape* createShape(sf::Vector2i);
		//creates and returns the round sensor that is uniquely implemented in the guardians
		b2FixtureDef* createRoundSensor();
		void createBrainInput();
	};


}
#endif // !GUARDIAN_H
