/*
 * statistics.hpp
 *
 *  Created on: Nov 28, 2013
 *		Author: fabiorjvieira
 */


#include "main.hpp"

int performance(int argn, char ** argc)
{
	if (argn != 4) {std::cout << "ops"; return 0;}

	std::ifstream file;
	std::stringstream fileName;
	std::vector < long double > slotSizes;
	unsigned long int squareSideSize, numberOfNetworks, slotSize, numberOfNodes, networkInstance, numberOfActivations, numberOfSquareSizes, numberOfLinks;
	std::string algorithm;
	long double average, standardDeviation;

	fileName.str(argc[1]);
	numberOfSquareSizes = atoi(argc[2]);
	numberOfNetworks = atoi(argc[3]);
	file.open(fileName.str().data(), std::ios::in);
	if (not file)
	 {
		std::cerr << fileName.str() << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "# Reading file:" << fileName.str() << std::endl;
	std::cout << "# nodes square algorithm average standardDeviation 'average - (standardDeviation/5)' 'average + (standardDeviation/5)'" << std::endl;

	for (unsigned long int squareSizeIndex = 0; squareSizeIndex < numberOfSquareSizes; squareSizeIndex++)
	{
		average = 0;
		standardDeviation = 0;
		slotSizes.clear();
		for (unsigned long int slotSizeIndex = 0; slotSizeIndex < numberOfNetworks; slotSizeIndex++)
		{
			file >> numberOfNodes >> squareSideSize >> networkInstance >> algorithm >> numberOfActivations >> slotSize >> numberOfLinks;
			if (numberOfLinks > 0)
			{
				average += slotSize/(long double)(numberOfLinks * numberOfActivations);
				slotSizes.push_back(slotSize/(long double)(numberOfLinks * numberOfActivations));
			}
			else
			{
				average += 1;
				slotSizes.push_back(1);
			}
		}
		average /= (long double) numberOfNetworks;
		for (unsigned long int slotSizeIndex = 0; slotSizeIndex < numberOfNetworks; slotSizeIndex++)
			standardDeviation += (average - slotSizes.at(slotSizeIndex))*(average - slotSizes.at(slotSizeIndex));
		standardDeviation /= numberOfNetworks;
		standardDeviation = std::sqrt(standardDeviation);
		std::cout << numberOfNodes << " " << squareSideSize << " " << algorithm << " " << average << " " << standardDeviation << " " << average - (standardDeviation/5) << " " << average + (standardDeviation/5) << std::endl;
	}
	file.close();

	return 0;
}

int connectivity(int argn, char ** argc)
{
	int result = -1;

	//graph
	std::vector < Position > nodePosition;
	std::vector < std::vector < bool > > neighborhood;
	unsigned long int numberOfNodes;
	//maximum communication distance between two nodes when there are only two nodes on the square
	long double maximumCommunicationDistance;
	//wireless network physical parameters
	long double transmissionPower, pathLoss, bandWidth, snrThreshold;
	//statistic
	unsigned int numberOfRounds, numberOfEdges, numberOfConnectedGraphs;
	std::vector < unsigned int > nodeDegree;
	std::vector < std::vector < unsigned int > > nodeDegreeDistribution;
	std::vector < long double > nodeDegreeDistributionAverage;
	std::vector < long double > nodeDegreeDistributionStandardDeviation;
	//side of the square area where nodes will be placed
	unsigned int maximumSquareSide, minimumSquareSide;
	//random x and y position on the square
	Position randomPostion;
	//scheduling
	std::vector < Link > links;

	//Nodes' file
	 std::ofstream file;
	 std::stringstream fileName, directory;

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
	//	power	band		pathloss	sinr	nodes	.dot
	// 0.030 20000000 4.0		25	 100	~/git/general/WISp/networks/100/nodes/graph.11447.0.dot.node 1
	// 0.030 20000000 4.0		0	  5	  ~/git/general/WISp/networks/5/nodes/graph.0.0.dot.node 1

	//load a network topology from a ".dot" file (nodePosition)
	//Calculate the neighborhood (link set)
	//Inform transmissionPower, pathLoss, snrThreshold, bandWidth

	//argument test
	if (argn < 8)
	{
		std::cout << "Seven arguments are required in this order: " << std::endl;
		std::cout << "transmission power greater than zero (W)" << std::endl;
		std::cout << "signal bandwidth greater than zero (H)" << std::endl;
		std::cout << "path loss exponent greater than two" << std::endl;
		std::cout << "signal to noise threshold greater than one (dB)" << std::endl;
		std::cout << "number of nodes greater than one" << std::endl;
		std::cout << "number of rounds greater than one" << std::endl;
		std::cout << "path of the graph files (.node)" << std::endl;
		return result;
	}
	transmissionPower = atof(argc[1]);
	bandWidth = atof(argc[2]);
	pathLoss = atof(argc[3]);
	snrThreshold = atof(argc[4]);
	numberOfNodes = atoi(argc[5]);
	numberOfRounds = atoi(argc[6]);
	directory.str(argc[7]);
	if (numberOfNodes < 2) return result;
	if (transmissionPower <= 0) return result;
	if (pathLoss < 2) return result;
	if (bandWidth <= 0) return result;
	if (snrThreshold < 0) return result;
	if (numberOfRounds < 1) return result;
	std::cout << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", rounds: " << numberOfRounds << ", nodes: " << numberOfNodes << ", directory: " << directory.str() << std::endl;

	//simulation area initialization
	maximumCommunicationDistance = linkSize(transmissionPower, pathLoss, bandWidth, snrThreshold);
	maximumSquareSide = (1.00*numberOfNodes)*maximumCommunicationDistance/std::sqrt(2);
	minimumSquareSide = (0.01*numberOfNodes)*maximumCommunicationDistance/std::sqrt(2);

	//graph initialization
	nodePosition.resize(numberOfNodes, randomPostion);

	//statistic initialization
	nodeDegreeDistribution.resize(numberOfRounds, std::vector < unsigned int >(numberOfNodes, 0.0));

	//Initializing the connectivity file
	fileName.str("graph.connectivity.statistic");
	file.open(fileName.str().data(), std::ios::out);
	file << "#square side, communication range, average number of disconnected nodes, average degree of the graphs, connected graphs" << std::endl;
	file << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", rounds: " << numberOfRounds << std::endl;
	file.close();

	for (unsigned int squareSide = minimumSquareSide; squareSide <= maximumSquareSide; squareSide += maximumCommunicationDistance/2)
	{
		//statistic reset
		nodeDegreeDistributionAverage.assign(numberOfNodes, 0.0);
		numberOfEdges = 0;
		numberOfConnectedGraphs = 0;

		for (unsigned int round = 0; round < numberOfRounds; round++)
		{
			//".dot" file for each graph
			fileName.str("");
			fileName << directory.str() << "/graph." << squareSide << "." << round << ".dot.node";

			//graph reset
			neighborhood.assign(numberOfNodes, std::vector < bool >(numberOfNodes, false));
			//load the network and discover the link set
			loadNetwork(fileName.str(), maximumCommunicationDistance, nodePosition, neighborhood, links);

			//statistic reset
			nodeDegree.assign(numberOfNodes, 0);
			nodeDegreeDistribution.at(round).assign(numberOfNodes, 0);

			for (unsigned int newNode = 1; newNode < numberOfNodes; newNode++)
			{
				for (unsigned int node = newNode -1; node != (unsigned int) -1; node--)
					if (euclideanDistance(nodePosition.at(newNode), nodePosition.at(node)) <= maximumCommunicationDistance)
					{
						//counting the degree of each node
						nodeDegree.at(node)++;
						nodeDegree.at(newNode)++;
						//counting the number of edges
						numberOfEdges++;
						neighborhood.at(newNode).at(node) = true;
						neighborhood.at(node).at(newNode) = true;
					}
			}

			//connectivity test
			if (connected(neighborhood)) numberOfConnectedGraphs++;

			//counting the number of nodes for each possible degree
			for (unsigned int node = 0; node < numberOfNodes; node++)
			{
				nodeDegreeDistribution.at(round).at(nodeDegree.at(node))++;
				nodeDegreeDistributionAverage.at(nodeDegree.at(node))+=1.0;
			}
		}

		fileName.str("graph.connectivity.statistic");
		file.open(fileName.str().data(), std::ios::app);
		if (not file)
		 {
			std::cerr << fileName.str() << " not created!" << std::endl;
		 	exit(1);
		}
		std::cout << "Writing file:" << fileName.str() << std::endl;

		//square side, communication range, average number of disconnected nodes of the graphs, average degree of the graphs, number of connected graphs
		file << squareSide << " " << maximumCommunicationDistance << " " << nodeDegreeDistributionAverage.at(0)/numberOfRounds << " " << (long double)numberOfEdges/(long double)(numberOfNodes * numberOfRounds) << " " << numberOfConnectedGraphs << std::endl;
		file.close();

		fileName << "statistics/graph." << squareSide << ".degreeDistribution.statistic";
		file.open(fileName.str().data(), std::ios::out);
		if (not file)
		 {
			std::cerr << fileName.str() << " not created!" << std::endl;
		 	exit(1);
		}
		std::cout << "Writing file:" << fileName.str() << std::endl;
		file << "#degree, average degree distribution, standard deviation of each degree" << std::endl;
		file << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", rounds: " << numberOfRounds << std::endl;

		//degree, degree distribution and standard deviation of each degree
		nodeDegreeDistributionStandardDeviation.assign(numberOfNodes, 0.0);
		for (unsigned int degree = 0; degree < numberOfNodes; degree++)
		{
			nodeDegreeDistributionAverage.at(degree)/=numberOfRounds;
			for (unsigned int round = 0; round < numberOfRounds; round++)
				nodeDegreeDistributionStandardDeviation.at(degree) += std::pow(nodeDegreeDistribution.at(round).at(degree) - nodeDegreeDistributionAverage.at(degree), 2);
			nodeDegreeDistributionStandardDeviation.at(degree) = std::sqrt(nodeDegreeDistributionStandardDeviation.at(degree)/numberOfRounds);
			file << degree << " " << nodeDegreeDistributionAverage.at(degree) << " " << nodeDegreeDistributionStandardDeviation.at(degree) << std::endl;
		}
		file.close();
	}

	return 0;
}

int compare(int argn, char ** argc)
{
	if (argn != 5) return 0;

	std::ifstream fileA, fileB;
	std::stringstream fileNameA, fileNameB;
	std::string valueA, valueB;
	unsigned int targetColumn, numberOfColumns;
	bool readOk;

	fileNameA.str(argc[1]);
	fileNameB.str(argc[2]);
	targetColumn = atoi(argc[3]);
	numberOfColumns = atoi(argc[4]);

	fileA.open(fileNameA.str().data(), std::ios::in);
	if (not fileA)
	 {
		std::cerr << fileNameA.str() << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "# Reading file A:" << fileNameA.str() << std::endl;

	fileB.open(fileNameB.str().data(), std::ios::in);
	if (not fileA)
	 {
		std::cerr << fileNameB.str() << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "# Reading file B:" << fileNameB.str() << std::endl;

	readOk = true;
	while (readOk)
	{
		for (unsigned int column = 0; column <= targetColumn and readOk; column++)
		{
			readOk = (fileA >> valueA);
			readOk = (fileB >> valueB);
		}
		if (readOk)
		{
			if (atof(valueA.c_str()) > atof(valueB.c_str())) std::cout << "A > B" << std::endl;
			else if (atof(valueA.c_str()) < atof(valueB.c_str())) std::cout << "A < B" << std::endl;
			else std::cout << "A = B" << std::endl;

			for (unsigned int column = 0; column < numberOfColumns - targetColumn -1 and readOk; column++)
			{
				readOk = (fileA >> valueA);
				readOk = (fileB >> valueB);
			}
		}
	}

	return 0;
}

