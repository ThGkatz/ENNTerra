#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "Guardian.h"
#include "Predator.h"
#include "River.h"
#include "RatioVar.h"
#include <iostream>
#include <array>

namespace ThGkatz
{
	Guardian::Guardian() : Organism() {
		
		roundSensor = b2FixtureDef();
	}

	Guardian::Guardian(b2World& world, sf::Vector2i position) : Organism(world, position)
	{
		setSpeed(60);//this is the maximum speed a guardian can achieve . ( 50 for gatherers , 65 for predator)
		setShape(createShape(position));
		
		roundSensor = *createRoundSensor();

		getBody()->CreateFixture(&roundSensor);
	}

	Guardian::~Guardian() {}

	sf::ConvexShape* Guardian::createShape(sf::Vector2i position)
	{
		float RATIO = RatioVar::instance()->getRATIO();

		sf::Texture* myTexture = new sf::Texture();
		if (!myTexture->loadFromFile("Guardian.jpg", sf::IntRect(200, 200, 50, 50)))
		{
			std::cout << "Error with the guardian texture" << std::endl;
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

	std::vector<Entity*> Guardian::getNearGatherers() {
		return nearGatherers;
	}

	void Guardian::checkCollision(Entity* otherEntity)
	{
		if (River* temp = dynamic_cast<River*>(otherEntity))
			drink();
		
	}

	void Guardian::feed()
	{
		if (getEnergy() < 10)
			setEnergy(getEnergy()+1);

	}

	b2FixtureDef* Guardian::createRoundSensor()
	{
		b2CircleShape* circleShape = new b2CircleShape;
		circleShape->m_radius = 8; //almost half of the vision radius (to make it hard)
		
		b2FixtureDef* myFixDef = new b2FixtureDef;
		myFixDef->shape = circleShape;
		myFixDef->isSensor = true;
		return myFixDef;
	}

	void Guardian::gathererEnter(Entity* other) {
		//in case we indeed have a gatherer , add it to the pile 
		if (Gatherer* temp = dynamic_cast<Gatherer*>(other))
		{
			nearGatherers.push_back(other);
			
		}
	}
	void Guardian::gathererLost(Entity* other) {
		if (Gatherer* temp = dynamic_cast<Gatherer*>(other)) {
			nearGatherers.erase(std::find(nearGatherers.begin(), nearGatherers.end(), other));
		}
	}

	bool Guardian::isNearGatherers()
	{
		
		if (nearGatherers.size() > 0 )
			return true;
		else
			return false;
		
	}

	void Guardian::update()
	{
		float RATIO = RatioVar::instance()->getRATIO();

		float angle = getBody()->GetAngle() / 3.14 * 180;
		while (angle <= 0) {
			angle += 360;
		}
		while (angle > 360) {
			angle -= 360;
		}

		getShape()->setRotation(angle);
		getShape()->setPosition(getBody()->GetPosition().x*RATIO, getBody()->GetPosition().y*RATIO);

		if ((int)getClock()->getElapsedTime().asSeconds() >= 2)
		{
			//if a guardian is near one or more gatherers he can feed
			if (isNearGatherers())
			{
				feed();
			}
			else//if not he hungers
				setEnergy(getEnergy() - 1);

			setMoisture(getMoisture() - 1);
			restartClock();
		}
		//boy it's time to die 
		if (getMoisture() <= 0 || getEnergy() <= 0)
			setDeadManWalking(true);

		for (int i = 0; i < nearGatherers.size(); i++)
		{
			
			try
			{
				//this is going to throw a nullptr exception when a gatherer dies while being near the guardian
				//it's the only way to erase a gatherer since the EndContact() method doesn't fire  .
				Organism* temp = dynamic_cast<Organism*>(nearGatherers[i]);
			}
			catch (...)
			{
				//if an exception is thrown it means a near gatherer has died . so just erase it from the vector
				nearGatherers.erase(nearGatherers.begin() + i);
			}
				
				
		}
	}

	void Guardian::createBrainInput()
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
		//now compute the distance and angle for all the gatherers near the guardian
		std::vector<Entity*> nearGatherers = getNearGatherers();
		for (short i = 0; i < nearGatherers.size(); i++) {
			Stimulus* stim;
			std::array<float, 2> distAngleArray = { 0,0 };
			Organism* temp = dynamic_cast<Organism*>(nearGatherers[i]);
			b2Body* body = temp->getBody();
			distAngleArray = getDistanceAndAngleCircle(body, 'y');
			stim->distance = distAngleArray[0];
			stim->angle = distAngleArray[1];
			stim->type = EntityTypes::NEARGATHERER;
			addStimulus(stim);
		}

	}

}