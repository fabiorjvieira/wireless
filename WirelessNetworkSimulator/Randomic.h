#ifndef RANDOMIC_H_
#define RANDOMIC_H_

#include <stdlib.h>
#include <fstream>
#include <vector>

namespace Support
{

namespace OperationalSystem
{

namespace Linux
{



class Randomic
{
public:
   inline static const unsigned int getSeed()
   {
      unsigned int seed = 0;
      char * seedStream = new char[sizeof(seed)];

      std::ifstream urandomFile;
      urandomFile.open("/dev/urandom", std::ios::binary);
      urandomFile.read(seedStream, sizeof(seed));
      for (unsigned int element = 0; element < sizeof(seed); element++)
      {
         seed += ((unsigned int) seedStream[element]) << ((sizeof(seed) - element -1) *8);
      }

      delete [] seedStream;
      return seed;
   }

   inline static void setSeed()
   {
      srand(Support::OperationalSystem::Linux::Randomic::getSeed());
   }

   inline static const unsigned int getRandomic(const int maximumNotIncluded)
   {
      return rand() % maximumNotIncluded;
   }

   inline static const unsigned long getRandomic(const unsigned long maximumNotIncluded)
   {
      return rand() % maximumNotIncluded;
   }

   inline static const long double getRandomic(const long double maximumIncluded)
   {
      return (maximumIncluded * ((long double) rand() / RAND_MAX));
   }


   inline static const unsigned long randomicFromList(std::vector <unsigned int> * listOfIdentifiers)
   {
      unsigned long element, value;
      element = Support::OperationalSystem::Linux::Randomic::getRandomic((unsigned long) listOfIdentifiers->size());
      value = listOfIdentifiers->at(element);
      listOfIdentifiers->erase(listOfIdentifiers->begin() + element);
      return value;
   }

   inline static const void listOfIdentifiers(unsigned int size, std::vector <unsigned int> * & list)
   {
      if (list == NULL) list = new std::vector <unsigned int> (size,0);
      else list->resize(size,0);
      for (;size > 0; size--) list->at(size -1) = size -1;
   }

};

}

}

}

#endif /*RANDOMIC_H_*/
