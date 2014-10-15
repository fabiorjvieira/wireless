/*
 * Node.cpp
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#include "Node.h"

namespace Networks
{

Node::Node(Networks::SpacialPosition sp, unsigned int identification, double minimumDistance, unsigned int networkSize)
{
   // TODO Auto-generated constructor stub
   this->position.x = sp.x;
   this->position.y = sp.y;
   this->position.z = sp.z;
   this->identification = identification;
   this->minimumDistance = minimumDistance;
   this->networkSize = networkSize;
   this->neighborhood = new bool * [this->networkSize];
   this->neighbors = new unsigned int [this->networkSize +1];
   this->neighborsQuantity = 0;
   for (unsigned int nextHop = 0; nextHop < this->networkSize; nextHop++)
   {
      this->neighborhood[nextHop] = new bool [this->networkSize];
      for (unsigned int abc = 0; abc < this->networkSize; abc++)
         this->neighborhood[nextHop][abc] = false;
   }
}

Node::~Node()
{
   // TODO Auto-generated destructor stub
   for (unsigned int nextHop = 0; nextHop < this->networkSize; nextHop++)
      delete [] this->neighborhood[nextHop];
   delete [] this->neighbors;
   delete [] this->neighborhood;
}

bool Node::setPosition(Networks::SpacialPosition sp)
{
   return true;
}

void Node::addNeighbor(unsigned int neighbor)
{
   if ((not this->neighborhood[neighbor][this->identification]) and neighbor != this->identification)
   {
      this->neighbors[this->neighborsQuantity++] = neighbor;
      this->neighbors[this->neighborsQuantity] = Support::INVALID_IDENTIFICATION;
   }
   this->neighborhood[neighbor][this->identification] = true;
   this->neighborhood[this->identification][neighbor] = true;

}

void Node::removeNeighbor(unsigned int neighbor)
{
   this->neighborhood[neighbor][this->identification] = false;
   this->neighborhood[this->identification][neighbor] = false;
}

}
