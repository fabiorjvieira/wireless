/*
 * solver.hpp
 *
 *  Created on: Dec 5, 2013
 *      Author: fabiorjvieira
 */

#include "main.hpp"

bool slotOk(Slot & slot, std::vector < Link > & links, long double snrThreshold, long double bandWidth)
{
   bool ok =true;
   unsigned long int scheduledLinkA, scheduledLinkB;
   long double inverseSNR;

   for (unsigned long int scheduledIndexA = 0; scheduledIndexA < slot.scheduled.size() and ok; scheduledIndexA++)
   {
      scheduledLinkA = slot.scheduled.at(scheduledIndexA);
      inverseSNR = noiseFloor(bandWidth) / links.at(scheduledLinkA).interferedBy.at(scheduledLinkA);
      for (unsigned long int scheduledIndexB = 0; scheduledIndexB < scheduledIndexA and ok; scheduledIndexB++)
      {
         scheduledLinkB = slot.scheduled.at(scheduledIndexB);
         inverseSNR += links.at(scheduledLinkA).interferedBy.at(scheduledLinkB) / links.at(scheduledLinkA).interferedBy.at(scheduledLinkA);
         if (inverseSNR > 1/dBtoSNR(snrThreshold)) ok = false;
      }
      for (unsigned long int scheduledIndexB = scheduledIndexA +1; scheduledIndexB < slot.scheduled.size() and ok; scheduledIndexB++)
      {
         scheduledLinkB = slot.scheduled.at(scheduledIndexB);
         inverseSNR += links.at(scheduledLinkA).interferedBy.at(scheduledLinkB) / links.at(scheduledLinkA).interferedBy.at(scheduledLinkA);
         if (inverseSNR > 1/dBtoSNR(snrThreshold)) ok = false;
      }
   }

   return ok;
}

void possibleSlots(Slot slot, std::vector < Link > & links, long double snrThreshold, long double bandWidth)
{
   Slot localSlot = slot;

   while (slot.candidates.size() > 0)
   {
      localSlot.scheduled.push_back(localSlot.candidates.front());
      slot.candidates.erase(slot.candidates.begin() +0);
      localSlot.candidates.erase(localSlot.candidates.begin() +0);
      possibleSlots(localSlot, links, snrThreshold, bandWidth);
      localSlot = slot;
   }

   if (slotOk(localSlot, links, snrThreshold, bandWidth))
   {
      for (unsigned long int scheduledIndexA = 0; scheduledIndexA < localSlot.scheduled.size(); scheduledIndexA++)
         std::cout << localSlot.scheduled.at(scheduledIndexA) << " ";
      std::cout << std::endl;
   }
}

int solver(int argn, char ** argc)
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

   std::string fileName;

   //802.11g bandwidth=20MHz transmission power=30mW beta=25dB (1% perda com 54Mbs)
   // power band     pathloss sinr  nodes .dot
   // 0.030 20000000 4.0         0    100      ~/git/general/WISp/networks/100/nodes/graph.11447.0.dot.node

   //load a network topology from a ".dot" file (nodePosition)
   //Calculate the neighborhood (link set)
   //Inform transmissionPower, pathLoss, snrThreshold, bandWidth

   //argument test
   if (argn != 7)
   {
      std::cout << "Six arguments are required in this order: " << std::endl;
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
   Slot slot;

   //load the network and discover the link set
   loadNetwork(fileName, maximumCommunicationDistance, nodePosition, neighborhood, links);

   //Calculate the relative interference between each pair of nodes for all links in the set of links
   interferedBy(links, nodePosition, transmissionPower, pathLoss, snrThreshold);

   slot.scheduled.push_back(0);
   slot.scheduled.push_back(3);
   if (slotOk(slot, links, snrThreshold, bandWidth)) std::cout << "ok" << std::endl;
   slot.scheduled.clear();

   slot.scheduled.push_back(0);
   slot.scheduled.push_back(4);
   if (slotOk(slot, links, snrThreshold, bandWidth)) std::cout << "ok" << std::endl;
   slot.scheduled.clear();

   slot.scheduled.push_back(1);
   slot.scheduled.push_back(2);
   if (slotOk(slot, links, snrThreshold, bandWidth)) std::cout << "ok" << std::endl;
   slot.scheduled.clear();

   slot.scheduled.push_back(1);
   slot.scheduled.push_back(4);
   if (slotOk(slot, links, snrThreshold, bandWidth)) std::cout << "ok" << std::endl;
   slot.scheduled.clear();

   slot.scheduled.push_back(2);
   slot.scheduled.push_back(3);
   if (slotOk(slot, links, snrThreshold, bandWidth)) std::cout << "ok" << std::endl;
   slot.scheduled.clear();

   //initializing the first empty Slot (every link is a viable candidate)
   for (unsigned long int link = 0; link < links.size(); link++)
      slot.candidates.push_back(link);

   possibleSlots(slot, links, snrThreshold, bandWidth);

   return 0;
}
