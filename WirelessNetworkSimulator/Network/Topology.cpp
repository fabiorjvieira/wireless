/*
 * Topology.cpp
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#include "Topology.h"

namespace Networks
{

Topology::Topology(unsigned int networkSize)
{
   // TODO Auto-generated constructor stub
   this->networkSize = networkSize;
   this->neighborhood = new double * [this->networkSize];

   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      this->neighborhood[line] = new double[this->networkSize];
   }
}

Topology::~Topology()
{
   // TODO Auto-generated destructor stub
   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      delete [] this->neighborhood[line];
   }
   delete [] this->neighborhood;
}

bool Topology::isNeighbor(Networks::Node * nodeA, Networks::Node * nodeB)
{
   double minimumDistance;

   if (nodeA->getMinimumDistance() < nodeB->getMinimumDistance())
      minimumDistance = nodeA->getMinimumDistance();
   else minimumDistance = nodeB->getMinimumDistance();

   return (minimumDistance >= this->neighborhood[nodeA->getIdentification()][nodeB->getIdentification()]);
}

void Topology::discoverNeighborhood(Networks::Node ** nodes)
{
   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      for (unsigned int column = 0; column < this->networkSize; column++)
      {
         this->neighborhood[line][column] = std::sqrt(
            std::pow(nodes[line]->getPosition().x - nodes[column]->getPosition().x, 2) +
            std::pow(nodes[line]->getPosition().y - nodes[column]->getPosition().y, 2) +
            std::pow(nodes[line]->getPosition().z - nodes[column]->getPosition().z, 2));
         this->neighborhood[column][line] = this->neighborhood[line][column];
         if (this->isNeighbor(nodes[line], nodes[column]))
         {
            nodes[line]->addNeighbor(column);
            nodes[column]->addNeighbor(line);
         }
      }
   }
}

void Topology::discoverNeighborsNeighborhood(Networks::Node ** nodes)
{
   for (unsigned int node = 0; node < this->networkSize; node++)
   {
      for (unsigned int neighborA = 0; neighborA < this->networkSize; neighborA++)
      {
         if (nodes[node]->isNeighbor(neighborA))
         {
            for (unsigned int neighborOfA = 0; neighborOfA < this->networkSize; neighborOfA++)
            {
               nodes[node]->getNeighborhood()[neighborA][neighborOfA] = nodes[neighborA]->getNeighborhood()[neighborA][neighborOfA];
               nodes[node]->getNeighborhood()[neighborOfA][neighborA] = nodes[neighborA]->getNeighborhood()[neighborA][neighborOfA];
            }
         }
      }
   }
}

}

/*
 * Ver se ser vizinho de si mesmo esta ajudando ou Ž inœtil
 * acho que alguns discoveries sao inœteis
 *
 */
