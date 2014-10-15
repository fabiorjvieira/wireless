/*
 * main.cpp
 *
 *  Created on: Oct 30, 2013
 *		Author: fabiorjvieira
 */

#include "main.hpp"

//g++ main.cpp -O3 -o ser.bin
//ser.bin networks/7.0.routes networks/7.0.neighborhood 2

int routes(int argn, char ** argc)
{
	std::string routesFileName;
   std::string neighborhoodFileName;
	Routes routes;
	NodeNeighborhood nodeNeighborhood;
	std::vector < Link > links;
	ConflictMatrix conflictMatrix, conflictMatrixPhotograph;
	FractionalColoring bestFractionalColoring, fractionalColoring;
	std::vector < Enumeration > enumerations;
	bool distance2, cycleDetected;
	unsigned long int slidingWindowSize, increaseFactor = 2;

   if (argn != 4)
 	{
		std::cout << "Three parameters are required: routes filename with path, neighborhood filename with path and the distance (distance 1 = 1 or distance 2 = 2)." << std::endl;
		return -1;
	}

	//load parameters
	routesFileName = argc[1];
	neighborhoodFileName = argc[2];
	if (argc[3][0] == '1') distance2 = false;
	else if (argc[3][0] == '2') distance2 = true;
	else
	{
		std::cout << "Three parameters are required: routes filename with path, neighborhood filename with path and the distance (distance 1 = 1 or distance 2 = 2)." << std::endl;
		return -1;
	}

	//load routes
	loadRoutes(routesFileName, routes);

	//load neighborhood
	loadNeighborhood(neighborhoodFileName, nodeNeighborhood);

	//different initial configurations obtained by the six enumerations
	enumerations.resize(6);
	enumerations.at(0).sortOrder =     ASCENDING; enumerations.at(0).search = INTERLEVED;
	enumerations.at(1).sortOrder =     ASCENDING; enumerations.at(1).search = DEPTH_FIRST;
	enumerations.at(2).sortOrder =     ASCENDING; enumerations.at(2).search = DEPTH_FIRST;
	enumerations.at(3).sortOrder = not ASCENDING; enumerations.at(3).search = INTERLEVED;
	enumerations.at(4).sortOrder = not ASCENDING; enumerations.at(4).search = BREADTH_FIRST;
	enumerations.at(5).sortOrder = not ASCENDING; enumerations.at(5).search = DEPTH_FIRST;

	//worst coloring
	bestFractionalColoring.numberOfColors = 1;
	bestFractionalColoring.cycleSize = -1;

	for (unsigned int enumerationIndex = 0; enumerationIndex < enumerations.size(); enumerationIndex++)
	{
		slidingWindowSize = links.size() +1;
		//sort routes
		sortRoutes(routes, enumerations.at(enumerationIndex).sortOrder);
		//build links
		buildLinks(routes, links, enumerations.at(enumerationIndex).search);
		//build conflict matrix
		buildConflictMatrix(links, nodeNeighborhood, conflictMatrix, distance2);
		//execute dynamic until cycle is found
		//photograph strategy + increasing slide windows
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
		//best?
		if (fractionalColoring.numberOfColors/(long double)fractionalColoring.cycleSize > bestFractionalColoring.numberOfColors/(long double)bestFractionalColoring.cycleSize)
			bestFractionalColoring = fractionalColoring;
	}

	//return fractional coloring
	std::cout << "Routes: " << routesFileName << ", Node Colors: " << bestFractionalColoring.numberOfColors << ", Graph Colors: " << bestFractionalColoring.cycleSize << std::endl;

	return 0;
}
