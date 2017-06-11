#pragma once
#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Obstacle.h"

#ifndef BUSH_H
#define BUSH_H

namespace ThGkatz
{
	class Bush : public Obstacle
	{
	public :
		//default constructor
		Bush();
		//creates a circle shaped object of Bush
		Bush(b2World&, sf::Vector2i, float);
		//deconstructor
		virtual ~Bush();
	
		const int getFood() const;
		void setFood(int);
		//resupplies the bush object adding 1 point of food .
		void updateFood();
		//gives one point of food if there is any , at a gatherer .
		//returns true if successful , false if not .
		bool giveFood();
		void checkCollision(Entity*);
	private:
		int food;
		sf::CircleShape* createShape(sf::Vector2i, float);


	};
}

#endif // !BUSH_H
