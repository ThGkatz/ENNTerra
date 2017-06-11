#pragma once
#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Entity.h"


#ifndef OBSTACLE_H
#define OBSTACLE_H
namespace ThGkatz
{
	class Obstacle : public Entity
	{	
	public:
		 b2Body* getBody() const;
		const sf::Shape* getShape() const;
		const sf::Texture* getTexture() const;
		const b2BodyDef& getBodyDef() const;
		const b2Shape* getPolyShape() const;
		const b2FixtureDef& getFixtureDef() const;
		void setShape(sf::Shape*);
		void setTexture(sf::Texture*);
		virtual void checkCollision(Entity*)=0;
		virtual ~Obstacle();
	private:
		//Member data
		b2Body* body;
		sf::Shape* shape ;
		sf::Texture* texture;
		b2BodyDef bodyDef;
		b2Shape* polyShape;
		b2FixtureDef fixtureDef;

	private:
		virtual sf::CircleShape* createShape(sf::Vector2i, float radius) = 0;

	protected:
		//default constructor
		Obstacle();
		//constructor for any Obstacle derrived class with a circle shape.
		//the member attribute shape is initiallised as NULL
		Obstacle( b2World& world, sf::Vector2i , float radius);
		//constructor for any Obstacle derrived class with a rectangle shape.
		//the member attribute shape is initiallised as NULL
		Obstacle(b2World& world, sf::Vector2i, sf::Vector2f);

		
	};
}//ThGkatz
#endif //OBSTACLE_H

