/*
 * parameters.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include "constants.hpp"
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static float PathLossExponent = 2; //alpha
static float SNRthreshold = 1; //beta
static float MinimumDistance = 20; //meters
static float BandWidth = 20e+6; //Hertz
static float TransmissionPower = 100e-3; //Watts
static float ThermalNoise = BOLTZMANN * REFERENCE_TEMPERATURE * BandWidth;
static float NoiseFloor = ThermalNoise; // * NOISE_FIGURE; // Noise Figure = perda dentro do receptor não ideal

static float LearningStep = 1e-3;

static std::string NodeFileName;
static std::string LinkFileName;

static void loadParameters(int nargs, char * args[])
{
	for(int parameterIndex = 1; parameterIndex < nargs;)
	{
		if (PARAMETER_BandWidth.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) BandWidth = atof(args[parameterIndex]);
		}
		else if (PARAMETER_PathLossExponent.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) PathLossExponent = atof(args[parameterIndex]);
		}
		else if (PARAMETER_SNRthreshold.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) SNRthreshold = atof(args[parameterIndex]);
		}
		else if (PARAMETER_MinimumDistance.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) MinimumDistance = atof(args[parameterIndex]);
		}
		else if (PARAMETER_TransmissionPower.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) TransmissionPower = atof(args[parameterIndex]);
		}
		else if (PARAMETER_ThermalNoise.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) ThermalNoise = atof(args[parameterIndex]);
		}
		else if (PARAMETER_NoiseFloor.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) NoiseFloor = atof(args[parameterIndex]);
		}
		else if (PARAMETER_LearningStep.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) LearningStep = atof(args[parameterIndex]);
		}
		else if (PARAMETER_NodeFileName.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) NodeFileName = args[parameterIndex];
		}
		else if (PARAMETER_LinkFileName.compare(args[parameterIndex]) == 0)
		{
			if (args[++parameterIndex][0] != PARAMETER_PREFIX[0]) LinkFileName = args[parameterIndex];
		}
		else if (HELP.compare(args[parameterIndex]) == 0)
		{
			std::cout << HELP << std::endl;
			std::cout << PARAMETER_PathLossExponent << std::endl;
			std::cout << PARAMETER_SNRthreshold << std::endl;
			std::cout << PARAMETER_MinimumDistance << std::endl;
			std::cout << PARAMETER_BandWidth << std::endl;
			std::cout << PARAMETER_TransmissionPower << std::endl;
			std::cout << PARAMETER_ThermalNoise << std::endl;
			std::cout << PARAMETER_NoiseFloor << std::endl;
			std::cout << PARAMETER_LearningStep << std::endl;
			std::cout << PARAMETER_NodeFileName << std::endl;
			std::cout << PARAMETER_LinkFileName << std::endl;
			exit(0);
		}
		else std::cout << args[parameterIndex] << PARAMETER_NOT_RECOGNIZED << std::endl;
	}
}

struct Position
{
	float x, y, z;
};

static float euclideanDistance(const Position positionA, const Position positionB)
{
	float x = (positionA.x - positionB.x);
	float y = (positionA.y - positionB.y);
	float z = (positionA.z - positionB.z);
	return sqrt(x*x + y*y + z*z);
}

static unsigned int getRandomSeed()
{
	unsigned int random = 1;//atol(getenv("RANDOM"));

	return time(NULL) * getpid() * random;
}

#endif /* PARAMETERS_HPP_ */
