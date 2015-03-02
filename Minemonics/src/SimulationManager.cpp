/*
 -----------------------------------------------------------------------------
 Filename:    SimulationManager.cpp
 -----------------------------------------------------------------------------

 based on the source file which is part of the
 ___                 __    __ _ _    _
 /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 / \_// (_| | | |  __/  \  /\  /| |   <| |
 \___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
 |___/
 Tutorial Framework
 http://www.ogre3d.org/tikiwiki/
 -----------------------------------------------------------------------------
 */
#include "SimulationManager.h"

//configuration
#include "configuration/EnvironmentConfiguration.h"

// view
#include "view/general/evolution/environments/Environment.h"
#include "view/ogre3D/evolution/environments/HillsO3D.h"
#include "view/ogre3D/evolution/environments/PlaneO3D.h"
#include "view/CEGUI/GUISheetHandler.h"
#include "view/CEGUI/CEGUIBuilder.h"

//Game component includes
#include "controller/input/SDL2InputHandler.h"
#include "controller/camera/CameraHandler.h"

#include <OgreWindowEventUtilities.h>

// needed to be able to create the CEGUI renderer interface
#include <CEGUI/RendererModules/Ogre/Renderer.h>

// CEGUI includes
#include <CEGUI/System.h>
#include <CEGUI/InputEvent.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/FontManager.h>

#include "CEGUI/widgets/PushButton.h"

#include "utils/misc/OgreSystemConfigStrings.h"

//#define _DEBUGGUI

BoostLogger SimulationManager::mBoostLogger;  // initialize the static variables
SimulationManager::_Init SimulationManager::_initializer;
//-------------------------------------------------------------------------------------
SimulationManager::SimulationManager(void) :
//		mInfoLabel(0),
		mCameraHandler(this), mRenderer(0), mLayout(NULL), mSystem(NULL), mInputHandler(
		NULL), mStateHandler(NULL), mGUISheetHandler(NULL), mTerrain(NULL), mDetailsPanel(
		NULL), mFpsPanel(NULL), mTestObject(NULL), parents(
				EvolutionConfiguration::PopSize,
				ChromosomeT<bool>(EvolutionConfiguration::Dimension)), offsprings(
				EvolutionConfiguration::PopSize,
				ChromosomeT<bool>(EvolutionConfiguration::Dimension)), jury(1), t(
				0) {

	// main frame timer initialization
	mStart = boost::posix_time::second_clock::local_time();
	mNow = boost::posix_time::second_clock::local_time();
	mRuntime = mNow - mStart;
}
//-------------------------------------------------------------------------------------
SimulationManager::~SimulationManager(void) {
	CEGUI::OgreRenderer::destroySystem();
}

void SimulationManager::destroyScene(void) {
	if (mTerrain->environmentType == Environment::HILLS)
		((HillsO3D*) mTerrain)->destroy();

}

//-------------------------------------------------------------------------------------
void SimulationManager::createFrameListener(void) {

	/// INPUT HANDLER

	// this next bit is for the sake of the input handler
	Ogre::LogManager::getSingletonPtr()->logMessage(
			"*** Initializing SDL2 ***");

	// set up the input handlers
	mStateHandler = new StateHandler();

	// since the input handler deals with pushing input to CEGUI, we need to give it a pointer
	// to the CEGUI System instance to use
	mStateHandler->requestStateChange(GUI);

	// make an instance of our GUI sheet handler class
	mGUISheetHandler = new GUISheetHandler(mSystem, mLayout, mStateHandler);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);

	// Populate the camera container
	mCameraHandler.setCamNode(mCamera->getParentSceneNode());

	unsigned i, t;

	//
	// initialize random number generator
	//
	boost::posix_time::time_duration duration(mNow.time_of_day());
	Rng::seed(duration.total_milliseconds());

	//
	// maximization task
	//
	parents.setMaximize();
	offsprings.setMaximize();

	//
	// initialize all chromosomes of parent population
	//
	for (i = 0; i < parents.size(); ++i)
		parents[i][0].initialize();

	//
	// evaluate parents (only needed for elitist strategy)
	//
	if (EvolutionConfiguration::NElitists > 0)
		for (i = 0; i < parents.size(); ++i) {
			jury.setEvaluationSubject(parents[i][0]);
			jury.evaluateFitness();
			parents[i].setFitness(jury.getFitness());
		}
}

//-------------------------------------------------------------------------------------
bool SimulationManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {

	// update main frame timer
	mNow = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration mRuntime = mNow - mStart;

	if (mWindow->isClosed() || mStateHandler->getCurrentState() == SHUTDOWN)
		return false;

	// Inject input into handlers;
	mInputHandler->injectInput();

	mCameraHandler.reposition(evt.timeSinceLastFrame);

	updatePanels();

	mTestObject->update(evt.timeSinceLastFrame);

	//TODO: Use for creature evolution, but clean up afterwards
	// updateEvolution();

	return true;
}

//TODO: Use for creature evolution, but clean up afterwards
void SimulationManager::updateEvolution() {
	//
	// recombine by crossing over two parents
	//
	offsprings = parents;
	int i = 0;
	for (i = 0; i < offsprings.size() - 1; i += 2)
		if (Rng::coinToss(EvolutionConfiguration::CrossProb))
			offsprings[i][0].crossover(offsprings[i + 1][0],
					EvolutionConfiguration::CrossPoints);
	//
	// mutate by flipping bits
	//
	for (i = 0; i < offsprings.size(); ++i)
		offsprings[i][0].flip(EvolutionConfiguration::FlipProb);
	//
	// evaluate objective function
	//
	for (i = 0; i < offsprings.size(); ++i) {
		jury.setEvaluationSubject((offsprings[i][0]));
		jury.evaluateFitness();
		offsprings[i].setFitness(jury.getFitness());
	}
	//
	// scale fitness values and use proportional selection
	//
	//offsprings.linearDynamicScaling(window, t);
	parents.selectProportional(offsprings, EvolutionConfiguration::NElitists);

	t++;
	//
	// print out best value found so far
	//
	std::cout << "Generation " << t << "s best individual has fitness value "
			<< "\t" << parents.best().fitnessValue() << std::endl;

}

void SimulationManager::updatePanels() {
	if (mFpsPanel != NULL) {
		if (mFpsPanel->isVisible()) // if fps panel is visible, then update its contents
		{
			if (mFpsPanel->size() == 3) {
				mFpsPanel->setParamValue(0,
						Ogre::StringConverter::toString(mWindow->getLastFPS()),
						true);
				mFpsPanel->setParamValue(1,
						Ogre::StringConverter::toString(
								mRuntime.total_milliseconds()), true);

				if (mTerrain->environmentType == Environment::HILLS) {
					if (((HillsO3D*) mTerrain)->mTerrainGroup->isDerivedDataUpdateInProgress()) {
						if (((HillsO3D*) mTerrain)->mTerrainsImported) {
							mFpsPanel->setParamValue(2,
									"Building terrain, please wait...", true);
						} else {
							mFpsPanel->setParamValue(2,
									"Updating textures, patience...", true);
						}
					} else {
						mFpsPanel->setParamValue(2, "Idle.", true);
						if (((HillsO3D*) mTerrain)->mTerrainsImported) {
							((HillsO3D*) mTerrain)->mTerrainGroup->saveAllTerrains(
									true);
							((HillsO3D*) mTerrain)->mTerrainsImported = false;
						}
					}
				}
			} else {
				Ogre::RenderTarget::FrameStats fs = mWindow->getStatistics();
				mFpsPanel->setParamValue(0,
						Ogre::StringConverter::toString(fs.lastFPS), false);
				mFpsPanel->setParamValue(1,
						Ogre::StringConverter::toString(fs.avgFPS), false);
				mFpsPanel->setParamValue(2,
						Ogre::StringConverter::toString(fs.bestFPS), false);
				mFpsPanel->setParamValue(3,
						Ogre::StringConverter::toString(fs.worstFPS), false);
				mFpsPanel->setParamValue(4,
						Ogre::StringConverter::toString(fs.triangleCount),
						false);
				mFpsPanel->setParamValue(5,
						Ogre::StringConverter::toString(fs.batchCount), true);
			}
		}
	}
	if (mDetailsPanel != NULL) {
		if (mDetailsPanel->isVisible()) // if details panel is visible, then update its contents
		{
			mDetailsPanel->setParamValue(0,
					Ogre::StringConverter::toString(
							mCamera->getDerivedPosition().x), false);
			mDetailsPanel->setParamValue(1,
					Ogre::StringConverter::toString(
							mCamera->getDerivedPosition().y), false);
			mDetailsPanel->setParamValue(2,
					Ogre::StringConverter::toString(
							mCamera->getDerivedPosition().z), false);
			mDetailsPanel->setParamValue(4,
					Ogre::StringConverter::toString(
							mCamera->getDerivedOrientation().w), false);
			mDetailsPanel->setParamValue(5,
					Ogre::StringConverter::toString(
							mCamera->getDerivedOrientation().x), false);
			mDetailsPanel->setParamValue(6,
					Ogre::StringConverter::toString(
							mCamera->getDerivedOrientation().y), false);
			mDetailsPanel->setParamValue(7,
					Ogre::StringConverter::toString(
							mCamera->getDerivedOrientation().z), true);
		}
	}
}

//-------------------------------------------------------------------------------------
bool SimulationManager::quit() {
	mStateHandler->requestStateChange(SHUTDOWN);
	mShutDown = true;
	return true;
}

//-------------------------------------------------------------------------------------
/**
 * Creates the scene of the application.
 */
void SimulationManager::createScene(void) {

// Initialize the logger
	Logger::init("minemonics.log");
	Logger::initTermSink();

	std::cout << "past this!";

	Ogre::RenderTarget* renderTarget = mRoot->getRenderTarget("Minemonics");

// CEGUI
// with a scene manager and window, we can create a the GUI renderer

// new way to instantiate a CEGUIOgreRenderer (Ogre 1.9)
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*renderTarget);

// This pointer is valid only locally
	mSystem = &CEGUI::System::getSingleton();

// tell us a lot about what is going on (see CEGUI.log in the working directory)
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

// use this CEGUI scheme definition (see CEGUI docs for more)
	CEGUI::SchemeManager::getSingleton().createFromFile(
			(CEGUI::utf8*) "Ogremonics.scheme", (CEGUI::utf8*) "GUI");
//	CEGUI::SchemeManager::getSingleton().createFromFile(
//			(CEGUI::utf8*) "TaharezLook.scheme", (CEGUI::utf8*) "GUI");
//	CEGUI::SchemeManager::getSingleton().createFromFile(
//			(CEGUI::utf8*) "WindowsLook.scheme", (CEGUI::utf8*) "GUI");

// hide the CEGUI mouse cursor (defined in the look-n-feel)
	mSystem->getDefaultGUIContext().getMouseCursor().setDefaultImage(NULL);

// use this font for text in the UI
	CEGUI::FontManager::getSingleton().createFromFile("Tahoma-8.font",
			(CEGUI::utf8*) "GUI");
	mSystem->getDefaultGUIContext().setDefaultFont("Tahoma-8");

// load a layout from the XML layout file (you'll find this in resources/gui.zip), and
// put it in the GUI resource group
	mLayout = CEGUI::WindowManager::getSingleton().createWindow(
			(CEGUI::utf8*) "DefaultWindow", (CEGUI::utf8*) "Sheet");

	CEGUIBuilder ceguiBuilder(this);
	CEGUI::Window* menu = ceguiBuilder.createMenu();
	mLayout->addChild(menu);
	setFpsPanel(ceguiBuilder.createFpsPanel());
	mLayout->addChild(getFpsPanel()->getWidgetPanel());
	setDetailsPanel(ceguiBuilder.createDetailsPanel());
	mLayout->addChild(getDetailsPanel()->getWidgetPanel());

	mTestObject = new MathGLWindow(this, 400, 400,
			CEGUI::USize(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.4f, 0)));
	mLayout->addChild(mTestObject->getMathGlWindow());
	mTestObject->getMathGlWindow()->setPosition(
			CEGUI::UVector2(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.3f, 0)));
// you need to tell CEGUI which layout to display. You can call this at any time to change the layout to
// another loaded layout (i.e. moving from screen to screen or to load your HUD layout). Note that this takes
// a CEGUI::Window instance -- you can use anything (any widget) that serves as a root window.

	CEGUI::GUIContext& guiContext =
			CEGUI::System::getSingleton().createGUIContext(
					mRenderer->getDefaultRenderTarget());
	//guiContext.setRootWindow(mLayout);
	mSystem->getDefaultGUIContext().setRootWindow(mLayout);

// ###################
// We create a test scene for testing SDL and bullet
// ###################
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Creating test environment for basic setups...";
	mCamera->setNearClipDistance(0.1);
	mCamera->setFarClipDistance(50000);

	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(
			Ogre::RSC_INFINITE_FAR_PLANE)) {
		mCamera->setFarClipDistance(0); // enable infinite far clip distance if we can
	}

// Play with startup Texture Filtering options
// Note: Pressing T on runtime will discard those settings
//  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
//  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	Ogre::Light* light = mSceneMgr->createLight("tstLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	// create the light
	Ogre::Light *light2 = mSceneMgr->createLight("Light1");
	light2->setType(Ogre::Light::LT_POINT);
	light2->setPosition(Ogre::Vector3(250, 150, 250));
	light2->setDiffuseColour(Ogre::ColourValue::White);
	light2->setSpecularColour(Ogre::ColourValue::White);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

	switch (EnvironmentConfiguration::environmentType) {
	case Environment::HILLS:
		// Create hills
		mTerrain = new HillsO3D(mSceneMgr);
		((HillsO3D*) mTerrain)->configureTerrainDefaults(light);
		((HillsO3D*) mTerrain)->buildTerrain();
		break;
	case Environment::PLANE:
		//create plane
		mTerrain = new PlaneO3D(mSceneMgr);
		((PlaneO3D*) mTerrain)->configureTerrainDefaults(light);
		((PlaneO3D*) mTerrain)->buildTerrain();
		break;
	case Environment::OPENSEA:
		break;
	}

// Create skyplane
	Ogre::Plane plane;
	plane.d = 100;
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;

//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 500);
	mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true,
			0.5, 150, 150);

// Ogrehead
	Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode =
			mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	headNode->attachObject(ogreHead);
	headNode->translate(Ogre::Vector3(1963, 50, 1660));

// add the ninja
	Ogre::Entity *ninja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	Ogre::SceneNode *ninjaNode =
			mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
	ninjaNode->attachObject(ninja);
	ninjaNode->translate(Ogre::Vector3(1963, 50, 1660));

// Create the scene node
	ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode1",
			Ogre::Vector3(-400, 200, 400));

// Make it look towards the ninja
	ninjaNode->yaw(Ogre::Degree(-45));

// Create the pitch node
	ninjaNode = ninjaNode->createChildSceneNode("PitchNode1");
	ninjaNode->attachObject(mCamera);

// create the second camera node/pitch node
	ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode2",
			Ogre::Vector3(0, 200, 400));
	ninjaNode = ninjaNode->createChildSceneNode("PitchNode2");

	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Creating test environment for basic setups...done.";
}

// Accessor methods
CameraHandler & SimulationManager::getCameraHandler() {
	return mCameraHandler;
}

SDL2InputHandler * SimulationManager::getInputHandler() {
	return mInputHandler;
}

StateHandler * SimulationManager::getStateHandler() {
	return mStateHandler;
}

Ogre::Camera * SimulationManager::getCamera() {
	return mCamera;
}

//Adjust mouse clipping area
void SimulationManager::windowResized(Ogre::RenderWindow* rw) {
	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Repositioning CEGUI pointer...";
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	int x,y;
	SDL_GetMouseState(&x,&y);

	// Align CEGUI mouse with SDL2 mouse
	CEGUI::Vector2f mousePos =
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(
			x - mousePos.d_x, y- mousePos.d_y);
	int w, h;
			SDL_GetWindowSize(mSdlWindow, &w, &h);
	#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
			mWindow->resize(w, h);
	#else
			mWindow->windowMovedOrResized();
	#endif
//#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
//	mWindow->resize(width, height);
//#else
//	mWindow->windowMovedOrResized();
//#endif

	BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info) << "Notifying CEGUI of resize....";
	mSystem->notifyDisplaySizeChanged(CEGUI::Size<float>(width,height));
}

void SimulationManager::windowFocusChange(Ogre::RenderWindow* rw) {
	if (rw->isVisible()) {
		BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info)<< "Window has gained focus...";
//		mInputHandler->initializeInputHandler();
		// Align CEGUI mouse with OIS mouse

		CEGUI::Vector2f mousePos =
		mSystem->getDefaultGUIContext().getMouseCursor().getPosition();
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(
				mInputHandler->getMousePositionX() - mousePos.d_x, mInputHandler->getMousePositionY() - mousePos.d_y);
	}
	else
	{
		BOOST_LOG_SEV(mBoostLogger, boost::log::trivial::info) << "Window has lost focus...";
//		mInputHandler->destroyInputHandler();
	}
}

CEGUI::System*& SimulationManager::getCEGUISystem() {
	return mSystem;
}

bool SimulationManager::configure(void) {

	mInputHandler = new SDL2InputHandler(mStateHandler, this);

	std::string windowTitle = "Minemonics";
	if (SDL_Init( SDL_INIT_EVERYTHING) != 0) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
				"Cannot initialize SDL2!", "GraphicsSystem::initialize");
	}

	Ogre::String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
#if OGRE_DEBUG_MODE
	pluginsPath = "plugins_d.cfg";
#else
	pluginsPath = "plugins.cfg";
#endif
#endif

	// Show the configuration dialog and initialize the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (!mRoot->restoreConfig()) {
		if (!mRoot->showConfigDialog()) {
//					mQuit = true;
//					return;
			assert(false);
		}
	}

	mRoot->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
	mRoot->initialise(false);

	Ogre::ConfigOption videoMode =
			mRoot->getRenderSystem()->getConfigOptions()[OgreConf::VIDEO_MODE];
	Ogre::ConfigOption opfullScreen =
			mRoot->getRenderSystem()->getConfigOptions()[OgreConf::FULL_SCREEN];

	std::vector<std::string> elems;
	std::stringstream ss(videoMode.currentValue);
	std::string item;
	while (std::getline(ss, item, 'x')) {
		elems.push_back(item);
	}

	char* end;
	int width = strtol(elems.at(0).c_str(), &end, 10);
	int height = strtol(elems.at(1).c_str(), &end, 10);

	std::cout << width;

	int screen = 0;
	int posX = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
	int posY = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);

	bool fullscreen = (opfullScreen.currentValue == OgreConf::YES);
	if (fullscreen) {
		posX = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
		posY = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
	}

	mSdlWindow = SDL_CreateWindow(
			windowTitle.c_str(),    // window title
			posX,               // initial x position
			posY,               // initial y position
			width,              // width, in pixels
			height,             // height, in pixels
			SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0)
					| SDL_WINDOW_RESIZABLE);

	//Get the native whnd
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	if (SDL_GetWindowWMInfo(mSdlWindow, &wmInfo) == SDL_FALSE) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
				"Couldn't get WM Info! (SDL2)", "GraphicsSystem::initialize");
	}

	Ogre::ConfigOptionMap& cfgOpts =
			mRoot->getRenderSystem()->getConfigOptions();
	Ogre::String winHandle;
	Ogre::NameValuePairList params;

	switch (wmInfo.subsystem) {
#ifdef WIN32
	case SDL_SYSWM_WINDOWS:
	// Windows code
	winHandle = Ogre::StringConverter::toString( (unsigned long)wmInfo.info.win.window );
	break;
#elif __MACOSX__
	case SDL_SYSWM_COCOA:
	//required to make OGRE play nice with our window
	params.insert( std::make_pair("macAPI", "cocoa") );
	params.insert( std::make_pair("macAPICocoaUseNSView", "true") );

	winHandle = Ogre::StringConverter::toString(WindowContentViewHandle(wmInfo));
	break;
#else
	case SDL_SYSWM_X11:
		winHandle = Ogre::StringConverter::toString(
				(unsigned long) wmInfo.info.x11.window);
		break;
#endif
	default:
		OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
				"Unexpected WM! (SDL2)", "GraphicsSystem::initialize");
		break;
	}

	params.insert(std::make_pair("title", windowTitle));
	params.insert(std::make_pair("gamma", "true"));
	params.insert(std::make_pair("FSAA", cfgOpts["FSAA"].currentValue));
	params.insert(std::make_pair("vsync", cfgOpts["VSync"].currentValue));

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	params.insert( std::make_pair("externalWindowHandle", winHandle) );
#else
	params.insert(std::make_pair("parentWindowHandle", winHandle));
#endif

	mWindow = Ogre::Root::getSingleton().createRenderWindow(windowTitle, width,
			height, fullscreen, &params);
	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
		{
// Create application object
	SimulationManager app;

	try {
		app.go();
	} catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: "
				<< e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}

ParamsPanel * &SimulationManager::getDetailsPanel() {
	return mDetailsPanel;
}

ParamsPanel * &SimulationManager::getFpsPanel() {
	return mFpsPanel;
}

void SimulationManager::setDetailsPanel(ParamsPanel* detailsPanel) {
	mDetailsPanel = detailsPanel;
}

void SimulationManager::setFpsPanel(ParamsPanel* fpsPanel) {
	mFpsPanel = fpsPanel;
}

CEGUI::Window * &SimulationManager::getLayout() {
	return mLayout;
}

Ogre::SceneManager* &SimulationManager::getSceneManager() {
	return mSceneMgr;
}

CEGUI::OgreRenderer* & SimulationManager::getRenderer() {
	return mRenderer;
}

Ogre::Root*& SimulationManager::getRoot() {
	return mRoot;
}

#endif
