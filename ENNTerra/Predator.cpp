#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "Predator.h"
#include "RatioVar.h"
#include "River.h"
#include "Gatherer.h"
#include "Guardian.h"
#include <iostream>
#include <array>

namespace ThGkatz
{
	Predator::Predator() : Organism()
	{}

	Predator::Predator(b2World& world, sf::Vector2i position) : Organism(world, position)
	{
		setSpeed(65);//this is the maximum speed a predator can achieve . ( 50 for gatherers , 60 for guardians)
		setShape(createShape(position));
	}

	Predator::~Predator() {}

	Gatherer* Predator::getClosestGatherer()
	{
		return closestGatherer;
	}

	void Predator::setClosestGatherer(Gatherer* closestG)
	{
		closestGatherer = closestG;
	}

	sf::ConvexShape* Predator::createShape(sf::Vector2i position)
	{
		float RATIO = RatioVar::instance()->getRATIO();

		sf::Texture* myTexture = new sf::Texture();
		if (!myTexture->loadFromFile("Predator.jpg", sf::IntRect(600, 650, 50, 50)))
		{
			std::cout << "Error with the predator texture" << std::endl;
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

	void Predator::checkCollision(Entity* otherEntity)
	{
		if (River* temp = dynamic_cast<River*>(otherEntity))
		{
			drink();
		}
		else if (Gatherer* temp = dynamic_cast<Gatherer*>(otherEntity))
		{
			feed(); 
		}
		else if (Guardian* temp = dynamic_cast<Guardian*>(otherEntity))
			setDeadManWalking(true);
	}

	void Predator::createBrainInput()
	{

		std::vector<Entity*> visibles = getVisibleEntities();
		//for each visible entity
		for (unsigned int i = 0; i < visibles.size(); i++) {
			Stimulus* myStim;
			std::array<float, 2> distAngleArray = { 0,0 };
			//organisms count as circle shapes (don't ask why)
			if (Organism* temp = dynamic_cast<Organism*>(visibles[i])) {
				 b2Body* body = temp->getBody();
				distAngleArray = getDistanceAndAngleCircle(body , 'y');
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

				if (temp->getFixtureDef().shape->GetType() == b2Shape::Type::e_polygon) {
					 b2Body* body = temp->getBody();
					distAngleArray = getDistanceAndAnglePolygon(body);
				}
				else if (temp->getFixtureDef().shape->GetType() == b2Shape::Type::e_circle){
					 b2Body* body = temp->getBody();
					distAngleArray = getDistanceAndAngleCircle(body , 'y');
					
				}
			}
			//rivers are only polygonal
			else if (River* temp = dynamic_cast<River*>(visibles[i])){
				 b2Body* body = temp->body;
				distAngleArray = getDistanceAndAnglePolygon(body);
				myStim->type = EntityTypes::RIVER;
			}
			myStim->distance = distAngleArray[0];
			myStim->angle = distAngleArray[1];
			addStimulus(myStim);//add the new entity angle , distance and type to the stimuli
		}
		Stimulus* stim;
		std::array<float, 2> distAngleArrayGath = { 0,0 };
		if (getClosestGatherer() == nullptr) {
			distAngleArrayGath = { 0,0 };
		}
		else {
			b2Body* body = getClosestGatherer()->getBody();
			distAngleArrayGath = getDistanceAndAngleCircle(body, 'n');
		}
		stim->distance = distAngleArrayGath[0];
		stim->angle = distAngleArrayGath[1];
		stim->type = EntityTypes::NEARGATHERER;
		addStimulus(stim);

	}
}