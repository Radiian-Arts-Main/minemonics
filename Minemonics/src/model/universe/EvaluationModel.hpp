#ifndef MODEL_UNIVERSE_EVALUATIONMODEL_HPP_
#define MODEL_UNIVERSE_EVALUATIONMODEL_HPP_

//# corresponding headers
//# forward declarations
//# system headers
#include <vector>

//## controller headers
//## model headers
//## view headers
//# custom headers
//## base headers
//## configuration headers
#include <configuration/EvaluationConfiguration.hpp>

//## controller headers
//## model headers
#include <model/universe/evolution/population/PopulationModel.hpp>

//## view headers
//## utils headers

/**
 * @brief		The evaluation model holds the information about the evaluation.
 * @details		Details
 * @date		2015-06-01
 * @author		Benjamin Ellenberger
 */
class EvaluationModel {
public:
	EvaluationModel();
	virtual ~EvaluationModel();

	void initialize(PlanetModel* planetModel, double evaluationTime =
			EvaluationConfiguration::DEFAULT_EVALUATION_TIME);

	void addPopulationModel(PopulationModel* populationModel);

	//Accessor methods

	PlanetModel* getPlanetModel() {
		return mPlanetModel;
	}

	void setPlanetModel(PlanetModel* planetModel) {
		mPlanetModel = planetModel;
	}

	double getEvaluationTime() const {
		return mEvaluationTime;
	}

	double getTimePassed() const {
		return mTimePassed;
	}

	void addTimePassed(double timePassed) {
		mTimePassed += timePassed;
	}

	bool isEvaluating() const {
		return mEvaluating;
	}

	bool isTornDown() const {
		return mTornDown;
	}

	void setEvaluating(bool evaluating) {
		mEvaluating = evaluating;
	}

	void setTornDown(bool tornDown) {
		mTornDown = tornDown;
	}

	//Serialization
	//TODO: Add serialization to the evaluation model.

private:

	/**
	 * The model of the planet.
	 */
	PlanetModel* mPlanetModel;

	/**
	 * The models of the populations.
	 */
	std::vector<PopulationModel*> mPopulationModels;

	/**
	 * If the evaluation is running or not.
	 */
	bool mEvaluating;

	/**
	 * If the evaluation is torn down or not.
	 */
	bool mTornDown;

	/**
	 * The time the evaluation runs.
	 */
	double mEvaluationTime;

	/**
	 * The time that has already passed.
	 */

	double mTimePassed;
};

#endif /* MODEL_UNIVERSE_EVALUATIONMODEL_HPP_ */