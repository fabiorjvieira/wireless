/*
 * main.cpp
 *
 *  Created on: Oct 30, 2013
 *		Author: fabiorjvieira
 */

#include "main.hpp"

//g++ main.cpp -O3 -o ser.bin
//ser.bin networks/7.0.routes networks/7.0.neighborhood 2

int conflicts(int argn, char ** argc)
{
   std::string conflictGraphFileName;
	ConflictMatrix conflictMatrix, conflictMatrixPhotograph;
	FractionalColoring fractionalColoring;
	bool cycleDetected, alreadyOriented;
	unsigned long int slidingWindowSize, increaseFactor = 2;

   if (argn != 3)
	{
		std::cout << "Two parameters are required: conflict graph filename with path and already oriented (1 = yes or 0 = no)." << std::endl;
		return -1;
	}

	//load parameters
   conflictGraphFileName = argc[1];
	if (argc[2][0] == '0') alreadyOriented = false;
	else if (argc[3][0] == '1') alreadyOriented = true;
	else
	{
      std::cout << "Two parameters are required: conflict graph filename with path and already oriented (1 = yes or 0 = no)." << std::endl;
		return -1;
	}

	//load conflict graph
	loadConflictMatrix(conflictGraphFileName, conflictMatrix);

   //initial orientation
	if (not alreadyOriented) acyclicOrientation(conflictMatrix);

	//worst coloring
	fractionalColoring.numberOfColors = 1;
	fractionalColoring.cycleSize = -1;

   slidingWindowSize = conflictMatrix.links.size() +1;
	cycleDetected = false;
	do
	{
		conflictMatrixPhotograph = conflictMatrix;
			for (unsigned long int configuration = 0; configuration < slidingWindowSize; configuration++)
			edgeReversalDynamics(conflictMatrix);
			for (unsigned long int configuration = 0; configuration < slidingWindowSize +1 and not cycleDetected; configuration++)
		{
			edgeReversalDynamics(conflictMatrix);
			cycleDetected = compareConflictMatrices(conflictMatrix, conflictMatrixPhotograph);
		}
      slidingWindowSize *= increaseFactor;
	}
	while (not cycleDetected);

	//capture cycle
	fractionalColoring = captureCycle(conflictMatrix);

	//return fractional coloring
	std::cout << "Routes: " << conflictGraphFileName << ", Node Colors: " << fractionalColoring.numberOfColors << ", Graph Colors: " << fractionalColoring.cycleSize << std::endl;

	return 0;
}
