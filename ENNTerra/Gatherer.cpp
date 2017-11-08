#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Gatherer.h"
#include "Bush.h"
#include "Rock.h"
#include "River.h"
#include"Predator.h"
#include "Guardian.h"
#include "RatioVar.h"
#include "Stimulus.h"
#include <iostream>
#include<cmath>
#include <array>
#include <list>


namespace ThGkatz
{
	
	
	

	Gatherer::Gatherer() : Organism()
	{}

	Gatherer::Gatherer(b2World& world, sf::Vector2i position) : Organism(world, position)
	{
		setSpeed(50);//this is the maximum speed a gatherer can achieve . ( 65 for predators , 60 for guardians)
		setShape(createShape(position));
		setNumberOfNeuralInputs(26);//4 inputs for each entity type (plus river) (24) and 2 more for energy , moisture
		setNeuralInputs(std::vector<float>(getNumberOfNeuralInputs(), 0)); 
		setStimuli(std::vector<std::list< Stimulus* >> (6));
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
		emptyStimuli();
		std::vector<Entity*> visibles = getVisibleEntities();
		//for each visible entity
		for (unsigned int i = 0; i < visibles.size(); i++) {
			Stimulus* myStim = new Stimulus;
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
		//create the actual array of inputs for the neural net.
		createNeuralInputs();
	}

	void Gatherer::createNeuralInputs() {
		getNeuralInputs().clear();
		//temp vector for the neuralInputs
		std::vector<float> myTempInputs(getNumberOfNeuralInputs(),0);

		std::vector<std::list<Stimulus*>> myStimuli =  getStimuli();
		//for each stimuli type list 
		int counter = 2;
		for (int i = 0; i < myStimuli.size(); i++) {
			std::list<Stimulus*>::iterator it;
			//for each stimulus object
			Stimulus* tempClosest = new Stimulus;
			tempClosest->distance = 0;
			tempClosest->angle = 0;
			Stimulus* tempAverage = new Stimulus;
			tempAverage->distance = 0;
			tempAverage->angle = 0;
			float totalDistance = 0;
			float totalAngle =0;
			for (it = myStimuli[i].begin(); it != myStimuli[i].end(); ++it) {
				//find the closest of the list (add to neuralInputs temp vector)
				if (it == myStimuli[i].begin()) {
					tempClosest = *it;
				}
				else if ((*it)->distance <= tempClosest->distance) tempClosest = *it;
				//find the average of the list (add to neuralInputs temp vector)
				totalDistance += (*it)->distance;
				totalAngle += (*it)->angle;
			}//end for stimuli list
			if (totalDistance != 0) {
				tempAverage->angle = totalAngle / myStimuli[i].size();
				tempAverage->distance = totalDistance / myStimuli[i].size();
			}

			myTempInputs[4 * i] = tempClosest->distance;
			myTempInputs[4 * i +1] = tempClosest->angle;
			myTempInputs[i+counter] = tempAverage->distance;
			myTempInputs[i + counter +1] = tempAverage->angle;
			counter += 3;
		}//end for stimuli
		myTempInputs[myTempInputs.size() - 2] = getEnergy(); //energy takes 1 neuron
		myTempInputs[myTempInputs.size() - 1] = getMoisture();//moisture takes one neuron
		 //set the new NeauralInputs vector.
		setNeuralInputs(myTempInputs);
			
	}



}