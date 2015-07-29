/*
 * parameters.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

float PathLossExponent = 2;
float MinimumDistance = 20;
float SNRthreshold = 1;
float BandWidth = 20e+6;
float TransmissionPower = 100e-3;

void loadParameters(char * args[])
{
	PathLossExponent = atof(args[1]);
	MinimumDistance = atof(args[2]);
	SNRthreshold = atof(args[3]);
	BandWidth = atof(args[4]);
	TransmissionPower = atof(args[5]);
}

struct Position
{
	float x, y, z;
};

float euclideanDistance(Position positionA, Position positionB)
{
	float x = (positionA.x - positionB.x);
	float y = (positionA.y - positionB.y);
	float z = (positionA.z - positionB.z);
	return sqrt(x*x + y*y + z*z);
}

#endif /* PARAMETERS_HPP_ */
