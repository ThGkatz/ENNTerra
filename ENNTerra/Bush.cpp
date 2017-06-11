#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Bush.h"
#include"Gatherer.h"
#include "RatioVar.h"
#include<iostream>

namespace ThGkatz
{
	

	Bush::Bush() : Obstacle() , food(0) {}
	
	Bush::Bush(b2World& world, sf::Vector2i position, float radius) : Obstacle(world , position , radius) , food(2) 
	{
		setShape(createShape(position, radius));
	}

	Bush::~Bush()
	{}

	void Bush::setFood(int newFood) { food = newFood; }

	bool Bush::giveFood()
	{
		if (food > 0)
		{
			food--;
			return true;
		}
		else
			return false;
	}

	const int Bush::getFood() const { return food; }

	void Bush::updateFood()
	{
		if (food < 2)
			food++;
	}

	void Bush::checkCollision(Entity* otherEntity)
	{			
	}
	//position in meters 
	sf::CircleShape* Bush::createShape(sf::Vector2i position, float radius)
	{
		float RATIO = RatioVar::instance()->getRATIO();
		sf::Texture* myTexture = new sf::Texture;
		
		if (!myTexture->loadFromFile("bush.png"))
		{
			std::cout << "Error with the bush texture file\n";
		}
		setTexture(myTexture);
		sf::CircleShape* myShape = new sf::CircleShape(radius*RATIO);
		myShape->setOrigin(radius*RATIO , radius*RATIO);
		myShape->setPosition(position.x*RATIO, position.y*RATIO);
		myShape->setTexture(myTexture);

		return myShape;

	}
}