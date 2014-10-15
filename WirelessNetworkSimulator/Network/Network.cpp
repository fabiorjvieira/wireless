/*
 * Network.cpp
 *
 *  Created on: Feb 6, 2009
 *      Author: fjimenez
 */

#include "Network.h"

namespace Networks
{

Network::Network(Networks::Topology * topology) {
   // TODO Auto-generated constructor stub
   this->nodes = NULL;
   this->topology = topology;
   this->networkSize = this->topology->getNetworkSize();
}

Network::~Network() {
   // TODO Auto-generated destructor stub
   if (this->nodes != NULL)
   {
      for (unsigned int nodeElement = 0; nodeElement < this->networkSize; nodeElement++)
      {
         delete this->nodes[nodeElement];
      }
      delete [] this->nodes;
   }

   delete this->topology;
}

Networks::Node ** Network::loadNetwork(const char * fileName, double minimumDistance)
{
   this->nodes = new Networks::Node * [networkSize];
   Networks::SpacialPosition spacialPosition;
   unsigned int identification = 0;
   int position, subPosition;
   char value[Support::MAX_READS];
   char in[Support::MAX_READS];
   std::ifstream file(fileName, std::ios::in);

   if (not file)
   {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }

   do
   {
      position = 0;
      subPosition = 0;
      file.getline(in, Support::MAX_READS);
      while (in[position++] != '(');
      while (in[position] != ')')
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      if (identification != atoi(value))
      {
         std::cerr << fileName << " format error!" << std::endl;
         exit(1);
      }
      subPosition = 0;
      while (in[position++] != '_');
      position++;
      while (position < file.gcount())
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      spacialPosition.x = atof(value);

      position = 0;
      subPosition = 0;
      file.getline(in, Support::MAX_READS);
      while (in[position++] != '(');
      while (in[position] != ')')
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      if (identification != atoi(value))
      {
         std::cerr << fileName << " format error!" << std::endl;
         exit(1);
      }
      subPosition = 0;
      while (in[position++] != '_');
      position++;
      while (position < file.gcount())
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      spacialPosition.y = atof(value);

      position = 0;
      subPosition = 0;
      file.getline(in, Support::MAX_READS);
      while (in[position++] != '(');
      while (in[position] != ')')
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      if (identification != atoi(value))
      {
         std::cerr << fileName << " format error!" << std::endl;
         exit(1);
      }
      subPosition = 0;
      while (in[position++] != '_');
      position++;
      while (position < file.gcount())
         value[subPosition++] = in[position++];
      value[subPosition] = Support::END_OF_LINE[0];
      spacialPosition.z = atof(value);

      this->nodes[identification] = new Networks::Node(spacialPosition, identification, minimumDistance, networkSize);
      identification++;
   }
   while (identification < networkSize and (not file.eof()) and file.gcount() > 1);

   return this->nodes;
}

}
