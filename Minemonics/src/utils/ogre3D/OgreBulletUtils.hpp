/*
 * OgreBulletUtils.h
 *
 *  Created on: Mar 19, 2015
 *      Author: leviathan
 */

#ifndef MODEL_BULLET_OGREBULLETUTILS_H_
#define MODEL_BULLET_OGREBULLETUTILS_H_

#include <LinearMath/btQuadWord.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>
#include <OgreQuaternion.h>
#include <OgreVector3.h>

class OgreBulletUtils {
public:
	static btVector3 convert(const Ogre::Vector3 &V) {
		return btVector3(V.x, V.y, V.z);
	}

	static Ogre::Vector3 convert(const btVector3&V) {
		return Ogre::Vector3(V.x(), V.y(), V.z());
	}

	static btQuaternion convert(const Ogre::Quaternion &Q) {
		return btQuaternion(Q.x, Q.y, Q.z, Q.w);
	}
	;

	static Ogre::Quaternion convert(const btQuaternion &Q) {
		return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
	}
	;
};

#endif /* MODEL_BULLET_OGREBULLETUTILS_H_ */