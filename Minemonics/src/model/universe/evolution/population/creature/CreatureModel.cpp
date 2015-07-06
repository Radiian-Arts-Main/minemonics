//# corresponding headers
#include <model/universe/evolution/population/creature/CreatureModel.hpp>

//# forward declarations
//# system headers
//## controller headers
//## model headers
//## view headers
//# custom headers
//## base headers
//## configuration headers
//## controller headers
//## model headers
#include <model/universe/evolution/population/PopulationModel.hpp>

//## view headers
//## utils headers
#include <utils/NameGenerator.hpp>
#include <utils/Randomness.hpp>

CreatureModel::CreatureModel() :
		mDeveloped(false), mPopulationModel(NULL), mPhenotypeModel(NULL) {

}

CreatureModel::CreatureModel(const CreatureModel& creatureModel) :
		mGenotype(creatureModel.mGenotype) {
	mDeveloped = creatureModel.mDeveloped;
	mFirstName = creatureModel.mFirstName;
	mPopulationModel = creatureModel.mPopulationModel;
	if (creatureModel.mPhenotypeModel != NULL) {
		mPhenotypeModel = creatureModel.mPhenotypeModel->clone();
	}
	mInitialPosition = creatureModel.mInitialPosition;
	mPosition = creatureModel.mPosition;

	std::vector<Jury*>::const_iterator jit = creatureModel.mJuries.begin();
	for (; jit != creatureModel.mJuries.end(); jit++) {
		mJuries.push_back((*jit)->clone());
	}
}

void CreatureModel::initialize(PopulationModel* populationModel,
		PhenomeModel* phenomeModel, Ogre::Vector3 position,
		double branchiness) {
	mPopulationModel = populationModel;
	mPhenotypeModel = phenomeModel;
	mInitialPosition = position;
	mPosition = position;
	Randomness randomness;
	NameGenerator nameGenerator;
	mFirstName = nameGenerator.generateFirstName();
	mGenotype.createRandomGenome(branchiness);
}

CreatureModel::~CreatureModel() {
	while (!mJuries.empty()) {
		Jury* f = mJuries.back();
		mJuries.pop_back();
		delete f;
	}
}

void CreatureModel::reset(Ogre::Vector3 position) {
	mInitialPosition = position;
	mPosition = position;
}

void CreatureModel::reposition(Ogre::Vector3 position) {
	mInitialPosition = position;
	mPosition = position;
}

double CreatureModel::getFitness() {
	double fitness;
	double weight;

	std::vector<Jury*>::iterator jit = mJuries.begin();
	for (; jit != mJuries.end(); jit++) {
		fitness += (*jit)->getFitness() * (*jit)->getWeight();
		weight += (*jit)->getWeight();
	}
	return fitness / weight;
}

bool CreatureModel::equals(const CreatureModel & creature) const {
	if (mFirstName != creature.mFirstName) {
		return false;
	}

	if (mDeveloped != creature.mDeveloped) {
		return false;
	}

	if (!mGenotype.equals(creature.mGenotype)) {
		return false;
	}

	std::vector<Jury*>::const_iterator it = mJuries.begin();
	std::vector<Jury*>::const_iterator it2 = creature.mJuries.begin();
	for (; it != mJuries.end(), it2 != creature.mJuries.end(); it++, it2++) {
		if (!(*it)->equals(**(it2))) {
			return false;
		}
	}

	if (mPhenotypeModel->equals(*creature.mPhenotypeModel)) {
		return false;
	}

	if (mPopulationModel != mPopulationModel) {
		return false;
	}

	return true;
}

CreatureModel* CreatureModel::clone() {
	return new CreatureModel(*this);
}
