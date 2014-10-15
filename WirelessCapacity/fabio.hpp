/*
 * fabio.hpp
 *
 *  Created on: Nov 26, 2013
 *      Author: fabiorjvieira
 */

#include "main.hpp"

int fabio(int argn, char ** argc)
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

	//Nodes file name
	std::string fileName;

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
	//	power	band		pathloss	sinr	nodes	.dot
   // 0.030 20000000 4.0      25    100   ~/git/general/WISp/networks/100/nodes/graph.11447.0.dot.node 1
   // 0.030 20000000 4.0      0     5     ~/git/general/WISp/networks/5/nodes/graph.0.0.dot.node 1

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
		std::cout << "graph '.dot' file" << std::endl;
		std::cout << "Number of schedules" << std::endl;
		return result;
	}
	transmissionPower = atof(argc[1]);
	bandWidth = atof(argc[2]);
	pathLoss = atof(argc[3]);
	snrThreshold = atof(argc[4]);
	numberOfNodes = atoi(argc[5]);
	fileName = argc[6];
	numberOfschedulesLimit = atoi(argc[7]);
	if (numberOfNodes < 2) return result;
	if (transmissionPower <= 0) return result;
	if (pathLoss < 2) return result;
	if (bandWidth <= 0) return result;
	if (snrThreshold < 0) return result;
	if (numberOfschedulesLimit < 1) return result;
	std::cout << "# schedule: " << numberOfschedulesLimit << ", power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", file: " << fileName << std::endl;

	//Initialization of the basic parameters
	maximumCommunicationDistance = linkSize(transmissionPower, pathLoss, bandWidth, snrThreshold);
	//graph reset
	neighborhood.assign(numberOfNodes, std::vector < bool >(numberOfNodes, false));
	//graph initialization
	nodePosition.resize(numberOfNodes);

	//scheduling
	std::vector < Link > links;
	std::vector < Slot > slots;
	Slot emptySlot;
	unsigned long int scheduledLink;
	bool candidateOk;

	//recursion - multi-coloring
	unsigned long int bestSlotsSize = 1, bestNumberOfschedules = 0, currentSlot;

	//candidate variables represent a link that may be selected to be included in a slot if it is a viable option (SNR) and better than the selected one
	long double selectedInverseSNR, candidateInverseSNR;
	unsigned long int selectedLink, selectedIndex, candidateLink;

   //load the network and discover the link set
   loadNetwork(fileName, maximumCommunicationDistance, nodePosition, neighborhood, links);

	//Calculate the relative interference between each pair of nodes for all links in the set of links
	interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);

	if (links.size() == 0)
	{
		printResults("fabio", fileName, 0, numberOfNodes, slots.size(), links.size(), false);
		return -1;
	}

	//fabio---
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

		//loop selecting the best link
		while (emptySlot.candidates.size() > 0)
		{
			//inclusion of the first slot
			slots.push_back(emptySlot);
			if (slots.at(currentSlot).candidates.size() == 0) currentSlot++;

			while (slots.at(currentSlot).candidates.size() > 0)
			{
				selectedIndex = bestLink(links, slots, currentSlot, snrThreshold);
				selectedLink =  slots.at(currentSlot).candidates.at(selectedIndex);

				//update the inverse SNR of all previous schedule links by considering the inclusion of the candidate link
				for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
				{
					scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
					slots.at(currentSlot).inverseSNR.at(scheduledIndex) += links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
				}

				//Eliminate it from the empty slot, because every link cannot be schedule more than once in the same "loop selecting the best link"
				for (unsigned long int emptySlotCandidateIndex = 0; emptySlotCandidateIndex < emptySlot.candidates.size(); emptySlotCandidateIndex++)
					if (emptySlot.candidates.at(emptySlotCandidateIndex) == selectedLink)
					{
						emptySlot.candidates.erase(emptySlot.candidates.begin() + emptySlotCandidateIndex);
						emptySlot.candidatesInverseSNR.erase(emptySlot.candidatesInverseSNR.begin() + emptySlotCandidateIndex);
						break;
					}

				//schedule the viable candidate link
				++links.at(selectedLink).numberOfSchedules;
				selectedInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(selectedIndex);
				slots.at(currentSlot).scheduled.push_back(selectedLink);
				slots.at(currentSlot).inverseSNR.push_back(selectedInverseSNR);

				//eliminate the candidate link from the current slot candidate list because every link cannot be schedule twice in the same slot
				slots.at(currentSlot).candidates.erase(slots.at(currentSlot).candidates.begin() + selectedIndex);
				slots.at(currentSlot).candidatesInverseSNR.erase(slots.at(currentSlot).candidatesInverseSNR.begin() + selectedIndex);
				//Eliminate it from the any other slot > current slot, because every link cannot be schedule more than once in the same "loop selecting the best link"
				for (unsigned long int slot = currentSlot +1; slot < slots.size(); slot++)
					for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(slot).candidates.size(); candidateIndex++)
						if (slots.at(slot).candidates.at(candidateIndex) == selectedLink)
						{
							slots.at(slot).candidates.erase(slots.at(slot).candidates.begin() + candidateIndex);
							slots.at(slot).candidatesInverseSNR.erase(slots.at(slot).candidatesInverseSNR.begin() + candidateIndex);
							break;
						}

				//eliminate the inviable candidate links and
				//update the viable ones from the current slot
				//by considering the inclusion the viable candidate link
				for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(currentSlot).candidates.size(); candidateIndex++)
				{
					candidateLink = slots.at(currentSlot).candidates.at(candidateIndex);
					slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex) += (links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink));
					candidateInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex);

					for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
					{
						scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
						if (candidateInverseSNR > 1/dBtoSNR(snrThreshold) or (links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
						{
							slots.at(currentSlot).candidates.erase(slots.at(currentSlot).candidates.begin() + candidateIndex);
							slots.at(currentSlot).candidatesInverseSNR.erase(slots.at(currentSlot).candidatesInverseSNR.begin() + candidateIndex);
							//se o candidato foi eliminado da lista, então não pode incrementar candidateIndex!
							candidateIndex--;
							break;
						}
					}
				}
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
	printResults("fabio", fileName, bestNumberOfschedules, numberOfNodes, bestSlotsSize, links.size(), false);

	return 0;
}
