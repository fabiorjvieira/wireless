/*
 * main.cpp
 *
 *  Created on: Oct 18, 2011
 *		Author: fabiorjvieira
 */

#include "main.hpp"

int valmir(int argn, char ** argc)
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

	//802.11g bandwidth=20MHz transmission power=30mW beta=25dB (25dB significa que 1% perda admisivel a 54Mbs)
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
	ScheduledLinks scheduledLinks;
	std::vector < unsigned long int > dummyScheduledLinkListsItem;
	Slot emptySlot;
	long double scheduleObjective = 0, linksStandardDeviation;
	unsigned long int scheduledLink;

	//search for the best variables
	unsigned long int bestSlotsSize = 1, bestNumberOfschedules = 0;

	//selected variables represent the current best link to be included in a slot
	long double selectedInverseSNR, selectedGain = 0, selectedLinkDeviation;
	unsigned long int selectedLink, selectedSlot, selectedCandidateIndex, selectedLinkNearestSlot;
	//candidate variables represent a link that may be selected to be included in a slot if it is a viable option (SNR) and better than the selected one
	long double candidateInverseSNR, candidateGain, candidateLinkDeviation;
	unsigned long int candidateLink, candidateLinkNearestSlot;

	//load the network and discover the link set
	loadNetwork(fileName, maximumCommunicationDistance, nodePosition, neighborhood, links);

	//Calculate the relative interference between each pair of nodes for all links in the set of links
	interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);

	/*
	 * Algoritmo
	 * Gera o escalonamento
	 * Enfileira os enlaces segundo um critério
	 *	 Randômico
	 *	 O quanto o enlace afeta o SNR dos escalonados
	 *	 O SNR do enlace
	 * Dentre todos os enlaces,
	 *	 escalona aquele enlace no grupo que maximiza o incremento da função objetivo,
	 *	 levando-se em consideração a criação de um novo grupo,
	 *	 até que não seja possível incrementar a função objetivo.
	 * Caso os enlaces tenhão sido escalonados em quantidades diferentes,
	 *	 execute o passo anterior limitando o número de escalonamentos de forma a equiparadar todos os enlaces.
	 */
	//numero de comparações é polinomial???

	//Initialization of the structure scheduledLinks
	scheduledLinksInitialization(links, scheduledLinks, dummyScheduledLinkListsItem);

	if (links.size() == 0)
	{
		printResults("valmir", fileName, 0, numberOfNodes, slots.size(), links.size(), false);
		return -1;
	}

	//initializing the first empty Slot (every link is a viable candidate)
	for (unsigned long int link = 0; link < links.size(); link++)
	{
		emptySlot.candidates.push_back(link);
		//in the beginning there is no scheduled link, so the inverse SNR of the candidates has to consider only the ambient noise
		emptySlot.candidatesInverseSNR.push_back(noiseFloor(bandWidth) / links.at(link).interferedBy.at(link));
	}
	//inclusion of the first slot
	slots.push_back(emptySlot);

	//loop selecting the link that most increase the value of the scheduleObjective (the best link)
	do //pensar como farei para fixar o numero maximo de escalonamentos dependendo do criterio de parada???
	{
		//Searching for the best candidate to be scheduled---

		//select a link to be the first candidate to be scheduled in a slot and calculate the gain
		//Let's start with candidate link index 0 and the last slot (the empty slot)
		selectedLink = slots.back().candidates.front();
		selectedInverseSNR = slots.back().candidatesInverseSNR.front();
		selectedSlot = slots.size() -1;
		selectedCandidateIndex = 0;
		selectedLinkDeviation = deviation(scheduledLinks, scheduledLinks.total +1, links.size(), links.at(selectedLink).numberOfSchedules +1);

		//calculate the first selected gain
		selectedGain = objective(scheduledLinks.total +1, links.size(), slots.size(), selectedLinkDeviation) - scheduleObjective;
		selectedLinkNearestSlot = nearestSlot(slots, selectedSlot, selectedLink, slots.size() + 1);

		//search for the best link/slot in the empty slot (last slot) by comparing it with the link index 0 scheduled in the empty slot
		for (unsigned long int candidateIndex = 1; candidateIndex < slots.back().candidates.size(); candidateIndex++)
		{
			candidateLink = slots.back().candidates.at(candidateIndex);
			candidateLinkDeviation = deviation(scheduledLinks, scheduledLinks.total +1, links.size(), links.at(candidateLink).numberOfSchedules +1);

			//calculate the candidate gain
			candidateGain = objective(scheduledLinks.total +1, links.size(), slots.size(), candidateLinkDeviation) - scheduleObjective;
			candidateLinkNearestSlot = nearestSlot(slots, selectedSlot, candidateLink, labs((long int)selectedLinkNearestSlot - (long int)selectedSlot));

			if ((candidateGain > selectedGain)
			      or (candidateGain == selectedGain and labs((long int)candidateLinkNearestSlot - (long int)selectedSlot) == labs((long int)selectedLinkNearestSlot - (long int)selectedSlot) and powerCriteria(links, candidateLink, selectedLink) > 0))
			{
				selectedGain = candidateGain;
				selectedInverseSNR = slots.back().candidatesInverseSNR.at(candidateIndex);
				//not necessary selectedSlot =  slots.size() -1;
				selectedLinkDeviation = candidateLinkDeviation;
				selectedCandidateIndex = candidateIndex;
				selectedLink = candidateLink;
				selectedLinkNearestSlot = candidateLinkNearestSlot;
			}
		}

		//search for the best link/slot within the slot's candidates by comparing it with the best link scheduled in the empty slot
		//the last slot (the empty slot) must not be included in this search, because it was searched before (see above)
		for (unsigned long int slot = 0; slot < slots.size() -1; slot++)
		{
			for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(slot).candidates.size(); candidateIndex++)
			{
				candidateLink = slots.at(slot).candidates.at(candidateIndex);
				candidateLinkDeviation = deviation(scheduledLinks, scheduledLinks.total +1, links.size(), links.at(candidateLink).numberOfSchedules +1);

				//calculate the candidate gain
				candidateGain = objective(scheduledLinks.total +1, links.size(), slots.size() -1, candidateLinkDeviation) - scheduleObjective;
				candidateLinkNearestSlot = nearestSlot(slots, slot, candidateLink, labs((long int)selectedLinkNearestSlot - (long int)selectedSlot));

				//update the selected link, slot, SNR, candidate index (position in the candidate list that each slot has) if the candidate link has better gain
				if ((candidateGain > selectedGain)
				      or (candidateGain == selectedGain and labs((long int)candidateLinkNearestSlot - (long int)slot) > labs((long int)selectedLinkNearestSlot - (long int)slot) and powerCriteria(links, candidateLink, selectedLink) > 0)
				      )
				{
					selectedGain = candidateGain;
					selectedInverseSNR = slots.at(slot).candidatesInverseSNR.at(candidateIndex);
					selectedSlot = slot;
					selectedLinkDeviation = candidateLinkDeviation;
					selectedCandidateIndex = candidateIndex;
					selectedLink = candidateLink;
					selectedLinkNearestSlot = candidateLinkNearestSlot;
				}
			}
		}

		/*debug*/
		linksStandardDeviation = standardDeviation(scheduledLinks, scheduledLinks.total +1, links.size(), links.at(selectedLink).numberOfSchedules +1);
		/*debug*/

		//Update of the structure scheduledLinks that monitors the number of times each link was schedule---

		//1-remove the selected link from scheduledLinks.linkList
		scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).erase(scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).begin() + scheduledLinks.linkPosition.at(selectedLink));
		//2-if the selected link was the last one to has minimum schedules, then update minimum number of schedules
		if (scheduledLinks.linkLists.at(scheduledLinks.minimum).size() == 0) scheduledLinks.minimum++;
		else
		{
			//3-update the position of the links that remained in the linkList
			//scheduledLinks.linkPosition.at(selectedLink) => selectedLink position in the list linkLists.at(links.at(selectedLink).numberOfSchedules)
			//that is, linkLists.at(links.at(selectedLink).numberOfSchedules).at(scheduledLinks.linkPosition.at(selectedLink)) = selectedLink
			for (unsigned long int linkIndex = scheduledLinks.linkPosition.at(selectedLink); linkIndex < scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).size(); linkIndex++)
				scheduledLinks.linkPosition.at(scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).at(linkIndex))--;
		}
		//4-if selectedLink belongs to the linkList where numberOfSchedules is the maximum, then insert a new empty list
		if (links.at(selectedLink).numberOfSchedules == scheduledLinks.maximum)
		{
			dummyScheduledLinkListsItem.clear();
			scheduledLinks.linkLists.push_back(dummyScheduledLinkListsItem);
			scheduledLinks.maximum++;
		}
		//5-updating link number of times it was scheduled
		links.at(selectedLink).numberOfSchedules++;
		//6-insert the selected link in the corresponding list
		scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).push_back(selectedLink);
		//7-updating link position with the last position in the list where the number of schedules = links.at(selectedLink).numberOfSchedules (links are always inserted in the last position)
		scheduledLinks.linkPosition.at(selectedLink) = scheduledLinks.linkLists.at(links.at(selectedLink).numberOfSchedules).size() -1;

		//update the SNR of the other links scheduled before in the selectedSlot by considering the selected link  (chose to be scheduled)
		for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(selectedSlot).scheduled.size(); scheduledIndex++)
		{
			scheduledLink = slots.at(selectedSlot).scheduled.at(scheduledIndex);
			slots.at(selectedSlot).inverseSNR.at(scheduledIndex) += links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
		}

		//schedule the selected link---

		//include the selected link in the selected Slot.
		slots.at(selectedSlot).scheduled.push_back(selectedLink);
		//selectedInverseSNR is already taking into account all links previously scheduled in the selected slot
		slots.at(selectedSlot).inverseSNR.push_back(selectedInverseSNR);
		//remove the selected link from candidates, because it cannot be schedule twice in the same slot
		slots.at(selectedSlot).candidates.erase(slots.at(selectedSlot).candidates.begin() + selectedCandidateIndex);
		slots.at(selectedSlot).candidatesInverseSNR.erase(slots.at(selectedSlot).candidatesInverseSNR.begin() + selectedCandidateIndex);

		//Eliminate inviable links by considering the SNR and the maximum number of schedules---

		//eliminate from the selected slot candidate list all inviable links every time a new link is scheduled
		//and update the inverseSNR of the viable candidate links by considering the selected link
		for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(selectedSlot).candidates.size(); candidateIndex++)
		{
			candidateLink = slots.at(selectedSlot).candidates.at(candidateIndex);
			//update the candidate inverse SNR by considering the selected link (the most recently scheduled)
			slots.at(selectedSlot).candidatesInverseSNR.at(candidateIndex) += (links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink));
			candidateInverseSNR = slots.at(selectedSlot).candidatesInverseSNR.at(candidateIndex);

			for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(selectedSlot).scheduled.size(); scheduledIndex++)
			{
				//test if any link already schedule in this slot will violate snrThreshold because of the candidate
				//if SNR of the candidate or SNR of any scheduled link < snrThreshold, then remove the candidate and continue,
				scheduledLink = slots.at(selectedSlot).scheduled.at(scheduledIndex);
				if (candidateInverseSNR > 1/dBtoSNR(snrThreshold) or (links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(selectedSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				{
					slots.at(selectedSlot).candidates.erase(slots.at(selectedSlot).candidates.begin() + candidateIndex);
					slots.at(selectedSlot).candidatesInverseSNR.erase(slots.at(selectedSlot).candidatesInverseSNR.begin() + candidateIndex);
					//se o candidato foi eliminado da lista, então não pode incrementar candidateIndex!
					candidateIndex--;
					break;
				}
			}
		}

		//eliminate the selected link from any candidate list if its number of schedules = maximum number of schedules,
		//including the candidates in the list of the empty slot
		if (links.at(selectedLink).numberOfSchedules == numberOfschedulesLimit)
		{
			for (unsigned long int slot = 0; slot < selectedSlot; slot++)
			{
				for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(slot).candidates.size(); candidateIndex++)
				{
					if (slots.at(slot).candidates.at(candidateIndex) == selectedLink)
					{
						slots.at(slot).candidates.erase(slots.at(slot).candidates.begin() + candidateIndex);
						slots.at(slot).candidatesInverseSNR.erase(slots.at(slot).candidatesInverseSNR.begin() + candidateIndex);
						break;
					}
				}
			}

			for (unsigned long int slot = selectedSlot +1; slot < slots.size(); slot++)
			{
				for (unsigned long int candidateIndex = 0; candidateIndex < slots.at(slot).candidates.size(); candidateIndex++)
				{
					if (slots.at(slot).candidates.at(candidateIndex) == selectedLink)
					{
						slots.at(slot).candidates.erase(slots.at(slot).candidates.begin() + candidateIndex);
						slots.at(slot).candidatesInverseSNR.erase(slots.at(slot).candidatesInverseSNR.begin() + candidateIndex);
						break;
					}
				}
			}

			for (unsigned long int candidateIndex = 0; candidateIndex < emptySlot.candidates.size(); candidateIndex++)
			{
				if (emptySlot.candidates.at(candidateIndex) == selectedLink)
				{
					emptySlot.candidates.erase(emptySlot.candidates.begin() + candidateIndex);
					emptySlot.candidatesInverseSNR.erase(emptySlot.candidatesInverseSNR.begin() + candidateIndex);
					break;
				}
			}
		}

		//Objective and empty slot---

		//include a empty slot to consider the possibility of a new slot, but only if the last slot is not empty
		if (slots.back().scheduled.size() > 0) slots.push_back(emptySlot);

		//updating the scheduleObjective value knowing that the last slot is a empty slot
		  //linkDeviation = deviation(scheduledLinks, scheduledLinks.total +1, links.size(), selectedLink);
		scheduleObjective = objective(scheduledLinks.total +1, links.size(), slots.size() -1, selectedLinkDeviation);
		scheduledLinks.total++;

		/*debug* /
		unsigned long int numberOfLinksScheduled = 0;
		std::cout << "# Schedule:" << std::endl;
		for (unsigned long int slotIndex = 0; slotIndex < slots.size() -1; slotIndex++)
		{
			std::cout << "#	";
			numberOfLinksScheduled += slots.at(slotIndex).scheduled.size();
			for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(slotIndex).scheduled.size(); scheduledIndex++)
				std::cout << slots.at(slotIndex).scheduled.at(scheduledIndex) << " ";
			std::cout << std::endl;
		}
		std::cout << numberOfNodes << " " << scheduleObjective << " " << slots.size() -1 << " " << numberOfLinksScheduled << " " << links.size() << " " << linksStandardDeviation << std::endl;
		/ *debug*/

		//search for the best
		if (bestNumberOfschedules/(long double)bestSlotsSize <= links.at(0).numberOfSchedules/(long double)(slots.size() -1))
		{
			bestNumberOfschedules = links.at(0).numberOfSchedules;
			bestSlotsSize = slots.size() -1;
		}

	}
	while (stopCriteria(scheduledLinks, selectedGain, links, slots, emptySlot) and linksStandardDeviation > 0);

	//Eliminate the the last slot, because it is always empty
	slots.pop_back();

	//results
	printResults("valmir", fileName, bestNumberOfschedules, numberOfNodes, bestSlotsSize, links.size(), false);

	return 0;
}
