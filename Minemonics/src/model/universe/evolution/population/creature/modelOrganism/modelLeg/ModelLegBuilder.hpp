#ifndef MODEL_UNIVERSE_EVOLUTION_POPULATION_CREATURE_MODELORGANISM_MODELLEG_MODELLEGBUILDER_HPP_
#define MODEL_UNIVERSE_EVOLUTION_POPULATION_CREATURE_MODELORGANISM_MODELLEG_MODELLEGBUILDER_HPP_

//# corresponding headers
//# forward declarations
class Genome;

//# system headers
//## controller headers
//## model headers
//## view headers
//# custom headers
//## base headers
//## configuration headers
//## controller headers
//## model headers
//## view headers
//## utils headers

/**
 * @brief		The model leg builder creates a simple model leg in the genome.
 * @details		Details
 * @date		2015-11-05
 * @author		Benjamin Ellenberger
 */
class ModelLegBuilder {
public:
	ModelLegBuilder();
	virtual ~ModelLegBuilder();

	static void build(Genome* genome);
};

#endif /* MODEL_UNIVERSE_EVOLUTION_POPULATION_CREATURE_MODELORGANISM_MODELLEG_MODELLEGBUILDER_HPP_ */