/*
 * Ones.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: leviathan
 */

#include "Ones.h"

Ones::Ones(double weight):Jury(Jury::ONES, weight),mEvaluationSubject(0){

}

Ones::~Ones() {
	// TODO Auto-generated destructor stub
}

void Ones::setEvaluationSubject(const std::vector<bool>& x) {
	mEvaluationSubject = x;
}

/**
 * fitness function for the counting ones problem
 */
void Ones::evaluateFitness() {
	unsigned i;
	for (mFitness = 0., i = 0; i < mEvaluationSubject.size(); mFitness += mEvaluationSubject[i++])
		;
}

