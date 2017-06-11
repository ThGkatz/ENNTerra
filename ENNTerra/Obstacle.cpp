#include"Box2D\Box2D.h"
#include "SFML/Graphics.hpp"
#include "Obstacle.h"

namespace ThGkatz
{
	Obstacle::Obstacle()
	{
		body = nullptr;
		shape = nullptr;
		texture = new sf::Texture();
		bodyDef = b2BodyDef();
		polyShape = new b2PolygonShape();
		fixtureDef = b2FixtureDef();

	}

	Obstacle::Obstacle(b2World& world , sf::Vector2i position, float radius)
	{
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(position.x, position.y);
		body = world.CreateBody(&bodyDef);

		polyShape = new b2CircleShape();
		dynamic_cast<b2CircleShape*>(polyShape)->m_p.Set(0, 0);
		polyShape->m_radius = radius;

		fixtureDef.shape = polyShape;
		fixtureDef.density = 0.7f;
		fixtureDef.friction = 0.9f;
		body->CreateFixture(&fixtureDef);

		body->SetUserData(this);

		shape = nullptr;
		
	}

	Obstacle::Obstacle(b2World& world, sf::Vector2i position, sf::Vector2f size)
	{
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(position.x, position.y);
		body = world.CreateBody(&bodyDef);

		polyShape = new b2PolygonShape();
		dynamic_cast<b2PolygonShape*>(polyShape)->SetAsBox(size.x*0.5, size.y*0.5);

		fixtureDef.shape = polyShape;
		fixtureDef.density = 0.7f;
		fixtureDef.friction = 0.9f;
		body->CreateFixture(&fixtureDef);

		body->SetUserData(this);

		shape = nullptr;
	}

	Obstacle::~Obstacle()
	{
		delete polyShape;
		delete shape;
	}

	 b2Body* Obstacle::getBody() const
	{
		return body;
	}

	const b2BodyDef& Obstacle::getBodyDef() const
	{
		return bodyDef;
	}

	const b2FixtureDef& Obstacle::getFixtureDef() const
	{
		return fixtureDef;
	}

	const b2Shape* Obstacle::getPolyShape() const
	{
		return polyShape;
	}

	const sf::Shape* Obstacle::getShape() const
	{
		return shape;
	}

	const sf::Texture* Obstacle::getTexture() const
	{
		return texture;
	}

	void Obstacle::setShape(sf::Shape* anotherShape)
	{
		this->shape = anotherShape;
	}
	void Obstacle::setTexture(sf::Texture* anotherTexture)
	{
		this->texture = anotherTexture;
	}


}//ThGkatz