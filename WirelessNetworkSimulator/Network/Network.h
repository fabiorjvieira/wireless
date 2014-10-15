/*
 * Network.h
 *
 *  Created on: Feb 6, 2009
 *      Author: fjimenez
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "Topology.h"

namespace Networks
{

class Network {
public:
   Network(Networks::Topology * topology);
   virtual ~Network();
   Networks::Node ** loadNetwork(const char * fileName, double minimumDistance);

//private:
   Networks::Node ** nodes;
   unsigned int networkSize;
   Networks::Topology * topology;
};

}

#endif /* NETWORK_H_ */
