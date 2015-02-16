/*
 * CameraConfiguration.h
 *
 *  Created on: Feb 13, 2015
 *      Author: leviathan
 */

#ifndef CAMERACONFIGURATION_H_
#define CAMERACONFIGURATION_H_

class CameraConfiguration {
public:
	CameraConfiguration();
	virtual ~CameraConfiguration();
	static const double CAMERA_MOVEMENT_SPEED = 250;
	static const double CAMERA_ROTATION_SPEED = 0.13;

};

#endif /* CAMERACONFIGURATION_H_ */