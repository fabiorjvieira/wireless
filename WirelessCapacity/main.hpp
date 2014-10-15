/*
 * main.hpp
 *
 *  Created on: Oct 19, 2011
 *		Author: fabiorjvieira
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>
#include <sstream>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <cfloat>

static const double BOLTZMANN = 1.3803e-23;
static const double REFERENCE_TEMPERATURE = 290.0; //kelvin
static const double NOISE_FIGURE = 0.031622776602; //15dBm

static const bool LESS = false;
static const bool MOST = true;

//Cartesian position of a vertex
struct Position
{
	long double xPosition, yPosition;
};

struct Link
{
	unsigned long int senderNode, receiverNode;
	unsigned long int numberOfSchedules;
	//inverse power received
	long double powerReceived; //olga
	unsigned long int interferenceNumberOfEdges; //santi
	//long double receivedInterference, transmittedInterference; //fabio

	//Each power received by the link receiver from the other link transmitters.
	//Also include the power received by the link receiver from the link sender in the absent of any noise
	//linkA link.interferedBy(linkA) is the power received by the linkA in the absent of any noise
	std::vector < long double > interferedBy;
};

struct Slot
{
	//Vector of link labels that are schedule in this slot. A link label is the position of a link in a vector of links.
	std::vector < unsigned long int > scheduled;
	//The inverse of SNR on every link receiver by considering the interference of the links scheduled in this slot
	//Same order of scheduled
	std::vector < long double > inverseSNR;

	//A link vector position (link label) that are not schedule in this slot
	std::vector < unsigned long int > candidates;
	//The inverse of SNR on every candidate link receiver by considering the interference of the links schedules in this slot
	std::vector < long double > candidatesInverseSNR;
};

//Monitors the number of times each link was scheduled.
//Has two data structures and two integers
//0-The maximum and minimum number of schedules
//1-A link labels list (lines) of lists (columns)
//If a link represented by the label X is in the list Y (line Y), then X was schedule Y times
//When a link is scheduled again, it changes to the next list (current line +1).
//A list Y (line Y) is empty only if there is no link schedule Y times.
//2-A list of the link position in the list (=column).
struct ScheduledLinks
{
	unsigned long int maximum, minimum, total;
	std::vector < std::vector < unsigned long int > > linkLists;
	std::vector < unsigned long int > linkPosition;
};

//objectives

//Average and Deviation
long double objectiveAverage(unsigned long int numberOfSchedules, unsigned long int numberOfLinks, unsigned long int numberOfSlots, long double deviation)
{
	return (numberOfSchedules/(long double)numberOfLinks) / (long double)(numberOfSlots * (deviation +1));
}

//Counting and Deviation
long double objectiveCounting(unsigned long int numberOfSchedules, unsigned long int numberOfSlots, long double deviation)
{
	return numberOfSchedules / (long double)(numberOfSlots * (deviation +1));
}

long double standardDeviation(ScheduledLinks & scheduledLinks, unsigned long int numberOfSchedules, unsigned long int numberOfLinks, unsigned long int recentlyScheduledLinkNumberOfSchedules)
{
	long double standardDeviation = 0, deviation;
	unsigned long int recentlyScheduledLinkOldNumberOfSchedules;

	deviation = numberOfSchedules/(long double) numberOfLinks;
	recentlyScheduledLinkOldNumberOfSchedules = recentlyScheduledLinkNumberOfSchedules -1;

	for (unsigned long int scheduledLinksIndex = scheduledLinks.minimum; scheduledLinksIndex < recentlyScheduledLinkOldNumberOfSchedules; scheduledLinksIndex++)
		standardDeviation += (deviation - scheduledLinksIndex) * (deviation - scheduledLinksIndex) * scheduledLinks.linkLists.at(scheduledLinksIndex).size();

	//recently scheduled link OLD position in link list of scheduledLinks structure
	standardDeviation += (deviation - recentlyScheduledLinkOldNumberOfSchedules) * (deviation - recentlyScheduledLinkOldNumberOfSchedules) * (scheduledLinks.linkLists.at(recentlyScheduledLinkOldNumberOfSchedules).size() -1);

	//recently scheduled link NEW position in link list of scheduledLinks structure
	if (recentlyScheduledLinkNumberOfSchedules <= scheduledLinks.maximum)
	{
		standardDeviation += (deviation - recentlyScheduledLinkNumberOfSchedules) * (deviation - recentlyScheduledLinkNumberOfSchedules) * (scheduledLinks.linkLists.at(recentlyScheduledLinkNumberOfSchedules).size() +1);
		for (unsigned long int scheduledLinksIndex = recentlyScheduledLinkNumberOfSchedules +1; scheduledLinksIndex <= scheduledLinks.maximum; scheduledLinksIndex++)
			standardDeviation += (deviation - scheduledLinksIndex) * (deviation - scheduledLinksIndex) * scheduledLinks.linkLists.at(scheduledLinksIndex).size();
	}
	else standardDeviation += (deviation - recentlyScheduledLinkNumberOfSchedules) * (deviation - recentlyScheduledLinkNumberOfSchedules) *1;

	standardDeviation /= (long double)numberOfLinks;
	standardDeviation = std::sqrt(standardDeviation);

	return standardDeviation;
}

long double maxDeviation(ScheduledLinks & scheduledLinks, unsigned long int numberOfLinks, unsigned long int recentlyScheduledLinkNumberOfSchedules)
{
	long double standardDeviation = 0, deviation;
	unsigned long int recentlyScheduledLinkOldNumberOfSchedules;

	deviation = scheduledLinks.maximum;
	recentlyScheduledLinkOldNumberOfSchedules = recentlyScheduledLinkNumberOfSchedules -1;

	if (recentlyScheduledLinkNumberOfSchedules > scheduledLinks.maximum) deviation++;

	for (unsigned long int scheduledLinksIndex = scheduledLinks.minimum; scheduledLinksIndex < recentlyScheduledLinkOldNumberOfSchedules; scheduledLinksIndex++)
		standardDeviation += (deviation - scheduledLinksIndex) * (deviation - scheduledLinksIndex) * scheduledLinks.linkLists.at(scheduledLinksIndex).size();

	//recently scheduled link OLD position in link list of scheduledLinks structure
	standardDeviation += (deviation - recentlyScheduledLinkOldNumberOfSchedules) * (deviation - recentlyScheduledLinkOldNumberOfSchedules) * (scheduledLinks.linkLists.at(recentlyScheduledLinkOldNumberOfSchedules).size() -1);

	//recently scheduled link NEW position in link list of scheduledLinks structure
	if (recentlyScheduledLinkNumberOfSchedules <= scheduledLinks.maximum)
	{
		standardDeviation += (deviation - recentlyScheduledLinkNumberOfSchedules) * (deviation - recentlyScheduledLinkNumberOfSchedules) * (scheduledLinks.linkLists.at(recentlyScheduledLinkNumberOfSchedules).size() +1);
		for (unsigned long int scheduledLinksIndex = recentlyScheduledLinkNumberOfSchedules +1; scheduledLinksIndex <= scheduledLinks.maximum; scheduledLinksIndex++)
			standardDeviation += (deviation - scheduledLinksIndex) * (deviation - scheduledLinksIndex) * scheduledLinks.linkLists.at(scheduledLinksIndex).size();
	}
	else standardDeviation += (deviation - recentlyScheduledLinkNumberOfSchedules) * (deviation - recentlyScheduledLinkNumberOfSchedules) *1;

	standardDeviation /= (long double)numberOfLinks;
	standardDeviation = std::sqrt(standardDeviation);

	return standardDeviation;
}

long double minMaxDeviation(ScheduledLinks & scheduledLinks, unsigned long int recentlyScheduledLinkNumberOfSchedules)
{
	long double deviation;
	unsigned long int recentlyScheduledLinkOldNumberOfSchedules;

	recentlyScheduledLinkOldNumberOfSchedules = recentlyScheduledLinkNumberOfSchedules -1;
	deviation = scheduledLinks.maximum - scheduledLinks.minimum;

	if (recentlyScheduledLinkNumberOfSchedules > scheduledLinks.maximum) deviation++;
	else if (recentlyScheduledLinkOldNumberOfSchedules == scheduledLinks.minimum and scheduledLinks.linkLists.at(scheduledLinks.minimum).size() == 1)
		deviation--;

	return deviation;
}

long double deviation(ScheduledLinks & scheduledLinks, unsigned long int numberOfSchedules, unsigned long int numberOfLinks, unsigned long int recentlyScheduledLinkNumberOfSchedules)
{
//	return maxDeviation(scheduledLinks, numberOfLinks, recentlyScheduledLinkNumberOfSchedules);
	return standardDeviation(scheduledLinks, numberOfSchedules, numberOfLinks, recentlyScheduledLinkNumberOfSchedules);
//	return minMaxDeviation(scheduledLinks, recentlyScheduledLinkNumberOfSchedules);
}

long double objective(unsigned long int numberOfSchedules, unsigned long int numberOfLinks, unsigned long int numberOfSlots, long double deviation)
{
//	return objectiveAverage(numberOfSchedules, numberOfLinks, numberOfSlots, deviation);
	return objectiveCounting(numberOfSchedules, numberOfSlots, deviation);
}

//Basic signal calculus (depends on the physical model type)
long double noiseFloor(long double bandWidth)
{
	long double thermalNoise = BOLTZMANN * REFERENCE_TEMPERATURE * bandWidth;
	long double noiseFloor = thermalNoise; //* NOISE_FIGURE; // noiseFigure; //perda dentro do receptor não ideal
	return noiseFloor;
}

long double dBtoSNR(long double snrThreshold)
{
	return std::pow(10,snrThreshold/ (long double)10);
}

//the maximum distance between two neighbors nodes in the absence of any interference, but taking into account the ambient noise and the path loss
long double linkSize(long double transmissionPower, long double pathLoss, long double bandWidth, long double snrThreshold)
{
 	return std::pow(transmissionPower/(noiseFloor(bandWidth)*dBtoSNR(snrThreshold)), (1/pathLoss));
}

//signal decay without ambient noise
long double pureSignalDecay(long double transmissionPower, long double pathLoss, long double distance)
{
	long double dzero = 1.0;

	if (distance < dzero) return transmissionPower;
	else return transmissionPower/std::pow(distance, pathLoss);
}

long double euclideanDistance(Position & a, Position & b)
{
	return std::sqrt(std::pow((a.xPosition - b.xPosition), 2) + std::pow((a.yPosition - b.yPosition), 2));
}

//Each power received by the link receiver from the other link transmitters (including the link sender)
void interferedBy(std::vector < Link > & links, std::vector < Position > & nodePosition, long double transmissionPower, long double pathLoss, long double snrThreshold)
{
	long double distance;

	for (unsigned linkAffected = 0; linkAffected < links.size(); linkAffected++)
	{
		links.at(linkAffected).interferedBy.clear();
		for (unsigned link = 0; link < linkAffected; link++)
		{
			//first order interference (each node has only one radio), 1.1/SNR means they are both not viable to be scheduled at the same slot together
			if (links.at(linkAffected).receiverNode == links.at(link).receiverNode or links.at(linkAffected).senderNode == links.at(link).senderNode or links.at(linkAffected).receiverNode == links.at(link).senderNode or links.at(linkAffected).senderNode == links.at(link).receiverNode)
				links.at(linkAffected).interferedBy.push_back(1.1/dBtoSNR(snrThreshold));
			//second order interference (SNR)
			else
			{
				distance = euclideanDistance(nodePosition.at(links.at(link).senderNode), nodePosition.at(links.at(linkAffected).receiverNode));
				links.at(linkAffected).interferedBy.push_back(pureSignalDecay(transmissionPower, pathLoss, distance));
			}
		}

		//linkA link.interferedBy(linkA) is the power received by the linkA in the absent of any noise
		distance = euclideanDistance(nodePosition.at(links.at(linkAffected).senderNode), nodePosition.at(links.at(linkAffected).receiverNode));
		links.at(linkAffected).interferedBy.push_back(pureSignalDecay(transmissionPower, pathLoss, distance));
		links.at(linkAffected).powerReceived = links.at(linkAffected).interferedBy.back();

		for (unsigned link = linkAffected +1; link < links.size(); link++)
		{
			//first order interference (each node has only one radio), 1.1/SNR means they are both not viable to be scheduled at the same slot together
			if (links.at(linkAffected).receiverNode == links.at(link).receiverNode or links.at(linkAffected).senderNode == links.at(link).senderNode or links.at(linkAffected).receiverNode == links.at(link).senderNode or links.at(linkAffected).senderNode == links.at(link).receiverNode)
				links.at(linkAffected).interferedBy.push_back(1.1/dBtoSNR(snrThreshold));
			//second order interference (SNR)
			else
			{
				distance = euclideanDistance(nodePosition.at(links.at(link).senderNode), nodePosition.at(links.at(linkAffected).receiverNode));
				links.at(linkAffected).interferedBy.push_back(pureSignalDecay(transmissionPower, pathLoss, distance));
			}
		}
	}
}

//The number of links that a link A cannot be schedule with
//Must be called after interferedBy
void interferenceNumberOfEdges(std::vector < Link > & links, long double snrThreshold, long double bandWidth)
{
	unsigned long int interferenceNumberOfEdgesA = 0;
	long double interferenceA, interferenceB;

	for (unsigned long int linkA = 0; linkA < links.size(); linkA++)
	{
		for (unsigned long int linkB = 0; linkB < links.size(); linkB++)
		{
			if (links.at(linkA).receiverNode != links.at(linkB).receiverNode and links.at(linkA).senderNode != links.at(linkB).senderNode)
			{
				interferenceA = (links.at(linkA).interferedBy.at(linkB) / links.at(linkA).interferedBy.at(linkA)) + (noiseFloor(bandWidth) / links.at(linkA).interferedBy.at(linkA));
				interferenceB = (links.at(linkB).interferedBy.at(linkA) / links.at(linkB).interferedBy.at(linkB)) + (noiseFloor(bandWidth) / links.at(linkB).interferedBy.at(linkB));
				if (interferenceA > 1/dBtoSNR(snrThreshold) or interferenceB > 1/dBtoSNR(snrThreshold)) interferenceNumberOfEdgesA++;
			}
		}
		links.at(linkA).interferenceNumberOfEdges = interferenceNumberOfEdgesA;
	}
}

bool connected(std::vector < std::vector < bool > > & neighborhood)
{
	std::vector < unsigned long int > reachable,  notExplored;
	bool visited;
	unsigned long int currentVertex;

	reachable.push_back(0);
	notExplored.push_back(0);

	while (notExplored.size() > 0)
	{
		currentVertex = notExplored.back();
		notExplored.pop_back();
		for (unsigned long int vertex = 0; vertex < neighborhood.size(); vertex++)
		{
			if (neighborhood.at(currentVertex).at(vertex))
			{
				visited = false;
				for (unsigned long int reach = 0; not visited and reach < reachable.size(); reach++)
					if (reachable.at(reach) == vertex)
						visited = true;
				if (not visited)
				{
					notExplored.push_back(vertex);
					reachable.push_back(vertex);
				}
			}
		}
	}

	return (reachable.size() == neighborhood.size());
}

void printResults(const char algorithm[], std::string fileName, unsigned long int numberOfColors, unsigned long int numberOfNodes, unsigned long int slotSize, unsigned long int numberOfLinks, bool printSlots)
{
	char delimiter;
//	unsigned long int numberOfLinks = 0;
	std::string squareSize, networkInstance;
	std::stringstream name;

	//get the file name without path
	delimiter = '/';
	fileName.assign(fileName.rbegin(), fileName.rend());
	name.str(fileName);
	std::getline(name, fileName, delimiter);
	fileName.assign(fileName.rbegin(), fileName.rend());
	name.str(fileName);

//	for (unsigned long int slot = 0; slot < slots.size(); slot++)
//	{
//		numberOfLinks += slots.at(slot).scheduled.size();
//		if (printSlots)
//		{
//		std::cout << "# Slot: " << slot << ", Links:";
//		for (unsigned long int scheduledIndex = 0; scheduledIndex < slots.at(slot).scheduled.size(); scheduledIndex++)
//			std::cout << " " << slots.at(slot).scheduled.at(scheduledIndex);
//		std::cout << std::endl;
//		}
//	}

	delimiter = '.';
	std::getline(name, squareSize, delimiter);std::getline(name, squareSize, delimiter);
	std::getline(name, networkInstance, delimiter);

	std::cout << "# nodes squareSize networkInstance algorithm colors slots links" << std::endl;
//	if (numberOfColors == 0)
//		std::cout << numberOfNodes << " " << squareSize << " " << networkInstance << " " << algorithm << " " << numberOfColors << " " << slots.size() << " " << 0 << std::endl;
//	else std::cout << numberOfNodes << " " << squareSize << " " << networkInstance << " " << algorithm << " " << numberOfColors << " " << slots.size() << " " << numberOfLinks/numberOfColors << std::endl;
	std::cout << numberOfNodes << " " << squareSize << " " << networkInstance << " " << algorithm << " " << numberOfColors << " " << slotSize << " " << numberOfLinks << std::endl;
}

void printLinks(std::vector < Link> & links, long double snrThreshold, long double bandWidth)
{
	for (unsigned long int link = 0; link < links.size(); link++)
	{
		std::cout << link << " " << links.at(link).interferedBy.at(link) << " " << links.at(link).interferedBy.at(link)/(dBtoSNR(snrThreshold)*noiseFloor(bandWidth));
		if (links.at(link).interferedBy.at(link)/(dBtoSNR(snrThreshold)*noiseFloor(bandWidth)) < 1) std::cout << " pow" << std::endl;
		else std::cout << std::endl;
	}
std::cout << std::endl;
}

//Link selection criterias

//The slot nearest to currentSlot having currentLink as an scheduled link
unsigned long int nearestSlot(std::vector < Slot > & slots, unsigned long int currentSlot, unsigned long int currentLink, unsigned long int minimum)
{
   unsigned long int nearestSlot = -1, end;
   long int begin;
   bool found = false;

   if (currentSlot + minimum < slots.size()) end = currentSlot + minimum;
   else end = slots.size() -1;
   if ((long int)(currentSlot - minimum) > 0) begin = currentSlot - minimum;
   else begin = 0;

   for (unsigned long int slot = currentSlot +1; (not found) and slot <= end; slot++)
      for (unsigned long int scheduledLinkIndex = 0; (not found) and scheduledLinkIndex < slots.at(slot).scheduled.size(); scheduledLinkIndex++)
         if (slots.at(slot).scheduled.at(scheduledLinkIndex) == currentLink)
         {
            nearestSlot = slot;
            found = true;
         }

   found = false;
   for (long int slot = currentSlot -1; (not found) and slot >= begin and (currentSlot - slot) < (nearestSlot - currentSlot); slot--)
      for (unsigned long int scheduledLinkIndex = 0; (not found) and scheduledLinkIndex < slots.at(slot).scheduled.size(); scheduledLinkIndex++)
         if (slots.at(slot).scheduled.at(scheduledLinkIndex) == currentLink)
         {
            nearestSlot = slot;
            found = true;
         }

   return nearestSlot;
}

//The number of candidates eliminated by scheduling link candidate pointed by selectedIndex
unsigned long int numberOfCandidatesEliminated(unsigned long int selectedIndex, Slot & slot, std::vector < Link > links, long double snrThreshold)
{
	unsigned long int selectedLink, candidateLink, scheduledLink;
	unsigned long int numberOfCandidatesEliminated;

	selectedLink = slot.candidates.at(selectedIndex);
	numberOfCandidatesEliminated = 0;

	for (unsigned long int candidateIndex = 0; candidateIndex < selectedIndex; candidateIndex++)
	{
		candidateLink = slot.candidates.at(candidateIndex);
		if ((links.at(selectedLink).interferedBy.at(candidateLink) / links.at(selectedLink).interferedBy.at(selectedLink)) + slot.candidatesInverseSNR.at(selectedIndex) > 1/dBtoSNR(snrThreshold) or
			(links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink)) + slot.candidatesInverseSNR.at(candidateIndex) > 1/dBtoSNR(snrThreshold))
			numberOfCandidatesEliminated++;
		else
		{
			for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
			{
				scheduledLink = slot.scheduled.at(scheduledIndex);
				//if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
						(links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
						slot.inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				{
					numberOfCandidatesEliminated++;
					break;
				}
			}
		}
	}
	for (unsigned long int candidateIndex = selectedIndex +1; candidateIndex < slot.candidates.size(); candidateIndex++)
	{
		candidateLink = slot.candidates.at(candidateIndex);
		if ((links.at(selectedLink).interferedBy.at(candidateLink) / links.at(selectedLink).interferedBy.at(selectedLink)) + slot.candidatesInverseSNR.at(selectedIndex) > 1/dBtoSNR(snrThreshold) or
			(links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink)) + slot.candidatesInverseSNR.at(candidateIndex) > 1/dBtoSNR(snrThreshold))
			numberOfCandidatesEliminated++;
		else
		{
			for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
			{
				scheduledLink = slot.scheduled.at(scheduledIndex);
				//if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) + slots.at(currentSlot).inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
						(links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
						slot.inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				{
					numberOfCandidatesEliminated++;
					break;
				}
			}
		}
	}

	return numberOfCandidatesEliminated;
}

unsigned long int numberOfArcCandidatesEliminated(unsigned long int selectedIndex, Slot & slot, std::vector < Link > links, long double snrThreshold)
{
	return numberOfCandidatesEliminated(selectedIndex, slot, links, snrThreshold);

	unsigned long int selectedLink, candidateLink, scheduledLink;
	unsigned long int numberOfArcCandidatesEliminated;

	selectedLink = slot.candidates.at(selectedIndex);
	numberOfArcCandidatesEliminated = 0;

	for (unsigned long int candidateIndex = 0; candidateIndex < selectedIndex; candidateIndex++)
	{
		candidateLink = slot.candidates.at(candidateIndex);
		if ((links.at(selectedLink).interferedBy.at(candidateLink) / links.at(selectedLink).interferedBy.at(selectedLink)) + slot.candidatesInverseSNR.at(selectedIndex) > 1/dBtoSNR(snrThreshold))
			numberOfArcCandidatesEliminated++;
		if ((links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink)) + slot.candidatesInverseSNR.at(candidateIndex) > 1/dBtoSNR(snrThreshold))
			numberOfArcCandidatesEliminated++;
		for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
		{
			scheduledLink = slot.scheduled.at(scheduledIndex);
			if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
				(links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
				slot.inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				numberOfArcCandidatesEliminated++;
		}
	}
	for (unsigned long int candidateIndex = selectedIndex +1; candidateIndex < slot.candidates.size(); candidateIndex++)
	{
		candidateLink = slot.candidates.at(candidateIndex);
		if ((links.at(selectedLink).interferedBy.at(candidateLink) / links.at(selectedLink).interferedBy.at(selectedLink)) + slot.candidatesInverseSNR.at(selectedIndex) > 1/dBtoSNR(snrThreshold))
			numberOfArcCandidatesEliminated++;
		if ((links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink)) + slot.candidatesInverseSNR.at(candidateIndex) > 1/dBtoSNR(snrThreshold))
			numberOfArcCandidatesEliminated++;
		for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
		{
			scheduledLink = slot.scheduled.at(scheduledIndex);
			if ((links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
				(links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink)) +
				slot.inverseSNR.at(scheduledIndex) > 1/dBtoSNR(snrThreshold))
				numberOfArcCandidatesEliminated++;
		}
	}

	return numberOfArcCandidatesEliminated;
}

//Inverse SNR received at link pointed by selectedIndex transmitted from scheduled links
long double inverseSNRreceivedFromScheduled(unsigned long int selectedIndex, Slot & slot)
{
		return slot.candidatesInverseSNR.at(selectedIndex);
}

//Inverse SNR transmitted from link pointed by selectedIndex received at scheduled links
long double inverseSNRtransmitedToScheduled(unsigned long int selectedIndex, Slot & slot, std::vector < Link > links)
{
	unsigned long int selectedLink, scheduledLink;
	long double inverseSNR;

	selectedLink = slot.candidates.at(selectedIndex);
	inverseSNR = 0;
	for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
	{
		scheduledLink = slot.scheduled.at(scheduledIndex);
		inverseSNR += links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
	}

	return inverseSNR;
}

//functions for the candidates???


//The link from slot candidates that less/most provokes interference in scheduled and viable candidate links by taking into account all transmissions from viable candidate and scheduled links.
unsigned long int totalInverseSNRSlot(std::vector < Link > & links, Slot & slot, long double snrThreshold)
{
	unsigned long int selectedLink, candidateLink, scheduledLink, bestLink, bestLinkIndex;
	long double receivedTotalInverseSNR, transmitedTotalInverseSNR = 0, bestReceivedTotalInverseSNR, bestTransmitedTotalInverseSNR = 0;
	long double candidateInverseSNR, selectedInverseSNR, scheduledInverseSNR, bestInverseSNR;

	//the first selected link is the first candidate (front(), selectedIndex = 0)
	bestLinkIndex = 0;
	bestLink = slot.candidates.at(bestLinkIndex);
	bestReceivedTotalInverseSNR = slot.candidatesInverseSNR.at(bestLinkIndex);
	for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
	{
		scheduledLink = slot.scheduled.at(scheduledIndex);
		bestTransmitedTotalInverseSNR += links.at(scheduledLink).interferedBy.at(bestLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
		for (unsigned long int candidateIndex = bestLinkIndex +1; candidateIndex < slot.candidates.size(); candidateIndex++)
		{
			candidateLink = slot.candidates.at(candidateIndex);
			//interferencia acumulada no candidateLink por causa de todos os scheduledLink + a transmitida pelo bestLink
			candidateInverseSNR = slot.candidatesInverseSNR.at(candidateIndex) +
					links.at(candidateLink).interferedBy.at(bestLink) / links.at(candidateLink).interferedBy.at(candidateLink);
			//interferencia acumulada no scheduledLink por causa de todos os scheduledLink + a transmitida pelo candidateLink + a transmitida pelo bestLink
			scheduledInverseSNR = slot.inverseSNR.at(scheduledIndex) +
					links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink) +
					links.at(scheduledLink).interferedBy.at(bestLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
			//interferencia acumulada no bestLink por causa de todos os scheduledLink + a transmitida pelo candidateLink
			bestInverseSNR = slot.candidatesInverseSNR.at(bestLinkIndex) +
					links.at(bestLinkIndex).interferedBy.at(candidateLink) / links.at(bestLinkIndex).interferedBy.at(bestLinkIndex);

			if (candidateInverseSNR <= 1/dBtoSNR(snrThreshold) and scheduledInverseSNR <= 1/dBtoSNR(snrThreshold) and bestInverseSNR <= 1/dBtoSNR(snrThreshold))
			{
				//acumula no bestReceivedTotalInverseSNR a interferencia (inverseSNR) do bestLink recebida dos candidates (viaveis) e de todos os scheduled
				bestReceivedTotalInverseSNR += links.at(bestLinkIndex).interferedBy.at(candidateLink) / links.at(bestLinkIndex).interferedBy.at(bestLinkIndex);
				//acumula no bestTransmitedTotalInverseSNR a interferencia (inverseSNR) em todos os cadidates (viaveis) transmitida pelo bestLink
				bestTransmitedTotalInverseSNR += links.at(candidateLink).interferedBy.at(bestLink) / links.at(candidateLink).interferedBy.at(candidateLink);
			}
		}
	}

	for (unsigned long int selectedIndex = 1; selectedIndex < slot.candidates.size(); selectedIndex++)
	{
		selectedLink = slot.candidates.at(selectedIndex);
		receivedTotalInverseSNR = slot.candidatesInverseSNR.at(selectedIndex);
		for (unsigned long int scheduledIndex = 0; scheduledIndex < slot.scheduled.size(); scheduledIndex++)
		{
			scheduledLink = slot.scheduled.at(scheduledIndex);
			transmitedTotalInverseSNR += links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
			for (unsigned long int candidateIndex = 0; candidateIndex < selectedIndex; candidateIndex++)
			{
				candidateLink = slot.candidates.at(candidateIndex);
				//interferencia acumulada no candidateLink por causa de todos os scheduledLink + a transmitida pelo selectedLink
				candidateInverseSNR = slot.candidatesInverseSNR.at(candidateIndex) +
						links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink);
				//interferencia acumulada no scheduledLink por causa de todos os scheduledLink + a transmitida pelo candidateLink + a transmitida pelo selectedLink
				scheduledInverseSNR = slot.inverseSNR.at(scheduledIndex) +
						links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink) +
						links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
				//interferencia acumulada no selectedLink por causa de todos os scheduledLink + a transmitida pelo candidateLink
				selectedInverseSNR = slot.candidatesInverseSNR.at(selectedIndex) +
						links.at(selectedIndex).interferedBy.at(candidateLink) / links.at(selectedIndex).interferedBy.at(selectedIndex);

				if (candidateInverseSNR <= 1/dBtoSNR(snrThreshold) and scheduledInverseSNR <= 1/dBtoSNR(snrThreshold) and selectedInverseSNR <= 1/dBtoSNR(snrThreshold))
				{
					//acumula no receivedTotalInverseSNR a interferencia (inverseSNR) do selectedLink recebida dos candidates (viaveis) e de todos os scheduled
					receivedTotalInverseSNR += links.at(selectedIndex).interferedBy.at(candidateLink) / links.at(selectedIndex).interferedBy.at(selectedIndex);
					//acumula no transmitedTotalInverseSNR a interferencia (inverseSNR) em todos os cadidates (viaveis) transmitida pelo selectedLink
					transmitedTotalInverseSNR += links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink);
				}
			}
			for (unsigned long int candidateIndex = selectedIndex +1; candidateIndex < slot.candidates.size(); candidateIndex++)
			{
				candidateLink = slot.candidates.at(candidateIndex);
				//interferencia acumulada no candidateLink por causa de todos os scheduledLink + a transmitida pelo selectedLink
				candidateInverseSNR = slot.candidatesInverseSNR.at(candidateIndex) +
						links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink);
				//interferencia acumulada no scheduledLink por causa de todos os scheduledLink + a transmitida pelo candidateLink + a transmitida pelo selectedLink
				scheduledInverseSNR = slot.inverseSNR.at(scheduledIndex) +
						links.at(scheduledLink).interferedBy.at(candidateLink) / links.at(scheduledLink).interferedBy.at(scheduledLink) +
						links.at(scheduledLink).interferedBy.at(selectedLink) / links.at(scheduledLink).interferedBy.at(scheduledLink);
				//interferencia acumulada no selectedLink por causa de todos os scheduledLink + a transmitida pelo candidateLink
				selectedInverseSNR = slot.candidatesInverseSNR.at(selectedIndex) +
						links.at(selectedIndex).interferedBy.at(candidateLink) / links.at(selectedIndex).interferedBy.at(selectedIndex);

				if (candidateInverseSNR <= 1/dBtoSNR(snrThreshold) and scheduledInverseSNR <= 1/dBtoSNR(snrThreshold) and selectedInverseSNR <= 1/dBtoSNR(snrThreshold))
				{
					//acumula no receivedTotalInverseSNR a interferencia (inverseSNR) do selectedLink recebida dos candidates (viaveis) e de todos os scheduled
					receivedTotalInverseSNR += links.at(selectedIndex).interferedBy.at(candidateLink) / links.at(selectedIndex).interferedBy.at(selectedIndex);
					//acumula no transmitedTotalInverseSNR a interferencia (inverseSNR) em todos os cadidates (viaveis) transmitida pelo selectedLink
					transmitedTotalInverseSNR += links.at(candidateLink).interferedBy.at(selectedLink) / links.at(candidateLink).interferedBy.at(candidateLink);
				}
			}

			if (receivedTotalInverseSNR + transmitedTotalInverseSNR < bestReceivedTotalInverseSNR + bestTransmitedTotalInverseSNR)
			{
				bestLinkIndex = selectedIndex;
				bestReceivedTotalInverseSNR = receivedTotalInverseSNR;
				bestTransmitedTotalInverseSNR = transmitedTotalInverseSNR;
			}
		}
	}

	//totalInverseSNR = bestReceivedTotalInverseSNR + bestTransmitedTotalInverseSNR;
	return bestLinkIndex;
}

unsigned long int bestLink(std::vector < Link > & links, std::vector < Slot > & slots, unsigned long int currentSlot, long double snrThreshold)
{
	//return totalInverseSNRSlot(links, slot, totalInverseSNR, snrThreshold, true);

	//---

	unsigned long int bestNumberOfCandidatesEliminated, bestCandidateIndex, bestNearestSlot, bestLink;
	unsigned long int candidateNearestSlot, candidateLink, numberOfEliminated;
	bestNumberOfCandidatesEliminated = numberOfArcCandidatesEliminated(0, slots.at(currentSlot), links, snrThreshold);
	bestCandidateIndex = 0;
	bestLink = slots.at(currentSlot).candidates.at(bestCandidateIndex);
	for (unsigned long int candidateIndex = 1; candidateIndex < slots.at(currentSlot).candidates.size(); candidateIndex++)
	{
		candidateLink = slots.at(currentSlot).candidates.at(candidateIndex);
		numberOfEliminated = numberOfArcCandidatesEliminated(candidateIndex, slots.at(currentSlot), links, snrThreshold);
		if (bestNumberOfCandidatesEliminated > numberOfEliminated)
		{
			bestNumberOfCandidatesEliminated = numberOfEliminated;
			bestCandidateIndex = candidateIndex;
         bestLink = candidateLink;
		}
		else if (bestNumberOfCandidatesEliminated == numberOfEliminated)
		{
			candidateNearestSlot = nearestSlot(slots, currentSlot, candidateLink, slots.size() +1);
			bestNearestSlot = nearestSlot(slots, currentSlot, bestLink, labs((long int)candidateNearestSlot - (long int)currentSlot));

			if (labs((long int)currentSlot - (long int)candidateNearestSlot) > labs((long int)currentSlot - (long int)bestNearestSlot))
			{
				bestNumberOfCandidatesEliminated = numberOfEliminated;
				bestCandidateIndex = candidateIndex;
				bestLink = candidateLink;
			}
//			else if (labs((long int)currentSlot - (long int)candidateNearestSlot) == labs((long int)currentSlot - (long int)bestNearestSlot))
//			{
//			}
		}
//      else if (bestNumberOfCandidatesEliminated == numberOfEliminated)
//      {
//      }
	}

	return bestCandidateIndex;
}

//stop criteria
bool stopCriteria(ScheduledLinks & scheduledLinks, long double gain, std::vector < Link > & links, std::vector < Slot > & slot, Slot & emptySlot)
{
	return (emptySlot.candidates.size() > 0);
}

//Ordering criterias

//Interference number
long double interferenceNumberCriteria(std::vector < Link > & links, unsigned long int linkA, unsigned long int linkB)
{
	return (links.at(linkA).interferenceNumberOfEdges - links.at(linkB).interferenceNumberOfEdges);
}

//The strongest or weakest link
long double powerCriteria(std::vector < Link > & links, unsigned long int linkA, unsigned long int linkB)
{
	return (links.at(linkA).interferedBy.at(linkA) - links.at(linkB).interferedBy.at(linkB));
}

//The nearest or farthest link
long double distanceCriteria(std::vector < Position > & nodePosition, std::vector < Link > & links, unsigned long int linkA, unsigned long int linkB)
{
	return euclideanDistance(nodePosition.at(links.at(linkA).senderNode), nodePosition.at(links.at(linkA).receiverNode)) -
	euclideanDistance(nodePosition.at(links.at(linkB).senderNode), nodePosition.at(links.at(linkB).receiverNode));

}

//The link that most/less interferes with others
//The link that is most/less interfered by others
//The link that most/less interferes other links scheduled on the referred slot
//The link that is most/less interfered by the other links scheduled on the referred slot
//The link that most/less interferes other links scheduled on the referred slot, but relative to the amount of SNR left on these links
//The link that is most/less interfered by other links scheduled on the referred slot, but relative to the amount of SNR left on this link

//---

//Initialization of the structure scheduledLinks that monitors the number of times each link was schedule
//In the beginning none of the links were scheduled, each link is in the line zero (number of schedules = 0) and in the position of its own label (links.at(1) has linkPosition.at(1) = 1 and scheduledLinks.linkLists.at(1) = 1);
void scheduledLinksInitialization(std::vector < Link > & links, ScheduledLinks & scheduledLinks, std::vector < unsigned long int > & dummyScheduledLinkListsItem)
{
	for (unsigned long int link = 0; link < links.size(); link++)
	{
		dummyScheduledLinkListsItem.push_back(link);
		scheduledLinks.linkPosition.push_back(link);
	}
	scheduledLinks.linkLists.push_back(dummyScheduledLinkListsItem);
	scheduledLinks.maximum = 0;
	scheduledLinks.minimum = 0;
	scheduledLinks.total = 0;
}

void loadNetwork(std::string fileName, long double maximumCommunicationDistance, std::vector < Position > & nodePosition, std::vector < std::vector < bool > > & neighborhood, std::vector < Link > & links)
{
	std::ifstream file;
	Link dummyLink;

	file.open(fileName.data(), std::ios::in);
	if (not file)
	 {
		std::cerr << fileName << " not found!" << std::endl;
		exit(1);
	}
	std::cout << "# Reading file:" << fileName << std::endl;

	//Loading every vertex position
	for (unsigned long int node = 0; node < nodePosition.size(); node++)
		file >> nodePosition.at(node).xPosition >> nodePosition.at(node).yPosition;
	file.close();

	//Reconstructing the neighborhood (it is a square matrix)
	for (unsigned long int newNode = 0; newNode < neighborhood.size(); newNode++)
	{
		for (unsigned long int node = newNode +1; node  < neighborhood.size(); node++)
			if (euclideanDistance(nodePosition.at(newNode), nodePosition.at(node)) <= maximumCommunicationDistance)
			{
				neighborhood.at(newNode).at(node) = true;
				neighborhood.at(node).at(newNode) = true;
			}
	}

	//Initialization of the set of links to be schedule
	for (unsigned long int nodeA = 0; nodeA < neighborhood.size(); nodeA++)
		for (unsigned long int nodeB = nodeA +1; nodeB < neighborhood.size(); nodeB++)
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

bool compareSlots(Slot & slotA, Slot & slotB)
{
   bool found = (slotA.scheduled.size() == slotB.scheduled.size());

   for (unsigned int scheduledLinkIndexA = 0; found and scheduledLinkIndexA < slotA.scheduled.size();  scheduledLinkIndexA++)
   {
      found = false;
      for (unsigned int scheduledLinkIndexB = 0; (not found) and scheduledLinkIndexB < slotB.scheduled.size();  scheduledLinkIndexB++)
      {
         if (slotA.scheduled.at(scheduledLinkIndexA) == slotB.scheduled.at(scheduledLinkIndexB))
            found = true;
      }
   }

   return found;
}

//not finished wet???
bool cycleDetection(std::vector < Slot > & slots, unsigned long int begin, unsigned long int end, unsigned int numberOfLinks)
{
   bool detected, found;
   std::vector < unsigned long int > scheduled;
   unsigned long int middle;

   //The last slot must be always empty, so begin must no be pointing to the last slot, that is, begin must not the last slot.
   //If cycle was not detected until half of the slots were investigated, then the cycle was not reached wet.
   //That is, a sequence of numbers cannot be repeated in a space smaller than the sequence size.
   middle = (end - begin) /2;
   found = false;
   detected = false;

   //search from begin to middle
   for (unsigned long int slotIndexBegin = begin; (not found) and slotIndexBegin <= middle; slotIndexBegin--)
   {
      //detect the begin of the repetition of the cycle
      found = compareSlots(slots.at(begin), slots.at(slotIndexBegin));
      if (found)
      {
         //detect the end of cycle repetition
         for (unsigned long int slotIndex = 1; found and slotIndex <= middle - slotIndexBegin; slotIndex--)
            found = compareSlots(slots.at(begin + slotIndex), slots.at(slotIndex + slotIndexBegin));
      }
      else
      {

         begin++;
         end = (end - begin) /2;
      }
   }

   return detected;
}

#endif /* MAIN_HPP_ */
