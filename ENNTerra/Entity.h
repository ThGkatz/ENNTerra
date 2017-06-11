#pragma once
#ifndef ENTITY_H
#define ENTITY_H

namespace ThGkatz
{

	class Entity
	{
	public:
		virtual void checkCollision(Entity* otherEntity) = 0;
		virtual ~Entity() {}
	};
}
#endif // !ENTITY_H
