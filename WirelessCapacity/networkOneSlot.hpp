/*
 * network.cpp
 *
 *  Created on: Nov 11, 2013
 *      Author: fabiorjvieira
 */

#include "main.hpp"

#define PI 3.14159265

int networkOneSlot(int argn, char ** argc)
{
	int result = -1;

	//graph
	std::vector < Position > nodePosition;
	std::vector < std::vector < bool > > neighborhood;
	unsigned int numberOfNodes;
	//statistic
	unsigned int numberOfRounds;
	std::vector < unsigned int > nodeDegree;
	std::vector < std::vector < unsigned int > > nodeDegreeDistribution;
	//side of the square area where nodes will be placed
	unsigned int maximumSquareSide, minimumSquareSide;
	//random x and y position on the square
	Position randomPostion;
	long double angle, distanceFromPair;
	//maximum communication distance between two nodes when there are only two nodes on the square
	long double maximumCommunicationDistance;
	//wireless network physical parameters
	long double transmissionPower, pathLoss, bandWidth, snrThreshold;

    std::ofstream file;
    std::stringstream fileName, directory;

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
	//power	band		pathloss	sinr	nodes	rounds
	//0.030	20000000	4.0			25		100		100
	//Part 1 - Network Topology

    /*
	 * gerar os grafos
	 *    escolher o numero de verties (n)
	 *    escolher a potencia, ruido de fundo, SINR, perda pela distância e   (802.11 => distancia d)
	 *    variar a area da incial a final (lado do quadrado d/sqrt(2)<s<=d*(n-1)/sqrt(2), s'=s-d, (n-2)/sqrt(2) rodadas)
	 *       repetir m vezes
	 *          posicionar os nodes aleatoriamente
	 *          Links = descobrir quais enlaces existem
	 *          histograma da distribuição do grau dos vértices (descobrir a partir de qual area o grafo passa a ser interessante)
	 */

	//argument test
	if (argn != 8)
	{
		std::cout << "Seven arguments are required in this order: " << std::endl;
		std::cout << "transmission power greater than zero (W)" << std::endl;
		std::cout << "signal band width greater than zero (H)" << std::endl;
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
	if (numberOfRounds < 1) return result;
	if (transmissionPower <= 0) return result;
	if (pathLoss < 2) return result;
	if (bandWidth <= 0) return result;
	if (snrThreshold < 1) return result;
	std::cout << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", rounds: " << numberOfRounds << ", directory: " << directory.str() << std::endl;

	//simulation area initialization
	maximumCommunicationDistance = linkSize(transmissionPower, pathLoss, bandWidth, snrThreshold);
	maximumSquareSide = 2*numberOfNodes*maximumCommunicationDistance/std::sqrt(2);
	minimumSquareSide = maximumCommunicationDistance/std::sqrt(2);

	//graph initialization
	nodePosition.resize(numberOfNodes, randomPostion);

	//statistic initialization
	nodeDegreeDistribution.resize(numberOfRounds, std::vector < unsigned int >(numberOfNodes, 0.0));

	//randomic seed initialization
	std::srand(time(NULL)*getpid());

	//reseting the connectivity file
	fileName.str("");
	fileName << "graph.connectivity.statistic";
	file.open(fileName.str().data(), std::ios::out);
	file << "#square side, communication range, average number of disconnected nodes of the graphs, average degree of the graphs" << std::endl;
	file << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", rounds: " << numberOfRounds << std::endl;
	file.close();

	bool fail;

	for (unsigned int squareSide = maximumSquareSide; squareSide > minimumSquareSide; squareSide -= maximumCommunicationDistance/(2*std::sqrt(2)))
	{
		for (unsigned int round = 0; round < numberOfRounds; round++)
		{
			//".dot" file for each graph
			fileName.str("");
			fileName << "./" << directory.str() << "/graph." << squareSide << "." << round << ".dot";
			file.open(fileName.str().data(), std::ios::out);
			if (not file)
		    {
				std::cerr << fileName.str() << " not created!" << std::endl;
		    	exit(1);
			}
			std::cout << "Writing file:" << fileName.str() << std::endl;
			file << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", rounds: " << numberOfRounds << std::endl;

			//graph reset
			neighborhood.assign(numberOfNodes, std::vector < bool >(numberOfNodes, false));

			//statistic reset
			nodeDegree.assign(numberOfNodes, 0);
			nodeDegreeDistribution.at(round).assign(numberOfNodes, 0);

			//the other nodes
			for (unsigned int newNode = 0; newNode < numberOfNodes;)
			{
            fail=false;
            for (unsigned int tries = 0; tries < 100000; tries++)
            {
               nodePosition.at(newNode).xPosition = (long double)rand()*(long double)squareSide/(long double)RAND_MAX;
               nodePosition.at(newNode).yPosition = (long double)rand()*(long double)squareSide/(long double)RAND_MAX;
               for (unsigned int node = 0; node < newNode; node++)
                  if (euclideanDistance(nodePosition.at(newNode), nodePosition.at(node)) <= maximumCommunicationDistance)
                  {
                     fail=true;
                     return 0;
                  }
				}

            if (not fail)
            {
               for (unsigned int tries = 0; tries < 100000; tries++)
               {
                  angle = (long double)rand()/((long double)RAND_MAX/(long double)PI*2);
                  distanceFromPair = (long double)rand()/((long double)RAND_MAX/maximumCommunicationDistance);
                  nodePosition.at(newNode +1).xPosition = (std::cos(angle)*distanceFromPair) + nodePosition.at(newNode).xPosition;
                  nodePosition.at(newNode +1).yPosition = (std::sin(angle)*distanceFromPair) + nodePosition.at(newNode).yPosition;
                  for (unsigned int node = 0; node < newNode; node++)
                     if (euclideanDistance(nodePosition.at(newNode +1), nodePosition.at(node)) <= maximumCommunicationDistance)
                     {
                        fail=true;
                        return 0;
                     }
               }
            }

            if (not fail)
            {
               neighborhood.at(newNode).at(newNode +1) = true;
               neighborhood.at(newNode +1).at(newNode) = true;
               newNode++;
            }
            else break;
			}

			if (not fail)
			{
	         file << "graph {" << std::endl;
	         for (unsigned int newNode = 0; newNode < numberOfNodes; newNode++)
	         {
	            file << "   " << newNode << " [ pos = " << '"' << nodePosition.at(newNode).xPosition << "," << nodePosition.at(newNode).yPosition << "!" << '"' << " ];" << std::endl;
	            file << "   " << newNode << " -- " << newNode +1 << ";" << std::endl;
	            file << "   " << newNode +1  << " [ pos = " << '"' << nodePosition.at(newNode +1).xPosition << "," << nodePosition.at(newNode +1).yPosition << "!" << '"' << " ];" << std::endl;
	         }
	         file << "}" << std::endl << std::endl;
			}
			file.close();
		}
	}

	return 0;
}
