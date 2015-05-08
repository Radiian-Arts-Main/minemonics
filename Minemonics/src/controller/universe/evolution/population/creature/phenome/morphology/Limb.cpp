//# corresponding headers
//# forward declarations
#ifndef NULL
#define NULL 0
#endif

//# system headers
//## controller headers
//## model headers
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btQuadWord.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

#include <boost/lexical_cast.hpp>

//## view headers
//# custom headers
//## base headers
#include <SimulationManager.hpp>

//## configuration headers
#include <configuration/MorphologyConfiguration.hpp>
#include <configuration/PhysicsConfiguration.hpp>

//## controller headers
#include <controller/universe/evolution/population/creature/phenome/morphology/Limb.hpp>
#include <controller/universe/evolution/population/creature/phenome/morphology/Limb.hpp>
#include <controller/physics/PhysicsController.hpp>

//## model headers
#include <model/universe/evolution/population/creature/phenome/morphology/limb/LimbBt.hpp>

//## view headers
#include <view/universe/evolution/population/creature/phenome/morphology/limb/LimbO3D.hpp>
#include <view/visualization/bulletphysics/OgreBulletUtils.hpp>

//## utils headers

Limb::Limb() :
		mLimbGraphics(NULL), mLimbPhysics(NULL) {
}

Limb::~Limb() {
	delete mLimbGraphics;
	mLimbGraphics = NULL;

	delete mLimbPhysics;
	mLimbPhysics = NULL;
}

void Limb::initialize(SimulationManager* simulationManager, Creature* creature,
		MorphologyConfiguration::PrimitiveType type, Ogre::Vector3 position,
		Ogre::Quaternion orientation, Ogre::Vector3 size, double mass,
		Ogre::ColourValue color) {
	mCreature = creature;

	// Define the new component as a limb
	Component::initialize(Component::LimbComponent);

	// initialize the graphics part of the limb
	mLimbGraphics = new LimbO3D();
	((LimbO3D*) mLimbGraphics)->initialize(simulationManager, type, size,
			color);

	// initialize the physics model of the limb
	mLimbPhysics = new LimbBt();
	((LimbBt*) mLimbPhysics)->initialize(
			simulationManager->getPhysicsController().getDynamicsWorld(), this,
			type, OgreBulletUtils::convert(position),
			OgreBulletUtils::convert(orientation),
			OgreBulletUtils::convert(size), btScalar(mass));

	// Update the state of the limb.
	update();
}

/**
 * Update the state of the limb.
 */
void Limb::update() {
	// get the rigid body of the limb
	btRigidBody* body = ((LimbBt*) mLimbPhysics)->getRigidBody();

	// if the limb's rigid body is existing
	if (body) {

		// update the position of the limb graphics
		btVector3 point = body->getCenterOfMassPosition();
		mLimbGraphics->setPosition(
				Ogre::Vector3(point.x(), point.y(), point.z()));

		// Get the Orientation of the rigid body as a bullet Quaternion
		// Convert it to an Ogre quaternion
		btQuaternion btq = body->getOrientation();
		Ogre::Quaternion quart = Ogre::Quaternion(btq.w(), btq.x(), btq.y(),
				btq.z());

		// update the orientation of the limb graphics
		mLimbGraphics->setOrientation(quart);
	}

	mLimbGraphics->update();
}

std::string Limb::getInfo() {
	std::string text;
	text.append("Box coordinate: ");
	text.append(
			boost::lexical_cast<std::string>(mLimbGraphics->getPosition().x));
	text.append(",");
	text.append(
			boost::lexical_cast<std::string>(mLimbGraphics->getPosition().y));
	text.append(",");
	text.append(
			boost::lexical_cast<std::string>(mLimbGraphics->getPosition().z));
	text.append(",\n");
	text.append("---------");
	return text;
}

/**
 * Add the limb to the world.
 */
void Limb::addToWorld() {
	mLimbGraphics->addToWorld();
	mLimbPhysics->addToWorld();
}

/**
 * Remove the limb from the world.
 */
void Limb::removeFromWorld() {
	mLimbGraphics->removeFromWorld();
	mLimbPhysics->removeFromWorld();
}

/**
 * Get intersection point with the limb graphics given a straight line defined by origin and direction.
 */
Ogre::Vector3 Limb::getIntersection(Ogre::Vector3 origin,
		Ogre::Vector3 direction) {
	return mLimbGraphics->getIntersection(origin, direction);
}
