/*
 * Creature.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: leviathan
 */

#include <controller/universe/evolution/population/creature/Creature.hpp>
//# forward declarations
class SimulationManager;

//# system headers
//## controller headers
//## model headers
//## view headers
//# custom headers
//## base headers
//## configuration headers
//## controller headers
#include <controller/universe/evolution/population/creature/phenome/Phenome.hpp>

//## model headers
#include <model/universe/evolution/population/creature/CreatureModel.hpp>

//## view headers
//## utils headers

Creature::Creature() :
		mCreature(NULL), mPhenotype(NULL) {

}

Creature::~Creature() {
	delete mCreature;
	mCreature = NULL;

	delete mPhenotype;
	mPhenotype = NULL;
}

void Creature::initialize(SimulationManager* simulationManager,
		Ogre::Vector3 position, double bushiness) {
	mCreature = new CreatureModel();
	mCreature->initialize(position, bushiness);
	mPhenotype = new Phenome();
	mPhenotype->initialize(simulationManager);
}

void Creature::performEmbryogenesis() {
	mPhenotype->performEmbryogenesis(this,&mCreature->getGenotype(),
			mCreature->getPosition());
}

void Creature::update() {
	mPhenotype->update();
}

void Creature::addToWorld() {
	// Add phenotype to world
	mPhenotype->addToWorld();
}

void Creature::removeFromWorld() {
	// Remove phenotype from world
	mPhenotype->removeFromWorld();
}