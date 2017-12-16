#include "Box2D\Box2D.h"
#include "SFML\Graphics.hpp"
#include "Guardian.h"
#include "Predator.h"
#include "River.h"
#include "RatioVar.h"
#include "Stimulus.h"
#include "GlobalVariables.h"
#include <iostream>
#include <array>
#include <list>

using namespace GlobalVariables ;

namespace ThGkatz
{
	Guardian::Guardian() : Organism() {
		
		roundSensor = b2FixtureDef();
	}

	Guardian::Guardian(b2World& world, sf::Vector2i position) : Organism(world, position , 27)
	{
		setSpeed(45);//this is the maximum speed a guardian can achieve . ( 50 for gatherers , 65 for predator)
		setShape(createShape(position));
		//setNumberOfNeuralInputs(27);//4 inputs for each entity type (plus river) (24) and 2 more for energy , moisture plus one for bool nearGatherer
		setNeuralInputs(std::vector<float>(getNumberOfNeuralInputs(), 0));
		setStimuli(std::vector<std::list< Stimulus* >>(7));
		roundSensor = *createRoundSensor();

		getBody()->CreateFixture(&roundSensor);
	}

	Guardian::~Guardian() {
		
	}

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
		else if (Predator* other = dynamic_cast<Predator*>(otherEntity)) {
			int myRandomNumber = rand() % 100 + 1;
			if (myRandomNumber > GUARDIAN_TO_PREDATOR) {
				setDeadManWalking(true);
				other->feed(PREDATOR_FOOD_GUARDIAN);
			}
			else {
				feed(GUARDIAN_FOOD_PREDATOR);
				other->setDeadManWalking(true);
			}
		}
		
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
				if (this != NULL)
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

		if ((int)getClock()->getElapsedTime().asSeconds()%2==0&&!getLostEnergy())
		{
			//if a guardian is near one or more gatherers he can feed
			if (isNearGatherers())
			{
				feed(GUARDIAN_FOOD_PET);
			}
			else//if not he hungers
				setEnergy(getEnergy() - 1);

			setMoisture(getMoisture() - 1);
			setLostEnergy(true);
		}
		else if ((int)getClock()->getElapsedTime().asSeconds() % 2 != 0) setLostEnergy(false);
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
				if (nearGatherers.size()>0)
				nearGatherers.erase(nearGatherers.begin() + i);
			}		
		}
		createBrainInput();
	}

	void Guardian::createBrainInput()
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
		//now compute the distance and angle for all the gatherers near the guardian
		std::vector<Entity*> nearGatherers = getNearGatherers();

		for (short i = 0; i < nearGatherers.size(); i++) {
			Stimulus* stim = new Stimulus;
			std::array<float, 2> distAngleArray = { 0,0 };
			try {
				Organism* temp = dynamic_cast<Organism*>(nearGatherers[i]);
				b2Body* body = temp->getBody();
				distAngleArray = getDistanceAndAngleCircle(body, 'y');
				stim->distance = distAngleArray[0];
				stim->angle = distAngleArray[1];
				stim->type = EntityTypes::NEARGATHERER;
				addStimulus(stim);
			}
			catch (...) {
				continue;
			}
			
		}
		//create the actual array of inputs for the neural net.
		createNeuralInputs();
	}

	void Guardian::createNeuralInputs() {
	
		getNeuralInputs().clear();
		//temp vector for the neuralInputs
		std::vector<float> myTempInputs(getNumberOfNeuralInputs(), 0);

		std::vector<std::list<Stimulus*>> myStimuli = getStimuli();
		//for each stimuli type list 
		int counter = 2;
		for (int i = 0; i < myStimuli.size(); i++) {
			if (i == EntityTypes::NEARGATHERER) continue;
			std::list<Stimulus*>::iterator it;
			//for each stimulus object
			Stimulus* tempClosest = new Stimulus;
			tempClosest->distance = 0;
			tempClosest->angle = 0;
			Stimulus* tempAverage = new Stimulus;
			tempAverage->distance = 0;
			tempAverage->angle = 0;
			float totalDistance = 0;
			float totalAngle = 0;
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
			
			//check definition of Organism::neuralInputs variable for information of the following code
			if (i != EntityTypes::NEARGATHERER) {
				myTempInputs[4 * i] = tempClosest->distance;
				myTempInputs[4 * i + 1] = tempClosest->angle;
				myTempInputs[i + counter] = tempAverage->distance;
				myTempInputs[i + counter + 1] = tempAverage->angle;
				counter += 3;
			}
		}//end for stimuli
		myTempInputs[myTempInputs.size() - 3] = (isNearGatherers()?1:0); //near gatherers boolean var takes 1 neuron(Guardian only)
		myTempInputs[myTempInputs.size() - 2] = getEnergy(); //energy takes 1 neuron
		myTempInputs[myTempInputs.size() - 1] = getMoisture();//moisture takes one neuron

		//set the new NeauralInputs vector.
		setNeuralInputs(myTempInputs);
		//call the think function and move accordingly
		think();
	}

}