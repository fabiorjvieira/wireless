/*
 * fabio.hpp
 *
 *  Created on: Nov 26, 2013
 *		Author: fabiorjvieira
 */

#include "main.hpp"

int fabioOneSlot(int argn, char ** argc)
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

	//Nodes file name
	std::string fileName;

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
	//	power	band		pathloss	sinr	nodes	.dot
	// 0.030 20000000 4.0		25	 100	~/git/general/WISp/networks/100/nodes/graph.11447.0.dot.node 1
	// 0.030 20000000 4.0		0	  5	  ~/git/general/WISp/networks/5/nodes/graph.0.0.dot.node 1

	//load a network topology from a ".dot" file (nodePosition)
	//Calculate the neighborhood (link set)
	//Inform transmissionPower, pathLoss, snrThreshold, bandWidth

	//argument test
	if (argn < 7)
	{
		std::cout << "Seven arguments are required in this order: " << std::endl;
		std::cout << "transmission power greater than zero (W)" << std::endl;
		std::cout << "signal bandwidth greater than zero (H)" << std::endl;
		std::cout << "path loss exponent greater than two" << std::endl;
		std::cout << "signal to noise threshold greater than one (dB)" << std::endl;
		std::cout << "number of nodes greater than one" << std::endl;
		std::cout << "graph '.dot' file" << std::endl;
		return result;
	}
	transmissionPower = atof(argc[1]);
	bandWidth = atof(argc[2]);
	pathLoss = atof(argc[3]);
	snrThreshold = atof(argc[4]);
	numberOfNodes = atoi(argc[5]);
	fileName = argc[6];
	if (numberOfNodes < 2) return result;
	if (transmissionPower <= 0) return result;
	if (pathLoss < 2) return result;
	if (bandWidth <= 0) return result;
	if (snrThreshold < 0) return result;
	std::cout << "# power: " << transmissionPower << ", band: " << bandWidth << ", path loss: " << pathLoss << ", SINR: " << snrThreshold << ", nodes: " << numberOfNodes << ", file: " << fileName << std::endl;

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
	std::vector < unsigned long int > linkEliminations;

	//recursion - multi-coloring
	unsigned long int currentSlot;

	//candidate variables represent a link that may be selected to be included in a slot if it is a viable option (SNR) and better than the selected one
	long double selectedInverseSNR, candidateInverseSNR;
	unsigned long int selectedLink, selectedIndex, candidateLink;

	//load the network and discover the link set
	loadNetwork(fileName, maximumCommunicationDistance, nodePosition, neighborhood, links);

	//Corrections on the neighborhood because of the One Slot algorithm
	for (unsigned long int linkIndex = 0; linkIndex < links.size();)
		if (links.at(linkIndex).senderNode % 2 != 0 or links.at(linkIndex).senderNode +1 != links.at(linkIndex).receiverNode)
			links.erase(links.begin() + linkIndex);
		else linkIndex++;
	for (unsigned long int nodeIndexA = 0; nodeIndexA < neighborhood.size(); nodeIndexA+=2)
		for (unsigned long int nodeIndexB = nodeIndexA +2; nodeIndexB < neighborhood.size(); nodeIndexB++)
		{
			neighborhood[nodeIndexA][nodeIndexB] = false;
			neighborhood[nodeIndexB][nodeIndexA] = false;
		}

	//Calculate the relative interference between each pair of nodes for all links in the set of links
	interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);

	//fabio---
	//initializing the first empty Slot (every link is a viable candidate)
	for (unsigned long int link = 0; link < links.size(); link++)
	{
		emptySlot.candidates.push_back(link);
		//in the beginning there is no scheduled link, so the inverse SNR of the candidates has to consider only the ambient noise
		emptySlot.candidatesInverseSNR.push_back(noiseFloor(bandWidth) / links.at(link).interferedBy.at(link));
	}
	currentSlot = 0;

	//loop selecting the best link
	//inclusion of the first slot
	slots.push_back(emptySlot);

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

		//schedule the viable candidate link
		++links.at(selectedLink).numberOfSchedules;
		selectedInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(selectedIndex);
		slots.at(currentSlot).scheduled.push_back(selectedLink);
		slots.at(currentSlot).inverseSNR.push_back(selectedInverseSNR);

		//eliminate the candidate link from the current slot candidate list because every link cannot be schedule twice in the same slot
		slots.at(currentSlot).candidates.erase(slots.at(currentSlot).candidates.begin() + selectedIndex);
		slots.at(currentSlot).candidatesInverseSNR.erase(slots.at(currentSlot).candidatesInverseSNR.begin() + selectedIndex);
		linkEliminations.push_back(selectedLink);

		//eliminate the inviable candidate links and
		//update the viable ones from the current slot
		//by considering the inclusion the viable candidate link
		for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(currentSlot).candidates.size(); candidateIndex++)
		{
			candidateLink = slots.at(currentSlot).candidates.at(candidateIndex);
			slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex) += (links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink));
			candidateInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex);

			if (candidateInverseSNR > 1/dBtoSNR(snrThreshold))
				linkEliminations.push_back(1);
			else
				linkEliminations.push_back(0);
			for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
			{
				scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
				if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
					linkEliminations.push_back(1);
				else
					linkEliminations.push_back(0);
			}

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
			linkEliminations.push_back(candidateLink);
		}
	}

	Slot fabioSlot = slots.at(0);
	std::vector < unsigned long int > fabioLinkEliminations = linkEliminations;

	if (links.size() == fabioSlot.scheduled.size())
	{
		std::cout << std::endl << "Fabio is the best: ";
		for (unsigned long int linkIndex = 0; linkIndex < fabioSlot.scheduled.size(); linkIndex++)
			std::cout << fabioSlot.scheduled.at(linkIndex) << " ";
		std::cout << std::endl;
		return 0;
	}

	//Optimal
	Slot currentBestSlot = slots.at(0);
	std::vector < unsigned long int > linkPermutations, currentBestLinkEliminations = linkEliminations;
	for (unsigned long int linkIndex = 0; linkIndex < links.size(); linkIndex++)
		linkPermutations.push_back(linkIndex);

	do
	{
		slots.at(0).scheduled.clear();
		slots.at(0).inverseSNR.clear();
		slots.at(0).candidates.resize(links.size());
		slots.at(0).candidatesInverseSNR.resize(links.size());
		linkEliminations.clear();

		for (unsigned long int permutationElement = 0; permutationElement < linkPermutations.size(); permutationElement++)
		{
			slots.at(0).candidates.at(permutationElement) = emptySlot.candidates.at(linkPermutations.at(permutationElement));
			slots.at(0).candidatesInverseSNR.at(permutationElement) = emptySlot.candidatesInverseSNR.at(linkPermutations.at(permutationElement));
		}

		while (slots.at(currentSlot).candidates.size() > 0 and slots.at(0).scheduled.size() + slots.at(0).candidates.size() > currentBestSlot.scheduled.size())
		{
			selectedIndex = 0;
			selectedLink =  slots.at(currentSlot).candidates.at(selectedIndex);

			//update the inverse SNR of all previous schedule links by considering the inclusion of the candidate link
			for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
			{
				scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
				slots.at(currentSlot).inverseSNR.at(scheduledIndex) += links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
			}

			//schedule the viable candidate link
			++links.at(selectedLink).numberOfSchedules;
			selectedInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(selectedIndex);
			slots.at(currentSlot).scheduled.push_back(selectedLink);
			slots.at(currentSlot).inverseSNR.push_back(selectedInverseSNR);

			//eliminate the candidate link from the current slot candidate list because every link cannot be schedule twice in the same slot
			slots.at(currentSlot).candidates.erase(slots.at(currentSlot).candidates.begin() + selectedIndex);
			slots.at(currentSlot).candidatesInverseSNR.erase(slots.at(currentSlot).candidatesInverseSNR.begin() + selectedIndex);
			linkEliminations.push_back(selectedLink);

			//eliminate the inviable candidate links and
			//update the viable ones from the current slot
			//by considering the inclusion the viable candidate link
			for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(currentSlot).candidates.size(); candidateIndex++)
			{
				candidateLink = slots.at(currentSlot).candidates.at(candidateIndex);
				slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex) += (links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink));
				candidateInverseSNR = slots.at(currentSlot).candidatesInverseSNR.at(candidateIndex);

				if (candidateInverseSNR > 1/dBtoSNR(snrThreshold))
					linkEliminations.push_back(1);
				else
					linkEliminations.push_back(0);
				for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(currentSlot).scheduled.size(); scheduledIndex++)
				{
					scheduledLink = slots.at(currentSlot).scheduled.at(scheduledIndex);
					if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
						linkEliminations.push_back(1);
					else
						linkEliminations.push_back(0);
				}

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
				linkEliminations.push_back(candidateLink);
			}
		}

		if (slots.at(0).scheduled.size() > currentBestSlot.scheduled.size())
		{
			currentBestSlot = slots.at(0);
			currentBestLinkEliminations = linkEliminations;
		}
		if (currentBestSlot.scheduled.size() > fabioSlot.scheduled.size())
		{
			std::cout << "Best: ";
			for (unsigned long int linkIndex = 0; linkIndex < currentBestSlot.scheduled.size(); linkIndex++)
				std::cout << currentBestSlot.scheduled.at(linkIndex) << " ";
			std::cout << " - ";
			for (unsigned long int linkEliminatedIndex = 0; linkEliminatedIndex < currentBestLinkEliminations.size(); linkEliminatedIndex++)
				std::cout << currentBestLinkEliminations.at(linkEliminatedIndex) << " ";
			std::cout << std::endl << "Fabio: ";
			for (unsigned long int linkIndex = 0; linkIndex < fabioSlot.scheduled.size(); linkIndex++)
				std::cout << fabioSlot.scheduled.at(linkIndex) << " ";
			std::cout << " - ";
			for (unsigned long int linkEliminatedIndex = 0; linkEliminatedIndex < fabioLinkEliminations.size(); linkEliminatedIndex++)
				std::cout << fabioLinkEliminations.at(linkEliminatedIndex) << " ";
			std::cout << std::endl;
			break;
		}
	}
	while (std::next_permutation(linkPermutations.begin(),linkPermutations.end()));
	/*debug*/ std::cout << fabioSlot.scheduled.size() << " " << currentBestSlot.scheduled.size() << std::endl;

	return 0;
}
