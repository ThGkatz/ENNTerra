#include"Box2D\Box2D.h"
#include "MyContactListener.h"
#include"Entity.h"
#include"Organism.h"
#include"Guardian.h"
#include "Gatherer.h"
#include<iostream>
#include<typeinfo>

namespace ThGkatz
{


	void MyContactListener::BeginContact(b2Contact* contact)
	{


		Entity* firstObject;
		Entity* secondObject;

		Sensors tempEnum;
		tempEnum = whichIsSensor(contact, firstObject, secondObject);

		if (tempEnum==ONE) // in case only one is a sensor
		{	
			dynamic_cast<Organism*>(firstObject)->acquiredVisual(secondObject);			
		}
		else if (tempEnum == ONEROUND) {
			dynamic_cast<Guardian*>(firstObject)->gathererEnter(secondObject);
		}
		else if(tempEnum==NONE)//in case both are normal textures
		{
			
			firstObject = (Entity*)(contact->GetFixtureA()->GetBody()->GetUserData());
			secondObject = (Entity*)(contact->GetFixtureB()->GetBody()->GetUserData());

			firstObject->checkCollision(secondObject);
			secondObject->checkCollision(firstObject);
		}
		//in case both are sensors nothing happens
	
	}

	void MyContactListener::EndContact(b2Contact* contact)
	{
		Entity* first;
		Entity* second;
		
		Sensors temp;
		temp = whichIsSensor(contact, first, second);
		//in case only 1 is a sensor , call the lostVisual function of it 
		if (temp == Sensors::ONE)
			dynamic_cast<Organism*>(first)->lostVisual(second);
		else if (temp == Sensors::ONEROUND) //the guardina stops sensing the gatherer's presence
			dynamic_cast<Guardian*>(first)->gathererLost(second);
	}

	Sensors MyContactListener::whichIsSensor(b2Contact* contact, Entity*& sensor, Entity*& otherEntity)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();
		Sensors tempEnum;//enumarator of 4 values indicating how many sensors collided

		bool sensorA = fixtureA->IsSensor();
		bool sensorB = fixtureB->IsSensor();

		if (sensorA && sensorB)//both are sensors
		{
			tempEnum = Sensors::BOTH;
			return tempEnum;
		}
		else if ((!sensorA) && (!sensorB))//neither is
		{
			tempEnum = Sensors::NONE;
			return tempEnum;
		}
			
		else//only 1 is a sensor
		{
			Entity* entityA = static_cast<Entity*>(fixtureA->GetBody()->GetUserData());
			Entity* entityB = static_cast<Entity*>(fixtureB->GetBody()->GetUserData());

			if (sensorA)
			{
				if (b2CircleShape* temp = dynamic_cast<b2CircleShape*>(fixtureA->GetShape()))//in case one is a round sensor , meaning we got a guardian 
				{
					
					tempEnum = Sensors::ONEROUND;
				}
				else if (b2PolygonShape* temp = dynamic_cast<b2PolygonShape*>(fixtureA->GetShape()))//normal case of cone sensor
				{
					
					tempEnum = Sensors::ONE;
				}
				sensor = entityA;
				otherEntity = entityB;
			}
			else
			{
				if (b2CircleShape* temp = dynamic_cast<b2CircleShape*>(fixtureB->GetShape()))//in case one is a round sensor , meaning we got a guardian 
				{
					
					tempEnum = Sensors::ONEROUND;
				}
				else if (b2PolygonShape* temp = dynamic_cast<b2PolygonShape*>(fixtureB->GetShape()))//normal case of cone sensor
				{
					
					tempEnum = Sensors::ONE;
				}
				sensor = entityB;
				otherEntity = entityA;
			}
			return tempEnum;
		}

		

	}
}