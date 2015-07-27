//# corresponding headers
//# forward declarations
//# system headers
#include <cstdlib>
#include <iterator>
#include <math.h>

//## controller headers
//## model headers
//## view headers
//# custom headers
//## base headers
//## configuration headers
#include <configuration/EvolutionConfiguration.hpp>
#include <configuration/MorphologyConfiguration.hpp>
#include <configuration/PopulationConfiguration.hpp>

//## controller headers
//## model headers
#include <model/universe/evolution/population/creature/genome/morphology/MorphogeneBranch.hpp>
#include <model/universe/evolution/population/creature/genome/MixedGenome.hpp>

//## view headers
//## utils headers
#include <utils/Randomness.hpp>

MixedGenome::MixedGenome() :
		mSegmentsDepthLimit(0), mTotalSegmentQtyLimit(0) {
}

MixedGenome::MixedGenome(const MixedGenome& mixedGenome) {
	mBranchiness = mixedGenome.mBranchiness;
	mGenomeType = mixedGenome.mGenomeType;
	mLength = mixedGenome.mLength;
	mSegmentsDepthLimit = mixedGenome.mSegmentsDepthLimit;
	mTotalSegmentQtyLimit = mixedGenome.mTotalSegmentQtyLimit;

	std::vector<Gene*>::const_iterator git = mixedGenome.mGenes.begin();
	for (; git != mixedGenome.mGenes.end(); git++) {
		mGenes.push_back((*git)->clone());
	}
}

MixedGenome::~MixedGenome() {
	while (!mGenes.empty()) {
		Gene* f = mGenes.back();
		mGenes.pop_back();
		delete f;
	}
}

void MixedGenome::createRandomGenome(double branchiness) {
	mBranchiness = branchiness;

	int geneQty =
			1
					+ abs(
							Randomness::getSingleton()->nextNormalInt(
									PopulationConfiguration::POPULATION_GENES_INITIAL_MEAN,
									PopulationConfiguration::POPULATION_GENES_INITIAL_VAR));
	for (int i = 0; i < geneQty; i++) {
		Morphogene* gene = new Morphogene();
		gene->initialize(branchiness);
		mGenes.push_back(gene);
	}

	mSegmentsDepthLimit = abs(
			Randomness::getSingleton()->nextNormalInt(
					MorphologyConfiguration::LIMB_DEPTH_INITIAL_MEAN,
					MorphologyConfiguration::LIMB_DEPTH_INITIAL_VAR));

	mTotalSegmentQtyLimit = abs(
			Randomness::getSingleton()->nextNormalInt(
					MorphologyConfiguration::LIMB_TOTAL_INITIAL_MEAN,
					MorphologyConfiguration::LIMB_TOTAL_INITIAL_VAR));

	repairGenes();
}

bool MixedGenome::equals(const MixedGenome & genome) const {
	if(!IndirectGenome::equals(genome)){
		return false;
	}

	/**Compare the total segment quantity limit*/
	if (mTotalSegmentQtyLimit != genome.mTotalSegmentQtyLimit) {
		return false;
	}

	/**Compare the segments depth limit*/
	if (mSegmentsDepthLimit != genome.mSegmentsDepthLimit) {
		return false;
	}

	return true;
}

void MixedGenome::addRandomGenes(double percentage) {
	//TODO:Add reasonable number
	for (int i = 0; i < 10; i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= percentage) {
			addRandomGene();
		}
	}
}

void MixedGenome::addRandomGene() {
	Gene::GeneType type =
			(Gene::GeneType) Randomness::getSingleton()->nextUnifPosInt(0,
					Gene::NUM_GENES);
	Gene* gene;
	switch (type) {
	case Gene::MorphoGene: {
		gene = new Morphogene();
		((Morphogene*) gene)->initialize(mBranchiness);
	}
		break;
	default:
		break;
	}
}

void MixedGenome::repairGenes() {
	integrateRandomGenes(1);
}

void MixedGenome::integrateRandomGenes(double integrationProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= integrationProbability) {
			integrateGene(i);
		}
	}
}

void MixedGenome::integrateRandomGene() {
	integrateGene(
			Randomness::getSingleton()->nextUnifPosInt(0, mGenes.size() - 1));
}

void MixedGenome::integrateGene(int geneIndex) {
	//TODO:: Only links morphology and not controller, might break with a controller
	switch (mGenes[geneIndex]->getType()) {
	case Gene::MorphoGene: {
		// randomly choose a follow up gene until you get one different from its own type
		do {
			((Morphogene*) mGenes[geneIndex])->setFollowUpGene(
					Randomness::getSingleton()->nextUnifPosInt(0,
							mGenes.size() - 1));
		} while (((Morphogene*) mGenes[geneIndex])->getFollowUpGene()
				!= geneIndex);

		std::vector<MorphogeneBranch*>::iterator branchIt =
				((Morphogene*) mGenes[geneIndex])->getGeneBranches().begin();
		for (;
				branchIt
						!= ((Morphogene*) mGenes[geneIndex])->getGeneBranches().end();
				branchIt++) {

			//randomly choose a branch gene type until you get one distinct from the follow up gene
			do {
				(*branchIt)->setBranchGeneType(
						Randomness::getSingleton()->nextUnifPosInt(0,
								mGenes.size() - 1));
			} while (mGenes.size() > 1
					&& (*branchIt)->getBranchGeneType()
							== ((Morphogene*) mGenes[geneIndex])->getFollowUpGene());

		}
		break;
	}
	default:
		break;
	}
}

void MixedGenome::replaceRandomGenesWithRandomGenes(
		double replacementProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= replacementProbability) {
			int replacementIndex = Randomness::getSingleton()->nextUnifPosInt(0,
					mGenes.size() - 1);
			replaceGeneWith(i, replacementIndex);
		}
	}
}

void MixedGenome::replaceRandomGeneWithRandomGene() {
	int geneIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			mGenes.size() - 1);
	int replacementIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			mGenes.size() - 1);
	replaceGeneWith(geneIndex, replacementIndex);
}

void MixedGenome::replaceGeneWith(int geneIndex, int replacementIndex) {
	Gene* gene = mGenes[geneIndex];
	mGenes[geneIndex] = mGenes[replacementIndex]->clone();
	delete gene;
}

void MixedGenome::duplicateRandomGenes(double duplicateProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= duplicateProbability) {
			duplicateGene(i);
		}
	}
}

void MixedGenome::duplicateRandomGene() {
	duplicateGene(
			Randomness::getSingleton()->nextUnifPosInt(0, mGenes.size() - 1));
}

void MixedGenome::duplicateGene(int geneIndex) {
	mGenes.push_back(mGenes[geneIndex]->clone());
}

void MixedGenome::splitRandomGenes(double splitProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= splitProbability) {
			splitGene(i,
					(SplitAxis) Randomness::getSingleton()->nextUnifPosInt(1,
							3));
		}
	}
}

void MixedGenome::splitRandomGene() {
	splitGene(Randomness::getSingleton()->nextUnifPosInt(0, mGenes.size() - 1),
			(SplitAxis) Randomness::getSingleton()->nextUnifPosInt(1, 3));
}

void MixedGenome::splitGene(int geneIndex, SplitAxis axis) {
	if (mGenes[geneIndex]->getType() == Gene::MorphoGene) {
		Morphogene* originalGene = ((Morphogene*) mGenes[geneIndex]);
		Morphogene* gene = originalGene->clone();
		mGenes.push_back(gene);

		MorphogeneBranch* branch = new MorphogeneBranch();
		branch->initialize();
		branch->setActive(true);

		//set the joint anchor to ZERO and then modify it depending on split axis.
		branch->setJointAnchorX(0);
		branch->setJointAnchorY(0);
		branch->setJointAnchorZ(0);

		// let the joint point straight outward from the original joint anchor
		branch->setJointPitch(0);
		branch->setJointYaw(0);
		branch->setJointRoll(0);
		branch->setBranchGeneType(mGenes.size() - 1);

		double limbMinSize = MorphologyConfiguration::LIMB_MIN_SIZE;
		switch (axis) {
		default:
		case X_AXIS:
			originalGene->setX(
					std::max(originalGene->getX() / 2.0f, limbMinSize));
			gene->setX(originalGene->getX());
			branch->setJointAnchorX(1);
			break;

		case Y_AXIS:
			originalGene->setY(
					std::max(originalGene->getY() / 2.0f, limbMinSize));
			gene->setY(originalGene->getY());
			branch->setJointAnchorY(1);
			break;

		case Z_AXIS:
			originalGene->setZ(
					std::max(originalGene->getZ() / 2.0f, limbMinSize));
			gene->setZ(originalGene->getZ());
			branch->setJointAnchorZ(1);
			break;
		}
		originalGene->getGeneBranches().clear();

		originalGene->getGeneBranches().push_back(branch);
	}
}

void MixedGenome::growRandomStubs(double growProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= growProbability) {
			int branchiness = Randomness::getSingleton()->nextUnifDouble(0,
					EvolutionConfiguration::REAPER_GROW_STUB_QTY);
			growStub(i, branchiness);
		}
	}
}

void MixedGenome::growRandomStub() {
	int geneIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			mGenes.size() - 1);
	int branchiness = Randomness::getSingleton()->nextUnifDouble(0,
			EvolutionConfiguration::REAPER_GROW_STUB_QTY);
	growStub(geneIndex, branchiness);
}

void MixedGenome::growStub(int geneIndex, int branchiness) {
	mGenes[geneIndex]->grow(branchiness);
}

void MixedGenome::mutateRandomGenes(double percentage) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= percentage) {
			mutateGene(i);
		}
	}
}

void MixedGenome::mutateRandomGene() {
	mutateGene(
			Randomness::getSingleton()->nextUnifPosInt(0, mGenes.size() - 1));
}

void MixedGenome::mutateGene(int geneIndex) {
	mGenes[geneIndex]->mutate();
}

void MixedGenome::mutateRandomBranches(double mutationProbability) {
	for (int i = 0; i < mGenes.size(); i++) {
		if (Randomness::getSingleton()->nextUnifPosInt(0, 1000.0f) / 1000.0f
				<= mutationProbability) {
			mutateRandomBranchOfGene(i);
		}
	}
	repairGenes();
}

void MixedGenome::mutateRandomBranch() {
	int geneIndex;

	//TODO: This can freeze up in case of certain genomes without any morphogenes (these are corrupt anyway, but that would break the code)
	//Better search for indices containing the right type and then choose from that array.
	do {
		geneIndex = Randomness::getSingleton()->nextUnifPosInt(0,
				mGenes.size() - 1);
	} while (mGenes[geneIndex]->getType() != Gene::MorphoGene);

	mutateBranch(geneIndex,
			Randomness::getSingleton()->nextUnifPosInt(0,
					((Morphogene*) mGenes[geneIndex])->getGeneBranches().size()
							- 1));
}

void MixedGenome::mutateRandomBranchOfGene(int geneIndex) {
	if (mGenes[geneIndex]->getType() == Gene::MorphoGene) {
		mutateBranch(geneIndex,
				Randomness::getSingleton()->nextUnifPosInt(0,
						((Morphogene*) mGenes[geneIndex])->getGeneBranches().size()
								- 1));
	}
	repairGenes();
}

void MixedGenome::mutateBranch(int geneIndex, int branchIndex) {
	if (mGenes[geneIndex]->getType() == Gene::MorphoGene) {
		((Morphogene*) mGenes[geneIndex])->getGeneBranches()[branchIndex]->mutate();
	}
}

void MixedGenome::crossoverRandomly(Genome* genome) {
	int motherStartSegmentIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			mGenes.size() - 1);
	int motherEndSegmentIndex = Randomness::getSingleton()->nextUnifPosInt(
			motherStartSegmentIndex, mGenes.size() - 1);

	int fatherStartSegmentIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			genome->getGenes().size() - 1);
	int fatherEndSegmentIndex = Randomness::getSingleton()->nextUnifPosInt(
			fatherStartSegmentIndex, genome->getGenes().size() - 1);

	crossover(genome, motherStartSegmentIndex, motherEndSegmentIndex,
			fatherStartSegmentIndex, fatherEndSegmentIndex);
	repairGenes();
}

void MixedGenome::crossover(Genome* fathergenome, int motherSegmentStartIndex,
		int motherSegmentEndIndex, int fatherSegmentStartIndex,
		int fatherSegmentEndIndex) {
	for (int i = mGenes.size() - 1; i > motherSegmentEndIndex; i--) {
		Gene* gene = mGenes[i];
		mGenes.erase(mGenes.begin() + i);
		delete gene;
	}

	for (int i = 0; i < motherSegmentStartIndex; i++) {
		Gene* gene = mGenes[0];
		mGenes.erase(mGenes.begin());
		delete gene;
	}

	for (int i = fatherSegmentStartIndex; i < fatherSegmentEndIndex; i++) {
		mGenes.push_back(fathergenome->getGenes()[i]->clone());
	}
}

void MixedGenome::graftRandomlyFrom(Genome* donator) {
	int attachmentIndex;
	int geneIndex;
	int maxLinkDepth;

	attachmentIndex = Randomness::getSingleton()->nextUnifPosInt(0,
			mGenes.size() - 1);

	//TODO: This can freeze up in case of certain genomes without any morphogenes (these are corrupt anyway, but that would break the code)
	//Better search for indices containing the right type and then choose from that array.
	do {
		geneIndex = Randomness::getSingleton()->nextUnifPosInt(0,
				donator->getGenes().size() - 1);
	} while (donator->getGenes()[geneIndex]->getType()
			!= mGenes[attachmentIndex]->getType());

	//TODO: Add reasonable numbers
	maxLinkDepth = Randomness::getSingleton()->nextNormalInt(10, 10);

	graftFrom(donator, attachmentIndex, geneIndex, maxLinkDepth);
	repairGenes();
}

void MixedGenome::graftFrom(Genome* donor, int attachmentIndex, int geneIndex,
		int geneQty) {
	//TODO: Make working for controller as well.
	if (mGenes[attachmentIndex]->getType() == Gene::MorphoGene) {

		MorphogeneBranch* branch = new MorphogeneBranch();
		branch->initialize();
		branch->setActive(true);
		((Morphogene*) mGenes[attachmentIndex])->getGeneBranches().push_back(
				branch);

		Morphogene* donorGene = ((Morphogene*) donor->getGenes()[geneIndex]);
		Morphogene* donorGeneCopy = donorGene->clone();
		mGenes.push_back(donorGeneCopy);
		branch->setBranchGeneType(mGenes.size() - 1);
		//int newGeneIndex = mGenes.size() - 1;

		std::vector<Morphogene*> visitGenes;
		visitGenes.push_back(donorGeneCopy);

		Morphogene* currentGene;

		int genesCopied = 0;

		while (!visitGenes.empty()) {
			currentGene = (*visitGenes.end());
			visitGenes.pop_back();

			std::vector<MorphogeneBranch*>::iterator gbit =
					currentGene->getGeneBranches().begin();
			for (; gbit != currentGene->getGeneBranches().end(); gbit++) {
				Morphogene* nextGeneCopy =
						((Morphogene*) donor->getGenes()[(*gbit)->getBranchGeneType()])->clone();
				if (genesCopied < geneQty) {
					visitGenes.push_back(nextGeneCopy);
					genesCopied++;
				}

				mGenes.push_back(nextGeneCopy);
				(*gbit)->setBranchGeneType(mGenes.size() - 1);
			}
		}

	} else {
		return;
	}

}

MixedGenome* MixedGenome::clone() {
	return new MixedGenome(*this);
}
