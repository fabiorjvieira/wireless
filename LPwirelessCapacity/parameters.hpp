/*
 * parameters.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

static float PathLossExponent = 2; //alpha
static float SNRthreshold = 1; //beta
static float MinimumDistance = 20; //meters
static float BandWidth = 20e+6; //Hertz
static float TransmissionPower = 100e-3; //Watts
static float LearningStep = 1e-3;

static std::string NodeFileName = "node.in";
static std::string LinkFileName = "link.in";

static void loadParameters(char * args[])
{
	PathLossExponent = atof(args[1]);
	MinimumDistance = atof(args[2]);
	SNRthreshold = atof(args[3]);
	BandWidth = atof(args[4]);
	TransmissionPower = atof(args[5]);
	TransmissionPower = atof(args[6]);
	NodeFileName = args[7];
	LinkFileName = args[8];
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

#endif /* PARAMETERS_HPP_ */
