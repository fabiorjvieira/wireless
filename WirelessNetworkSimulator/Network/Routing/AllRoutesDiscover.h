/*
 * AllRoutesDiscover.h
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#ifndef ALLROUTESDISCOVER_H_
#define ALLROUTESDISCOVER_H_

#include "../Network.h"

namespace Networks
{

namespace Routing
{

class AllRoutesDiscover
{
public:
   AllRoutesDiscover(unsigned int networkSize);
   virtual ~AllRoutesDiscover();

   void loadRoutes(const char * fileName);
   void loadRoutes(const char * fileName, unsigned short quantityLimit);
   static void printTable(std::vector < std::vector <unsigned int> > * routeTable);
   void allRoutesDiscover(Networks::Node ** nodes, char * baseFileName);
   void minimumRouteDiscover(const char * baseFileName);

   void nRoutesThroughEachNeighbor(Networks::Node ** nodes, const char * baseFileName, unsigned int limit);
   void nDijkstraRoutesDiscover(Networks::Node ** nodes, const char * baseFileName);
   void nNeighborsRoutes(const char * baseFileName, unsigned int limit);
   void routesOrganizer(const char * baseFileName, const char * sufixFileName, unsigned int limit);
   void allnNeighborsRoutes(const char * baseFileName, unsigned int limit);
   void routesOrganizerFile(const char * baseFileName, const char * sufixFileName);

   static void saveRoutes(const char * fileName, std::vector < std::vector <unsigned int> > * routeTable);
   static void routeSort(std::vector < std::vector <unsigned int> > * routeTable, bool order);

   AllRoutesDiscover & operator = (const AllRoutesDiscover & router)
   {
      this->networkSize = router.networkSize;
      this->nodeQuantity = router.nodeQuantity;
      this->maximumColumnQuantity = router.maximumColumnQuantity;
      this->routeTable = router.routeTable;
      return * this;
   }

//private:
   unsigned int networkSize;
   std::vector < std::vector <unsigned int> > routeTable;
   //std::vector <unsigned int> routeTableNodesCount;
   //unsigned int routeTableLineQuantity;
   //unsigned int routeTableColumnQuantity;
   unsigned int nodeQuantity;
   unsigned int maximumColumnQuantity;

private:
   static std::vector < std::vector <unsigned int> > * loadRoutes(const char * fileName, unsigned int  & maximumColumnQuantity);
};

}

}

#endif /* ALLROUTESDISCOVER_H_ */
