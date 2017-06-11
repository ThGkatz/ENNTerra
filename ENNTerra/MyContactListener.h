#pragma once
#include"Box2D\Box2D.h"
#include "Entity.h"
#ifndef MYCONTACTLISTENER_H
#define MYCONTACTLISTENER_H

namespace ThGkatz
{
	//helper enum for the return value of helper function whichIsSensor
	enum Sensors { NONE = 0, ONE = 1, BOTH = 2 , ONEROUND =3 };

	class MyContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact*);
		void EndContact(b2Contact*);
	private:
		//this method finds out if any of the two collision objects is a sensor fixture
		//it returnes a boolean value for the above and also uses the 2 parameter pointers
		//to return the 2 entities.
		Sensors whichIsSensor(b2Contact*, Entity*&, Entity*&);
	};

}//ThGkatz
#endif // !MYCONTACTLISTENER_H
