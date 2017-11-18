#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Organism.h"
#include "Gatherer.h"
#include "Bush.h"
#include "River.h"
#include "Rock.h"
#include "RatioVar.h"
#include "Stimulus.h"
#include <iostream>
#include<vector>
#include <array>
#include <list>

namespace ThGkatz 
{


	Organism::Organism()
	{
		body = nullptr;
		shape = nullptr;
		texture = nullptr;
		brain = nullptr;
		sensorDef = b2FixtureDef();
		myB2Shape = new b2PolygonShape();
		bodyDef = b2BodyDef();
		fixtureDef = b2FixtureDef();
		maxSpeed = 50;
		energy = 0;
		moisture = 0;
		deadManWalking = false;

	}

	Organism::Organism(b2World& world, sf::Vector2i position, int numberOfNeuralInputs)
	{
		maxSpeed = 0;
		energy = 10;
		moisture = 10;
		deadManWalking = false;
		deathClock = sf::Clock();

		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		body = world.CreateBody(&bodyDef);

		b2Vec2 vertices[3];
		vertices[0].Set(1, 2);
		vertices[1].Set(-1, 2);
		vertices[2].Set(0, -2);
		//each organism is 4 meters long , 2 meters wide .
		myB2Shape = new b2PolygonShape();
		myB2Shape->Set(vertices, 3);

		fixtureDef = b2FixtureDef();
		fixtureDef.shape = myB2Shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.9f;
		fixtureDef.restitution = 0.1f;
		body->CreateFixture(&fixtureDef);
		//damping is used to lessen the forces applied over time
		body->SetLinearDamping(0.9f);
		body->SetAngularDamping(0.9f);

		//the sensor fixture is created in method createSensor()
		sensorDef = *createSensor();
		body->CreateFixture(&sensorDef);

		body->SetUserData(this);
		shape = nullptr;
		setNumberOfNeuralInputs(numberOfNeuralInputs);
		//create an instance of neural network with 1 hidden layer and 3 output neurons
		brain = new NeuralNetWrapper(getNumberOfNeuralInputs(), 3 , 3);
	}

	Organism::~Organism()
	{
		//so...in order to avoid exceptions we have to first delete all the fixtures 
		//of an organisms and so call the EndContact method of the collision listener
		//before actually deleting the organism .
		b2Fixture* temp = body->GetFixtureList();
		do {
			b2Fixture* fixToDestroy = temp;
			temp = temp->GetNext();
			body->DestroyFixture(fixToDestroy);		
		} while (temp != nullptr);

		body->GetWorld()->DestroyBody(body);
		
		delete brain;
	}

	//getter methods
	b2Body* Organism::getBody() const
	{
		return body;
	}

	sf::Shape* Organism::getShape() const
	{
		return shape;
	}

	const sf::Texture* Organism::getTexture() const
	{
		return texture;
	}

	const b2PolygonShape* Organism::getB2Shape() const
	{
		return myB2Shape;
	}

	const b2BodyDef& Organism::getBodyDef() const
	{
		return bodyDef;
	}

	const b2FixtureDef& Organism::getFixtureDef() const
	{
		return fixtureDef;
	}

	const float Organism::getMaxSpeed() const
	{
		return maxSpeed;
	}

	const int Organism::getEnergy() const
	{
		return energy;
	}

	const int Organism::getMoisture() const
	{
		return moisture;
	}

	const b2FixtureDef& Organism::getSensorDef() const
	{
		return sensorDef;
	}

	const float Organism::getAngle() const
	{
		return body->GetAngle();
	}

	const std::vector<Entity*> Organism::getVisibleEntities() const
	{
		return visibleEntities;
	}

	const bool Organism::getDeadManWalking() const
	{
		return deadManWalking;
	}

	sf::Clock* Organism::getClock() 
	{
		sf::Clock* myClock;
		myClock = &deathClock;
		return myClock;
	}

	const std::vector<std::list<Stimulus*>> Organism::getStimuli() const{
		return stimuli;
	}

	std::vector<float> Organism::getNeuralInputs() {
		return neuralInputs;
	}

	const int Organism::getNumberOfNeuralInputs() {
		return numberOfNeuralInputs;
	}
	//setter methods
	void Organism::setTexture(sf::Texture* anotherTexture)
	{
		this->texture = anotherTexture;
	}

	void Organism::setShape(sf::ConvexShape* otherShape)
	{
		this->shape = otherShape;
	}

	void Organism::setEnergy(int newEnergy)
	{
		if (newEnergy>=0&&newEnergy<=10)
			energy = newEnergy;
	}

	void Organism::setMoisture(int newMoisture)
	{
		moisture = newMoisture;
	}

	void Organism::setSpeed(float speed)
	{
		maxSpeed = speed;
	}

	void Organism::setDeadManWalking(bool dead)
	{
		deadManWalking = dead;
	}

	void Organism::setNeuralInputs(const std::vector<float> inputsVector) {
		neuralInputs = inputsVector;
	}

	void Organism::setNumberOfNeuralInputs(const int number) {
		numberOfNeuralInputs = number;
	}

	void Organism::setStimuli(std::vector<std::list<Stimulus*>> myStimuli) {
		stimuli = myStimuli;
	}

	void Organism::addStimulus(Stimulus* stim) {
		stimuli[stim->type].push_back(stim);
	}

	void Organism::emptyStimuli() {
		
		for (int i = 0; i < stimuli.size(); i++) {
			stimuli[i].clear();
		}

	}

	void Organism::update()
	{
		float RATIO = RatioVar::instance()->getRATIO();

		float angle = body->GetAngle() / 3.14 * 180;
		while (angle <= 0) {
			angle += 360;
		}
		while (angle > 360) {
			angle -= 360;
		}

		shape->setRotation(angle);
		shape->setPosition(body->GetPosition().x*RATIO, body->GetPosition().y*RATIO);
		
		//every 2 seconds the organism  loses a point of moisture and energy 
		if ((int)deathClock.getElapsedTime().asSeconds() >= 2)
		{
			moisture--;
			energy--;
			deathClock.restart();
		}

		if (moisture <= 0 || energy <= 0)
			setDeadManWalking(true);//the organism has died and is waiting for termination 

		createBrainInput();
			
	}

	void Organism::move(fann_type* outputs)
	{
		float desiredSpeed = outputs[1];
		float desiredLeft = outputs[0];
		float desiredRight = outputs[2];

		//this is the angle in RADS returned by the body property .
		//we change it in DEEGREES to make sense .
		float angle = body->GetAngle() / 3.14 * 180;
		
		float speed = desiredSpeed*maxSpeed;
		//we need a cortesian vector for the direction of the applied forces .
		float m_x, m_y;
		//adding a 90 degrees worth of rads to the calculation because the box2d engine and 
		//SFML "sees" 0 degrees diferently . to be precise it's 90 degrees off .
		m_x = speed*cosf(angle*3.14 / 180 - 90 * 3.14 / 180);
		m_y = speed*sinf(angle*3.14 / 180 - 90 * 3.14 / 180);

		
			getBody()->ApplyTorque(-10*desiredLeft, true);

			getBody()->ApplyTorque(10*desiredRight, true);
		
			getBody()->ApplyForce(b2Vec2(m_x, m_y), getBody()->GetWorldCenter(), true);
		
	}

	void Organism::think() {
		std::vector<float> inputsVector = getNeuralInputs();
		fann_type* inputsArray = &inputsVector[0];
		
		move(brain->run(inputsArray));
	
	}

	void Organism::drink()
	{
		moisture = 30;
	}

	void Organism::feed()
	{
		energy = 10;
	}

	b2FixtureDef* Organism::createSensor()
	{
		float radius = 15;//in meters . represents the distance of vision
		b2Vec2 vertices[8];
		vertices[0].Set(0, 0);
		for (int i = 0; i < 7; i++) {
			float angle = 225*3.14/180 + i / 6.0 * 90 * 3.14 / 180;
			vertices[i + 1].Set(radius * cosf(angle), radius * sinf(angle));
		}
		b2PolygonShape* polyShape = new b2PolygonShape();
		polyShape->Set(vertices, 8);
		b2FixtureDef* mySensorDef = new b2FixtureDef();
		mySensorDef->shape = polyShape;
		mySensorDef->isSensor = true;
		return mySensorDef;
	}

	void Organism::acquiredVisual(Entity* other)
	{
		if (Gatherer* temp = dynamic_cast<Gatherer*>(other)) {
			//std::cout << "Hey look ! that's a Gatherer !\n";
		}
		else if (Rock* temp = dynamic_cast<Rock*>(other)) {
			//std::cout << "Hey look ! that's a Rock !\n";
		}
		else if (River* temp = dynamic_cast<River*>(other))
		{
			//std::cout << "Hey look ! that's a River !\n";
		}
		else if (Bush* temp = dynamic_cast<Bush*>(other))
		{
			//std::cout << "Hey look ! that's a Bush !\n";
		}
		
		visibleEntities.push_back(other);
		
	}

	void Organism::lostVisual(Entity* other)
	{
		//search and delete 
		visibleEntities.erase(std::find(visibleEntities.begin(), visibleEntities.end(), other));
		
	}

	void Organism::restartClock()
	{
		deathClock.restart();
	}

	std::array<float , 2> Organism::getDistanceAndAngleCircle( b2Body*& body , char visible)
	{
		
			//myArray stores the distance and the angle in degrees
			std::array<float, 2> distAngleArray = { 0,0 };
			//position of the visible entity
			b2Vec2 visiblePosition(body->GetPosition().x, body->GetPosition().y);
			//radius of the entity
			float rad = 0;
			if (body->GetFixtureList()->GetShape()->GetType() == b2Shape::Type::e_polygon)//organism
				rad = 2;
			else
				rad = body->GetFixtureList()->GetShape()->m_radius;
			//distance calculation (1 meter = impact)
			float distance = b2Distance(this->getBody()->GetPosition(), visiblePosition) - abs(rad) - 2;
			if (distance <= 0)
				distance += abs(distance);
			//minimum distance returned is 1 meter . 0 meters will indicate a non existant entity
			//in the brain input
			distAngleArray[0] = distance + 1;

			//translation of the cartesian plane to the new point of origin of this organism
			//newPos is the new position of the visible entity
			b2Vec2 newPos;
			newPos.x = this->getBody()->GetPosition().x - body->GetPosition().x;
			newPos.y = this->getBody()->GetPosition().y - body->GetPosition().y;
			//the organism's angle showing at 45 degrees at the point of its sight focus
			float myAngle = this->getAngle() + 45 * 3.14 / 180;
			//recalculating the position of the entity depending on the angle of our organism
			b2Vec2 newPosAngle;
			newPosAngle.x = newPos.x*cos(myAngle) + newPos.y*sin(myAngle);
			newPosAngle.y = -(newPos.x)*sin(myAngle) + newPos.y*cos(myAngle);

			float angle;
			//calculation of the angle
			angle = atan2(newPosAngle.y, newPosAngle.x) / 3.14 * 180;
			//only 0-360 values
			angle = (int(angle) + 360) % 360;
			//only 1-91 values in case the entity is on field of vision
			if (visible == 'y') {
				if (angle <= 0)
					angle += abs(angle) + 1;
				if (angle > 91)
					angle -= angle - 89;
			}
			else {//in case of 0-360 make it 1-360
				if (angle < 1)
					angle += angle + 1;
			}


			distAngleArray[1] = angle;

			return distAngleArray;
			
	}

	std::array<float, 2>Organism::getDistanceAndAnglePolygon(b2Body*& enemyBody)
	{
		//myArray stores the distance and the angle in degrees
		std::array<float, 2> distAngleArray = { 0,0 };
		b2Vec2 enemyLocation = enemyBody->GetPosition();
		//vertices array stores the 4 vertices of the polygon translated in world coordinates
		b2Vec2* vertices = new b2Vec2[4] ;
		//shape of the enemy entity
		b2PolygonShape* enemyShape = dynamic_cast<b2PolygonShape*>(enemyBody->GetFixtureList()[0].GetShape());
		
		for (short i = 0; i < 4; i++) {
			vertices[i].x = enemyShape->m_vertices[i].x + enemyLocation.x;
			vertices[i].y = enemyShape->m_vertices[i].y + enemyLocation.y;
		}

		b2Vec2 point=getClosestPoint(vertices );

		float distance = b2Distance(this->getBody()->GetPosition(), point) -2;

		if (distance < 0)
			distance = 0;
		//minimum distance returned is 1 meter . 0 meters will indicate a non existant entity
		//in the brain input
		distAngleArray[0] = distance + 1;

		//translation of the cartesian plane to the new point of origin of this organism
		//newPos is the new position of the visible entity
		b2Vec2 newPos;
		newPos.x = this->getBody()->GetPosition().x - point.x;
		newPos.y = this->getBody()->GetPosition().y - point.y;
		//the organism's angle showing at 45 degrees at the point of its sight focus
		float myAngle = this->getAngle() + 45 * 3.14 / 180;
		//recalculating the position of the entity depending on the angle of our organism
		b2Vec2 newPosAngle;
		newPosAngle.x = newPos.x*cos(myAngle) + newPos.y*sin(myAngle);
		newPosAngle.y = -(newPos.x)*sin(myAngle) + newPos.y*cos(myAngle);
		
		float angle;
		//calculation of the angle
		angle = atan2(newPosAngle.y, newPosAngle.x) / 3.14 * 180;
		//only 0-360 values
		angle = (int(angle)) % 360;
		//only 1-91 values
		if (angle <= 0)
			angle += abs(angle) + 1;
		if (angle > 91)
			angle -= angle - 89;

		distAngleArray[1] = angle;
		

		return distAngleArray;
	}

	b2Vec2 Organism::getClosestPoint(b2Vec2* vertices )
	{
		b2Vec2 myCoords = this->getBody()->GetPosition();
		//closest point of the 4 vertices
		b2Vec2 point1(vertices[0].x, vertices[0].y);
		for (short i = 1; i <4; i++)
		{
			if (b2Distance(this->getBody()->GetPosition(), vertices[i]) <= b2Distance(this->getBody()->GetPosition(), point1)) {
				point1 = vertices[i];
			}
		}
		//find and store the points with the same x and y as point1
		b2Vec2 pointX(0,0) , pointY(0,0);
		for (short i = 1; i < 4; i++) {
			if (vertices[i].x == point1.x&&vertices[i].y != point1.y)
				pointX = vertices[i];
			if (vertices[i].y == point1.y&&vertices[i].x != point1.x)
				pointY = vertices[i];
		}
		//find the closest point on the shape's side created by point1
		//and the other vertice with the same x as point1 
		b2Vec2 tempX = point1, tempY=point1 , tempPoint1 = point1;
		float y=point1.y;
		while (true) {
			if (pointX.y > point1.y) y=y+0.2;
			else y = y - 0.2;
			tempX = b2Vec2(point1.x, y);
			if (b2Distance(myCoords, tempX) > b2Distance(myCoords, tempPoint1))
				break;
			tempPoint1 = tempX;
		}
		//find the closest point on the shape's side created by point1
		//and the other vertice with the same y as point1 
		tempPoint1 = point1;
		float x = point1.x;
		while (true) {
			if (pointY.x > point1.x) x = x + 0.2;
			else x = x - 0.2;
			tempY = b2Vec2(x, point1.y);
			if (b2Distance(myCoords, tempY) > b2Distance(myCoords, tempPoint1))
				break;
			tempPoint1 = tempY;
		}
		//get the closest one of the two
		b2Vec2 closestPoint = (b2Distance(myCoords, tempX) < b2Distance(myCoords, tempY) ? tempX : tempY);
		return closestPoint;
	}

	

}