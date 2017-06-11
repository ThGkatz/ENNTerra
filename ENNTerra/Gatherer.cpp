#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Gatherer.h"
#include "Bush.h"
#include "Rock.h"
#include "River.h"
#include"Predator.h"
#include "Guardian.h"
#include "RatioVar.h"
#include <iostream>
#include<cmath>
#include <array>


namespace ThGkatz
{
	
	
	

	Gatherer::Gatherer() : Organism()
	{}

	Gatherer::Gatherer(b2World& world, sf::Vector2i position) : Organism(world, position)
	{
		setSpeed(50);//this is the maximum speed a gatherer can achieve . ( 65 for predators , 60 for guardians)
		setShape(createShape(position));
	}

	Gatherer::~Gatherer()
	{}

	sf::ConvexShape* Gatherer::createShape(sf::Vector2i position)
	{
		float RATIO = RatioVar::instance()->getRATIO();

		sf::Texture* myTexture = new sf::Texture();
		if (!myTexture->loadFromFile("Gatherer.jpg", sf::IntRect(550, 200, 50, 50)))
		{
			std::cout << "Error with the gatherer texture" << std::endl;
		}
		setTexture(myTexture);

		sf::ConvexShape* myShape = new sf::ConvexShape();
		myShape->setPointCount(3);
		myShape->setPoint(0, sf::Vector2f(0, 4 * RATIO));
		myShape->setPoint(1, sf::Vector2f(2 * RATIO, 4 * RATIO));
		myShape->setPoint(2, sf::Vector2f(RATIO, 0));
		myShape->setTexture(myTexture);
		myShape->setOrigin(1 * RATIO, 2 * RATIO);
		myShape->setPosition(position.x*RATIO, position.y*RATIO);

		return myShape;
		
	}

	void Gatherer::checkCollision(Entity* otherEntity)
	{
		//in case we hit a bush we can feed but only if it actually has food and we are hungry (energy <= 60%)
		if (Bush* other = dynamic_cast<Bush*>(otherEntity))
		{
			if (getEnergy() <= 6) {
				if (other->giveFood())
					feed();
			}
			
		}
		else if (Rock* other = dynamic_cast<Rock*>(otherEntity))
			std::cout << "I hit a rock" << std::endl;
		else if (River* other = dynamic_cast<River*>(otherEntity))
		{
			drink();
		}
		else if (Predator* other = dynamic_cast<Predator*>(otherEntity))
			setDeadManWalking(true);
	}
	
	void Gatherer::createBrainInput()
	{
		std::vector<Entity*> visibles = getVisibleEntities();
		//for each visible entity
		for (unsigned int i = 0; i < visibles.size(); i++) {
			Stimulus* myStim;
			std::array<float, 2> distAngleArray = { 0,0 };
			//organisms count as circle shapes (don't ask why)
			if (Organism* temp = dynamic_cast<Organism*>(visibles[i])) {
				b2Body* body = temp->getBody();
				distAngleArray = getDistanceAndAngleCircle(body, 'y');
				//check the organism's type
				if (Gatherer* myTemp = dynamic_cast<Gatherer*>(temp)) myStim->type = EntityTypes::VIGATHERER;//gatherer
				else if (Guardian* myTemp = dynamic_cast<Guardian*>(temp)) myStim->type = EntityTypes::GUARDIAN;//guardian
				else myStim->type = EntityTypes::PREDATOR;//predator
			}
			//obstacles can be both type of shapes
			else if (Obstacle* temp = dynamic_cast<Obstacle*>(visibles[i])) {
				//check the type of the obstacle
				if (Rock* myTemp = dynamic_cast<Rock*>(temp)) myStim->type = EntityTypes::ROCK;//it's a rock
				else myStim->type = EntityTypes::BUSH;//bush
				//check the fixture type
				if (temp->getFixtureDef().shape->GetType() == b2Shape::Type::e_polygon) {
					b2Body* body = temp->getBody();
					distAngleArray = getDistanceAndAnglePolygon(body);
				}
				else if (temp->getFixtureDef().shape->GetType() == b2Shape::Type::e_circle) {
					b2Body* body = temp->getBody();
					distAngleArray = getDistanceAndAngleCircle(body, 'y');

				}
			}
			//rivers are only polygonal
			else if (River* temp = dynamic_cast<River*>(visibles[i])) {
				b2Body* body = temp->body;
				distAngleArray = getDistanceAndAnglePolygon(body);
				myStim->type = EntityTypes::RIVER;
			}
			myStim->distance = distAngleArray[0];
			myStim->angle = distAngleArray[1];
			addStimulus(myStim);//add the new entity angle , distance and type to the stimuli
		}
	}



}