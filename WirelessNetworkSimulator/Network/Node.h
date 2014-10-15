/*
 * Node.h
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#ifndef NODE_H_
#define NODE_H_

#include "Routing/Routing.h"

namespace Networks
{

struct Package
{
   unsigned int * routeTableLines;
   unsigned int * blackNodes; // blackNodes[last position] = networkSize
   unsigned int * maximumHeightedIndependentSet; // maximumIndependentSet[last position] = networkSize
   unsigned int column, routeTableLinesQuantity;
};

struct SpacialPosition
{
   double x, y, z;
};

class Node {
public:
   Node(Networks::SpacialPosition sp, unsigned int identification, double minimumDistance, unsigned int networkSize);
   virtual ~Node();

   Networks::SpacialPosition getPosition() {return this->position;};
   unsigned int getIdentification() {return this->identification;};
   double getMinimumDistance() {return this->minimumDistance;};
   bool ** getNeighborhood() {return this->neighborhood;};

   bool setPosition(Networks::SpacialPosition sp);
   void addNeighbor(unsigned int neighbor);
   void removeNeighbor(unsigned int neighbor);

   bool isNeighbor(unsigned int neighbor)
   {
      return this->neighborhood[this->identification][neighbor];
   }

   void printNeighborhood()
   {
      std::cout << "Node Neighborhood:" << this->identification << std::endl;
      for (unsigned int line = 0; line < this->networkSize; line++)
      {
         for (unsigned int column = 0; column < this->networkSize; column++)
         {
            std::cout << this->neighborhood[line][column] << ",";
         }
         std::cout << std::endl;
      }
      std::cout << "Neighbors:" ;
      for (unsigned int neighbor = 0; this->neighbors[neighbor] != Support::INVALID_IDENTIFICATION; neighbor++)
      {
         std::cout << this->neighbors[neighbor] << " ";
      }
      std::cout << "-" << this->neighborsQuantity << std::endl;
   }

   void printNode()
   {
      std::cout << "Node:" << this->identification << " - " << "Position (x,y,z):" << this->position.x << "," << this->position.y << "," << this->position.z << std::endl;
   }

   unsigned int getNeighbors(unsigned int position) {return this->neighbors[position];}
   unsigned int getNeighborsQuantity() {return this->neighborsQuantity;}

private:
   unsigned int identification;
   double minimumDistance;
   bool ** neighborhood;
   unsigned int * neighbors;
   unsigned int neighborsQuantity;

   Networks::SpacialPosition position;
   unsigned int networkSize;
};

}

#endif /* NODE_H_ */
