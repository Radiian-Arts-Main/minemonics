//# corresponding headers
#include <controller/universe/evolution/population/creature/phenome/morphology/Limb.hpp>

//# forward declarations
//# system headers
//## controller headers
//## model headers
//### boost headers
#include <boost/lexical_cast.hpp>

//### bullet headers
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btQuadWord.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

//## view headers
//# custom headers
//## base headers
#include <SimulationManager.hpp>

//## configuration headers
#include <configuration/MorphologyConfiguration.hpp>
#include <configuration/PhysicsConfiguration.hpp>

//## controller headers
#include <controller/universe/evolution/population/creature/Creature.hpp>

//## model headers
#include <model/universe/environments/physics/PhysicsController.hpp>
#include <model/universe/evolution/population/creature/phenome/morphology/limb/LimbModel.hpp>
#include <model/universe/evolution/population/creature/phenome/morphology/limb/LimbBt.hpp>
#include <model/universe/environments/EnvironmentModel.hpp>

//## view headers
#include <view/universe/evolution/population/creature/phenome/morphology/limb/LimbO3D.hpp>

//## utils headers
#include <utils/ogre3D/OgreBulletUtils.hpp>

BoostLogger Limb::mBoostLogger; /*<! initialize the boost logger*/
Limb::_Init Limb::_initializer;
Limb::Limb() :
		mLimbGraphics(NULL), mCreature(NULL), mLimbModel(NULL) {
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::debug)<< "Limb created.";
	mLimbModel = new LimbModel();
}

Limb::Limb(const Limb& limb) :
		mLimbModel(limb.mLimbModel) {
	mCreature = limb.mCreature;
	mLimbGraphics = limb.mLimbGraphics->clone();
}

Limb::Limb(Creature* const creature, LimbModel* const limbModel) {
	mLimbModel = limbModel;
	mCreature = creature;

	// Define the new component as a limb
	Component::initialize(limbModel);

	// initialize the graphics part of the limb
	mLimbGraphics = new LimbO3D(mLimbModel);

//	// Update the state of the limb.
	update(0);
}

Limb::~Limb() {
	delete mLimbGraphics;
	mLimbGraphics = NULL;

	delete mLimbModel;
	mLimbModel = NULL;

	mCreature = NULL;
}

void Limb::initialize(Creature* const creature,
		const LimbModel::PrimitiveType type, const Ogre::Vector3 position,
		const Ogre::Quaternion orientation,
		const Ogre::Vector3 initialRelativePosition,
		const Ogre::Quaternion initialOrientation,
		const Ogre::Vector3 dimensions, const double mass,
		const double restitution, const double friction,
		const Ogre::ColourValue color, const int ownIndex) {

	//initialize the model of the limb
	mLimbModel = new LimbModel();
	mLimbModel->initialize(
			creature->getPlanet()->getEnvironmentModel()->getPhysicsController()->getDynamicsWorld(),
			creature->getCreatureModel(), type, position, orientation,
			initialRelativePosition, initialOrientation, dimensions, mass,
			restitution, friction, color, ownIndex);

	mCreature = creature;

	// Define the new component as a limb
	Component::initialize(mLimbModel);

	// initialize the graphics part of the limb
	mLimbGraphics = new LimbO3D(mLimbModel);

	// Update the state of the limb.
	update(0);
}

/**
 * Update the state of the limb.
 */
void Limb::update(double timeSinceLastTick) {
	//update the limb graphics
	mLimbGraphics->update(timeSinceLastTick);
}

void Limb::reset(const Ogre::Vector3 position) {
	mLimbModel->reset(position);
}

void Limb::reposition(const Ogre::Vector3 position) {
	mLimbModel->reposition(position);
}

Limb* Limb::clone() {
	return new Limb(*this);
}

/**
 * Add the limb to the physics world.
 */
void Limb::addToPhysicsWorld() {

	if (!getLimbPhysics()->isInWorld()) {
		getLimbPhysics()->addToWorld();
	}
}

/**
 * Add the limb to the world.
 */
void Limb::addToWorld() {
	if (!mLimbGraphics->isInWorld()) {
		mLimbGraphics->addToWorld();
	}

	if (!getLimbPhysics()->isInWorld()) {
		getLimbPhysics()->addToWorld();
	}
}

/**
 * Remove the limb from the world.
 */
void Limb::removeFromWorld() {
	if (mLimbGraphics->isInWorld()) {
		mLimbGraphics->removeFromWorld();
	}

	if (getLimbPhysics()->isInWorld()) {
		getLimbPhysics()->removeFromWorld();
	}
}

/**
 * Get intersection point with the limb graphics given a straight line defined by origin and direction.
 */
Ogre::Vector3 Limb::getIntersection(const Ogre::Vector3 origin,
		const Ogre::Vector3 direction) {
	return mLimbGraphics->getIntersection(origin, direction);
}
