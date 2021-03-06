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
#include "myIncrementalGA.h"	
#include "ga/std_stream.h"
#include "GlobalVariables.h"
#include<iostream>
#include<string>
#include<vector>
#include<typeinfo>
#include<string>
#include<cmath>
#include<cstdlib>
#include<time.h>
#include <stdio.h>



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
using ThGkatz::myIncrementalGA;
using namespace GlobalVariables;



//world parameters
 

 float height = 100;//probably the most important attribute of the world	
 float width = 0.04*height ;//4% of height
 int X_AXIS = 1000;//height of world in pixels
 int Y_AXIS =X_AXIS;//width of world in pixels
 //the user can play with these
 int numberOfRocks, numberOfBushes;

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
float Objective(GAGenome &);
void  Initializer(GAGenome&);
int main()
{

	// Now that we have the genome, we create the genetic algorithm and set
	// its parameters - number of generations, mutation probability, and crossover
	// probability.  And finally we tell it to evolve itself.

	

	//initialisation of the global singleton variable RATIO
	RatioVar::instance()->setRATIO(X_AXIS / height);
	float RATIO = RatioVar::instance()->getRATIO();
	
	


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
	sf::Clock generationClock;
	//creation of the organisms
	std::vector<Organism*> organisms = organismFactory(world, obstacles, initializationNumberOfOrganisms);
	//get an organism of each spieces to get its parameters
	Predator* myTestPredator = nullptr;
	Guardian* myTestGuardian = nullptr;
	Gatherer* myTestGatherer = nullptr;
	for (unsigned int i = 0; i < organisms.size(); i++) {
		if (Predator* test = dynamic_cast<Predator*>(organisms[i])) { myTestPredator = test; }
		if (Guardian* test = dynamic_cast<Guardian*>(organisms[i])) { myTestGuardian = test; }
		if (Gatherer* test = dynamic_cast<Gatherer*>(organisms[i])) { myTestGatherer = test; }
	}
	assert(myTestGatherer != nullptr&&myTestGuardian != nullptr&&myTestPredator != nullptr);

	//we need 3 ga for each spieces and of course 3 genomes
	GA1DArrayGenome<float> predatorGenome(myTestPredator->getNeuralWeightsLength() + 1, Objective);
	GA1DArrayGenome<float> guardianGenome(myTestGuardian->getNeuralWeightsLength() + 1, Objective);
	GA1DArrayGenome<float> gathererGenome(myTestGatherer->getNeuralWeightsLength() + 1, Objective);
	predatorGenome.initializer(::Initializer);
	guardianGenome.initializer(::Initializer);
	gathererGenome.initializer(::Initializer);
	myIncrementalGA predatorGA(predatorGenome);
	myIncrementalGA guardianGA(guardianGenome);
	myIncrementalGA gathererGA(gathererGenome);
	GAParameterList params;
	myIncrementalGA::registerDefaultParameters(params);
	params.set(gaNpopulationSize, initializationNumberOfOrganisms*2);	// population size
	params.set(gaNpCrossover, 0.9);	// probability of crossover
	params.set(gaNpMutation, 0.001);	// probability of mutation
	params.set(gaNnGenerations, 10000);	// number of generations
	params.set(gaNscoreFrequency, 10);	// how often to record scores
	params.set(gaNflushFrequency, 50);	// how often to dump scores to file
	params.set(gaNnOffspring, 1);
	
	predatorGA.parameters(params);
	guardianGA.parameters(params);
	gathererGA.parameters(params);
	//the selection method will be RANK (only the bet ones can be selected as parents)
	GARankSelector& s = GARankSelector(GASelectionScheme::RAW);
	GANoScaling& scale = GANoScaling();
	predatorGA.selector(s);
	predatorGA.scaling(scale);
	guardianGA.selector(s);
	guardianGA.scaling(scale);
	gathererGA.selector(s);
	gathererGA.scaling(scale);
	predatorGA.replacement(myIncrementalGA::WORST);
	guardianGA.replacement(myIncrementalGA::WORST);
	gathererGA.replacement(myIncrementalGA::WORST);
	predatorGA.initialize();
	guardianGA.initialize();
	gathererGA.initialize();
	
	//now that the populations have been initialised , let's replace them with the actuall neural weights of our organisms.
	int predatorCounter = 0, gathererCounter = 0, guardianCounter = 0;
	for (unsigned int i = 0; i < organisms.size(); i++) {
		if (Predator* test = dynamic_cast<Predator*>(organisms[i])) {
			unsigned int arrayLength = test->getNeuralWeightsLength();
			fann_connection* predatorConnection = new fann_connection[arrayLength];
			test->getNeuralWeights(predatorConnection);

			arrayLength++;//for the clock
			GA1DArrayGenome<float>* myPredatorGenome = new GA1DArrayGenome<float>(arrayLength);
			for (unsigned int gene = 0; gene < arrayLength - 1; gene++) {
				(*myPredatorGenome)[gene] = predatorConnection[gene].weight;
			}
			(*myPredatorGenome)[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();

			GAPopulation myPop = predatorGA.population();
			GAGenome& myIndividual = predatorGA.population().individual(predatorCounter);
			GAGenome* test1 = myPop.replace(myPredatorGenome , predatorCounter);
			predatorCounter++;

			predatorGA.population(myPop);
		}
		if (Guardian* test = dynamic_cast<Guardian*>(organisms[i])) {
			unsigned int arrayLength = test->getNeuralWeightsLength();
			fann_connection* guardianConnection = new fann_connection[arrayLength];
			test->getNeuralWeights(guardianConnection);

			arrayLength++;//for the clock
			GA1DArrayGenome<float>* myGuardianGenome= new GA1DArrayGenome<float>(arrayLength);
			for (unsigned int gene = 0; gene < arrayLength - 1; gene++) {
				(*myGuardianGenome)[gene] = guardianConnection[gene].weight;
			}
			(*myGuardianGenome)[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();
			GAPopulation myPop = guardianGA.population();
			GAGenome& myIndividual = guardianGA.population().individual(guardianCounter+1);
			GAGenome* test1 = myPop.replace(myGuardianGenome, guardianCounter);
			guardianCounter++;
			guardianGA.population(myPop);
			
		}
		if (Gatherer* test = dynamic_cast<Gatherer*>(organisms[i])) {
			unsigned int arrayLength = test->getNeuralWeightsLength();
			fann_connection* gathererConnection = new fann_connection[arrayLength];
			test->getNeuralWeights(gathererConnection);

			arrayLength++;//for the clock
			GA1DArrayGenome<float>* myGathererGenome = new GA1DArrayGenome<float>(arrayLength);
			for (unsigned int gene = 0; gene < arrayLength - 1; gene++) {
				(*myGathererGenome)[gene] = gathererConnection[gene].weight;
			}
			(*myGathererGenome)[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();
			GAPopulation myPop = gathererGA.population();
			GAGenome& myIndividual = gathererGA.population().individual(gathererCounter);
			GAGenome* test1 = myPop.replace(myGathererGenome, gathererCounter);
			gathererCounter++;
			gathererGA.population(myPop);
			
		
		}
	}

	

	float timestep =1/ 60.0f;
	
	//let's test the viewer
	/*sf::Vector2f myCameraCenter;
	myCameraCenter.x = triangle->getShape()->getPosition().x;
	myCameraCenter.y = triangle->getShape()->getPosition().y;
	sf::View camera(myCameraCenter, sf::Vector2f(500, 500));*/
	//myWindow.setView(camera);

	//variable needed for refilling the bushes' energy (food) . Nothing to see here.
	bool bushRefilled = false;
	bool generationChanged = false;
	while (myWindow.isOpen())//main loop
	{
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
		//get the seconds ellapsed till this instance of the loop.
		int secondsOfCurrentIteration = static_cast<int>(myTextureClock.getElapsedTime().asSeconds());
		int secondsOfCurrentGeneration = static_cast<int>(generationClock.getElapsedTime().asSeconds());

		//box2d Step function that does all the physics computations.
		world.Step(timestep, 8, 6);
		
		//in every loop instance follow a clear-draw-display cycle
		myWindow.clear(sf::Color::Black);
		//draws the ground 
		myWindow.draw(*backGroundPointer);

		//depending on the clock , river texture changes for animation purposes
		for (unsigned int i = 0; i < rivers.size(); i++)
		{
			if ((secondsOfCurrentIteration % 2) == 0)
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
		 //draw the obstacles
		for (unsigned int i = 0; i < obstacles.size(); i++)
		{
			myWindow.draw(*obstacles[i]->getShape());
		}
		//every 10 seconds refill the bushes with food
		if (secondsOfCurrentIteration % 10 == 0 && !bushRefilled)
		{
		
			//refilling the bushes with 1 point of food
			for (unsigned int i = 0; i < obstacles.size(); i++)
			{
				//checking if the obstacle is actually a bush type
				if (typeid(*obstacles[i]) == typeid(ThGkatz::Bush))
					dynamic_cast<Bush*>(obstacles[i])->updateFood();
			}
			bushRefilled = true;
		}
		else if (secondsOfCurrentIteration % 10 != 0) bushRefilled = false;
		for (unsigned int i = 0; i < organisms.size(); i++)
		{
			//update all the organisms
			if (organisms[i] != NULL&&organisms[i] != nullptr) {
				if (!organisms[i]->getDeadManWalking()) {
					organisms[i]->update();
					//and draw them
					myWindow.draw(*organisms[i]->getShape());
				}
				//check if some died
				else
				{
					//if an organism has died we delete it calling the deconstructors
					//and then erase it from the vector
					Organism* temp = organisms[i];					
					//completely delete this organism.
					organisms.erase(organisms.begin() + i);
					delete temp;
					temp = NULL;
				}
			}
		}
		if (secondsOfCurrentGeneration>0&& secondsOfCurrentGeneration % intervalBetweenGenerations == 0&&!generationChanged) {//time for a new generation babe !!!
			
			
			numberOfPredators = 0;
			numberOfGuardians = 0;
			numberOfGatherers = 0;

			//get the total number of living organisms for each species
			for (unsigned int i = 0; i < organisms.size(); i++) {
				Organism* temp = organisms[i];
				if (Predator* test = dynamic_cast<Predator*>(organisms[i]))
					numberOfPredators++;
				if (Guardian* test = dynamic_cast<Guardian*>(organisms[i]))
					numberOfGuardians++;
				if (Gatherer* test = dynamic_cast<Gatherer*>(organisms[i]))
					numberOfGatherers++;
			}
			
			//if one of the species died, terminate
			if (numberOfGatherers == 0 || numberOfGuardians == 0 || numberOfPredators == 0) {
				std::cout << "Shit ! One of the species has been exterminated" << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return 0;
			}
			//depending on those numbers let's try to balance the populations by controlling the number of children
			numberOfChildrenPredator = 2;
			numberOfChildrenGuardian = 2;
			numberOfChildrenGatherer = 2;
			//PREDATORS
			/*if (numberOfPredators > numberOfGatherers*2 || numberOfPredators >numberOfGuardians*2) {
				numberOfChildrenPredator = 1;
			}
			else if (numberOfPredators < initializationNumberOfOrganisms / 2) numberOfChildrenPredator=2;
			
			//GUARDIANS
			if (numberOfGuardians > numberOfPredators*2 || numberOfGuardians> numberOfGatherers*2) {
				numberOfChildrenGuardian = 1;
			}
			else if (numberOfGuardians < initializationNumberOfOrganisms / 2) numberOfChildrenGuardian=2;
			//GATHERERS
			if (numberOfGatherers > numberOfPredators*2 || numberOfGatherers > numberOfPredators*2) {
				numberOfChildrenGatherer = 1;
			}
			else if (numberOfGatherers < initializationNumberOfOrganisms / 2)numberOfChildrenGatherer=2;*/

			
			
			Predator* bestPredator = nullptr;
			Guardian* bestGuardian = nullptr;
			Gatherer* bestGatherer = nullptr;
			
			std::vector<GA1DArrayGenome<float>> predatorGenomes;
			std::vector<GA1DArrayGenome<float>> guardianGenomes;
			std::vector<GA1DArrayGenome<float>> gathererGenomes;
			for (unsigned int j = 0; j < organisms.size(); j++) {				
					if (Predator* test = dynamic_cast<Predator*>(organisms[j])) {
						bestPredator = test;
						unsigned int arrayLength = test->getNeuralWeightsLength();
						fann_connection* predatorConnection = new fann_connection[arrayLength];
						test->getNeuralWeights(predatorConnection);
						
						arrayLength++;//for the clock
						GA1DArrayGenome<float> myPredatorGenome(arrayLength);
						for (unsigned int gene = 0; gene < arrayLength-1; gene++) {
							myPredatorGenome[gene] = predatorConnection[gene].weight;
						}
						myPredatorGenome[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();
						myPredatorGenome.score(Objective(myPredatorGenome));
						predatorGenomes.push_back(myPredatorGenome);
					
					}								
					if (Guardian* test = dynamic_cast<Guardian*>(organisms[j])) {
						bestGuardian = test;
						unsigned int arrayLength = test->getNeuralWeightsLength();
						fann_connection* guardianConnection = new fann_connection[arrayLength];
						test->getNeuralWeights(guardianConnection);

						arrayLength++;//for the clock
						GA1DArrayGenome<float> myGuardianGenome(arrayLength);
						for (unsigned int gene = 0; gene < arrayLength - 1; gene++) {
							myGuardianGenome[gene] = guardianConnection[gene].weight;
						}
						myGuardianGenome[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();
						myGuardianGenome.score(Objective(myGuardianGenome));
						guardianGenomes.push_back(myGuardianGenome);
					}
					if (Gatherer* test = dynamic_cast<Gatherer*>(organisms[j])) {
						bestGatherer = test;
						unsigned int arrayLength = test->getNeuralWeightsLength();
						fann_connection* gathererConnection = new fann_connection[arrayLength];
						test->getNeuralWeights(gathererConnection);
					
						arrayLength++;//for the clock
						GA1DArrayGenome<float> myGathererGenome(arrayLength);
						for (unsigned int gene = 0; gene < arrayLength - 1; gene++) {
							myGathererGenome[gene] = gathererConnection[gene].weight;
						}
						myGathererGenome[arrayLength - 1] = (float)test->getClock()->getElapsedTime().asSeconds();
						myGathererGenome.score(Objective(myGathererGenome));
						gathererGenomes.push_back(myGathererGenome);
					}								
			}

			//1. Update the populations of each GA with the living organisms' genomes.
			//Predators
			int genomeSizePredator = predatorGenomes[0].size();
			GAPopulation myPopPredator = predatorGA.population();
			//destroy all the existing genomes.
			GA1DArrayGenome<float> initializeGenomePredator(genomeSizePredator, Objective);
			unsigned int livingTimer = initializeGenomePredator.size()-1;
			initializeGenomePredator[livingTimer] = -10.0f;
			initializeGenomePredator.initializer(Initializer);
			for (unsigned int g = 0; g < myPopPredator.size(); g++) {
				myPopPredator.individual(g) = initializeGenomePredator;
			}
			//append the actual living ones.
			for (unsigned int i = 0; i < predatorGenomes.size(); i++) {
				myPopPredator.individual(i) = predatorGenomes[i];
			}
			
			predatorGA.population(myPopPredator);
			//Guardians
			int genomeSizeGuardian = guardianGenomes[0].size();
			GAPopulation myPopGuardian = guardianGA.population();
			//destroy all the existing genomes.
			GA1DArrayGenome<float> initializeGenomeGuardian(genomeSizeGuardian, Objective);
			unsigned int livingTimerG1 = initializeGenomeGuardian.size() - 1;
			initializeGenomeGuardian[livingTimerG1] = -10.0f;
			initializeGenomeGuardian.initializer(Initializer);

			for (unsigned int g = 0; g < myPopGuardian.size(); g++) {
				myPopGuardian.individual(g) = initializeGenomeGuardian;
			}
			//append the actual living ones.
			for (unsigned int i = 0; i < guardianGenomes.size(); i++) {
				myPopGuardian.individual(i) = guardianGenomes[i];
			}

			guardianGA.population(myPopGuardian);
			//Gatherer
			int genomeSizeGatherer = gathererGenomes[0].size();
			GAPopulation myPopGatherer = gathererGA.population();
			//destroy all the existing genomes.
			GA1DArrayGenome<float> initializeGenomeGatherer(genomeSizeGatherer, Objective);
			unsigned int livingTimerG2 = initializeGenomeGatherer.size() - 1;
			initializeGenomeGatherer[livingTimerG2] = -10.0f;
			initializeGenomeGatherer.initializer(Initializer);

			for (unsigned int g = 0; g < myPopGatherer.size(); g++) {
				myPopGatherer.individual(g) = initializeGenomeGatherer;
			}
			//append the actual living ones.
			for (unsigned int i = 0; i < gathererGenomes.size(); i++) {
				myPopGatherer.individual(i) = gathererGenomes[i];
			}
			gathererGA.population(myPopGatherer);
			
			//2. Step it up after configuring the number of children for each spieces
			predatorGA.nOffspring(numberOfChildrenPredator);
			guardianGA.nOffspring(numberOfChildrenGuardian);
			gathererGA.nOffspring(numberOfChildrenGatherer);
			
			
			//selection, crossover , mutations , replacing...all done inside the step functions 
			predatorGA.step();
			guardianGA.step();
			gathererGA.step();
		
			//3. Get the new genomes (children) and create the new organisms with them.
			GAPopulation myPredatorPopulation = predatorGA.population();
			GAPopulation myGuardianPopulation = guardianGA.population();
			GAPopulation myGathererPopulation = gathererGA.population();
			
			for (int x = 0; x < myPredatorPopulation.size(); x++) {
				std::cout << myPredatorPopulation.individual(x).score()<< std::endl;
			}

			for (unsigned int predatorCounter = 0; predatorCounter <myPredatorPopulation.size(); predatorCounter++) {
				
				GA1DArrayGenome<float> & child = (GA1DArrayGenome<float> &)myPredatorPopulation.individual(predatorCounter);
				if (child.gene(child.size() - 1) != 1000) continue;
				//std::cout << "Predator Child: " << child << std::endl;
				//create the fann_connection using the genome and add the new organism to the pile
				fann_connection* childConnection = new fann_connection[child.size() - 1];
				assert(bestPredator->getNeuralWeightsLength() == child.size() - 1);
				bestPredator->getNeuralWeights(childConnection);
				for (unsigned int i = 0; i < child.size() - 1; i++) {
					childConnection[i].weight = child.gene(i);
				}
				//create the new organism
				sf::Vector2i position = findCoordinates(organisms, obstacles, 2);
				Predator* predatorChild = new Predator(world, position);
				//set the neural inputs array we got from the genome
				predatorChild->setNeuralWeights(childConnection);
				//add it to the organisms vector
				organisms.push_back(predatorChild);
			}
			for (unsigned int guardianCounter = 0; guardianCounter < myGuardianPopulation.size(); guardianCounter++) {
				GA1DArrayGenome<float> & child = (GA1DArrayGenome<float> &)myGuardianPopulation.individual(guardianCounter);
				if (child.gene(child.size() - 1) != 1000) continue;
				//std::cout<< "Guardian Child: " << child << std::endl;
				//create the fann_connection using the genome and add the new organism to the pile
				fann_connection* childConnection = new fann_connection[child.size() - 1];
				assert(bestGuardian->getNeuralWeightsLength() == child.size() - 1);
				bestGuardian->getNeuralWeights(childConnection);
				for (unsigned int i = 0; i < child.size() - 1; i++) {
					childConnection[i].weight = child.gene(i);
				}
				//create the new organism
				sf::Vector2i position = findCoordinates(organisms, obstacles, 2);
				Guardian* guardianChild = new Guardian(world, position);
				//set the neural inputs array we got from the genome
				guardianChild->setNeuralWeights(childConnection);
				//add it to the organisms vector
				organisms.push_back(guardianChild);
			}
			for (unsigned int gathererCounter = 0; gathererCounter < myGathererPopulation.size(); gathererCounter++) {
				GA1DArrayGenome<float> & child = (GA1DArrayGenome<float> &)myGathererPopulation.individual(gathererCounter);
				if (child.gene(child.size() - 1) != 1000) continue;
				//std::cout << "Gatherer Child: " << child.gene(child.size() - 1) << std::endl;
				//create the fann_connection using the genome and add the new organism to the pile
				fann_connection* childConnection = new fann_connection[child.size() - 1];
				assert(bestGatherer->getNeuralWeightsLength() == child.size() - 1);
				bestGatherer->getNeuralWeights(childConnection);
				for (unsigned int i = 0; i < child.size() - 1; i++) {
					childConnection[i].weight = child.gene(i);
				}
				//create the new organism
				sf::Vector2i position = findCoordinates(organisms, obstacles, 2);
				Gatherer* gathererChild = new Gatherer(world, position);
				//set the neural inputs array we got from the genome
				gathererChild->setNeuralWeights(childConnection);
				//add it to the organisms vector
				organisms.push_back(gathererChild);
			
			}

			generationChanged = true;
			generationClock.restart();
		
		}
		else if (secondsOfCurrentGeneration % intervalBetweenGenerations != 0) generationChanged = false;

		//get the closest gatherer for each predator .
		setClosestGathererOfPredator(organisms);
		
		//draws the text of the world prameters
		myWindow.draw(*myText);
		myWindow.display();
	}//while window.isOpen
	
	int max = 0;
	Organism* bestGuardian = nullptr;
	for (unsigned int j = 0; j < organisms.size(); j++) {
		
			if (organisms[j]->getClock()->getElapsedTime().asSeconds() > max) {
				max = organisms[j]->getClock()->getElapsedTime().asSeconds();
				bestGuardian = dynamic_cast<Organism*>(organisms[j]);
			}	
	}
	std::cout << "Best Living Organism so far." << std::endl;
	std::cout << "Time : " << bestGuardian->getClock()->getElapsedTime().asSeconds() << std::endl;
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
			float distance = 10000;
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
float
Objective(GAGenome& g) {
	GA1DArrayGenome<float> & genome = (GA1DArrayGenome<float> &)g;
	float score = genome.gene(genome.size() - 1);
	if (score < 0) score = 0;
	return score;
}

void
Initializer(GAGenome& g)
{
	GA1DArrayGenome<float>& genome = (GA1DArrayGenome<float>&)g;
	genome.gene(0, GARandomFloat(-100.0, 100.0));
}
