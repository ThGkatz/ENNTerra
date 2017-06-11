#pragma once
#include"Box2D\Box2D.h"
#include"SFML\Graphics.hpp"
#include"Organism.h"

#ifndef GATHERER_H
#define GATHERER_H

namespace ThGkatz
{
	class Gatherer : public Organism
	{
	public:
		//default constructor
		Gatherer();
		Gatherer(b2World&, sf::Vector2i);
		//deconstructor
		~Gatherer();
		
		void checkCollision(Entity*);
	private:
		sf::ConvexShape* createShape(sf::Vector2i);
		void createBrainInput();
		
	};
}//ThGkatz
#endif // !GATHERER_H
