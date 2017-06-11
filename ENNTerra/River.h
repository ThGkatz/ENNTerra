#pragma once
#include"Box2D\Box2D.h"
#include"SFML\Graphics.hpp"
#include"Entity.h"
#include"Bush.h"
#include"Rock.h"
#include<iostream>

#ifndef RIVER_H
#define RIVER_H
namespace ThGkatz
{
	struct River : public Entity
	{
		b2Body* body;
		sf::RectangleShape shape;
		b2BodyDef bodyDef;
		b2PolygonShape polyShape;
		b2FixtureDef fixDef;
		sf::Texture texture;

	public:
		//checks for a collision with other objects
		void checkCollision(Entity* otherEntity) {};
		
	};
}//ThGkatz
#endif // !RIVER_H
