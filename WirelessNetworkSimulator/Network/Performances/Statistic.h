/*
 * Statistic.h
 *
 *  Created on: Nov 29, 2008
 *      Author: fjimenez
 */

#ifndef STATISTIC_H_
#define STATISTIC_H_

#include "../Node.h"
#include "../../Statistics/studenttdistr.h"

namespace Performances {

class Statistic {
public:
   Statistic() {};
   virtual ~Statistic() {};

   static unsigned int edgeQuantity(Networks::Node ** nodes, unsigned int networkSize);
   static void neighborhoodQuantities(Networks::Node ** nodes, unsigned int ** quantities, unsigned int networkSize);
   static double average(double * values, unsigned int size);
   static double std(double * values, double average, unsigned int size);
   static double average(unsigned int * values, unsigned int size);
   static double std(unsigned int * values, double average, unsigned int size);
   static double std( std::vector <double> values, double average, unsigned int size);
   static Networks::SpacialPosition averageNodePosition(Networks::Node ** nodes, unsigned int networkSize);
   static Networks::SpacialPosition stdNodePosition(Networks::Node ** nodes, Networks::SpacialPosition average, unsigned int networkSize);
   static double scaling(double value, double average, double std);
   static double ** histogram(double ** values, unsigned int size, unsigned int length);
   static double confidence(double confidenceFactor, double std, unsigned int size);
};

}

#endif /* STATISTIC_H_ */
