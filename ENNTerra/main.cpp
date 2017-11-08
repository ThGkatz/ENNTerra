//includes
#include "SFML/Graphics.hpp"
#include "Box2D\Box2D.h"
#include "River.h"
#include "Obstacle.h"
#include "Rock.h"
#include "Bush.h"
#include"MyContactListener.h"
#include "Gatherer.h"
#include "RatioVar.h"
#include "Predator.h"
#include "Guardian.h"
#include "Organism.h"
#include "Stimulus.h"
#include<iostream>
#include<string>
#include<vector>
#include<typeinfo>
#include<string>
#include<cmath>
#include<cstdlib>
#include<time.h>



//using
using ThGkatz::River;
using ThGkatz::Obstacle;
using ThGkatz::Rock;
using ThGkatz::Bush;
using ThGkatz::MyContactListener;
using ThGkatz::Gatherer;
using ThGkatz::RatioVar;
using ThGkatz::Predator;
using ThGkatz::Guardian;
using ThGkatz::Organism;
using ThGkatz::Stimulus;


//world parameters
 

 float height = 100;//probably the most important attribute of the world	
 float width = 0.04*height ;//4% of height
 int X_AXIS = 1000;//height of world in pixels
 int Y_AXIS =X_AXIS;//width of world in pixels
 
 MyContactListener contactListener;

 typedef sf::Sprite* spritePointer;
/*creates the background of the world using a grass texture and a sprite .*/
spritePointer backGroundCreation(sf::Texture&);

/*creates and returns an sf::Text pointer with the height and width of the window .Is called on resizing and inialiazing of the world*/
sf::Text* createText();

/*creates and returns a vector of 4 River struct object pointers . b2World object must be 
initialized before the call of this method . The rivers are created on fixed positions creating 
a net around the world .
author : ThGkatz*/
std::vector<std::unique_ptr<River>> createRivers(b2World&);

/*prerequisites : world is initiated , height and width variables are as well . In case the user 
doesn't know how many obstacles to build the method chooces for itself . default values are 
number of Rock objects = height / 10 (integer value)
number of Bush objects = height / 20 (integer value)
The method has the full responsibillity of building the obstacles of the world . They are not 
drawn here though . see main loop for that . 
Uses cstdlib::rand() and srand() with a timer to reinitialize the seed every time we run this program
author : ThGkatz*/
std::vector<Obstacle*> obstacleFactory(b2World& , int = (height/10) , int = (height / 20));

/*This method is responsible for finding coordinates in the world for the creation of an obstacle object 
while leaving enough room around them and the rivers for the organisms to pass . 
it is called every time an object is to be created and returns 1 vector with 2 integers 
pointing to a position . the second passed arguement returns a value indicating a radius 
of an invisible circle around the returned coordinates , showing the maximum area the object
can use . Someone remind me to not try to reinvent the fucking wheel again and use google instead .
author : ThGkatz*/
sf::Vector2i findCoordinates(const std::vector<Obstacle*>, float& ,const float);

//same as the above but this one is responsible for the creation of organisms . Given that 
//it has to consider the distances between our coordinates and those of each existing organism as well
//as it does with the obstacles . The const float argument is the minimumRange fixed to the width/2 of an organism .
//author : ThGkatz
sf::Vector2i findCoordinates(const std::vector<Organism*>, const std::vector<Obstacle*>, const float);

/*prerequisites : world is initiated , height and width variables are as well along with the obstacles . 
In case the user doesn't know how many organisms to build,
the method chooces for itself . default values are 
number of Organism objects = height / 20 (integer value)
Each type of organism creates as many objects as the above value indicates.
The method has the full responsibillity of building the organisms of the world . They are not 
drawn here though . see main loop for that .  
author : ThGkatz*/
std::vector<Organism*> organismFactory(b2World&, const std::vector<Obstacle*>, int = (height / 20));

/*prerequisites : Organism factory has been called .
The function finds the closest gatherer organism for each predator
using a basic loop. Nothing to see here , move along.
Parameter : the already defined Organism's vector */
void setClosestGathererOfPredator(std::vector<Organism*>);

int main()
{
	//initialisation of the global singleton variable RATIO
	RatioVar::instance()->setRATIO(X_AXIS / height);
	float RATIO = RatioVar::instance()->getRATIO();
	
	//the user can play with these
	int numberOfRocks, numberOfBushes, numberOfGatherers, numberOfGuardians, numberOfPredators;



	sf::RenderWindow myWindow(sf::VideoMode(X_AXIS, Y_AXIS), "ENNTerra", sf::Style::Default);//creation of the window
	myWindow.setVerticalSyncEnabled(true);//vertical sync enabled
	
	b2Vec2 gravity(0.0f, 0.0f);
	b2World world(gravity);
	world.SetContactListener(&contactListener);
	
	//initialization of the text with the world parameters
	sf::Text* myText = createText();
	
	
	//loading the water texture for the rivers
	sf::Texture waterTexture1;
	if (!waterTexture1.loadFromFile("water4.jpg"))
	{
		std::cout << "error with the water file \n";
		
	}
	//loading a second water texture for animation
	sf::Texture waterTexture2;
	if (!waterTexture2.loadFromFile("water4.jpg", sf::IntRect(100,100,2700,2000)))
	{
		std::cout << "error with the water file \n";

	}
	//creation of the 4 rivers
	std::vector<std::unique_ptr<River>> rivers;
	rivers = createRivers(world);

	//creation of the obstacles 
	std::vector<Obstacle*> obstacles;
	obstacles = obstacleFactory(world);

	//creation of the background 
	sf::Texture grassTexture;
	spritePointer backGroundPointer = backGroundCreation(grassTexture);

	sf::Clock myTextureClock;
	//creation of the organisms
	std::vector<Organism*> organisms;//= organismFactory(world, obstacles);
	
	
	float timestep = 1 / 60.0f;

	//let's test the viewer
	/*sf::Vector2f myCameraCenter;
	myCameraCenter.x = triangle->getShape()->getPosition().x;
	myCameraCenter.y = triangle->getShape()->getPosition().y;
	sf::View camera(myCameraCenter, sf::Vector2f(500, 500));*/
	
	//myWindow.setView(camera);
	sf::Vector2i position = findCoordinates(organisms, obstacles, 2);
	Guardian* testPre = new Guardian(world, position);
	organisms.push_back(testPre);
	sf::Vector2i position2 = findCoordinates(organisms, obstacles, 2);
	Gatherer* testGath = new Gatherer(world, position2);
	organisms.push_back(testGath);
	sf::Vector2i position3 = findCoordinates(organisms, obstacles, 2);
	Gatherer* testGath2 = new Gatherer(world, position3);
	organisms.push_back(testGath2);
	while (myWindow.isOpen())//main loop
	{
		//call the step function of box2d which handles all the physical computing
		world.Step(timestep, 8, 6);
		sf::Event event;
		while (myWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)//user closes the app
				myWindow.close();
			else if (event.type == sf::Event::Resized)//resizing by any means
			{
				X_AXIS = event.size.height;
				Y_AXIS = event.size.width;

				myText = createText();//call of createText() with the new parameters of the world
			}
			
		}
		

		bool left = false;
		bool right = false;
		bool forward = false;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{

			left = true;

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			right = true;
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{

			forward = true;
		}
		//for (unsigned int i = 0; i < organisms.size(); i++)
			organisms[0]->move(left , right , forward);

			

		//in every loop instance follow a clear-draw-display cycle
		myWindow.clear(sf::Color::Black);

		//draws the ground 
		myWindow.draw(*backGroundPointer);

		//depending on the clock , river texture changes for animation purposes
		for (unsigned int i = 0; i < rivers.size(); i++)
		{
			if ((static_cast<int>(myTextureClock.getElapsedTime().asSeconds()) % 2) ==0)
			{				
				rivers[i]->shape.setTexture(&waterTexture1);
				
			}
			else
			{				
				rivers[i]->shape.setTexture(&waterTexture2);	
			}
			//draw the rivers	
			myWindow.draw(rivers[i]->shape);
		}//end-for
	
		
		if (myTextureClock.getElapsedTime().asSeconds() >= 10)
		{
			//restarting the clock every 10 seconds
			myTextureClock.restart();
			//refilling the bushes with 1 point of food
			for (unsigned int i = 0; i < obstacles.size(); i++)
			{	
				//checking if the obstacle is actually a bush type
				if (typeid(*obstacles[i]) == typeid(ThGkatz::Bush))
					dynamic_cast<Bush*>(obstacles[i])->updateFood();
			}
		}
			
		
		//draws the text of the world prameters
		myWindow.draw(*myText);
		//get the closest gatherer for each predator .
		setClosestGathererOfPredator(organisms);
		for (unsigned int i = 0; i < organisms.size(); i++)
		{
			//update all the organisms 
			organisms[i]->update();
			//and draw them
			myWindow.draw(*organisms[i]->getShape());
			//check if some died
			if (organisms[i]->getDeadManWalking())
			{
				//if an organism has died we delete it calling the deconstructors 
				//and then erase it from the vector
				Organism* temp = organisms[i];
				delete temp;
				organisms.erase(organisms.begin() + i);
			}
		}

		//draw the obstacles
		for (unsigned int i = 0; i < obstacles.size(); i++)
		{
			myWindow.draw(*obstacles[i]->getShape());
		}
		
		myWindow.display();
	}//while window.isOpen
	
	Organism* temp = organisms[0];
	std::vector<float> myTest = temp->getNeuralInputs();
	for (short i = 0; i < myTest.size(); i++) {
			std::cout << "-------------"<< i << "-------------" << std::endl;
			std::cout << myTest[i] << std::endl;
	}
	std::cout << "Press ENTER to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return 0;
}

//creates the background
spritePointer backGroundCreation(sf::Texture& grassTexture)
{
	spritePointer backGround = new sf::Sprite ;
	if (!grassTexture.loadFromFile("grass.jpg"))
	{
		std::cout << "grass file missing\n";
		
	}
		
	grassTexture.setRepeated(true);
	grassTexture.setSmooth(true);

	backGround->setTexture(grassTexture);
	backGround->setScale(sf::Vector2f(1, 1));

	return backGround;
}//backGroundCreation-end

//creates a sf::Text on the upper left cornet showing the current height and width
sf::Text* createText()
{
	sf::Font* font = new sf::Font;
	if (!font->loadFromFile("arial.ttf"))
	{
		std::cout << "Text file missing\n";
		system("EXIT");
	}		
	sf::Text* myText = new sf::Text;
	
	myText->setFont(*font);
	myText->setString("Height : " + std::to_string(X_AXIS) + "\nWidth : " + std::to_string(Y_AXIS));
	myText->setCharacterSize(25);
	myText->setFillColor(sf::Color::White);

	return myText;
}//createText-end

//creates the 4 rivers without using texture
std::vector<std::unique_ptr<River>> createRivers(b2World& world)
{
	//each of the 4 rivers is created first as an object of box2d and then visualized in the sfml world 
	//Rivers are static bodies
	float RATIO = RatioVar::instance()->getRATIO();
	std::vector<std::unique_ptr<River>> rivers;

	River* upperRiver = new River;
	upperRiver->bodyDef.position.Set(height/2 , width/2);
	upperRiver->polyShape.SetAsBox(height/2 , width/2);
	upperRiver->body = world.CreateBody(&upperRiver->bodyDef);
	upperRiver->fixDef.shape = &upperRiver->polyShape;
	upperRiver->fixDef.density = 0.7f;
	upperRiver->fixDef.friction = 0.9f;
	upperRiver->body->CreateFixture(&upperRiver->fixDef);
	upperRiver->shape = sf::RectangleShape(sf::Vector2f(height*RATIO , width*RATIO));
	upperRiver->shape.setOrigin(height * RATIO / 2, width * RATIO / 2);
	upperRiver->shape.setPosition(upperRiver->body->GetPosition().x*RATIO, upperRiver->body->GetPosition().y*RATIO);
	upperRiver->body->SetUserData(upperRiver);

	

	River* leftRiver = new River;
	leftRiver->bodyDef.position.Set(width/2 , height/2);
	leftRiver->polyShape.SetAsBox(width/2, height/2);
	leftRiver->body = world.CreateBody(&leftRiver->bodyDef);
	leftRiver->fixDef.shape = &leftRiver->polyShape;
	leftRiver->fixDef.density = 0.7f;
	leftRiver->fixDef.friction = 0.9f;
	leftRiver->body->CreateFixture(&leftRiver->fixDef);
	leftRiver->shape = sf::RectangleShape(sf::Vector2f(width * RATIO, height * RATIO));
	leftRiver->shape.setOrigin(width*RATIO /2  , height*RATIO/2);
	leftRiver->shape.setPosition(leftRiver->body->GetPosition().x *RATIO, leftRiver->body->GetPosition().y*RATIO);
	leftRiver->body->SetUserData(leftRiver);
	

	River* rightRiver = new River;
	rightRiver->bodyDef.position.Set(height-width/2, height/2);
	rightRiver->polyShape.SetAsBox(width/2, height/2);
	rightRiver->body = world.CreateBody(&rightRiver->bodyDef);
	rightRiver->fixDef.shape = &rightRiver->polyShape;
	rightRiver->fixDef.density = 0.7f;
	rightRiver->fixDef.friction = 0.9f;
	rightRiver->body->CreateFixture(&rightRiver->fixDef);
	rightRiver->shape = sf::RectangleShape(sf::Vector2f(width * RATIO, height * RATIO));
	rightRiver->shape.setOrigin(width * RATIO / 2, height * RATIO / 2);
	rightRiver->shape.setPosition(rightRiver->body->GetPosition().x*RATIO, rightRiver->body->GetPosition().y*RATIO);
	rightRiver->body->SetUserData(rightRiver);

	River* bottomRiver = new River;
	bottomRiver->bodyDef.position.Set(height/2 , height-width/2);
	bottomRiver->polyShape.SetAsBox(height/2, width/2);
	bottomRiver->body = world.CreateBody(&bottomRiver->bodyDef);
	bottomRiver->fixDef.shape = &bottomRiver->polyShape;
	bottomRiver->fixDef.density = 0.7f;
	bottomRiver->fixDef.friction = 0.9f;
	bottomRiver->body->CreateFixture(&bottomRiver->fixDef);
	bottomRiver->shape = sf::RectangleShape(sf::Vector2f(height * RATIO, width* RATIO));
	bottomRiver->shape.setOrigin(height * RATIO / 2, width * RATIO / 2);
	bottomRiver->shape.setPosition(bottomRiver->body->GetPosition().x*RATIO, bottomRiver->body->GetPosition().y*RATIO);
	bottomRiver->body->SetUserData(bottomRiver);

	rivers.push_back(std::unique_ptr<River>(upperRiver));
	rivers.push_back(std::unique_ptr<River>(leftRiver));
	rivers.push_back(std::unique_ptr<River>(rightRiver));
	rivers.push_back(std::unique_ptr<River>(bottomRiver));
	

	
	return rivers;
	
	
}//createRivers-end

std::vector<Obstacle*> obstacleFactory(b2World& world , int numberOfRocks, int numberOfBushes)
{
	srand(time(NULL));//every time the program runs , we won't have the same rand() values
	std::vector<Obstacle*> obstacles ;//will be returned by the method
	float radius=0  , tempRadius; 
	//the minimum range is the fixed radius of a bush
	//and we don't want smaller obstacles than that .
	const float minimumRange = 4;
	


	for (int i = 0; i < numberOfRocks; i++)
	{
		sf::Vector2i position = findCoordinates(obstacles , radius , minimumRange);

		Obstacle* temp;//stores the temporal obstacle object that we create
		int circleOrPolygon = rand() % 2 + 1;//rolling dice to decide for cirlce shape or polygon shape Rock
		sf::Vector2f tempSize;//will be used to create a polygonal Rock 

		switch (circleOrPolygon)
		{
		case 1://we have a polygon

			//in case findCoordinates() returned radius = minimumRange , we avoid a division by zero
			//by creating a fixed-sized 4x4 Rock
			if (int(radius) == int(minimumRange))
			{
				tempSize.x =  minimumRange;
				tempSize.y =  minimumRange;
			}
			else //if radius <> minimumRange we can safelly roll the dice for the Sizing
			{
				tempSize.x = rand() % int(radius - minimumRange) + minimumRange;
				tempSize.y = rand() % int(radius - minimumRange) + minimumRange;
			}
			temp = new Rock(world, position, tempSize);
			break;
		case 2://we have a circle
			   //in case findCoordinates() returned radius = minimumRange , we avoid a division by zero
			   //by creating a fixed-sized 4meter - radius Rock
			if (int(radius) == int(minimumRange))
			{
				tempRadius = minimumRange;
			}
			else //if radius <> minimumRange we can safelly roll the dice for the Sizing
			{
				tempRadius = rand() % int(radius - minimumRange) + minimumRange;
			}
			
			temp = new Rock(world, position, tempRadius);
			break;
		default:
			std::cout << "Houston ... We fucked up \n";
			std::cout << "Check ObstacleFactory \n";
			system("EXIT");
			break;//lel
		}
		
		obstacles.push_back(temp);
	}
	//creation of the bushes , same as with Rocks but every object is fixed sized
	for (int i = 0; i < numberOfBushes; i++)
	{

		sf::Vector2i position = findCoordinates(obstacles , radius , minimumRange);
		Obstacle* temp;

		temp = new Bush(world, position, minimumRange);
		obstacles.push_back(temp);
	}
	
	return obstacles;
	
}

std::vector<Organism*> organismFactory(b2World& world , const std::vector<Obstacle*> obstacles, int numberOfOrganisms)
{
	std::vector<Organism*> organisms;
	const float minimumRange = 2;
	sf::Vector2i position;
	for (int i = 0; i < numberOfOrganisms; i++)
	{
		position = findCoordinates(organisms ,obstacles , minimumRange) ;
		Predator* tempPredator = new Predator(world, position);
		organisms.push_back(tempPredator);

		position = findCoordinates(organisms, obstacles, minimumRange);
		Gatherer* tempGatherer = new Gatherer(world, position);
		organisms.push_back(tempGatherer);

		position = findCoordinates(organisms, obstacles, minimumRange);
		Guardian* tempGuardian = new Guardian(world, position);
		organisms.push_back(tempGuardian);
	}

	return organisms;
}

sf::Vector2i findCoordinates(const std::vector<Organism*> builtOrganisms, const std::vector<Obstacle*> builtObstacles, const float minimumRange)
{
	int safeDistance = width;
	int minimumCoord = width + minimumRange + safeDistance;
	int maximumCoord = height - width - minimumCoord - safeDistance;

	bool canBuild = false;
	int x, y;
	while (!canBuild)
	{
		//the coordinates all the ruckus is about 
		x = rand() % (maximumCoord - minimumCoord) + minimumCoord;
		y = rand() % (maximumCoord - minimumCoord) + minimumCoord;

		int temp = 0;//helper variable showing how many obstacles are far enough
		float rad = 0;//helper variable storing the radius of a rock

		//loop for checking every existing obstacle's distance from our coordinates
		for (int i = 0; i < builtObstacles.size(); i++)
		{
			//in case the obstacle is a circle shape we only need it's radius
			if (builtObstacles[i]->getFixtureDef().shape->GetType() == b2Shape::Type::e_circle)
				rad = abs(builtObstacles[i]->getFixtureDef().shape->m_radius);//the radius of the obstacle 
																			  //if the obstacle is a polygonal shape we drink bleach 
			else if (builtObstacles[i]->getFixtureDef().shape->GetType() == b2Shape::Type::e_polygon)
			{
				//getting the coordinates of the first point of a rectangle shape . each point is a coordinate on a cartesian level with it's center 
				//being the center of the shape 
				b2Vec2 tempObsSize = dynamic_cast<b2PolygonShape*>(builtObstacles[i]->getBody()->GetFixtureList()->GetShape())->m_vertices[0];
				//we store the biggest out of the two coords on the rad variable . because i was bored to calculate the right coord every time .
				rad = abs(tempObsSize.x) >= abs(tempObsSize.y) ? abs(tempObsSize.x) : abs(tempObsSize.y);
			}


			//the position of the obstacle we are currently checking 
			b2Vec2 positionOfObstacle = builtObstacles[i]->getBody()->GetPosition();
			//calculating the distance from it . All hail Pythagoras
			float distance = sqrt(pow(abs(x - positionOfObstacle.x), 2) + pow(abs(y - positionOfObstacle.y), 2));
			//if the distance between the 2 objects is enough 
			if (distance - (minimumRange + rad) >= safeDistance)
			{
				//party
				temp++;
			}

		}

		for (int i = 0; i < builtOrganisms.size(); i++)
		{
			b2Vec2 positionOfOrganism = builtOrganisms[i]->getBody()->GetPosition();

			float distance = sqrt(pow(abs(x - positionOfOrganism.x), 2) + pow(abs(y - positionOfOrganism.y), 2));

			if (distance - (minimumRange + minimumRange) >= safeDistance)
			{
				//party
				temp++;
			}
		}
		if (temp == (builtObstacles.size() + builtOrganisms.size()))
			canBuild = true;

	}
	sf::Vector2i tempVector(x, y);
	return tempVector;
}

sf::Vector2i findCoordinates(const std::vector<Obstacle*> builtObstacles, float& radius ,const float minimumRange)
{
	
	
	int safeDistance = width;//meters to leave between obstacles so taht organisms can pass by 
	if (minimumRange < (height / 10)) //in case the world is very small go to else 
		radius = rand() % int((height) / 10 - minimumRange )+ minimumRange;//no obstsacle should be bigger than height/10
	else
		radius = minimumRange;
	
	//variables pointing to the start and end sides of the invisible map the obstacles can be built upon
	int minimumCoord = width + radius + safeDistance; 
	int maximumCoord = height - width - radius-safeDistance;
	
	//turns true if we find coordinates good enough 
	bool canBuild = false;
	int x, y;//these are the return Vector's x , y
	int retries = 0;//after 10 tries with no successful match , the radius srinks 
	while (!canBuild)
	{
		
		//the coordinates all the ruckus is about 
		x = rand() % (maximumCoord - minimumCoord) + minimumCoord;
		y = rand() % (maximumCoord - minimumCoord) + minimumCoord;
		
		int temp = 0;//helper variable showing how many obstacles are far enough 
		float rad = 0;//helper variable storing the radius of a rock
		
		//loop for checking every existing obstacle's distance from our coordinates
		for (int i = 0; i < builtObstacles.size(); i++)
		{
			//in case the obstacle is a circle shape we only need it's radius
			if (builtObstacles[i]->getFixtureDef().shape->GetType() == b2Shape::Type::e_circle)
				rad = abs(builtObstacles[i]->getFixtureDef().shape->m_radius);//the radius of the obstacle 
			//if the obstacle is a polygonal shape we drink bleach 
			else if (builtObstacles[i]->getFixtureDef().shape->GetType() == b2Shape::Type::e_polygon)
			{
				//getting the coordinates of the first point of a rectangle shape . each point is a coordinate on a cartesian level with it's center 
				//being the center of the shape 
				b2Vec2 tempObsSize = dynamic_cast<b2PolygonShape*>(builtObstacles[i]->getBody()->GetFixtureList()->GetShape())->m_vertices[0];
				//we store the biggest out of the two coords on the rad variable . because i was bored to calculate the right coord every time .
				rad = abs(tempObsSize.x) >= abs(tempObsSize.y) ? abs(tempObsSize.x) : abs(tempObsSize.y);				
			}
			
			
			//the position of the obstacle we are currently checking 
			b2Vec2 positionOfObstacle = builtObstacles[i]->getBody()->GetPosition();
			//calculating the distance from it . All hail Pythagoras
			float distance = sqrt(pow(abs(x - positionOfObstacle.x),2) + pow(abs(y - positionOfObstacle.y),2));
			//if the distance between the 2 objects is enough 
			if (distance - (radius + rad) >= safeDistance)
			{
				//party
				temp++;
			}
			
		}
		//if all the existing obstacles are far enough we can build . finally . 
		if (temp == builtObstacles.size())
			canBuild = true;
		else
			retries++;
		// after 10 retries we can start crying and reduce the radius by 1 . Could be lower but it's gonna take ages to initialise the world 
		if (retries >= 10)
		{
			radius--;
			retries = 0;
		}
	}
	//Vector to return . 
	sf::Vector2i tempVector(x, y);
	return tempVector;
}//findCoordinates - end


void setClosestGathererOfPredator(std::vector<Organism*> organisms)
{

	for (int i = 0; i < organisms.size(); i++)
	{
		if (Predator* tempPredator = dynamic_cast<Predator*>(organisms[i])) {//for each predator
			Gatherer* closestGatherer = nullptr;
			float distance = 1000;
			for (int j = 0; j < organisms.size(); j++) {//for each gatherer
				if (Gatherer* tempGatherer = dynamic_cast<Gatherer*>(organisms[j])) {
					if (b2Distance(tempPredator->getBody()->GetPosition(), tempGatherer->getBody()->GetPosition()) < distance) {
						closestGatherer = tempGatherer;
						distance = b2Distance(tempPredator->getBody()->GetPosition(), tempGatherer->getBody()->GetPosition());
					}
				}
			}
			tempPredator->setClosestGatherer(closestGatherer);
		}
	}
}

