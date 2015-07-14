#ifndef CONTROLLER_UNIVERSE_UNIVERSE_HPP_
#define CONTROLLER_UNIVERSE_UNIVERSE_HPP_

//# corresponding header
//# forward declarations
class Planet;
class Evaluation;

//# system headers
#include <vector>

//## controller headers
//## model headers
#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/basic_logger.hpp>

//## view headers
//# custom headers
//## base headers
//## configuration headers
//## controller headers
#include <controller/EvaluationController.hpp>

//## model headers
#include <model/universe/UniverseModel.hpp>

//## view headers
//## utils headers

/**
 * @brief		The universe is the class that contains everything: Creatures in populations under evolutionary rules and in environments on different planets.
 * @details		The Answer to the Ultimate Question of Life, the Universe, and Everything: 42. But we do not know the question.
 * @date		2015-04-06
 * @author		Benjamin Ellenberger
 */

class Universe {
public:
	Universe();
	virtual ~Universe();

	/**
	 * Initializes the universe.
	 */
	void initialize(SimulationManager* const simulationManager,
			const int parallelEvaluationsQty);

	/**
	 * Add a planet to the universe.
	 * @param planet The planet to add to the universe.
	 */
	void addPlanet(Planet* const planet);

	/**
	 * Proceed with the evaluation. In in serial mode, this just goes on to the next evaluation, in parallel, this schedules one more evaluation.
	 */
	void proceedEvaluation();

	/**
	 * Draw the debug world if debug drawing is enabled.
	 */
	void drawDebugWorld();

	/**
	 * Step the universe physics forward.
	 * @param timeSinceLastFrame The size of the time step.
	 */
	void stepPhysics(const double timeSinceLastFrame);

	/**
	 * Update the model of the universe.
	 */
	void update(const double timeSinceLastFrame);

	//Accessor methods
	/**
	 * Get the planets of the universe.
	 * @return The planets of the universe.
	 */
	std::vector<Planet*>& getPlanets() {
		return mPlanets;
	}

	const std::vector<Planet*>& getPlanets() const {
		return mPlanets;
	}

	/**
	 * Get the model of the universe.
	 * @return The model of the universe.
	 */
	const UniverseModel& getUniverseModel() const {
		return mUniverseModel;
	}

	EvaluationController& getEvaluationController() {
		return mEvaluationController;
	}

private:
	/**
	 * The boost logger.
	 */
	static BoostLogger mBoostLogger;

	/**
	 * Initializer of the boost logger to include the class name into the logging messages.
	 */
	static class _Init {
	public:
		_Init() {
			mBoostLogger.add_attribute("ClassName",
					boost::log::attributes::constant<std::string>("Universe"));
		}
	} _initializer;

	/**
	 * The evaluation controller of this universe.
	 */
	EvaluationController mEvaluationController;

	/**
	 * The model representation of the universe.
	 */
	UniverseModel mUniverseModel;

	/**
	 * The planets of the universe.
	 */
	std::vector<Planet*> mPlanets;
};

#endif /* CONTROLLER_UNIVERSE_UNIVERSE_HPP_ */
