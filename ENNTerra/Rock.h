#pragma once
#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Obstacle.h"

#ifndef ROCK_H
#define ROCK_H

namespace ThGkatz
{
	class Rock : public Obstacle
	{
	public:
		//default constructor
		Rock();
		//constructor for Rock type Obstacles with Circle shape
		Rock(b2World&, sf::Vector2i, float);
		//constructor for Rock type Obstacles with Circle shape
		Rock(b2World&, sf::Vector2i, sf::Vector2f);
		//deconstructor
		virtual ~Rock();

		void checkCollision(Entity*) {/*do nothing*/};
		
	private:
		sf::CircleShape* createShape(sf::Vector2i, float);
		sf::RectangleShape* createShape(sf::Vector2i, sf::Vector2f);
		
	};
}

#endif // !ROCK_H
