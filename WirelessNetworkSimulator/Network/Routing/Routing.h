/*
 * Routing.h
 *
 *  Created on: Sep 23, 2008
 *      Author: fjimenez
 */

#ifndef ROUTING_H_
#define ROUTING_H_

#include "../../Support.hpp"

namespace Networks
{

namespace Routing
{

#define pathCombination(networkSize) (networkSize)*((networkSize)-1)/2

inline unsigned int pathPosition(unsigned int origin, unsigned int destiny, unsigned int size)
{
   unsigned int smaller, bigger;

   if (origin < destiny)
   {
      smaller = origin;
      bigger = destiny;
   }
   else
   {
      smaller = destiny;
      bigger = origin;
   }

   return (bigger - 1) + ((smaller * ((2*size) - smaller - 3)) / 2);
// unsigned int smaller, bigger, result = 0;
//   if (origin < destiny)
//   {
//      smaller = origin;
//      bigger = destiny;
//   }
//   else
//   {
//      smaller = destiny;
//      bigger = origin;
//   }
//
//   for (unsigned int counter = 0; counter < smaller; counter++)
//      result += size - counter -1;
//
//   return result + bigger - smaller -1;
}

struct NextHop
{
   unsigned int * hopCount, lastHopCount, * routeTableLine;
};

struct BucketNextHop
{
   NextHop * nextHops;
   unsigned int lastNextHops;
   unsigned int * nextHopsQueue;
};

struct RouteTable
{
   unsigned int ** table;
   unsigned int * hopQuantity;
   unsigned int columnQuantity, lineQuantity;
   bool * linesUsed;
};

class Path
{
public:
   Path(unsigned int * path, unsigned int hopCount, double distance)
   {
      this->hopCount = hopCount;
      this->distance = distance;
      this->path = new unsigned int[this->hopCount];
      this->next = NULL;
      this->previous = NULL;
      for (unsigned int hop = 0; hop < this->hopCount; hop++)
      {
         this->path[hop] = path[hop];
      }
   };
   virtual ~Path()
   {
      delete [] this->path;
   };
   Path * next;
   Path * previous;
   unsigned int * path;
   unsigned int hopCount;
   double distance;
};

class Route
{
public:
   Path * first;

   Route()
   {
      this->first = NULL;
   };
   virtual ~Route()
   {
      Path * current;
      current = this->first->next;
      while (current != NULL)
      {
         delete this->first;
         current = this->first->next;
      }
   };

   Path * getPaths();
   void addPath(unsigned int * path, unsigned int hopCount, double distance)
   {
     Path * newPath = new Path(path, hopCount, distance);
     if (this->first != NULL)
     {
        this->first->previous = newPath;
        newPath->next = this->first;
        this->first = newPath;
     }
     else this->first = newPath;
   };
};

class RouteTableLine
{
public:
   std::vector <unsigned int> * line;
   RouteTableLine (std::vector <unsigned int> * line)
   {
      this->line = line;
   }
   RouteTableLine()
   {
      this->line = NULL;
   }
   bool operator < (const Routing::RouteTableLine & route) const
   {
      bool result;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() -1 and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = this->line->at(hop) < route.line->at(hop);
      }
      else result = this->line->size() < route.line->size();

      return result;
   }
   bool operator > (const Routing::RouteTableLine & route) const
   {
      bool result;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() -1 and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = this->line->at(hop) > route.line->at(hop);
      }
      else result = this->line->size() > route.line->size();

      return result;
   }
   bool operator == (const Routing::RouteTableLine & route) const
   {
      bool result = false;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = hop == route.line->size();
      }

      return result;
   }
   bool operator <= (const Routing::RouteTableLine & route) const
   {
      bool result;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() -1 and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = this->line->at(hop) <= route.line->at(hop);
      }
      else result = this->line->size() <= route.line->size();

      return result;
   }
   bool operator >= (const Routing::RouteTableLine & route) const
   {
      bool result;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() -1 and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = this->line->at(hop) >= route.line->at(hop);
      }
      else result = this->line->size() >= route.line->size();

      return result;
   }
   bool operator != (const Routing::RouteTableLine & route) const
   {
      bool result = true;
      unsigned int hop;

      if (this->line->size() == route.line->size())
      {
         for (hop = 0; hop < this->line->size() and this->line->at(hop) == route.line->at(hop); hop++) {}
         result = hop != route.line->size();
      }

      return result;
   }
   Routing::RouteTableLine & operator = (const Routing::RouteTableLine & route) {this->line = route.line; return *this;}
};

}

}

#endif /* ROUTING_H_ */
