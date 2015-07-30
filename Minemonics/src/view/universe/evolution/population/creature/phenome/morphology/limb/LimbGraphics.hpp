#ifndef VIEW_UNIVERSE_EVOLUTION_POPULATION_CREATURE_PHENOME_MORPHOLOGY_LIMB_LIMBGRAPHICS_HPP_
#define VIEW_UNIVERSE_EVOLUTION_POPULATION_CREATURE_PHENOME_MORPHOLOGY_LIMB_LIMBGRAPHICS_HPP_

//# corresponding header
//# forward declarations
class LimbModel;

//# system headers
//## controller headers
//## model headers
#include <OgreVector3.h>
#include <OgreQuaternion.h>

//## view headers
//# custom headers
//## base headers
//## configuration headers
//## controller headers
//## model headers
//## view headers
//## utils headers

/**
 * @brief		The generic graphical representation of a limb.
 * @details		Details
 * @date		2015-03-24
 * @author		Benjamin Ellenberger
 */
class LimbGraphics {
public:
	LimbGraphics(const LimbModel* const limbModel);
	virtual ~LimbGraphics();

	/**
	 * Add the limb to the graphical world.
	 */
	virtual void addToWorld() = 0;

	/**
	 * Remove the limb from the graphical world.
	 */
	virtual void removeFromWorld() = 0;

	/**
	 * Update the graphical representation of the limb with the physical representation.
	 */
	virtual void update(double timeSinceLastTick) = 0;

	/**
	 * Clone the limb graphics.
	 * @return The clone of the limb graphics.
	 */
	virtual LimbGraphics* clone() = 0;

	/**
	 * Get the intersection of a ray with origin and direction and the limb surface in the global reference frame.
	 * @param origin The origin of the ray in the global reference frame.
	 * @param direction The direction of the ray.
	 * @return The intersection of a ray with origin and direction and the limb surface in the global reference frame.
	 */
	virtual Ogre::Vector3 getIntersection(const Ogre::Vector3 origin,
			const Ogre::Vector3 direction) = 0;

	/**
	 * Get the intersection of a ray with origin and direction and the limb surface in the local reference frame of this limb. This might be computationally less expensive than the precise version.
	 * @param origin The origin of the ray in the global reference frame.
	 * @param direction The direction of the ray.
	 * @return The intersection of a ray with origin and direction and the limb surface in the local reference frame of this limb.
	 */
	virtual Ogre::Vector3 getLocalIntersection(const Ogre::Vector3 origin,
			const Ogre::Vector3 direction) = 0;

	/**
	 * Get the precise intersection of a ray with origin and direction and the limb surface in the local reference frame of this limb. This might be computationally more expensive than the non-precise version.
	 * @param origin The origin of the ray in the global reference frame.
	 * @param direction The direction of the ray.
	 * @return The precise intersection of a ray with origin and direction and the limb surface in the local reference frame of this limb.
	 */
	virtual Ogre::Vector3 getLocalPreciseIntersection(const Ogre::Vector3 origin,
			const Ogre::Vector3 direction) = 0;

	//Accessor methods

	bool isInWorld() const {
		return mInWorld;
	}

protected:

	void setInWorld(bool mInWorld) {
		this->mInWorld = mInWorld;
	}

protected:

	/**
	 * A const handle of the limb model to get the information to render.
	 */
	const LimbModel* mLimbModel;

	/**
	 * Whether the limb is in the world or not.
	 */
	bool mInWorld;
};

#endif /* VIEW_UNIVERSE_EVOLUTION_POPULATION_CREATURE_PHENOME_MORPHOLOGY_LIMB_LIMBGRAPHICS_HPP_ */
