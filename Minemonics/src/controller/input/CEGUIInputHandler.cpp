/*
 * CEGUIInputHandler.cpp
 *
 *  Created on: Feb 17, 2015
 *      Author: leviathan
 */

// view
#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>
#include <CEGUI/GUIContext.h>

// controller
#include <OISKeyboard.h>
#include <OISMouse.h>

// view
#include <OgreStringConverter.h>

// base
#include "SimulationManager.h"

// controller
#include "controller/input/CEGUIInputHandler.h"
#include "controller/StateHandler.h"

// logging
#include "utils/logging/Logger.h"

CEGUIInputHandler::CEGUIInputHandler(StateHandler* stateHandler,
		unsigned long hWnd, SimulationManager* simulationMgr) :
		OISInputHandler(simulationMgr) {
//	OIS::ParamList pl;
//	pl.insert(
//			OIS::ParamList::value_type("WINDOW",
//					Ogre::StringConverter::toString(hWnd)));

	// For debug, in case something goes wrong the mouse can go out.
	/*
	 pl.insert(OIS::ParamList::value_type("x11_keyboard_grab", "false"));
	 pl.insert(OIS::ParamList::value_type("x11_mouse_grab", "false"));
	 */

//	mhWnd = hWnd;
//	m_ois = OIS::InputManager::createInputSystem(pl);
//	mMouse = static_cast<OIS::Mouse*>(m_ois->createInputObject(OIS::OISMouse,
//			true));
//	mKeyboard = static_cast<OIS::Keyboard*>(m_ois->createInputObject(
//			OIS::OISKeyboard, true));
//	mMouse->setEventCallback(this);
//	mKeyboard->setEventCallback(this);

	mStateHandler = stateHandler;
}

CEGUIInputHandler::~CEGUIInputHandler() {
//	if (m_pMouse)
//		m_ois->destroyInputObject(m_pMouse);
//	if (m_pKeyboard)
//		m_ois->destroyInputObject(m_pKeyboard);
//	OIS::InputManager::destroyInputSystem(m_ois);
}

//-------------------------------------------------------------------------------------
bool CEGUIInputHandler::keyPressed(const OIS::KeyEvent &arg) {
	if (CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(
			(CEGUI::Key::Scan) arg.key))
		return true;

	return OISInputHandler::keyPressed(arg);
}
//-------------------------------------------------------------------------------------
bool CEGUIInputHandler::keyReleased(const OIS::KeyEvent &arg) {
	if (CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(
			(CEGUI::Key::Scan) arg.key))
		return true;

	return OISInputHandler::keyReleased(arg);
}

// OIS::MouseListener
bool CEGUIInputHandler::mouseMoved(const OIS::MouseEvent &arg) {
	CEGUI::GUIContext& context =
			CEGUI::System::getSingleton().getDefaultGUIContext();

	// Scroll wheel.
	if (arg.state.Z.rel)
		context.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

	if (context.injectMouseMove(arg.state.X.rel, arg.state.Y.rel))
		return true;

	return OISInputHandler::mouseMoved(arg);
}
bool CEGUIInputHandler::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	CEGUI::GUIContext& context =
			CEGUI::System::getSingleton().getDefaultGUIContext();

	if (context.injectMouseButtonDown(convertButton(id)))
		return true;
	return OISInputHandler::mousePressed(arg, id);

}
bool CEGUIInputHandler::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	CEGUI::GUIContext& context =
			CEGUI::System::getSingleton().getDefaultGUIContext();
	if (context.injectMouseButtonUp(convertButton(id)))
		return true;
	return OISInputHandler::mouseReleased(arg, id);
}

CEGUI::MouseButton CEGUIInputHandler::convertButton(
		OIS::MouseButtonID buttonID) {
	switch (buttonID) {
	case OIS::MB_Left:
		return CEGUI::LeftButton;
		break;

	case OIS::MB_Right:
		return CEGUI::RightButton;
		break;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
		break;

	default:
		return CEGUI::NoButton;
		break;
	}
	return CEGUI::NoButton;
}
