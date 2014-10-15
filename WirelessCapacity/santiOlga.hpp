/*
 * main.cpp
 *
 *  Created on: Oct 18, 2011
 *		Author: fabiorjvieira
 */

#include "main.hpp"

int santiOlga(int argn, char ** argc)
{
	int result = -1;

	//graph
	std::vector < Position > nodePosition;
	std::vector < std::vector < bool > > neighborhood;
	unsigned long int numberOfNodes, numberOfschedulesLimit;
	//maximum communication distance between two nodes when there are only two nodes on the square
	long double maximumCommunicationDistance;
	//wireless network physical parameters
	long double transmissionPower, pathLoss, bandWidth, snrThreshold;
	Position randomPostion;

	std::ifstream file;
	std::stringstream fileName;

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
	//	power	band		pathloss	sinr	nodes	.dot
	//	0.030	20000000	4.0			25		100		~/git/general/WISp/networks/100/nodes/graph.11447.0.dot.node

	//Part 2 - Link Schedule
	//load a network topology from a ".dot" file (nodePosition)
	//Calculate the neighborhood (link set)
	//Inform transmissionPower, pathLoss, snrThreshold, bandWidth

   //argument test
   if (argn < 4)
   {
      std::cout << "3 arguments are required in this order: " << std::endl;
      std::cout << "number of nodes greater than one" << std::endl;
      std::cout << "graph '.dot' file" << std::endl;
      std::cout << "Number of schedules" << std::endl;
      return result;
   }
   transmissionPower = 9600;
   bandWidth = 0;
   pathLoss = 3;
   snrThreshold = 1.2;
   numberOfNodes = atoi(argc[1]);
   fileName.str(argc[2]);
   numberOfschedulesLimit = atoi(argc[3]);
   if (numberOfNodes < 2) return result;
   if (numberOfschedulesLimit < 1) return result;
   std::cout << "# schedule: " << numberOfschedulesLimit << ", power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", file: " << fileName << std::endl;

   //Initialization of the basic parameters
   maximumCommunicationDistance = 20;
	//graph reset
	neighborhood.assign(numberOfNodes, std::vector < bool >(numberOfNodes, false));
	//graph initialization
	nodePosition.resize(numberOfNodes, randomPostion);

	file.open(fileName.str().data(), std::ios::in);
	if (not file)
	 {
		std::cerr << fileName.str() << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "#Reading file:" << fileName.str() << std::endl;

	//Loading every vertex position
	for (unsigned long int node = 0; node < numberOfNodes; node++)
		file >> nodePosition.at(node).xPosition >> nodePosition.at(node).yPosition;
	file.close();

	//Reconstructing the neighborhood
   //Corrections on the neighborhood because of Olga scenarios
   for (unsigned long int newNode = 0; newNode < numberOfNodes; newNode+=2)
   {
      neighborhood.at(newNode).at(newNode +1) = true;
      neighborhood.at(newNode +1).at(newNode) = true;
   }

	//scheduling
	std::vector < Link > links;
	std::vector < Slot > slots;
	Slot emptySlot;
	Link dummyLink;
	unsigned long int scheduledLink;

	//recursion - multi-coloring
	unsigned long int bestSlotsSize = 1, bestNumberOfschedules = 0, currentSlot;

	//candidate variables represent a link that may be selected to be included in a slot if it is a viable option (SNR) and better than the selected one
	long double candidateInverseSNR;
	unsigned long int candidateLink;
	bool candidateOk;

	//Initialization of the set of links to be schedule
	for (unsigned long int nodeA = 0; nodeA < numberOfNodes; nodeA++)
		for (unsigned long int nodeB = nodeA +1; nodeB < numberOfNodes; nodeB++)
		{
			if (neighborhood.at(nodeA).at(nodeB))
			{
				//One hop one direction single demand network load
				dummyLink.senderNode = nodeA;
				dummyLink.receiverNode = nodeB;
				dummyLink.numberOfSchedules = 0;
				links.push_back(dummyLink);
				//I am not using the most general model of one hop network load
				//The most general model is given by the following procedure:
				//0-For a link Y
				//1-Choose at random an integer X to be the link demand
				//2-Replicate link Y X times in the set of links to be schedule
				//2-Choose at random the direction of each replica
			}
		}

   //Calculate the relative interference between each pair of nodes for all links in the set of links
	interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);
	interferenceNumberOfEdges(links, snrThreshold, bandWidth);

	//santi---

	//insertion sort
	unsigned long int holePosition;
	Link linkToInsert;

	//links must be order by the interference number of edges in the receiver
	for (unsigned long int link = 1; link < links.size(); link++)
	{
		linkToInsert = links.at(link);
		holePosition = link;
		while (holePosition > 0 and linkToInsert.interferenceNumberOfEdges > links.at(holePosition - 1).interferenceNumberOfEdges)
		{
			links.at(holePosition) = links.at(holePosition - 1);
			holePosition--;
		}
		links.at(holePosition) = linkToInsert;
	}

	//Recalculate the relative interference because of the reordering procedure (stupid solution, but ...)
	interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);
	interferenceNumberOfEdges(links, snrThreshold, bandWidth);

	if (links.size() == 0)
	{
		printResults("santi", fileName.str(), 0, numberOfNodes, slots.size(), links.size(), false);
		return -1;
	}

	do
	{
		//initializing the first empty Slot (every link is a viable candidate)
		for (unsigned long int link = 0; link < links.size(); link++)
		{
			emptySlot.candidates.push_back(link);
			//in the beginning there is no scheduled link, so the inverse SNR of the candidates has to consider only the ambient noise
			emptySlot.candidatesInverseSNR.push_back(noiseFloor(bandWidth) / links.at(link).interferedBy.at(link));
		}
		currentSlot = 0;

		//populate every slot candidate list with viable candidate links
		for (unsigned long int slot = 0; slot < slots.size(); slot++)
		{
			for (candidateLink = 0; candidateLink < links.size(); candidateLink++)
			{
				candidateOk = true;
				candidateInverseSNR = noiseFloor(bandWidth) / links.at(candidateLink).interferedBy.at(candidateLink);
				for (unsigned long int scheduledIndex = 0; candidateOk and scheduledIndex < slots.at(slot).scheduled.size(); scheduledIndex++)
				{
					scheduledLink = slots.at(slot).scheduled.at(scheduledIndex);
					candidateInverseSNR += (links.at(candidateLink).interferedBy.at(scheduledLink) / links.at(candidateLink).interferedBy.at(candidateLink));
					if (candidateInverseSNR > 1/dBtoSNR(snrThreshold) or scheduledLink == candidateLink or (links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(slot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
						candidateOk = false;
				}
				if (candidateOk)
				{
					slots.at(slot).candidates.push_back(candidateLink);
					slots.at(slot).candidatesInverseSNR.push_back(candidateInverseSNR);
				}
			}
		}

		while (emptySlot.candidates.size() > 0)
		{
			//inclusion of the first slot
			slots.push_back(emptySlot);
			if (slots.at(currentSlot).candidates.size() == 0) currentSlot++;

			while (slots.at(currentSlot).candidates.size() > 0)
			{
				candidateLink = slots.at(currentSlot).candidates.front();
				candidateOk = true;
				for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
				{
					//schedule a link under the restriction of power and distance until a new slot or remove it from the candidate list
					//remove the scheduled link from the empty slot
					scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
					slots.at(currentSlot).candidatesInverseSNR.front() += (links.at(candidateLink).interferedBy.at(scheduledLink) / links.at(candidateLink).interferedBy.at(candidateLink));
					candidateInverseSNR = slots.at(currentSlot).candidatesInverseSNR.front();

					if (candidateInverseSNR > 1/dBtoSNR(snrThreshold) or (links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
					{
						candidateOk = false;
						break;
					}
				}

				if (candidateOk)
				{
					//update the inverse SNR of all previous schedule links by considering the inclusion of the candidate link
					for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
					{
						scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
						slots.at(currentSlot).inverseSNR.at(scheduledIndex) += links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
					}

					//eliminate the candidate link from the empty slot because every link cannot be schedule twice in the schedule
					for (unsigned long int candidateIndex = 0; candidateIndex < emptySlot.candidates.size(); candidateIndex++)
						if (emptySlot.candidates.at(candidateIndex) == candidateLink)
						{
							emptySlot.candidates.erase(emptySlot.candidates.begin() + candidateIndex);
							emptySlot.candidatesInverseSNR.erase(emptySlot.candidatesInverseSNR.begin() + candidateIndex);
							break;
						}

					//schedule the viable candidate link
					++links.at(candidateLink).numberOfSchedules;
					slots.at(currentSlot).scheduled.push_back(candidateLink);
					slots.at(currentSlot).inverseSNR.push_back(candidateInverseSNR);

					//Eliminate it from the any other slot > current slot, because every link cannot be schedule more than once in the same "loop selecting the best link"
					for (unsigned long int slot = currentSlot +1; slot < slots.size(); slot++)
						for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(slot).candidates.size(); candidateIndex++)
							if (slots.at(slot).candidates.at(candidateIndex) == candidateLink)
							{
								slots.at(slot).candidates.erase(slots.at(slot).candidates.begin() + candidateIndex);
								slots.at(slot).candidatesInverseSNR.erase(slots.at(slot).candidatesInverseSNR.begin() + candidateIndex);
								break;
							}
				}
				//eliminate the candidate link from the current slot because every link cannot be schedule twice in the same slot OR because candidate is not a viable link (candidateOk == false)
				slots.at(currentSlot).candidates.erase(slots.at(currentSlot).candidates.begin());
				slots.at(currentSlot).candidatesInverseSNR.erase(slots.at(currentSlot).candidatesInverseSNR.begin());
			}
		}

		if (bestNumberOfschedules/(long double)bestSlotsSize <= links.at(0).numberOfSchedules/(long double)slots.size())
		{
			bestNumberOfschedules = links.at(0).numberOfSchedules;
			bestSlotsSize = slots.size();
		}
		else break;
	}
	while (links.at(0).numberOfSchedules < numberOfschedulesLimit);

	//results
	printResults("santi", fileName.str(), bestNumberOfschedules, numberOfNodes, bestSlotsSize, links.size(), false);

	return 0;
}
