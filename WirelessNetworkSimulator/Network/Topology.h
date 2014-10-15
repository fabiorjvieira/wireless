/*
 * Topology.h
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "Node.h"

namespace Networks
{

class Topology {
public:
   Topology(unsigned int networkSize);
   virtual ~Topology();

   bool isNeighbor(Networks::Node * nodeA, Networks::Node * nodeB);
   unsigned int getNetworkSize() {return this->networkSize;};
   double getNeighborsDistance(Networks::Node * nodeA, Networks::Node * nodeB) {return this->neighborhood[nodeA->getIdentification()][nodeB->getIdentification()];};

   void discoverNeighborhood(Networks::Node ** nodes);
   void discoverNeighborsNeighborhood(Networks::Node ** nodes);

   void printTopology()
   {
      std::cout << "Topology" << std::endl;
      for (unsigned int line = 0; line < this->networkSize; line++)
      {
         for (unsigned int column = 0; column < this->networkSize; column++)
         {
            std::cout << this->neighborhood[line][column] << ",";
         }
         std::cout << std::endl;
      }
   }

private:
   double ** neighborhood;
   unsigned int networkSize;
};

}

#endif /* TOPOLOGY_H_ */
