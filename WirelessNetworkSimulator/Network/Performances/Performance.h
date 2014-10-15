/*
 * Performance.h
 *
 *  Created on: Jan 25, 2009
 *      Author: fjimenez
 */

#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#include "../Routing/AllRoutesDiscover.h"
#include "Statistic.h"

namespace Performances {

namespace File
{

namespace Format
{

   static const unsigned char ORIGIN = 0;
   static const unsigned char DESTINY = 1;
   static const unsigned char ROUTE_SIZE = 2;
   static const unsigned char CYCLE_SIZE = 3;
   static const unsigned char TO_DESTINY_TRANSMISSION_QUANTITY = 4;
   static const unsigned char TO_DESTINY_UNIQUE_TRANSMISSION_QUANTITY = 5;
   static const unsigned char FROM_ORIGIN_UNIQUE_TRANSMISSION_QUANTITY = 6;
   static const unsigned char ROUTES_QUANTITY = 7;
   static const unsigned char MAXIMUM_ROUTE_SIZE = 8;
   static const unsigned char TRANSMISSION_QUANTITY = 9;
   static const unsigned char NODE_QUANTITY = 10;
   static const unsigned char UNIQUE_TRANSMISSION_QUANTITY = 11;
   static const unsigned char TRANSMISSION_GRAPH_EDGE_QUANTITY = 12;
   static const unsigned char TRANSMISSION_GRAPH_D2_EDGE_QUANTITY = 13;
   static const unsigned char BUFFER_SIZE = 14;
   static const unsigned char PARTIAL_ORDER_CYCLE_MEMBER = 15;

   static const unsigned char RUN = 0;
   static const unsigned char PAIR_QUANTITY = 1;

   static const unsigned char ROUTE_SIZE_AVG = 0;
   static const unsigned char PERFORMANCE_FACTOR_AVG = 1;
   static const unsigned char STANDARD_DERIVATION = 2;
   static const unsigned char CONFIDENCE = 3;
   static const unsigned char UP_CONFIDENCE_INTERVAL = 4;
   static const unsigned char DOWN_CONFIDENCE_INTERVAL = 5;
}

}

struct TransmissionList
{
   unsigned int order;
   Networks::Node * origin;
   Networks::Node * destiny;
   unsigned int quantity;
};

struct performanceFactor
{
   double factor;
   unsigned int quantity;
   double normalizedQuantity;
};

struct squareMatrix
{
   unsigned int ** matrix;
   unsigned int begin, end;
   unsigned int size;
};

class PartialOrder
{
private:
   unsigned int sizeOf;
   unsigned int * layers[2];
   int * buffer[2];

public:

   PartialOrder(unsigned int size)
   {
      this->sizeOf = size;
      this->layers[0] = new unsigned int [this->sizeOf];
      this->layers[1] = new unsigned int [this->sizeOf];
      this->buffer[0] = new int [this->sizeOf];
      this->buffer[1] = new int [this->sizeOf];
   }
   PartialOrder(const Performances::PartialOrder & partialOrder)
   {
      this->sizeOf = 0;
      this->layers[0] = NULL;
      this->layers[1] = NULL;
      this->buffer[0] = NULL;
      this->buffer[1] = NULL;
      *this = partialOrder;
   }
   PartialOrder()
   {
      this->sizeOf = 0;
      this->layers[0] = NULL;
      this->layers[1] = NULL;
      this->buffer[0] = NULL;
      this->buffer[1] = NULL;
   }
   virtual ~PartialOrder()
   {
      this->clear();
   }
   unsigned int & at(unsigned int position)
   {
      return this->layers[0][position];
   }
   unsigned int & operator [] (unsigned int position)
   {
      return this->layers[0][position];
   }
   void clear()
   {
      if (this->sizeOf > 0)
      {
         delete [] this->layers[0];
         delete [] this->layers[1];
         delete [] this->buffer[0];
         delete [] this->buffer[1];
         this->sizeOf = 0;
      }
   }
   void reserve(unsigned int size)
   {
      this->clear();
      this->sizeOf = size;
      this->layers[0] = new unsigned int [this->sizeOf];
      this->layers[1] = new unsigned int [this->sizeOf];
      this->buffer[0] = new int [this->sizeOf];
      this->buffer[1] = new int [this->sizeOf];
   }
   unsigned int size() {return this->sizeOf;}
   Performances::PartialOrder & operator = (const Performances::PartialOrder & partialOrder)
   {
      this->reserve(partialOrder.sizeOf);
      for (unsigned int member = 0; member < this->sizeOf; member++)
      {
         this->layers[0][member] = partialOrder.layers[0][member];
         this->layers[1][member] = partialOrder.layers[1][member];
         this->buffer[0][member] = partialOrder.buffer[0][member];
         this->buffer[1][member] = partialOrder.buffer[1][member];
      }
      return *this;
   }
   unsigned int * layersHistory() {return this->layers[1];}
   int * bufferHistory() {return this->buffer[1];}
   int * getBuffer() {return this->buffer[0];}
};

class Performance {
public:
   Performance(Networks::Node ** nodes, unsigned int networkSize);
   virtual ~Performance();

   void loadRoutes(Networks::Routing::AllRoutesDiscover * router, bool enumerationType);
   void initialize();

   Performances::PartialOrder * initializePartialOrder(Performances::squareMatrix * concurrentGraph);
   Performances::PartialOrder * initializePartialOrder()
   {
      Performances::squareMatrix * concurrentGraph = this->initializeConcurrenceGraph();
      Performances::PartialOrder * partialOrder = this->initializePartialOrder(concurrentGraph);
      for (unsigned int concurrentMember = 0; concurrentMember < concurrentGraph->size; concurrentMember++)
            delete [] concurrentGraph->matrix[concurrentMember];
      delete [] concurrentGraph->matrix;
      delete concurrentGraph;
      return partialOrder;
   }

   std::vector < Performances::PartialOrder * > * sortCycle(Performances::PartialOrder * partialOrder, std::vector < Performances::PartialOrder * > * partialOrderCycle, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder));
   std::vector < Performances::PartialOrder * > * captureCycle(Performances::PartialOrder * partialOrder, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder));
   Performances::PartialOrder * partialOrderDynamics(Performances::PartialOrder * partialOrder, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder));

   void vizingDynamicsHeuristics(Networks::Routing::AllRoutesDiscover * router);

   void allDynamics(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix, void (Performances::Performance::*dynamicsType)(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity));
   void allMinimumPathDynamics(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix, void (Performances::Performance::*dynamicsType)(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity));

   void dominanceTest(const char * difference, const char * baseFileNameA, const char * baseFileNameB, unsigned short quantity, const char * basePerfFileNameSuffixA, const char * basePerfFileNameSuffixB, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity));
   void averageGroupedByXYcolumns(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, unsigned char xColumn, unsigned char yColumn, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity));
   void histogramXYcolumns(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, unsigned char xColumn, unsigned char yColumn, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity));
   void ju(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, double maximum,  double minimum, double increment, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity));
   void averageAllrouteQuantity(const char * baseFileName, unsigned short networkQuantity, unsigned short runs, const char * basePerfFileNameSuffix, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity));
   void allMinimumPathSizeHistogram(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector <unsigned int> * histogram);
   void allMinimumPathSharedLinkHistogram(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector <unsigned int> * histogram, const char * histogramType);
   void allMinimumPathSharedLinkAverage(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector < double > * linkAverage, std::vector < double > * pathSizeAverage, std::vector < double > * cliqueAverage);
   void networkDegreeHistogram(std::vector <unsigned int> * histogram);

   bool lowFilter(unsigned int * values, unsigned int quantity);
   bool trueFilter(unsigned int * values, unsigned int quantity);
   bool falseFilter(unsigned int * values, unsigned int quantity);
   bool valmirFilter(unsigned int * values, unsigned int quantity);

   /*comment*///temporariamente inœteis
   void debugSquareMatriz(Performances::squareMatrix * concurrentGraph, bool jump);
   void allVIZ(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix);
   void simpleDynamics(Performances::PartialOrder * partialOrder);
   static void fix(const char * networkBaseFileName, const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, const char * baseFileNameSuffix, unsigned int bufferSize, unsigned int state);
   static void search(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix);

   void antecipationBufferedRule(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder)
   {
      unsigned int initialHole;
      /*comment*///regra com antecipacao - montando um mapa unidimensinal da localizacao dos vizinhos nas camadas
      lastHole = 0;
      for (unsigned int layer = 1; layer < holes.size(); layer++) holes[layer] = Support::EMPTY_LAYER;
      for (unsigned int neighbor = 0; neighbor < neighbors[transmission].size(); neighbor++)
      {
         holes[partialOrder->at(neighbors[transmission][neighbor])] = Support::NEIGHBOR_IN_LAYER;
         if (lastHole < partialOrder->at(neighbors[transmission][neighbor]))
            lastHole = partialOrder->at(neighbors[transmission][neighbor]);
      }
      /*comment*///atualizando e verificando o buffer; levar em considera‹o o -1 e o destino que possui buffer infinito
      initialHole = 1;
      if (/*buffer[transmission] > -1*/ neighbors[transmission][0] != transmission)
      {
         partialOrder->getBuffer()[transmission]--;
         if (0 == partialOrder->getBuffer()[transmission]) initialHole = partialOrder->at(neighbors[transmission][0]) + 1;
      }
      if (/*buffer[neighbors[transmission][1]] > -1*/ neighbors[transmission][1] != transmission)
      {
         partialOrder->getBuffer()[neighbors[transmission][1]]++;
         if (Support::BUFFER_SIZE == partialOrder->getBuffer()[neighbors[transmission][1]] and initialHole < partialOrder->at(neighbors[transmission][1]) + 1)
            initialHole = partialOrder->at(neighbors[transmission][1]) + 1;
      }
      /*comment*///regra com antecipacao - descobrindo o primeiro buraco
      firstHole = lastHole + 1;
      for (unsigned int layer = initialHole; layer < lastHole; layer++)
         if (holes[layer] == Support::EMPTY_LAYER)
         {
            firstHole = layer;
            break;
         }
      /*comment*///reposicionando os ativos
      partialOrder->at(transmission) = firstHole;
   }

   void antecipationRule(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder)
   {
      //regra com antecipacao - montando um mapa unidimensinal da localizacao dos vizinhos nas camadas
      lastHole = 0;
      for (unsigned int layer = 1; layer < holes.size(); layer++) holes[layer] = Support::EMPTY_LAYER;
      for (unsigned int neighbor = 0; neighbor < neighbors[transmission].size(); neighbor++)
      {
         holes[partialOrder->at(neighbors[transmission][neighbor])] = Support::NEIGHBOR_IN_LAYER;
         if (lastHole < partialOrder->at(neighbors[transmission][neighbor]))
            lastHole = partialOrder->at(neighbors[transmission][neighbor]);
      }
      //regra com antecipacao - descobrindo o primeiro buraco
      firstHole = lastHole + 1;
      for (unsigned int layer = 1; layer < lastHole; layer++)
         if (holes[layer] == Support::EMPTY_LAYER)
         {
            firstHole = layer;
            break;
         }
      /*comment*///reposicionando os ativos
      partialOrder->at(transmission) = firstHole;
   }

   void normalRule(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder)
   {
      //regra sem antecipacao - decobrindo o ultimo buraco
      lastHole = 0;
      for (unsigned int neighbor = 0; neighbor < neighbors[transmission].size(); neighbor++)
         if (lastHole < partialOrder->at(neighbors[transmission][neighbor]))
            lastHole = partialOrder->at(neighbors[transmission][neighbor]);

      if (neighbors[transmission][0] != transmission) partialOrder->getBuffer()[transmission]--;
      if (neighbors[transmission][1] != transmission) partialOrder->getBuffer()[neighbors[transmission][1]]++;

      /*comment*///reposicionando os ativos
      lastHole++;
      partialOrder->at(transmission) = lastHole;
   }

   void normalDynamics(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity)
   {
      Performances::PartialOrder * partialOrder;

      if (this->lastFullTransmission > 0)
      {
         this->initialize();
         partialOrder = this->initializePartialOrder();
         partialOrder = this->partialOrderDynamics(partialOrder, &Performances::Performance::normalRule);
         delete partialOrder;
      }
      else
      {
         this->cycleSize = 1;
         this->toDestinyTransmissionQuantity = 1;
      }
      file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << 1 << Support::SEPARATOR << 0 << Support::END_OF_INFORMATION  << std::endl;
   }

   void antecipationDynamics(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity)
   {
      Performances::PartialOrder * partialOrder;

      if (this->lastFullTransmission > 0)
      {
         this->initialize();
         partialOrder = this->initializePartialOrder();
         partialOrder = this->partialOrderDynamics(partialOrder, &Performances::Performance::antecipationRule);
         delete partialOrder;
      }
      else
      {
         this->cycleSize = 1;
         this->toDestinyTransmissionQuantity = 1;
      }
      file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << 1 << Support::SEPARATOR << 0 << Support::END_OF_INFORMATION  << std::endl;
   }

   void antecipationBufferedDynamics(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity)
   {
      Performances::PartialOrder * partialOrder;
      Performances::PartialOrder partialOrderCopy;

         if (this->lastFullTransmission > 0)
         {
            this->initialize();
            partialOrder = this->initializePartialOrder();
            for (Support::BUFFER_SIZE = Support::INITIAL_BUFFER_SIZE; Support::BUFFER_SIZE <= Support::FINAL_BUFFER_SIZE; Support::BUFFER_SIZE++)
            {
               partialOrderCopy = *partialOrder;
               this->partialOrderDynamics(&partialOrderCopy, &Performances::Performance::antecipationBufferedRule);
               file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << Support::BUFFER_SIZE << Support::SEPARATOR << 0 << Support::END_OF_INFORMATION  << std::endl;
            }
            delete partialOrder;
         }
         else
         {
            this->cycleSize = 1;
            this->toDestinyTransmissionQuantity = 1;
            file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << Support::INITIAL_BUFFER_SIZE << Support::SEPARATOR << 0 << Support::END_OF_INFORMATION  << std::endl;
         }
   }

   void hybridDynamics(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity)
   {
      Performances::PartialOrder * partialOrder;
      std::vector < Performances::PartialOrder * > * partialOrderCycle;

      if (this->lastFullTransmission > 0)
      {
         this->initialize();
         partialOrder = this->initializePartialOrder();
         partialOrder = this->partialOrderDynamics(partialOrder, &Performances::Performance::normalRule);
         partialOrderCycle = this->captureCycle(partialOrder, &Performances::Performance::normalRule);
         delete partialOrder;
         partialOrder = this->initializePartialOrder();
         partialOrderCycle = this->sortCycle(partialOrder, partialOrderCycle, &Performances::Performance::normalRule);
         delete partialOrder;

         for (Support::BUFFER_SIZE = Support::INITIAL_BUFFER_SIZE; Support::BUFFER_SIZE <= Support::FINAL_BUFFER_SIZE; Support::BUFFER_SIZE++)
         {
            for (unsigned int partialOrderCycleMember = Support::FIRST_PARTIAL_ORDER_CYCLE_MEMBER; partialOrderCycleMember < partialOrderCycle->size() and partialOrderCycleMember <= Support::LAST_PARTIAL_ORDER_CYCLE_MEMBER; partialOrderCycleMember++)
            {
               partialOrder = new Performances::PartialOrder(*partialOrderCycle->at(partialOrderCycleMember));
               partialOrder = this->partialOrderDynamics(partialOrder, &Performances::Performance::antecipationBufferedRule);
               file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << Support::BUFFER_SIZE << Support::SEPARATOR << partialOrderCycleMember << Support::END_OF_INFORMATION  << std::endl;
               delete partialOrder;
            }
         }
         for (unsigned int partialOrderCycleMember = 0; partialOrderCycleMember < partialOrderCycle->size(); partialOrderCycleMember++)
            delete partialOrderCycle->at(partialOrderCycleMember);
         delete partialOrderCycle;
      }
      else
      {
         this->cycleSize = 1;
         this->toDestinyTransmissionQuantity = 1;
         file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::SEPARATOR << this->fromOriginUniqueTransmissionQuantity << Support::SEPARATOR << this->routesQuantity << Support::SEPARATOR << this->maximumHopCount << Support::SEPARATOR << this->lastFullTransmission + 1 << Support::SEPARATOR << nodeQuantity << Support::SEPARATOR << this->lastTransmission +1 << Support::SEPARATOR << this->transmissionGraphEdgesQuantity << Support::SEPARATOR << this->transmissionGraphD2EdgesQuantity << Support::SEPARATOR << Support::INITIAL_BUFFER_SIZE << Support::SEPARATOR << 0 << Support::END_OF_INFORMATION  << std::endl;
      }
   }

private:
   void activatedDiscover(std::vector <unsigned int> & activatedMembers, Performances::squareMatrix * squareMatrix);
   Performances::squareMatrix * initializeConcurrenceGraph();
   void clear();
   void init();

   Performances::TransmissionList * transmissionList;

   unsigned int * transmissionQueue;
   unsigned int lastTransmission;

   std::vector <unsigned int> fullTransmissionQueue;
   std::vector < std::vector <unsigned int> > transmissionNeighborhood;
   std::vector < std::vector <unsigned int> > transmissionNeighbors;
   unsigned int lastFullTransmission;
   unsigned int ** concurrentTransmissions;

   unsigned int historyLimit;
   unsigned long int lastHistory;
   unsigned long int cycleSize;
   unsigned long int toDestinyTransmissionQuantity;
   unsigned long int maximumHopCount;
   unsigned long int routesQuantity;
   unsigned int toDestinyUniqueTransmissionQuantity;
   unsigned int fromOriginUniqueTransmissionQuantity;

   std::vector <bool> toDestinyTransmissions;
   std::vector <bool> fromOriginTransmissions;

   unsigned int transmissionGraphEdgesQuantity;
   unsigned int transmissionGraphD2EdgesQuantity;

   unsigned int networkSize;
   Networks::Node ** nodes;
};

}

#endif /* PERFORMANCE_H_ */
