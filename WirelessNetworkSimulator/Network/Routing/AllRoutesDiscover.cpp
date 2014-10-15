/*
 * AllRoutesDiscover.cpp
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#include "AllRoutesDiscover.h"

namespace Networks
{

namespace Routing
{

AllRoutesDiscover::AllRoutesDiscover(unsigned int networkSize)
{
   // TODO Auto-generated constructor stub
   this->networkSize = networkSize;
}

AllRoutesDiscover::~AllRoutesDiscover()
{
   // TODO Auto-generated destructor stub
}

void AllRoutesDiscover::loadRoutes(const char * fileName)
{
   this->loadRoutes(fileName, (unsigned short) 0);
}

void AllRoutesDiscover::loadRoutes(const char * fileName, unsigned short quantityLimit)
{
   std::ifstream file(fileName, std::ios::in);
   char in[Support::MAX_READS];
   char value[Support::MAX_READS];
   unsigned int position, subPosition, columnSize = 0;
   unsigned int routeTableLineQuantity = 0;
   std::vector <unsigned int> nodesCount;
   std::vector <bool> nodePresence(this->networkSize, false);

   this->nodeQuantity = 0;
   this->maximumColumnQuantity = 0;

   if (not file)
   {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }

   /*comments*///computing maximum column size and line quantity
   file.getline(in, Support::MAX_READS);
   while(file.gcount() > 3)
   {
      if (in[0] != Support::COMMENTARY[0])
      {
         position = 0;
         columnSize = 0;
         do
         {
            subPosition = 0;
            do
             value[subPosition++] = in[position++];
            while (in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
            position++;
            subPosition = 0;
            columnSize++;
         }
         while (in[position -1] != Support::END_OF_INFORMATION[0]);
         routeTableLineQuantity++;
         nodesCount.push_back(columnSize);
         if (this->maximumColumnQuantity < columnSize) this->maximumColumnQuantity = columnSize;
      }
      if (not file.eof()) file.getline(in, Support::MAX_READS);
      else break;
   }
   file.close();

   if (quantityLimit > 0) routeTableLineQuantity = quantityLimit;
   /*comments*///Allocation of routes
   this->routeTable.resize(routeTableLineQuantity);
   for (unsigned int line = 0; line < routeTableLineQuantity; line++)
   {
      this->routeTable[line].resize(nodesCount[line]);
      for (unsigned int column = 0; column < nodesCount[line]; column++)
         this->routeTable[line][column] = Support::INVALID_IDENTIFICATION;
   }

   /*comments*///load routes
   file.open(fileName, std::ios::in);
   for (unsigned int line = 0; line < this->routeTable.size(); line++)
   {
      do file.getline(in, Support::MAX_READS);
      while (in[0] == Support::COMMENTARY[0]);

      position = 0;
      columnSize=0;
      do
      {
         subPosition = 0;
         do
          value[subPosition++] = in[position++];
         while (in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
         value[subPosition] = Support::END_OF_LINE[0];
         this->routeTable[line][columnSize] = atoi(value);
         if (not nodePresence[this->routeTable[line][columnSize]])
         {
            nodePresence[this->routeTable[line][columnSize]] = true;
            this->nodeQuantity++;
         }
         columnSize++;
         position++;
         subPosition = 0;
      }
      while (in[position -1] != Support::END_OF_INFORMATION[0]);
   }
   file.close();
}

void AllRoutesDiscover::printTable(std::vector < std::vector <unsigned int> > * routeTable)
{
   std::cout << std::endl << Support::COMMENTARY << "Table" << std::endl;
   for (unsigned int line = 0; line < routeTable->size(); line++)
   {
      for (unsigned int column = 0; column < routeTable->at(line).size(); column++)
         std::cout << (*routeTable)[line][column] << Support::SEPARATOR;
      std::cout << std::endl;
   }
   std::cout << std::endl;
}

void AllRoutesDiscover::nRoutesThroughEachNeighbor(Networks::Node ** nodes, const char * baseFileName, unsigned int limit)
{
   unsigned int limitCounter;
   std::string fileName = baseFileName;
   std::stringstream MyStream;

   if (limit == Support::INFINIT_LIMIT) fileName.append(Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX);
   else
   {
      MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
      fileName.append(Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX);
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
   }
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file <<  Support::COMMENTARY << limit << " routes per neighbor" << std::endl;

   unsigned int neighbor, lineAuxiliar = 0, routeTableAuxiliarLineQuantity = pathCombination(this->networkSize) + this->routeTable.size() /** Support::MAXIMUM_ROUTES_FACTOR*/;
   bool loop;
   std::vector < std::vector <unsigned int>  > routeTableAuxiliar(routeTableAuxiliarLineQuantity);
   for (unsigned int line = 0; line < routeTableAuxiliarLineQuantity; line++)
   {
      /*comment*///3 ˆ cause des origine, destin et Support::INVALID_IDENTIFICATION
      routeTableAuxiliar[line].resize(3);
      routeTableAuxiliar[line][0] = Support::INVALID_IDENTIFICATION;
   }

   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      for (unsigned int destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         if (nodes[origin]->isNeighbor(destiny))
         {
            //routeTableAuxiliar[lineAuxiliar].resize(1 +2);
            routeTableAuxiliar[lineAuxiliar][0] = origin;
            routeTableAuxiliar[lineAuxiliar][1] = destiny;
            routeTableAuxiliar[lineAuxiliar++][2] = Support::INVALID_IDENTIFICATION;
            if (lineAuxiliar >=  routeTableAuxiliarLineQuantity) routeTableAuxiliar.resize(routeTableAuxiliar.size() *Support::REALLOCATION_FACTOR);
            continue;
         }
         for (unsigned int neighborMember = 0;neighborMember < nodes[origin]->getNeighborsQuantity(); neighborMember++)
         {
            limitCounter = limit;
            neighbor = nodes[origin]->getNeighbors(neighborMember);
            if (neighbor < destiny)
            {
               for (unsigned int line = 0; line < this->routeTable.size() and limitCounter > 0; line++)
               {
                  if (this->routeTable[line][0] == neighbor and
                        this->routeTable[line].back() == destiny and
                        this->routeTable[line][1] != origin)
                  {
                     loop = false;
                     /*comment*///+2 ˆ cause des destin et Support::INVALID_IDENTIFICATION
                     routeTableAuxiliar[lineAuxiliar].resize(this->routeTable[line].size() +2);
                     routeTableAuxiliar[lineAuxiliar][0] = origin;
                     routeTableAuxiliar[lineAuxiliar][1] = neighbor;
                     for (unsigned int column = 1; (not loop) and column < this->routeTable[line].size(); column++)
                     {
                        loop = nodes[origin]->isNeighbor(this->routeTable[line][column]) or (origin == this->routeTable[line][column]);
                        routeTableAuxiliar[lineAuxiliar][column +1] = this->routeTable[line][column];
                     }
                     if (not loop)
                     {
                        routeTableAuxiliar[lineAuxiliar++].back() = Support::INVALID_IDENTIFICATION;
                        if (lineAuxiliar >=  routeTableAuxiliarLineQuantity) routeTableAuxiliar.resize(routeTableAuxiliar.size() *Support::REALLOCATION_FACTOR);
                        limitCounter--;
                     }
                  }
               }
            }
            else if (neighbor > destiny)
            {
               for (unsigned int line = 0; line < this->routeTable.size() and limitCounter > 0; line++)
               {
                  if (this->routeTable[line][0] == destiny and
                        this->routeTable[line].back() == neighbor and
                        this->routeTable[line][this->routeTable[line].size() -2] != origin)
                  {
                     loop = false;
                     /*comment*///+2 ˆ cause des destin et Support::INVALID_IDENTIFICATION
                     routeTableAuxiliar[lineAuxiliar].resize(this->routeTable[line].size() +2);
                     routeTableAuxiliar[lineAuxiliar][0] = origin;
                     routeTableAuxiliar[lineAuxiliar][1] = neighbor;
                     for (long int column = this->routeTable[line].size() -2; (not loop) and column >= 0; column--)
                     {
                        loop = nodes[origin]->isNeighbor(this->routeTable[line][column]) or (origin == this->routeTable[line][column]);
                        routeTableAuxiliar[lineAuxiliar][this->routeTable[line].size() - column] = this->routeTable[line][column];
                     }
                     if (not loop)
                     {
                        routeTableAuxiliar[lineAuxiliar++].back() = Support::INVALID_IDENTIFICATION;
                        if (lineAuxiliar >=  routeTableAuxiliarLineQuantity) routeTableAuxiliar.resize(routeTableAuxiliar.size() *Support::REALLOCATION_FACTOR);
                        limitCounter--;
                     }
                  }
               }
            }
         }
      }
      for (unsigned int line = 0; line < lineAuxiliar; line++)
      {
         file << routeTableAuxiliar[line][0];
         for (unsigned int column = 1; routeTableAuxiliar[line][column] != Support::INVALID_IDENTIFICATION; column++)
            file << Support::SEPARATOR << routeTableAuxiliar[line][column];
         file << Support::END_OF_INFORMATION << std::endl;
      }
      lineAuxiliar = 0;
   }


   file.close();
}

void AllRoutesDiscover::nDijkstraRoutesDiscover(Networks::Node ** nodes, const char * baseFileName)
{
   std::string fileName;
   fileName.append(baseFileName);
   fileName.append(Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }

   unsigned int * visited = new unsigned int [this->networkSize];
   std::vector <unsigned int *> * routeTable =  new std::vector <unsigned int *> ();
   routeTable->reserve(this->networkSize);
   unsigned int * route, neighbor, waveBegin, waveEnd, waveLength;

   file << Support::COMMENTARY << "Minimum paths discovery" << std::endl;
   for (unsigned int origin = 0; origin < this->networkSize; origin++)
   {
      waveBegin = 0;
      waveEnd = 0;
      route = new unsigned int [this->networkSize +1];
      route[0] = origin;
      route[1] = Support::INVALID_IDENTIFICATION;
      for (unsigned int visitedMember = 0; visitedMember < this->networkSize; visitedMember++) visited[visitedMember] = 0;
      visited[origin] = -1;
      waveLength = 1;
      routeTable->push_back(route);
      do
      {
         do
         {
            for (unsigned int neighborMember = 0; neighborMember < nodes[routeTable->at(waveBegin)[waveLength -1]]->getNeighborsQuantity(); neighborMember++)
            {
               neighbor = nodes[routeTable->at(waveBegin)[waveLength -1]]->getNeighbors(neighborMember);
               if (visited[neighbor] == 0 or visited[neighbor] == waveLength)
               {
                  visited[neighbor] = waveLength;
                  route = new unsigned int [this->networkSize +1];
                  for (unsigned int pathMember = 0; pathMember < waveLength; pathMember++)
                     route[pathMember] = routeTable->at(waveBegin)[pathMember];
                  route[waveLength] = neighbor;
                  route[waveLength +1] = Support::INVALID_IDENTIFICATION;
                  routeTable->push_back(route);

                  file << route[0];
                  for (unsigned int routeMember = 1; route[routeMember] != Support::INVALID_IDENTIFICATION; routeMember++)
                     file << Support::SEPARATOR << route[routeMember];
                  file << Support::END_OF_INFORMATION << std::endl;
               }
            }
            delete [] routeTable->at(waveBegin);
            waveBegin++;
         }
         while (waveBegin <= waveEnd);
         if (waveBegin == routeTable->size()) break;
         else
         {
            waveLength++;
            waveEnd = routeTable->size() -1;
         }
      }
      while (true);

      //for (unsigned int routeTableMember = 0; routeTableMember < routeTable->size(); routeTableMember++)
         //delete [] routeTable->at(routeTableMember);
      routeTable->clear();
   }

   file.close();
   delete [] visited;
   delete routeTable;
}

void AllRoutesDiscover::allRoutesDiscover(Networks::Node ** nodes, char * baseFileName)
{
   std::string fileName = baseFileName;
   fileName.append(Support::ALL_ROUTE_TABLE_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }

   std::vector <unsigned int *> * routeTable =  new std::vector <unsigned int *> ();
   std::vector <bool *> * visitedTable =  new std::vector <bool *> ();
   routeTable->reserve(this->networkSize);
   visitedTable->reserve(this->networkSize);
   unsigned int * route, neighbor, waveBegin, waveEnd, waveLength;
   bool * visited;

   file << Support::COMMENTARY << "All paths discovery" << std::endl;
   for (unsigned int origin = 0; origin < this->networkSize; origin++)
   {
      waveBegin = 0;
      waveEnd = 0;
      waveLength = 1;

      visited = new bool [this->networkSize +1];
      for (unsigned int visitedMember = 0; visitedMember < this->networkSize; visitedMember++) visited[visitedMember] = false;
      visited[origin] = true;
      visitedTable->push_back(visited);

      route = new unsigned int [this->networkSize +1];
      route[0] = origin;
      route[1] = Support::INVALID_IDENTIFICATION;
      routeTable->push_back(route);
      do
      {
         do
         {
            for (unsigned int neighborMember = 0; neighborMember < nodes[routeTable->at(waveBegin)[waveLength -1]]->getNeighborsQuantity(); neighborMember++)
            {
               neighbor = nodes[routeTable->at(waveBegin)[waveLength -1]]->getNeighbors(neighborMember);
               if (visitedTable->at(waveBegin)[neighbor] == false)
               {
                  visited = new bool [this->networkSize +1];
                  for (unsigned int visitedMember = 0; visitedMember < this->networkSize; visitedMember++) visited[visitedMember] = false;
                  route = new unsigned int [this->networkSize +1];
                  for (unsigned int pathMember = 0; pathMember < waveLength; pathMember++)
                  {
                     visited[routeTable->at(waveBegin)[pathMember]] = true;
                     route[pathMember] = routeTable->at(waveBegin)[pathMember];
                  }
                  visited[neighbor] = true;
                  visitedTable->push_back(visited);

                  route[waveLength] = neighbor;
                  route[waveLength +1] = Support::INVALID_IDENTIFICATION;
                  routeTable->push_back(route);

                  file << route[0];
                  for (unsigned int routeMember = 1; route[routeMember] != Support::INVALID_IDENTIFICATION; routeMember++)
                     file << Support::SEPARATOR << route[routeMember];
                  file << Support::END_OF_INFORMATION << std::endl;
               }
            }
            waveBegin++;
         }
         while (waveBegin <= waveEnd);
         if (waveBegin == routeTable->size()) break;
         else
         {
            waveLength++;
            waveEnd = routeTable->size() -1;
         }
      }
      while (true);
      routeTable->clear();
      visitedTable->clear();
   }

   delete visitedTable;
   delete routeTable;
   file.close();
}

void AllRoutesDiscover::minimumRouteDiscover(const char * baseFileName)
{
   std::string fileName = baseFileName;
   fileName.append(Support::MINIMUM_ROUTE_TABLE_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }

   unsigned int ** minimumRouteTableLine = new unsigned int * [this->networkSize];
   unsigned int ** minimumRouteTableLineHopCount = new unsigned int * [this->networkSize];
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION;
   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      minimumRouteTableLineHopCount[line] = new unsigned int [this->networkSize];
      minimumRouteTableLine[line] = new unsigned int [this->networkSize];
      for (unsigned int column = 0; column < this->networkSize; column++)
      {
         minimumRouteTableLineHopCount[line][column] = Support::INVALID_IDENTIFICATION;
         minimumRouteTableLine[line][column] = Support::INVALID_IDENTIFICATION;
      }
   }

   for (unsigned int line = 0; line < this->routeTable.size(); line++)
   {
      origin = this->routeTable[line][0];
      destiny = this->routeTable[line].back();
      if (this->routeTable[line].size() < minimumRouteTableLineHopCount[origin][destiny])
      {
         minimumRouteTableLineHopCount[origin][destiny] = this->routeTable[line].size();
         minimumRouteTableLine[origin][destiny] = line;
      }
   }

   file << Support::COMMENTARY << "Minimum routes" << std::endl;
   for (unsigned originNode = 0; originNode < this->networkSize; originNode++)
   {
      for (unsigned destinyNode = originNode +1; destinyNode < this->networkSize; destinyNode++)
      {
//         if (destinyNode != originNode)
         {
            file << this->routeTable[minimumRouteTableLine[originNode][destinyNode]][0];
            for (unsigned column = 1; column < this->routeTable[minimumRouteTableLine[originNode][destinyNode]].size(); column++)
               file << Support::SEPARATOR << this->routeTable[minimumRouteTableLine[originNode][destinyNode]][column];
            file << Support::END_OF_INFORMATION << std::endl;
         }
      }
   }

   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      delete [] minimumRouteTableLineHopCount[line];
      delete [] minimumRouteTableLine[line];
   }
   delete [] minimumRouteTableLine;
   delete [] minimumRouteTableLineHopCount;
   file.close();
}

void AllRoutesDiscover::nNeighborsRoutes(const char * baseFileName, unsigned int limit)
{
   std::string fileName = baseFileName;
   std::stringstream MyStream;

   MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
   fileName.append(Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX);
   fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }

   std::vector <Networks::Routing::RouteTableLine> bestsRouteTable(this->routeTable.size());
   for (unsigned int line = 0; line < this->routeTable.size(); line++)
      bestsRouteTable[line].line = &this->routeTable[line];

   std::sort(bestsRouteTable.begin(), bestsRouteTable.end());

   for (unsigned line = 0; line < limit and line < this->routeTable.size(); line++)
   {
      file << bestsRouteTable[line].line->at(0);
      for (unsigned column = 1; column < bestsRouteTable[line].line->size(); column++)
         file << Support::SEPARATOR << bestsRouteTable[line].line->at(column);
      file << Support::END_OF_INFORMATION << std::endl;
   }

   file.close();
}

void AllRoutesDiscover::allnNeighborsRoutes(const char * baseFileName, unsigned int limit)
{
   std::string fileName;
   std::stringstream MyStream;
   Networks::Routing::AllRoutesDiscover * router;

   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      for (unsigned int destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         MyStream.str("");
         fileName = baseFileName;
         MyStream << Support::FILE_NAME_SEPARATOR << origin << Support::FILE_NAME_SEPARATOR << destiny;
         fileName.append(MyStream.str());
         fileName.append(Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX);

         router = new Networks::Routing::AllRoutesDiscover(this->networkSize);
         router->loadRoutes(fileName.data());

         MyStream.str("");
         fileName = baseFileName;
         MyStream << Support::FILE_NAME_SEPARATOR << origin << Support::FILE_NAME_SEPARATOR << destiny;
         fileName.append(MyStream.str());
         router->nNeighborsRoutes(fileName.data(), limit);
         delete router;
      }
   }
}

/*comment*///so funciona no mac os, linux tem limitacao de arquivos abertos
void AllRoutesDiscover::routesOrganizerFile(const char * baseFileName, const char * sufixFileName)
{
   std::string fileName;
   std::stringstream MyStream;
   std::ofstream ** file = new std::ofstream * [this->networkSize];
   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      file[origin] = new std::ofstream[this->networkSize];
      for (unsigned int destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         MyStream.str("");
         fileName = baseFileName;
         MyStream << Support::FILE_NAME_SEPARATOR << origin << Support::FILE_NAME_SEPARATOR << destiny;
         fileName.append(MyStream.str());
         fileName.append(sufixFileName);
         file[origin][destiny].open(fileName.data(), std::ios::out);
         std::cout << "Creating:" << fileName << std::endl;
         if (not file[origin][destiny])
         {
            std::cerr << fileName << " not created!" << std::endl;
            exit(1);
         }
      }
   }

   for (unsigned line = 0; line < this->routeTable.size(); line++)
   {
      if (this->routeTable[line][0] < this->routeTable[line].back())
      {
         file[this->routeTable[line][0]][this->routeTable[line].back()] << this->routeTable[line][0];
         for (unsigned column = 1; column < this->routeTable[line].size(); column++)
            file[this->routeTable[line][0]][this->routeTable[line].back()] << Support::SEPARATOR << this->routeTable[line][column];
         file[this->routeTable[line][0]][this->routeTable[line].back()] << Support::END_OF_INFORMATION << std::endl;
      }
   }

   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      for (unsigned int destiny = origin +1; destiny < this->networkSize; destiny++)
         file[origin][destiny].close();
      delete [] file[origin];
   }
   delete [] file;
}

void AllRoutesDiscover::routesOrganizer(const char * baseFileName, const char * sufixFileName, unsigned int limit)
{
   std::string fileName;
   std::stringstream MyStream;
   std::ofstream file;
   std::vector < std::vector < std::string > > routes(this->networkSize);
   std::vector < std::vector < unsigned int > > routesQuantity(this->networkSize);

   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      routes[origin].resize(this->networkSize);
      routesQuantity[origin].resize(this->networkSize,0);
   }

   for (unsigned line = 0; line < this->routeTable.size(); line++)
   {
      if (this->routeTable[line][0] < this->routeTable[line].back())
      {
         if (routesQuantity[this->routeTable[line][0]][this->routeTable[line].back()] < limit)
            routesQuantity[this->routeTable[line][0]][this->routeTable[line].back()]++;
         else continue;
         MyStream.str(""); MyStream << this->routeTable[line][0];
         routes[this->routeTable[line][0]][this->routeTable[line].back()].append(MyStream.str());
         for (unsigned column = 1; column < this->routeTable[line].size(); column++)
         {
            routes[this->routeTable[line][0]][this->routeTable[line].back()].append(Support::SEPARATOR);
            MyStream.str(""); MyStream << this->routeTable[line][column];
            routes[this->routeTable[line][0]][this->routeTable[line].back()].append(MyStream.str());
         }
         routes[this->routeTable[line][0]][this->routeTable[line].back()].append(Support::END_OF_INFORMATION);
         MyStream.str(""); MyStream << std::endl;
         routes[this->routeTable[line][0]][this->routeTable[line].back()].append(MyStream.str());
      }
   }

   for (unsigned int origin = 0; origin < this->networkSize -1; origin++)
   {
      for (unsigned int destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         MyStream.str("");
         fileName = baseFileName;
         MyStream << Support::FILE_NAME_SEPARATOR << origin << Support::FILE_NAME_SEPARATOR << destiny;
         fileName.append(MyStream.str());
         fileName.append(sufixFileName);
         file.open(fileName.data(), std::ios::out);
         std::cout << "Creating:" << fileName << std::endl;
         if (not file)
         {
            std::cerr << fileName << " not created!" << std::endl;
            exit(1);
         }
         file << routes[origin][destiny].data();
         file.close();
      }
   }
}

void AllRoutesDiscover::saveRoutes(const char * fileName, std::vector < std::vector <unsigned int> > * routeTable)
{
   std::ofstream file(fileName, std::ios::out);

   for (unsigned line = 0; line < routeTable->size(); line++)
   {
      if ((*routeTable)[line][0] < (*routeTable)[line].back())
      {
         file << (*routeTable)[line][0];
         for (unsigned column = 1; column < (*routeTable)[line].size(); column++)
            file << Support::SEPARATOR << (*routeTable)[line][column];
         file << Support::END_OF_INFORMATION << std::endl;
      }
   }
}

std::vector < std::vector <unsigned int> > * AllRoutesDiscover::loadRoutes(const char * fileName, unsigned int  & maximumColumnQuantity)
{
   std::vector < std::vector <unsigned int> > * routeTable = new std::vector < std::vector <unsigned int> >;
   std::vector <unsigned int> nodesCount;
   std::ifstream file(fileName, std::ios::in);
   char in[Support::MAX_READS];
   char value[Support::MAX_READS];
   unsigned int position, subPosition, columnSize = 0, lineQuantity = 0;

   if (not file)
   {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }

   /*comments*///computing maximum column size and line quantity
   file.getline(in, Support::MAX_READS);
   while(file.gcount() > 3)
   {
      if (in[0] != Support::COMMENTARY[0])
      {
         position = 0;
         columnSize = 0;
         do
         {
            subPosition = 0;
            do
             value[subPosition++] = in[position++];
            while (in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
            position++;
            subPosition = 0;
            columnSize++;
         }
         while (in[position -1] != Support::END_OF_INFORMATION[0]);
         lineQuantity++;
         nodesCount.push_back(columnSize);
         if (maximumColumnQuantity < columnSize) maximumColumnQuantity = columnSize;
      }
      if (not file.eof()) file.getline(in, Support::MAX_READS);
      else break;
   }
   file.close();

   /*comments*///Allocation of routes
   routeTable->resize(lineQuantity);
   for (unsigned int line = 0; line < lineQuantity; line++)
   {
      (*routeTable)[line].resize(nodesCount[line]);
      for (unsigned int column = 0; column < nodesCount[line]; column++)
         (*routeTable)[line][column] = Support::INVALID_IDENTIFICATION;
   }

   /*comments*///load routes
   file.open(fileName, std::ios::in);
   for (unsigned int line = 0; line < lineQuantity; line++)
   {
      do file.getline(in, Support::MAX_READS);
      while (in[0] == Support::COMMENTARY[0]);

      position = 0;
      columnSize=0;
      do
      {
         subPosition = 0;
         do
          value[subPosition++] = in[position++];
         while (in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
         value[subPosition] = Support::END_OF_LINE[0];
         (*routeTable)[line][columnSize] = atoi(value);
         columnSize++;
         position++;
         subPosition = 0;
      }
      while (in[position -1] != Support::END_OF_INFORMATION[0]);
   }
   file.close();

   return routeTable;
}

void AllRoutesDiscover::routeSort(std::vector < std::vector <unsigned int> > * routeTable, bool order)
{
   std::vector <Networks::Routing::RouteTableLine> sortedRouteTable(routeTable->size());
   std::vector < std::vector <unsigned int> > table;
   table = *routeTable;

   for (unsigned int line = 0; line < sortedRouteTable.size(); line++)
      sortedRouteTable[line].line = &table[line];

   if (order == Support::ASCENDING)
      std::sort(sortedRouteTable.begin(), sortedRouteTable.end());
   else std::sort(sortedRouteTable.rbegin(), sortedRouteTable.rend());

   for (unsigned int line = 0; line < sortedRouteTable.size(); line++)
      routeTable->at(line) = *(sortedRouteTable[line].line);
}

}

}

