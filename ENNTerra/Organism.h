#pragma once
#include"Box2D\Box2D.h"
#include"SFML\Graphics.hpp"
#include"Entity.h"
#include "Stimulus.h"
#include <vector>

#ifndef ORGANISM_H
#define ORGANISM_H

namespace ThGkatz
{
	enum EntityTypes {RIVER , ROCK , BUSH , VIGATHERER , NEARGATHERER , GUARDIAN , PREDATOR};

	class Organism : public Entity
	{
	public:
		//after a collision detect this method is called to deal with the collider object
		virtual void checkCollision(Entity*) = 0;
		//deconstructor
		virtual ~Organism();
		//
		void move(bool , bool , bool);
		//updates the position and the angle of the body . Also handles the depletion rates 
		//of energy and moisture using the deathClock property .
		virtual void update();
		//when the sensor collides with an entity it is added in the visibleEntities vector
		void acquiredVisual(Entity*);
		//called after collision has ended . the entity is removed from the vector
		void lostVisual(Entity*);
		void addStimulus(Stimulus*);
		//getter methods
		const float getAngle() const;
		b2Body* getBody() const;
		sf::Shape* getShape() const;
		const sf::Texture* getTexture() const;
		const b2BodyDef& getBodyDef() const;
		const b2PolygonShape* getB2Shape() const;
		const b2FixtureDef& getFixtureDef() const;
		const b2FixtureDef& getSensorDef() const;
		const float getMaxSpeed() const;
		const int getEnergy() const;
		const int getMoisture() const;
		const bool getDeadManWalking() const;
		sf::Clock* getClock() ;
		//returns a vector with all the visible objects in the field of vision of this organism
		const std::vector<Entity*> getVisibleEntities() const;
		const std::vector<Stimulus*> getStimuli() const;
		//setters
		void setTexture(sf::Texture*);
		void setShape(sf::ConvexShape*);
		void setEnergy(int);
		void setMoisture(int);
		void setSpeed(float);
		void setDeadManWalking(bool);

	private:
		b2Body* body;
		sf::Shape* shape;
		sf::Texture* texture;
		b2BodyDef bodyDef;
		b2PolygonShape* myB2Shape;
		b2FixtureDef fixtureDef;
		b2FixtureDef sensorDef;
		std::vector<Entity*> visibleEntities;//this vector stores every entity that collides with the 90 degree sensor
		bool deadManWalking;//if this is true it mean that the organism has died and will soon be deleted
		float maxSpeed;//indicates the maximum power of thrust an organism can achieve
		int energy;//gets depleted with time , refilling depends on the organism type
		int moisture;//gets depleted with time , refills at rivers
		sf::Clock deathClock;//every 2 seconds the organism loses 1 point of energy and moisture . 
		std::vector<Stimulus*> stimuli;


	private:
		//creates and returns the virtual repressentation of the organism as an sf::ConvexShape (triangle)
		virtual sf::ConvexShape* createShape(sf::Vector2i)=0;
		//creates a fixtureDef defining a cone fixture that uses collision
		//callbacks to serve as a field of vision for the organism
		b2FixtureDef* createSensor();

		


	protected:
		//default constructor
		Organism() ;
		Organism(b2World& , sf::Vector2i);
		//the organism drinks water from a River , replenishing his moisture level ;
		void drink();
		//the organism replenishes his energy level . the way depends on the organism
		virtual void feed();
		//just restarts the Deathclock property 
		void restartClock();
		virtual void createBrainInput() = 0;
		/*
		the parameter passed on the function is the body in box2d of the 
		visible entity . The function calculates the distance between this body
		and the calling one as well as the angle given that the point of origin 
		O(0,0) is the 'nose' of the calling Organism. 
		Returns a float array of size 2 with the distance (1-15 meters) and angle(1-89 degrees 
		for visible entities and 1-360 for closest gatherer of a predator). 
		*/
		std::array<float, 2> getDistanceAndAngleCircle( b2Body*& , char);
		std::array<float, 2> getDistanceAndAnglePolygon( b2Body*&);
		/*
		the function calculates the closest coordinates point on the exterior of the 
		entity . the parameter passed is an array that stores the 4 vertices of the 
		entity's shape . 
		Returns a b2Vec2 point 
		*/
		b2Vec2 getClosestPoint(b2Vec2*);
	};
}
#endif // !ORGANISM_H
