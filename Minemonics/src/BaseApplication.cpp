//# corresponding header
#include <BaseApplication.hpp>
//# forward declarations
//# system headers
#include <stddef.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>

//## controller headers
#include <SDL.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>
#include <SDL_syswm.h>
#include <SDL_video.h>

//## model headers
//## view headers
#include <OgreCamera.h>
#include <OgreColourValue.h>
#include <OgreConfigFile.h>
#include <OgreIteratorWrapper.h>
#include <OgreLogManager.h>
#include <OgreOverlaySystem.h>
#include <OgrePlatform.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSingleton.h>
#include <OgreString.h>
#include <OgreTextureManager.h>
#include <OgreVector3.h>
#include <OgreViewport.h>

//# custom headers
//## base headers
//## configuration headers
//## controller headers
//## model headers
//## view headers
//## utils headers

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <macUtils.h>
#endif

//---------------------------------------------------------------------------
BoostLogger BaseApplication::mBoostLogger;  // initialize the static variables
BaseApplication::_Init BaseApplication::_initializer;
BaseApplication::BaseApplication(void) :
	mRoot(0), mSceneMgr(0), mWindow(0), mResourcesCfg(Ogre::StringUtil::BLANK), mPluginsCfg(
		Ogre::StringUtil::BLANK), mCursorWasVisible(false), mShutDown(false), mOverlaySystem(
		0), mSdlWindow(NULL) {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	m_ResourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
	m_ResourcePath = "";
#endif
}

//---------------------------------------------------------------------------
BaseApplication::~BaseApplication(void) {
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown Information overlay...";
	if (mOverlaySystem) {
		delete mOverlaySystem;
	}
	mOverlaySystem = NULL;
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown Information overlay...done.";

	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown GUI listeners...";
	// Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown GUI listeners...done.";

	// Shut down ogre
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown Ogre3D...";
	mSceneMgr->clearScene();
	mRoot->destroySceneManager(mSceneMgr);
	delete mRoot;
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown Ogre3D...done.";

	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown SDL...";
	SDL_Quit();
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Shutdown SDL...done.";
}

//---------------------------------------------------------------------------
bool BaseApplication::configure(void) {
	// Show the configuration dialog and initialise the system.
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg.
	if (mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise.
		// Here we choose to let the system create a default rendering window by passing 'true'.
		mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

		return true;
	} else {
		return false;
	}
}
//---------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void) {
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	// Initialize the OverlaySystem (changed for Ogre 1.9)
	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
}

//---------------------------------------------------------------------------
void BaseApplication::destroyScene(void) {
}
//---------------------------------------------------------------------------
void BaseApplication::setupResources(void) {
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app.
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location.
			if (!Ogre::StringUtil::startsWith(archName, "/", false))// only adjust relative directories
			archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void) {

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void) {
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//---------------------------------------------------------------------------
void BaseApplication::go(void) {
#ifdef _DEBUG
#ifndef OGRE_STATIC_LIB
	mResourcesCfg = m_ResourcePath + "resources_d.cfg";
	mPluginsCfg = m_ResourcePath + "plugins_d.cfg";
#else
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#endif
#else
#ifndef OGRE_STATIC_LIB
	mResourcesCfg = m_ResourcePath + "resources.cfg";
	mPluginsCfg = m_ResourcePath + "plugins.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif
#endif

	if (!setup())
		return;

	mRoot->startRendering();

	destroyScene(); /**!< Clean up */
}
//---------------------------------------------------------------------------
bool BaseApplication::setup(void) {
	mRoot = new Ogre::Root(mPluginsCfg);

	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW); /**!< Reduce the ogre log detail */

	setupResources(); /**!< Setup the ressources the application uses */

	bool carryOn = configure();
	if (!carryOn)
		return false;

	chooseSceneManager(); /**!< Choose the scene manager */

	setupView(); /**!< Setup the viewport */

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5); /**!< Set default mipmap level (NB some APIs ignore this) */

	createResourceListener(); /**!< Create any resource listeners (for loading screens) */

	loadResources(); /**!< Load resources */

	createScene(); /**!< Create the scene*/

	createFrameListener(); /**!< Create the framelistener */

	return true;
}

Ogre::RenderWindow*& BaseApplication::getWindow() {
	return mWindow;
}

void BaseApplication::setWindow(Ogre::RenderWindow*& window) {
	mWindow = window;
}
//---------------------------------------------------------------------------
