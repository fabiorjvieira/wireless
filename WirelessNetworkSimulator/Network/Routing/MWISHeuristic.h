/*
 * MWISHeuristic.h
 *
 *  Created on: Feb 7, 2009
 *      Author: fjimenez
 */

#ifndef MWISHEURISTIC_H_
#define MWISHEURISTIC_H_

#include "AllRoutesDiscover.h"

namespace Networks {

namespace Routing {

namespace Heuristics {

#define MWISscore(score, alfa) ((alfa) * (score[Support::SUM])) + ((1-(alfa)) * (score[Support::MINIMUM]))

class MWISHeuristic {
public:
   MWISHeuristic();
   virtual ~MWISHeuristic();
   static unsigned int * maximumWeightedIndependentSet(Networks::Routing::BucketNextHop * nextHops, Networks::Node ** nodes);
   static void scoreHeuristics(double candidatesScore[2], Networks::Routing::NextHop nextHop, bool action);
   void routing(Networks::Node ** nodes, Networks::Routing::AllRoutesDiscover * router, unsigned int origin, unsigned int destiny, char * baseFileName);

private:

};

}

}

}

#endif /* MWISHEURISTIC_H_ */
