#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Rock.h"
#include "RatioVar.h"
#include<iostream>
namespace ThGkatz
{


	Rock::Rock() : Obstacle()
	{}

	Rock::Rock(b2World& world, sf::Vector2i position, float radius) : Obstacle(world, position, radius)
	{

		setShape(createShape(position, radius));
	}

	Rock::Rock(b2World& world, sf::Vector2i position, sf::Vector2f size) : Obstacle(world, position, size)
	{
		setShape(createShape(position , size));
	}

	Rock::~Rock() 
	{}

	//position in METERS
	sf::CircleShape* Rock::createShape(sf::Vector2i position, float radius)
	{
		float RATIO = RatioVar::instance()->getRATIO();
		sf::Texture* myTexture = new sf::Texture;
		
		if (!myTexture->loadFromFile("rock.png"))
		{
			std::cout << "Error with the Rock image\n";
		}
		setTexture(myTexture);

		sf::CircleShape* myShape = new sf::CircleShape(radius*RATIO);
		myShape->setOrigin(radius*RATIO, radius*RATIO);
		myShape->setPosition(position.x*RATIO, position.y*RATIO);
		myShape->setTexture(myTexture);

		return myShape;
	}
	//position , size in METERS
	sf::RectangleShape* Rock::createShape(sf::Vector2i position, sf::Vector2f size)
	{
		float RATIO = RatioVar::instance()->getRATIO();

		sf::Texture* myTexture = new sf::Texture;
		
		if (!myTexture->loadFromFile("rock.png", sf::IntRect(0, 0, size.x*RATIO * 4, size.y*RATIO * 4)))
		{
			std::cout << "Error with the Rock image\n";
		}
		setTexture(myTexture);
		size.x = size.x*RATIO;
		size.y = size.y*RATIO;
		sf::RectangleShape* myShape = new sf::RectangleShape(size);//size is now in Pixels
		myShape->setOrigin(size.x*0.5, size.y*0.5);
		myShape->setPosition(position.x*RATIO, position.y*RATIO);
		myShape->setTexture(myTexture);

		return myShape;

	}


}