/*
 * Genome_test.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: leviathan
 */

//# corresponding header
#include <gtest/gtest.h>
//## controller headers
//## model headers
#include <boost/math/constants/constants.hpp>
#include <configuration/PopulationConfiguration.hpp>
#include <controller/SaveController.hpp>
#include <model/universe/evolution/population/creature/genome/MixedGenome.hpp>

//## view headers
//# custom headers
//## base headers
//## configuration headers

//## controller headers

//## model headers
//## view headers
//## utils headers

class GenomeTest: public ::testing::Test {
protected:
	virtual void SetUp() {
		// Set up an object of the class you want to test
		genome = new MixedGenome();
		genome->createRandomGenome(30);
		genome->linkRandomGenes();
	}

	virtual void TearDown() {
		// delete and set the pointer to zero
		delete genome;
		genome = 0;
	}
	MixedGenome* genome;
};

class GenomeSerializationTest: public ::testing::Test {
protected:
	virtual void SetUp() {
		genome = new MixedGenome();
		genome->createRandomGenome(30);

		genome2 = new MixedGenome();

		SaveController<MixedGenome> saveController;

		saveController.save(*genome, "Genome.test");

		saveController.restore(*genome2, "Genome.test");
	}

	virtual void TearDown() {
		delete genome;
		genome = NULL;
		delete genome2;
		genome2 = NULL;
	}
	MixedGenome* genome;

	MixedGenome* genome2;
};

TEST_F(GenomeTest,hasNormalGenes) {
	ASSERT_TRUE(
			PopulationConfiguration::POPULATION_GENES_INITIAL_MEAN
					- PopulationConfiguration::POPULATION_GENES_INITIAL_VAR
					<= genome->getGenes().size()
					<= PopulationConfiguration::POPULATION_GENES_INITIAL_MEAN
							+ PopulationConfiguration::POPULATION_GENES_INITIAL_VAR);
}

TEST_F(GenomeTest,areAllBranchesSet) {
	std::vector<Morphogene*>::const_iterator geneIt = genome->getGenes().begin();
	for (; geneIt != genome->getGenes().end(); geneIt++) {
		std::vector<MorphogeneBranch*>::const_iterator branchIt =
				(*geneIt)->getGeneBranches().begin();
		for (; branchIt != (*geneIt)->getGeneBranches().end(); branchIt++) {
			ASSERT_TRUE((*branchIt)->getBranchGeneType() != -1);
		}
	}
}

TEST_F(GenomeSerializationTest,isEqualAfterSerialization) {
	ASSERT_TRUE(genome != genome2);
	ASSERT_TRUE(genome->equals(*genome2));
}

