/*
 * Performance.cpp
 *
 *  Created on: Jan 25, 2009
 *      Author: fjimenez
 */

#include "Performance.h"

namespace Performances {

Performance::Performance(Networks::Node ** nodes, unsigned int networkSize) {
   // TODO Auto-generated constructor stub
   this->networkSize = networkSize;
   this->nodes = nodes;
   this->init();
}

Performance::~Performance() {
   // TODO Auto-generated destructor stub
   this->clear();
}

void Performance::init()
{
   unsigned int i = 0;

   i = pathCombination(this->networkSize);

   this->toDestinyTransmissions.reserve(i);
   this->transmissionQueue = new unsigned int [i];
   this->fullTransmissionQueue.reserve(i);
   this->transmissionList = new Performances::TransmissionList [i];
   for (unsigned int transmissionMember = 0; transmissionMember < i; transmissionMember++)
   {
      this->transmissionQueue[transmissionMember] = Support::INVALID_IDENTIFICATION;
      //this->fullTransmissionQueue[transmissionMember] = Support::INVALID_IDENTIFICATION;
      this->transmissionList[transmissionMember].order = Support::INVALID_IDENTIFICATION;
      this->transmissionList[transmissionMember].origin = NULL;
      this->transmissionList[transmissionMember].destiny = NULL;
      this->transmissionList[transmissionMember].quantity = 0;
   }
   this->concurrentTransmissions = NULL;
   this->lastTransmission = 0;
   this->lastFullTransmission = 0;
   this->toDestinyTransmissionQuantity = 0;
   this->fromOriginUniqueTransmissionQuantity = 0;
   this->cycleSize = 0;
   this->lastHistory = 0;
   this->historyLimit = 0;

   this->transmissionGraphEdgesQuantity = 0;
   this->transmissionGraphD2EdgesQuantity = 0;
}

void Performance::clear()
{
   this->transmissionNeighborhood.clear();
   this->transmissionNeighbors.clear();
   this->toDestinyTransmissions.clear();
   this->fromOriginTransmissions.clear();
   if (this->transmissionQueue != NULL)
   {
      delete [] this->transmissionQueue;
      this->transmissionQueue = NULL;
   }
   this->fullTransmissionQueue.clear();
   if (this->transmissionList != NULL)
   {
      delete [] this->transmissionList;
      this->transmissionList = NULL;
   }
   if (this->concurrentTransmissions != NULL)
   {
      for (unsigned int concurrentTransmissionsMember = 0; concurrentTransmissionsMember <= this->lastFullTransmission; concurrentTransmissionsMember++)
         delete [] this->concurrentTransmissions[concurrentTransmissionsMember];
      delete [] this->concurrentTransmissions;
      this->concurrentTransmissions = NULL;
   }
}

void Performance::debugSquareMatriz(Performances::squareMatrix * squareMatrix, bool jump = true)
{
   std::cout << "    ";
   for (unsigned int column = squareMatrix->begin; column <= squareMatrix->end; column++)
   {
      if (column < 10)
         std::cout << "00" << column << " ";
      else if (column < 100)
         std::cout << "0" << column << " ";
      else std::cout << column << " ";
   }
   std::cout << std::endl;
   for (unsigned int line = squareMatrix->begin; line <= squareMatrix->end; line++)
   {
      if (line < 10)
         std::cout << "00" << line << " ";
      else if (line < 100)
         std::cout << "0" << line << " ";
      else std::cout << line << " ";
      for (unsigned int column = squareMatrix->begin; column <= squareMatrix->end; column++)
      {
         if (column < line) std::cout << "    ";
         else if (jump and column == line)  std::cout << "    ";
         else if (squareMatrix->matrix[line][column] == Support::NO_EDGE) std::cout << "    ";
         else std::cout << "  " << squareMatrix->matrix[line][column] << " ";
      }
      std::cout << std::endl;
   }
}

void Performance::initialize()
{
   this->historyLimit = this->lastFullTransmission +1;
   this->transmissionGraphEdgesQuantity = 0;
   this->transmissionGraphD2EdgesQuantity = 0;

   this->concurrentTransmissions = new unsigned int * [this->lastFullTransmission +1];
   for (unsigned int concurrentTransmissionsMember = 0; concurrentTransmissionsMember <= this->lastFullTransmission; concurrentTransmissionsMember++)
      this->concurrentTransmissions[concurrentTransmissionsMember] = new unsigned int [this->lastFullTransmission +1];

   /*comment*///initiate concurrence graph with fullQueue elements
   for (unsigned int concurrentTransmissionsMember = 0; concurrentTransmissionsMember <= this->lastFullTransmission; concurrentTransmissionsMember++)
   {
      for (unsigned int concurrentTransmissionsNextMember = concurrentTransmissionsMember +1; concurrentTransmissionsNextMember <= this->lastFullTransmission; concurrentTransmissionsNextMember++)
      {
         if (
               this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].origin == this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].origin or
               this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].origin == this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].destiny or
               this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].destiny == this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].destiny or
               this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].destiny == this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].origin
            )
         {
            this->transmissionGraphEdgesQuantity++;
            this->concurrentTransmissions[concurrentTransmissionsMember][concurrentTransmissionsNextMember] = Support::IN_EDGE;
            this->concurrentTransmissions[concurrentTransmissionsNextMember][concurrentTransmissionsMember] = Support::IN_EDGE;
         }
         /*comment*///distance-two graph
         else if (
                  this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].origin->isNeighbor(this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].origin->getIdentification()) or
                  this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].origin->isNeighbor(this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].destiny->getIdentification()) or
                  this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].destiny->isNeighbor(this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].destiny->getIdentification()) or
                  this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsMember]].destiny->isNeighbor(this->transmissionList[this->fullTransmissionQueue[concurrentTransmissionsNextMember]].origin->getIdentification())
                 )
         {
            this->transmissionGraphD2EdgesQuantity++;
            this->concurrentTransmissions[concurrentTransmissionsMember][concurrentTransmissionsNextMember] = Support::IN_EDGE;
            this->concurrentTransmissions[concurrentTransmissionsNextMember][concurrentTransmissionsMember] = Support::IN_EDGE;
         }
         else
         {
            this->concurrentTransmissions[concurrentTransmissionsNextMember][concurrentTransmissionsMember] = Support::NO_EDGE;
            this->concurrentTransmissions[concurrentTransmissionsMember][concurrentTransmissionsNextMember] = Support::NO_EDGE;
         }
      }
   }

   /*comment*///quem Ž origin se coloca como pre
   for (unsigned int fromOrigin = 0; fromOrigin < this->fromOriginTransmissions.size(); fromOrigin++)
      if (this->fromOriginTransmissions[fromOrigin])
         this->transmissionNeighbors[fromOrigin].push_back(fromOrigin);
   /*comment*///descobre demais pres
   for (unsigned int line = 0; line < this->transmissionNeighborhood.size(); line++)
      for (unsigned int column = 0; column < this->transmissionNeighborhood[line].size() -1; column++)
         this->transmissionNeighbors[this->transmissionNeighborhood[line][column +1]].push_back(this->transmissionNeighborhood[line][column]);
   /*comment*///quem Ž destiny se coloca como post
   for (unsigned int toDestiny = 0; toDestiny < this->toDestinyTransmissions.size(); toDestiny++)
      if (this->toDestinyTransmissions[toDestiny])
         this->transmissionNeighbors[toDestiny].push_back(toDestiny);
   /*comment*///descobre demais posts
   for (unsigned int line = 0; line < this->transmissionNeighborhood.size(); line++)
      for (unsigned int column = 0; column < this->transmissionNeighborhood[line].size() -1; column++)
         this->transmissionNeighbors[this->transmissionNeighborhood[line][column]].push_back(this->transmissionNeighborhood[line][column +1]);

#ifdef PERFORMANCE_DEBUG
   std::cout << "Vizinhos" << std::endl;
   for (unsigned int line = 0; line < this->transmissionNeighbors.size(); line++)
   {
      std::cout << line << "-";
      for (unsigned int column = 0; column < this->transmissionNeighbors[line].size(); column++)
         std::cout << this->transmissionNeighbors[line][column] << ",";
      std::cout << std::endl;
   }
#endif /* PERFORMANCE_DEBUG */

   /*comment*///inicializa a lista do resto dos vizinhos para cada transmissao
   for (unsigned int transmission = 0; transmission <= this->lastFullTransmission; transmission++)
   {
      for (unsigned int neighbor = transmission +1; neighbor <= this->lastFullTransmission; neighbor++)
      {
         if (this->concurrentTransmissions[transmission][neighbor] != Support::NO_EDGE and this->transmissionNeighbors[transmission][0] != neighbor and this->transmissionNeighbors[transmission][1] != neighbor)
         {
            this->transmissionNeighbors[transmission].push_back(neighbor);
            this->transmissionNeighbors[neighbor].push_back(transmission);
         }
      }
   }

#ifdef PERFORMANCE_DEBUG
   std::cout << "Vizinhos" << std::endl;
   for (unsigned int line = 0; line < this->transmissionNeighbors.size(); line++)
   {
      std::cout << line << "-";
      for (unsigned int column = 0; column < this->transmissionNeighbors[line].size(); column++)
         std::cout << this->transmissionNeighbors[line][column] << ",";
      std::cout << std::endl;
   }
   std::cout << std::endl;
#endif /* PERFORMANCE_DEBUG */
}

Performances::squareMatrix * Performance::initializeConcurrenceGraph()
{
   Performances::squareMatrix * concurrentGraph = new Performances::squareMatrix;

   concurrentGraph->begin = 0;
   concurrentGraph->end = this->lastFullTransmission;
   concurrentGraph->size = concurrentGraph->end + 1;
   concurrentGraph->matrix = new unsigned int * [concurrentGraph->size];

   /*comment*///load first history
   for (unsigned int concurrentMember = concurrentGraph->begin; concurrentMember <= concurrentGraph->end; concurrentMember++)
   {
      concurrentGraph->matrix[concurrentMember] = new unsigned int [concurrentGraph->size];
      for (unsigned int concurrentNextMember = concurrentMember +1; concurrentNextMember <= concurrentGraph->end; concurrentNextMember++)
         concurrentGraph->matrix[concurrentMember][concurrentNextMember] = this->concurrentTransmissions[concurrentMember][concurrentNextMember];
   }

   return concurrentGraph;
}

void Performance::loadRoutes(Networks::Routing::AllRoutesDiscover * router, bool enumerationType)
{
   unsigned int identification  = Support::INVALID_IDENTIFICATION;
   this->toDestinyUniqueTransmissionQuantity = 0;
   this->fromOriginUniqueTransmissionQuantity = 0;
   std::vector <unsigned int> toDestinyTransmissionsQueue, fromOriginTransmissionsQueue;
   unsigned int innerCounter, outerCounter;
   unsigned int * column = &outerCounter, * line = &innerCounter;
   unsigned int outerCounterLimit = router->maximumColumnQuantity, innerCounterLimit = router->routeTable.size();
   this->maximumHopCount = router->maximumColumnQuantity;
   this->routesQuantity = router->routeTable.size();

   if (enumerationType == Support::DEPTH)
   {
      line = &outerCounter;
      column = &innerCounter;
      outerCounterLimit = router->routeTable.size();
      innerCounterLimit = router->maximumColumnQuantity;
   }

   /*comment*///identificar os pre, post
   this->transmissionNeighborhood.resize(router->routeTable.size());

   /*comments*///load transmissions
   for (outerCounter = 0; outerCounter < outerCounterLimit; outerCounter++)
   {
      for (innerCounter = 0; innerCounter < innerCounterLimit; innerCounter++)
      {
         if (*column > router->routeTable[*line].size() -2) continue;
         identification = Networks::Routing::pathPosition(router->routeTable[*line][*column], router->routeTable[*line][*column +1], this->networkSize);
         /*debug*///std::cout << "Origin:" << router->routeTable[*line][*column] << " Destiny:" << router->routeTable[*line][*column +1] << " Combine:" << identification << std::endl;
         /*comment*///removing node pars repetitions
         if (this->transmissionList[identification].order == Support::INVALID_IDENTIFICATION)
         {
             this->transmissionList[identification].order = this->lastTransmission;
             this->transmissionList[identification].origin = this->nodes[router->routeTable[*line][*column]];
             this->transmissionList[identification].destiny = this->nodes[router->routeTable[*line][*column +1]];
             this->transmissionList[identification].quantity++;
             this->transmissionQueue[this->lastTransmission] = identification;
             this->lastTransmission++;
             if (*column == router->routeTable[*line].size() -2)
                this->toDestinyUniqueTransmissionQuantity++;
             if (*column == 0)
                this->fromOriginUniqueTransmissionQuantity++;
         }
         else this->transmissionList[identification].quantity++;
         /*comment*///identificar os pre, post
         this->transmissionNeighborhood[*line].push_back(this->lastFullTransmission);
         /*comment*///without removing node pars repetitions
         this->fullTransmissionQueue.push_back(identification);
         if (*column == router->routeTable[*line].size() -2 /*this->transmissionList[identification].destiny->getIdentification() == destiny*/)
         {
            toDestinyTransmissionsQueue.push_back(this->lastFullTransmission);
            /*debug*///std::cout << "Dest Trans:" << this->lastFullTransmission << std::endl;
         }
         if (*column == 0 /*this->transmissionList[identification].origin->getIdentification() == origin*/)
         {
            fromOriginTransmissionsQueue.push_back(this->lastFullTransmission);
            /*debug*///std::cout << "Orig Trans:" << this->lastFullTransmission << std::endl;
         }
         this->lastFullTransmission++;
      }
   }

   this->transmissionNeighbors.resize(this->lastFullTransmission);
   this->toDestinyTransmissions.resize(this->lastFullTransmission, false);
   this->fromOriginTransmissions.resize(this->lastFullTransmission, false);
   for (unsigned int toDestinyTransmissionsQueueElement = 0; toDestinyTransmissionsQueueElement < toDestinyTransmissionsQueue.size(); toDestinyTransmissionsQueueElement++)
      this->toDestinyTransmissions[toDestinyTransmissionsQueue[toDestinyTransmissionsQueueElement]] = true;
   for (unsigned int fromOriginTransmissionsQueueElement = 0; fromOriginTransmissionsQueueElement < fromOriginTransmissionsQueue.size(); fromOriginTransmissionsQueueElement++)
      this->fromOriginTransmissions[fromOriginTransmissionsQueue[fromOriginTransmissionsQueueElement]] = true;

   this->lastTransmission--;
   this->lastFullTransmission--;
}

void Performance::allDynamics(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix, void (Performances::Performance::*dynamicsType)(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity))
{
   Networks::Routing::AllRoutesDiscover * router;
   std::stringstream MyStream;
   std::string fileName = baseFileName;

   fileName.append(baseFileNameSuffix);
   fileName.append(basePerfFileNameSuffix);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   unsigned int ** minimumRouteTableLineHopCount = new unsigned int * [this->networkSize];
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION;
   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      minimumRouteTableLineHopCount[line] = new unsigned int [this->networkSize];
      for (unsigned int column = 0; column < this->networkSize; column++)
         minimumRouteTableLineHopCount[line][column] = Support::INVALID_IDENTIFICATION;
   }

   for (unsigned int line = 0; line < minimumRouteRouter->routeTable.size(); line++)
   {
      origin = minimumRouteRouter->routeTable[line][0];
      destiny = minimumRouteRouter->routeTable[line].back();
      if (minimumRouteRouter->routeTable[line].size() < minimumRouteTableLineHopCount[origin][destiny])
         minimumRouteTableLineHopCount[origin][destiny] = minimumRouteRouter->routeTable[line].size();
   }

   for (origin = 0; origin < this->networkSize -1; origin++)
   {
      for (destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         fileName = baseFileName;
         fileName.append(Support::FILE_NAME_SEPARATOR);
         MyStream.str("");
         MyStream << origin;
         fileName.append((MyStream).str());
         fileName.append(Support::FILE_NAME_SEPARATOR);
         MyStream.str("");
         (MyStream) << destiny;
         fileName.append((MyStream).str());

         fileName.append(baseFileNameSuffix);
         std::cout << "Reading:" << fileName << std::endl;

         router = new Networks::Routing::AllRoutesDiscover(this->networkSize);
         router->loadRoutes(fileName.data());
         Networks::Routing::AllRoutesDiscover::routeSort(&router->routeTable, Support::ROUTES_SORT_ORDER);
         this->loadRoutes(router, Support::ROUTES_ENUMERATION_TYPE);

         /*debug*/std::cout << "Origin:" << origin << " Destiny:" << destiny << std::endl;
         (this->*dynamicsType)(origin, destiny, file, minimumRouteTableLineHopCount[origin][destiny], router->nodeQuantity);

         this->clear();
         this->init();
         delete router;
      }
   }

   for (unsigned int line = 0; line < this->networkSize; line++)
      delete [] minimumRouteTableLineHopCount[line];
   delete [] minimumRouteTableLineHopCount;
   file.close();
}

void Performance::averageGroupedByXYcolumns(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, unsigned char xColumn, unsigned char yColumn, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity))
{
   unsigned int ** table;
   double dividendo, divisor;
   unsigned int line;
   std::vector <unsigned int> performanceFactorQueue;
   std::string fileName;
   std::stringstream MyStream;
   std::vector <Performances::performanceFactor> performanceFactor(this->networkSize*this->networkSize);
   std::vector < std::vector <double> > data(this->networkSize*this->networkSize);
   double standardDerivation, confidence, average;
   unsigned int value;

   //for (unsigned int dataElement = 0; dataElement < data.size(); dataElement++)
      //data[dataElement].reserve(this->networkSize);

   performanceFactorQueue.reserve(this->networkSize);

   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
   {
      performanceFactor.at(performanceFactorElement).factor = 0;
      performanceFactor.at(performanceFactorElement).quantity = 0;
   }

   for (unsigned int network = 1; network <= quantity; network++)
   {
      fileName = baseFileName;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffix);

      std::cout << "Reading:" << fileName << std::endl;
      table = Support::readCSVfile(fileName.data());
      if (table == NULL) exit(1);

      for (line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
         if (xColumn == yColumn) value = table[line][xColumn];
         else value = ((float)table[line][xColumn]/table[line][yColumn]) + 0.5;

         dividendo = table[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY];
         divisor = table[line][Performances::File::Format::CYCLE_SIZE];
         dividendo /= divisor;

         if ((this->*filter)(table[line], 0))
         {
            performanceFactor.at(value).factor += dividendo;
            performanceFactor.at(value).quantity++;
            if (performanceFactor.at(value).quantity == 1) performanceFactorQueue.push_back(value);
            data[value].push_back(dividendo);
         }

         delete [] table[line];
      }
      delete [] table[line];
      delete [] table;
   }

   std::sort(performanceFactorQueue.begin(), performanceFactorQueue.end());

   fileName = baseFileName;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());

   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());
   fileName.append(basePerfFileNameSuffix);
   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << (int)xColumn;
   fileName.append(MyStream.str());
   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << (int)yColumn;
   fileName.append(MyStream.str());
   fileName.append(Support::GRAPHIC_SUFFIX);

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactorQueue.size(); performanceFactorElement++)
   {
      average = performanceFactor.at(performanceFactorQueue.at(performanceFactorElement)).factor / performanceFactor.at(performanceFactorQueue.at(performanceFactorElement)).quantity;
      standardDerivation = Statistic::std(data[performanceFactorQueue.at(performanceFactorElement)], average, data[performanceFactorQueue.at(performanceFactorElement)].size());
      confidence = Statistic::confidence(Support::CONFIDENCE_FACTOR, standardDerivation, data[performanceFactorQueue.at(performanceFactorElement)].size());
      file << performanceFactorQueue.at(performanceFactorElement) << Support::SEPARATOR << average << Support::SEPARATOR << standardDerivation << Support::SEPARATOR << confidence << Support::SEPARATOR << (average + confidence) << Support::SEPARATOR << (average - confidence) << Support::END_OF_INFORMATION << std::endl;
   }
   file.close();
}

void Performance::vizingDynamicsHeuristics(Networks::Routing::AllRoutesDiscover * router)
{
   unsigned int currentCycleSize;
   bool ** adjacencyMatriz = new bool * [this->networkSize];
   for (unsigned int adjacencyMatrizElement = 0; adjacencyMatrizElement < this->networkSize; adjacencyMatrizElement++)
      adjacencyMatriz[adjacencyMatrizElement] = new bool [this->networkSize];
   for (unsigned int line = 0; line < this->networkSize; line++)
      for (unsigned int column = 0; column < this->networkSize; column++)
         adjacencyMatriz[line][column] = false;


   for (unsigned int line = 0; line < router->routeTable.size(); line++)
   {
      for (unsigned int column = 0; column < router->routeTable[line].size(); column++)
      {
         for (unsigned int nextLine = line; nextLine < router->routeTable.size(); nextLine++)
         {
            for (unsigned int nextColumn = column +1; nextColumn < router->routeTable[nextLine].size(); nextColumn++)
            {
               if (this->nodes[router->routeTable[line][column]]->isNeighbor(router->routeTable[nextLine][nextColumn]))
               {
                  adjacencyMatriz[router->routeTable[line][column]][router->routeTable[nextLine][nextColumn]] = true;
                  adjacencyMatriz[router->routeTable[nextLine][nextColumn]][router->routeTable[line][column]] = true;
               }
            }
         }
      }
   }

   this->toDestinyUniqueTransmissionQuantity = 0;
   for (unsigned int column = 0; column < this->networkSize; column++)
      if (adjacencyMatriz[router->routeTable[0].back()][column]) this->toDestinyUniqueTransmissionQuantity++;

   this->cycleSize = 0;
   this->toDestinyTransmissionQuantity = this->toDestinyUniqueTransmissionQuantity;

   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      currentCycleSize = 0;
      for (unsigned int column = 0; column < this->networkSize; column++)
         if (adjacencyMatriz[line][column]) currentCycleSize++;
      delete [] adjacencyMatriz[line];
      if (this->cycleSize < currentCycleSize) this->cycleSize = currentCycleSize;
   }

   this->cycleSize *=2;

   //biggest power number of the graph plus 2
   //fazer a matriz de adjacencias e contar.
   //concertar o graphic para fazer o grafico de qualquer .perf

   delete [] adjacencyMatriz;
}

void Performance::allVIZ(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix)
{
   Networks::Routing::AllRoutesDiscover * router;
   std::stringstream MyStream;
   std::string fileName = baseFileName;

   fileName.append(baseFileNameSuffix);
   fileName.append(basePerfFileNameSuffix);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   unsigned int ** minimumRouteTableLineHopCount = new unsigned int * [this->networkSize];
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION;
   for (unsigned int line = 0; line < this->networkSize; line++)
   {
      minimumRouteTableLineHopCount[line] = new unsigned int [this->networkSize];
      for (unsigned int column = 0; column < this->networkSize; column++)
         minimumRouteTableLineHopCount[line][column] = Support::INVALID_IDENTIFICATION;
   }

   for (unsigned int line = 0; line < minimumRouteRouter->routeTable.size(); line++)
   {
      origin = minimumRouteRouter->routeTable[line][0];
      destiny = minimumRouteRouter->routeTable[line].back();
      if (minimumRouteRouter->routeTable[line].size() < minimumRouteTableLineHopCount[origin][destiny])
         minimumRouteTableLineHopCount[origin][destiny] = minimumRouteRouter->routeTable[line].size();
   }

   for (origin = 0; origin < this->networkSize -1; origin++)
   {
      for (destiny = origin +1; destiny < this->networkSize; destiny++)
      {
         fileName = baseFileName;
         fileName.append(Support::FILE_NAME_SEPARATOR);
         MyStream.str("");
         MyStream << origin;
         fileName.append((MyStream).str());
         fileName.append(Support::FILE_NAME_SEPARATOR);
         MyStream.str("");
         (MyStream) << destiny;
         fileName.append((MyStream).str());

         fileName.append(baseFileNameSuffix);
         std::cout << "Reading:" << fileName << std::endl;

         router = new Networks::Routing::AllRoutesDiscover(this->networkSize);
         router->loadRoutes(fileName.data());

         /*debug*/std::cout << "Origin:" << origin << " Destiny:" << destiny << std::endl;

         this->vizingDynamicsHeuristics(router);

         file << origin << Support::SEPARATOR << destiny << Support::SEPARATOR << minimumRouteTableLineHopCount[origin][destiny] << Support::SEPARATOR << this->cycleSize << Support::SEPARATOR << this->toDestinyTransmissionQuantity << Support::SEPARATOR << this->toDestinyUniqueTransmissionQuantity << Support::END_OF_INFORMATION  << std::endl;
         delete router;
      }
   }

   for (unsigned int line = 0; line < this->networkSize; line++)
      delete [] minimumRouteTableLineHopCount[line];
   delete [] minimumRouteTableLineHopCount;
   file.close();
}

void Performance::activatedDiscover(std::vector <unsigned int> & activatedMembers, Performances::squareMatrix * squareMatrix)
{
   bool activated;
   activatedMembers.clear();
   for (unsigned int concurrentMember = 0; concurrentMember <= squareMatrix->end; concurrentMember++)
   {
      activated = true;
      for (unsigned int concurrentNextMember = concurrentMember; activated and concurrentNextMember <= squareMatrix->end; concurrentNextMember++)
         if (squareMatrix->matrix[concurrentMember][concurrentNextMember] == Support::OUT_EDGE) activated = false;
      for (unsigned int concurrentNextMember = 0; activated and concurrentNextMember < concurrentMember; concurrentNextMember++)
         if (squareMatrix->matrix[concurrentNextMember][concurrentMember] == Support::IN_EDGE) activated = false;
      if (activated) activatedMembers.push_back(concurrentMember);
   }
}

Performances::PartialOrder * Performance::partialOrderDynamics(Performances::PartialOrder * partialOrder, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder))
{
   unsigned int repeatedCycle = 1;
   bool repeated = false;
   unsigned int lastHole = 0, firstHole;
   std::vector <char> holes;

   for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
   {
      partialOrder->layersHistory()[transmission] = partialOrder->at(transmission);
      partialOrder->bufferHistory()[transmission] = partialOrder->getBuffer()[transmission];
      if (lastHole < partialOrder->at(transmission)) lastHole = partialOrder->at(transmission);
   }
   holes.resize(lastHole +1);

#ifdef PERFORMANCE_DEBUG
   std::cout << "Tabela de transmissoes" << std::endl;
   for (unsigned int line = 0; line < this->transmissionNeighborhood.size(); line++)
   {
      std::cout << line << "-";
      for (unsigned int column = 0; column < this->transmissionNeighborhood[line].size(); column++)
         std::cout << this->transmissionNeighborhood[line][column] << ",";
      std::cout << std::endl;
   }
#endif /* PERFORMANCE_DEBUG */

#ifdef PERFORMANCE_DEBUG
   std::cout << "history:" << std::endl;
   for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
      std::cout << transmission << ",";
   std::cout << std::endl;
   for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
      std::cout << partialOrder->at(transmission) << ",";
   std::cout << std::endl;
   for (unsigned bufferMember = 0; bufferMember < partialOrder->size(); bufferMember++)
      std::cout << partialOrder->getBuffer()[bufferMember] << ",";
   std::cout << std::endl;
#endif /* PERFORMANCE_DEBUG */

   /*comment*///counting transmissions to destiny
   this->lastHistory = this->historyLimit;
   this->toDestinyTransmissionQuantity = 0;
   this->cycleSize = 0;
   do
   {
      this->cycleSize++;

      /*comment*///coping history
      this->lastHistory--;
      if (this->lastHistory == 0)
      {
         lastHole = 0;
         for (unsigned int column = 0; column < partialOrder->size(); column++)
         {
            partialOrder->layersHistory()[column] = partialOrder->at(column);
            partialOrder->bufferHistory()[column] = partialOrder->getBuffer()[column];
            if (lastHole < partialOrder->at(column)) lastHole = partialOrder->at(column);
         }
         holes.resize(lastHole +1);
         repeatedCycle++;
         //this->lastHistory = pow(this->historyLimit, repeatedCycle);
         this->lastHistory += this->historyLimit * (repeatedCycle +1);
         /*comment*///reset counters
         this->cycleSize = 1;
         this->toDestinyTransmissionQuantity = 0;
         std::cout << "Reset history! TransSize:" << this->lastFullTransmission << " Exponent:"  << repeatedCycle << " LastHistory:" << this->lastHistory << std::endl;
      }

      /*comment*///reorientacao ou reposicionamento
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         /*comment*///descobrindo os ativos
         if (partialOrder->at(transmission) == 0)
         {
            /*comment*///counting transmissions to destiny
            if (this->toDestinyTransmissions[transmission]) this->toDestinyTransmissionQuantity++;
            (this->*reOrderRule)(transmission, lastHole, firstHole, holes, this->transmissionNeighbors, partialOrder);
         }

      /*comment*///reposicionando as camadas
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         partialOrder->at(transmission)--;

#ifdef PERFORMANCE_DEBUG
      std::cout << "history:" << std::endl;
      for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
         std::cout << transmission << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
         std::cout << partialOrder->at(transmission) << ",";
      std::cout << std::endl;
      for (unsigned bufferMember = 0; bufferMember < partialOrder->size(); bufferMember++)
         std::cout << partialOrder->getBuffer()[bufferMember] << ",";
      std::cout << std::endl;
#endif /* PERFORMANCE_DEBUG */

      /*comment*///verificando se entrou no ciclo
      repeated = true;
      for (unsigned int column = 0; column < partialOrder->size(); column++)
         if (partialOrder->at(column) != partialOrder->layersHistory()[column] or partialOrder->getBuffer()[column] != partialOrder->bufferHistory()[column])
         {
            repeated = false;
            break;
         }
   }
   while (not repeated);

   /*debug*/std::cout << "Arrive:" << this->toDestinyTransmissionQuantity << " Cycle:" << this->cycleSize << std::endl;
   return partialOrder;
}

std::vector < Performances::PartialOrder * > * Performance::captureCycle(Performances::PartialOrder * partialOrder, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder))
{
   unsigned int lastHole = 0, firstHole;
   std::vector <char> holes;
   std::vector < Performances::PartialOrder * > * partialOrderCycle = new std::vector < Performances::PartialOrder * >;

   for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
      if (lastHole < partialOrder->at(transmission)) lastHole = partialOrder->at(transmission);
   holes.resize(lastHole +1);

   partialOrderCycle->push_back(new Performances::PartialOrder(*partialOrder));

   for (unsigned int cycleMember = 1; cycleMember < this->cycleSize; cycleMember++)
   {
      /*comment*///reorientacao ou reposicionamento
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         /*comment*///descobrindo os ativos
         if (partialOrder->at(transmission) == 0) (this->*reOrderRule)(transmission, lastHole, firstHole, holes, this->transmissionNeighbors, partialOrder);
      /*comment*///reposicionando as camadas
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         partialOrder->at(transmission)--;
      partialOrderCycle->push_back(new Performances::PartialOrder(*partialOrder));
   }

   return partialOrderCycle;
}

std::vector < Performances::PartialOrder * > * Performance::sortCycle(Performances::PartialOrder * partialOrder, std::vector < Performances::PartialOrder * > * partialOrderCycle, void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder))
{
   bool repeated = false;
   unsigned int lastHole = 0, firstHole, cycleState;
   std::vector <char> holes;

   for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
      if (lastHole < partialOrder->at(transmission)) lastHole = partialOrder->at(transmission);
   holes.resize(lastHole +1);

   partialOrderCycle->insert(partialOrderCycle->begin(), new Performances::PartialOrder(*partialOrder));

   while (not repeated)
   {
      /*comment*///copiando possivel estado anterior ao ciclo
      delete partialOrderCycle->front();
      partialOrderCycle->erase(partialOrderCycle->begin());
      partialOrderCycle->insert(partialOrderCycle->begin(), new Performances::PartialOrder(*partialOrder));

      /*comment*///reorientacao ou reposicionamento
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         /*comment*///descobrindo os ativos
         if (partialOrder->at(transmission) == 0) (this->*reOrderRule)(transmission, lastHole, firstHole, holes,this->transmissionNeighbors, partialOrder);
      /*comment*///reposicionando as camadas
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         partialOrder->at(transmission)--;

      /*comment*///verificando se entrou no ciclo
      repeated = false;
      for (cycleState = 1; (not repeated) and cycleState < partialOrderCycle->size(); cycleState++)
      {
         repeated = true;
         for (unsigned int column = 0; column < partialOrder->size(); column++)
            if (partialOrder->at(column) != partialOrderCycle->at(cycleState)->at(column) or partialOrder->getBuffer()[column] != partialOrderCycle->at(cycleState)->getBuffer()[column])
            {
               repeated = false;
               break;
            }
      }
      cycleState--;
   }

   /*comment*///reordenando partialOrderCycle
   for (unsigned int cycleMember = 1; cycleMember < cycleState; cycleMember++)
   {
      partialOrderCycle->push_back(partialOrderCycle->at(cycleMember));
      partialOrderCycle->erase(partialOrderCycle->begin() + cycleMember);
   }

   return partialOrderCycle;
}

Performances::PartialOrder * Performance::initializePartialOrder(Performances::squareMatrix * concurrentGraph)
{
   std::vector <unsigned int> activatedElements;
   Performances::PartialOrder * partialOrder = new Performances::PartialOrder(concurrentGraph->size);
   unsigned int activatedCounter = 0, partialOrderColumn = 0;

   /*comment*///inicializacao do buffer
   for (unsigned int concurrentMember = concurrentGraph->begin; concurrentMember < concurrentGraph->size; concurrentMember++)
   {
      concurrentGraph->matrix[concurrentMember][concurrentMember] = Support::NO_EDGE;
      if (this->fromOriginTransmissions[concurrentMember])
      {
         partialOrder->getBuffer()[concurrentMember] = -1;
         partialOrder->bufferHistory()[concurrentMember] = -1;
      }
      else
      {
         partialOrder->getBuffer()[concurrentMember] = 0;
         partialOrder->bufferHistory()[concurrentMember] = 0;
      }
   }
   activatedElements.reserve(concurrentGraph->size);

   /*comment*///load first partial order
   while (activatedCounter <= this->lastFullTransmission)
   {
      this->activatedDiscover(activatedElements, concurrentGraph);
      activatedCounter += activatedElements.size();
      for (unsigned int activatedElement = 0; activatedElement < activatedElements.size(); activatedElement++)
      {
         partialOrder->at(activatedElements[activatedElement]) = partialOrderColumn;
         partialOrder->layersHistory()[activatedElements[activatedElement]] = partialOrderColumn;
         for (unsigned int line = 0; line < activatedElements[activatedElement]; line++)
            concurrentGraph->matrix[line][activatedElements[activatedElement]] = Support::NO_EDGE;
         for (unsigned int column = activatedElements[activatedElement] +1; column <= concurrentGraph->end; column++)
            concurrentGraph->matrix[activatedElements[activatedElement]][column] = Support::NO_EDGE;
         concurrentGraph->matrix[activatedElements[activatedElement]][activatedElements[activatedElement]] = Support::OUT_EDGE;
      }
      partialOrderColumn++;
   }

   return partialOrder;
}

void Performance::simpleDynamics(Performances::PartialOrder * partialOrder)
{
   unsigned int repeatedCycle = 1;
   bool repeated = false;
   unsigned int lastHole = 0, firstHole;
   std::vector <char> holes;

   for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
   {
      partialOrder->layersHistory()[transmission] = partialOrder->at(transmission);
      if (lastHole < partialOrder->at(transmission)) lastHole = partialOrder->at(transmission);
   }
   holes.resize(lastHole +1);

#ifdef PERFORMANCE_DEBUG
   std::cout << "history:" << std::endl;
   for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
      std::cout << transmission << ",";
   std::cout << std::endl;
   for (unsigned transmission = 0; transmission <= this->lastFullTransmission; transmission++)
      std::cout << partialOrder->at(transmission) << ",";
   std::cout << std::endl;
#endif /* PERFORMANCE_DEBUG */

   /*comment*///counting transmissions to destiny
   this->lastHistory = this->historyLimit;
   this->toDestinyTransmissionQuantity = 0;
   this->cycleSize = 0;
   do
   {
      this->cycleSize++;

      /*comment*///coping history
      this->lastHistory--;
      if (this->lastHistory == 0)
      {
         lastHole = 0;
         for (unsigned int column = 0; column < partialOrder->size(); column++)
         {
            partialOrder->layersHistory()[column] = partialOrder->at(column);
            if (lastHole < partialOrder->at(column)) lastHole = partialOrder->at(column);
         }
         holes.resize(lastHole +1);
         repeatedCycle++;
         //this->lastHistory = pow(this->historyLimit, repeatedCycle);
         this->lastHistory += this->historyLimit * (repeatedCycle +1);
         /*comment*///reset counters
         this->cycleSize = 1;
         this->toDestinyTransmissionQuantity = 0;
         std::cout << "Reset history! TransSize:" << this->lastFullTransmission << " Exponent:"  << repeatedCycle << " LastHistory:" << this->lastHistory << std::endl;
      }

      /*comment*///reorientacao ou reposicionamento
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         /*comment*///descobrindo os ativos
         if (partialOrder->at(transmission) == 0)
         {
            /*comment*///counting transmissions to destiny
            if (this->toDestinyTransmissions[transmission]) this->toDestinyTransmissionQuantity++;
            lastHole = 0;
            for (unsigned int layer = 1; layer < holes.size(); layer++) holes[layer] = Support::EMPTY_LAYER;
            for (unsigned int neighbor = 0; neighbor < this->transmissionNeighbors[transmission].size(); neighbor++)
            {
               holes[partialOrder->at(this->transmissionNeighbors[transmission][neighbor])] = Support::NEIGHBOR_IN_LAYER;
               if (lastHole < partialOrder->at(this->transmissionNeighbors[transmission][neighbor]))
                  lastHole = partialOrder->at(this->transmissionNeighbors[transmission][neighbor]);
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

      /*comment*///reposicionando as camadas
      for (unsigned int transmission = 0; transmission < partialOrder->size(); transmission++)
         partialOrder->at(transmission)--;

#ifdef PERFORMANCE_DEBUG
      std::cout << "history:" << std::endl;
      for (unsigned transmission = 0; transmission < partialOrder->size(); transmission++)
         std::cout << transmission << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission < partialOrder->size(); transmission++)
         std::cout << partialOrder->at(transmission) << ",";
      std::cout << std::endl;
#endif /* PERFORMANCE_DEBUG */

      /*comment*///verificando se entrou no ciclo
      repeated = true;
      for (unsigned int column = 0; column < partialOrder->size(); column++)
         if (partialOrder->at(column) != partialOrder->layersHistory()[column])
         {
            repeated = false;
            break;
         }
   }
   while (not repeated);

   /*debug*/std::cout << "Arrive:" << this->toDestinyTransmissionQuantity << " Cycle:" << this->cycleSize << std::endl;
}

void Performance::dominanceTest(const char * difference, const char * baseFileNameA, const char * baseFileNameB, unsigned short quantity, const char * basePerfFileNameSuffixA, const char * basePerfFileNameSuffixB, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity))
{
   unsigned int ** tableA, ** tableB;
   //double averageCycleA = 0, averageCycleB = 0, averageRoutesA = 0, averageRoutesB = 0;
   double match = 0, dominates = 0, beDominated = 0;
   double filteredMatch = 0, filteredDominates = 0, filteredBeDominated = 0;
   unsigned int line = 0;
   std::string fileName;
   std::stringstream MyStream;

//   fileName = Support::FILE_NAME_SEPARATOR;
//   fileName.append(Support::FILE_NAME_SEPARATOR);
//   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
//   fileName.append(baseFileNameA);
//   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
//   MyStream.str("");
//   MyStream << this->networkSize;
//   fileName.append((MyStream).str());
//   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);

   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());
   fileName.append(basePerfFileNameSuffixA);
   fileName.append(basePerfFileNameSuffixB);
   fileName.append(Support::FILE_NAME_SEPARATOR);
   fileName.append(difference);
   fileName.append(Support::DOMINANCE_SUFFIX);

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Dominance (A <= B)" << std::endl;
   file << Support::COMMENTARY << "A:" << baseFileNameA << " " << basePerfFileNameSuffixA << std::endl;
   file << Support::COMMENTARY << "B:" << baseFileNameB << " " << basePerfFileNameSuffixB << std::endl;

   for (unsigned int network = 1; network <= quantity; network++)
   {
      fileName = baseFileNameA;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffixA);

      std::cout << "Reading:" << fileName << std::endl;
      tableA = Support::readCSVfile(fileName.data());
      if (tableA == NULL) exit(1);

      fileName = baseFileNameB;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffixB);

      std::cout << "Reading:" << fileName << std::endl;
      tableB = Support::readCSVfile(fileName.data());
      if (tableB == NULL) exit(1);

      for (line = 0; tableA[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
//         averageRoutesA += tableA[line][Performances::File::Format::TO_DESTINY_UNIQUE_TRANSMISSION_QUANTITY];
//         averageRoutesB += tableB[line][Performances::File::Format::TO_DESTINY_UNIQUE_TRANSMISSION_QUANTITY];
//         averageCycleA += tableA[line][Performances::File::Format::CYCLE_SIZE];
//         averageCycleB += tableB[line][Performances::File::Format::CYCLE_SIZE];
         if ((this->*filter)(tableA[line], 0) and (this->*filter)(tableB[line], 0))
         {
            if ((double) tableA[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableA[line][Performances::File::Format::CYCLE_SIZE] ==
               (double) tableB[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableB[line][Performances::File::Format::CYCLE_SIZE])
               match++;
            else if ((double) tableA[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableA[line][Performances::File::Format::CYCLE_SIZE] <
               (double) tableB[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableB[line][Performances::File::Format::CYCLE_SIZE])
               dominates++;
            else
            {
               beDominated++;
               file << this->networkSize << Support::FILE_NAME_SEPARATOR << network << Support::SEPARATOR << tableA[line][Performances::File::Format::ORIGIN] << Support::SEPARATOR << tableA[line][Performances::File::Format::DESTINY] << Support::SEPARATOR << tableB[line][Performances::File::Format::ORIGIN] << Support::SEPARATOR << tableB[line][Performances::File::Format::DESTINY] << Support::END_OF_INFORMATION << std::endl;
            }
         }
         else
         {
            if ((double) tableA[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableA[line][Performances::File::Format::CYCLE_SIZE] ==
               (double) tableB[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableB[line][Performances::File::Format::CYCLE_SIZE])
               filteredMatch++;
            else if ((double) tableA[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableA[line][Performances::File::Format::CYCLE_SIZE] <
               (double) tableB[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY] / (double) tableB[line][Performances::File::Format::CYCLE_SIZE])
               filteredDominates++;
            else
            {
               filteredBeDominated++;
               file << "Filtered:" << this->networkSize << Support::FILE_NAME_SEPARATOR << network << Support::SEPARATOR << tableA[line][Performances::File::Format::ORIGIN] << Support::SEPARATOR << tableA[line][Performances::File::Format::DESTINY] << Support::SEPARATOR << tableB[line][Performances::File::Format::ORIGIN] << Support::SEPARATOR << tableB[line][Performances::File::Format::DESTINY] << Support::END_OF_INFORMATION << std::endl;
            }
         }
      }
   }

   file << "Total:" << line*quantity << std::endl;
//   file << "A - Media(|ciclo|):" << averageCycleA/(line*quantity) << std::endl;
//   file << "B - Media(|ciclo|):" << averageCycleB/(line*quantity) << std::endl;
//   file << "A - Media(quantidade de rotas):" << averageRoutesA/(line*quantity) << std::endl;
//   file << "B - Media(quantidade de rotas):" << averageRoutesB/(line*quantity) << std::endl;
   file << "A < B:" << dominates << std::endl;
   file << "A = B:" << match << std::endl;
   file << "A > B:" << beDominated << std::endl;
   file << "Filtered:" << line*quantity << std::endl;
   file << "A < B:" << filteredDominates << std::endl;
   file << "A = B:" << filteredMatch << std::endl;
   file << "A > B:" << filteredBeDominated << std::endl;

   file.close();
}

void Performance::histogramXYcolumns(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, unsigned char xColumn, unsigned char yColumn, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity))
{
   unsigned int ** table;
   unsigned int line;
   std::vector <unsigned int> performanceFactorQueue;
   std::string fileName;
   std::stringstream MyStream;
   std::vector <Performances::performanceFactor> performanceFactor(this->networkSize*this->networkSize);
   unsigned int value;

   performanceFactorQueue.reserve(this->networkSize);

   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
   {
      performanceFactor.at(performanceFactorElement).factor = 0;
      performanceFactor.at(performanceFactorElement).quantity = 0;
   }

   for (unsigned int network = 1; network <= quantity; network++)
   {
      fileName = baseFileName;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffix);

      std::cout << "Reading:" << fileName << std::endl;
      table = Support::readCSVfile(fileName.data());
      if (table == NULL) exit(1);

      for (line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
         if ((this->*filter)(table[line], 0))
         {
            if (xColumn == yColumn) value = table[line][xColumn];
            else value = ((float)table[line][xColumn]/table[line][yColumn]) + 0.5;
            performanceFactor.at(value).quantity++;
            if (performanceFactor.at(value).quantity == 1) performanceFactorQueue.push_back(value);
         }

         delete [] table[line];
      }
      delete [] table[line];
      delete [] table;
   }

   std::sort(performanceFactorQueue.begin(), performanceFactorQueue.end());

   fileName = baseFileName;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());

   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());
   fileName.append(basePerfFileNameSuffix);
   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << (int)xColumn;
   fileName.append(MyStream.str());
   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << (int)yColumn;
   fileName.append(MyStream.str());
   fileName.append(Support::HISTOGRAM_SUFFIX);

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   for (unsigned int performanceFactorElement = performanceFactorQueue.size(); performanceFactorElement > 0; performanceFactorElement--)
      file << performanceFactorQueue.at(performanceFactorElement -1) << Support::SEPARATOR << performanceFactor.at(performanceFactorQueue.at(performanceFactorElement -1)).quantity << Support::END_OF_INFORMATION << std::endl;
   file.close();
}

void Performance::search(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix)
{
   unsigned int ** table;
   unsigned int line;
   std::string fileName;
   std::stringstream MyStream;
   double result;

   for (unsigned int network = 1; network <= quantity; network++)
   {
      fileName = baseFileName;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << 80;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << 80;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffix);

      std::cout << "Reading:" << fileName << std::endl;
      table = Support::readCSVfile(fileName.data());
      if (table == NULL)
      {
         std::cout << "Missing:" << fileName << std::endl;
         continue;
      }

      for (line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
         result = (double)table[line][4]/(double)table[line][3];
         if (result < ((double)1/(double)3))
            std::cout << "Found:" << table[line][0] << "->" << table[line][1] << std::endl;
         delete [] table[line];
      }

      delete [] table[line];
      delete [] table;
   }
}

bool Performance::lowFilter(unsigned int * values, unsigned int quantity)
{
   return values[Performances::File::Format::ROUTES_QUANTITY] > 1 and
            values[Performances::File::Format::NODE_QUANTITY] > 5 and
   values[Performances::File::Format::UNIQUE_TRANSMISSION_QUANTITY] > 5
   /*maximum clique < values[Performances::File::Format::TRANSMISSION_QUANTITY]*/
   ;
}

bool Performance::valmirFilter(unsigned int * values, unsigned int quantity)
{
   return values[Performances::File::Format::ROUTE_SIZE] < Support::MAXIMUM_ROUTE_SIZE and
   values[Performances::File::Format::ROUTE_SIZE] > Support::MINIMUM_ROUTE_SIZE and
   this->lowFilter(values, quantity);
}

bool Performance::trueFilter(unsigned int * values, unsigned int quantity)
{
   return true;
}

bool Performance::falseFilter(unsigned int * values, unsigned int quantity)
{
   return false;
}

void Performance::ju(const char * baseFileName, unsigned short quantity, const char * basePerfFileNameSuffix, double maximum,  double minimum, double increment, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity))
{
   bool firstTime;
   unsigned int ** table;
   double dividendo, divisor;
   unsigned int line;
   std::string fileName;
   std::stringstream MyStream;
   std::vector <Performances::performanceFactor> performanceFactor(((maximum - minimum)/increment) +0.5);

   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
   {
      performanceFactor.at(performanceFactorElement).factor = maximum - ((double)performanceFactorElement * increment);
      performanceFactor.at(performanceFactorElement).quantity = 0;
   }
   if (performanceFactor.back().factor != minimum)
   {
      performanceFactor.resize(performanceFactor.size() +1);
      performanceFactor.back().factor = minimum;
      performanceFactor.back().quantity = 0;
   }

   for (unsigned int network = 1; network <= quantity; network++)
   {
      fileName = baseFileName;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffix);

      std::cout << "Reading:" << fileName << std::endl;
      table = Support::readCSVfile(fileName.data());
      if (table == NULL) exit(1);

      for (line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
         dividendo = table[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY];
         divisor = table[line][Performances::File::Format::CYCLE_SIZE];
         dividendo /= divisor;

         if ((this->*filter)(table[line], 0))
            for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
               if (performanceFactor.at(performanceFactorElement).factor <= dividendo)
                  performanceFactor.at(performanceFactorElement).quantity++;

         delete [] table[line];
      }
      delete [] table[line];
      delete [] table;
   }

   fileName = baseFileName;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());

   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());
   fileName.append(basePerfFileNameSuffix);
   fileName.append(Support::GRAPHIC_JU_SUFFIX);

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   /*comment*///normalizacao do histograma acumulado normalizado
   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
      performanceFactor.at(performanceFactorElement).normalizedQuantity = (double) performanceFactor.at(performanceFactorElement).quantity / (double) performanceFactor.back().quantity;

   /*comment*///retirando as duplicatas
//   while (performanceFactor.back().quantity == performanceFactor.at(performanceFactor.size()-2).quantity)
//      performanceFactor.erase(performanceFactor.end());
//   firstTime = true;
//   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size() -1;)
//      if (performanceFactor.at(performanceFactorElement).quantity == performanceFactor.at(performanceFactorElement +1).quantity)
//         if (firstTime)
//         {
//            firstTime = false;
//            performanceFactorElement++;
//         }
//         else performanceFactor.erase(performanceFactor.begin() + performanceFactorElement);
//      else
//      {
//         firstTime = true;
//         performanceFactorElement++;
//      }
//   while (performanceFactor.at(0).quantity == performanceFactor.at(1).quantity)
//      performanceFactor.erase(performanceFactor.begin());

   for (unsigned int performanceFactorElement = performanceFactor.size(); performanceFactorElement > 0; performanceFactorElement--)
      file << performanceFactor.at(performanceFactorElement -1).factor << Support::SEPARATOR << performanceFactor.at(performanceFactorElement -1).quantity << Support::SEPARATOR << performanceFactor.at(performanceFactorElement -1).normalizedQuantity << Support::END_OF_INFORMATION << std::endl;

   file.close();
}

void Performance::allMinimumPathDynamics(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, const char * baseFileName, const char * baseFileNameSuffix, const char * basePerfFileNameSuffix, void (Performances::Performance::*dynamicsType)(unsigned int origin, unsigned int destiny, std::ofstream & file, unsigned int minimumRouteTableLineHopCount, unsigned int nodeQuantity))
{
   Support::OperationalSystem::Linux::Randomic::setSeed();
   Networks::Routing::AllRoutesDiscover * router, * cloneRouter;
   std::stringstream MyStream;
   std::string fileName = baseFileName;
   std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION, routeTableLine;

#ifdef LOCAL_OUTPUT
   fileName = Support::LOCAL_OUTPUT_BASE_DIR + fileName.substr(fileName.find_last_of(Support::DIRECTORY_NAME_SEPARATOR));
#endif /* LOCAL_OUTPUT */
   fileName.append(baseFileNameSuffix);
   fileName.append(basePerfFileNameSuffix);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   std::ofstream routeFile;
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   for (unsigned int run = 0; run < Support::MAXIMUM_RUNS; run++)
   {
      Support::OperationalSystem::Linux::Randomic::listOfIdentifiers(this->networkSize, listOfNodeIdentifiers);

      fileName = baseFileName;
      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << run;
      fileName.append((MyStream).str());
      fileName.append(baseFileNameSuffix);
      routeFile.open(fileName.data(), std::ios::out);
      if (not routeFile)
      {
         std::cerr << fileName << " not created!" << std::endl;
         exit(1);
      }
      std::cout << "Writing route file:" << fileName << std::endl;

      while (listOfNodeIdentifiers->size() > 0)
      {
         origin = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         destiny = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         routeTableLine = Networks::Routing::pathPosition(origin, destiny, this->networkSize);
         routeFile << minimumRouteRouter->routeTable[routeTableLine][0];
         for (unsigned int column = 1; column < minimumRouteRouter->routeTable[routeTableLine].size(); column++)
            routeFile << Support::SEPARATOR << minimumRouteRouter->routeTable[routeTableLine][column];
         routeFile << Support::END_OF_INFORMATION  << std::endl;
      }
      routeFile.close();

      std::cout << "Reading:" << fileName << std::endl;
      router = new Networks::Routing::AllRoutesDiscover(this->networkSize);
      cloneRouter = new Networks::Routing::AllRoutesDiscover(this->networkSize);
      router->loadRoutes(fileName.data(), 0);

      while (router->routeTable.size() > 0)
      {
         *cloneRouter = *router;

         Networks::Routing::AllRoutesDiscover::routeSort(&cloneRouter->routeTable, Support::ROUTES_SORT_ORDER);
         this->loadRoutes(cloneRouter, Support::ROUTES_ENUMERATION_TYPE);

#ifdef PERFORMANCE_DEBUG
         for (int debugL = 0; debugL < router->routeTable.size(); debugL++)
         {
            std::cout << "->";
            for (int debugC = 0; debugC < router->routeTable[debugL].size(); debugC++)
               std::cout << router->routeTable[debugL][debugC] << ",";
            std::cout << std::endl;
         }
         std::cout << "---" << std::endl;
         for (int debugL = 0; debugL < cloneRouter->routeTable.size(); debugL++)
         {
            std::cout << "->";
            for (int debugC = 0; debugC < cloneRouter->routeTable[debugL].size(); debugC++)
               std::cout << cloneRouter->routeTable[debugL][debugC] << ",";
            std::cout << std::endl;
         }
#endif /* PERFORMANCE_DEBUG */

         /*debug*/std::cout << "Run:" << run << " Pairs: " << cloneRouter->routeTable.size() << std::endl;
         (this->*dynamicsType)(run, cloneRouter->routeTable.size(), file, 0, 0/*router->nodeQuantity*/);

#ifdef REDUCED_RUNS
         if (router->routeTable.size() > 10)
         {
             router->routeTable.pop_back();
             router->routeTable.pop_back();
             router->routeTable.pop_back();
             router->routeTable.pop_back();
         }
#endif /* REDUCED_RUNS */

         router->routeTable.pop_back();
         this->clear();
         this->init();

#ifdef ONLY_MAX_RUNS
         router->routeTable.clear();
#endif /* ONLY_MAX_RUNS */
      }

      delete router;
      delete cloneRouter;
      delete listOfNodeIdentifiers;
      listOfNodeIdentifiers = NULL;
   }

   file.close();
}

void Performance::averageAllrouteQuantity(const char * baseFileName, unsigned short networkQuantity, unsigned short runs, const char * basePerfFileNameSuffix, bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity))
{
   unsigned int ** table;
   double dividendo, divisor;
   unsigned int line;
   std::string fileName;
   std::stringstream MyStream;
   std::vector <Performances::performanceFactor> performanceFactor(this->networkSize);
   std::vector < std::vector <double> > data(this->networkSize);
   double standardDerivation, confidence, average;
   unsigned int value;

   for (unsigned int dataElement = 0; dataElement < data.size(); dataElement++)
      data[dataElement].reserve(this->networkSize);

   for (unsigned int performanceFactorElement = 0; performanceFactorElement < performanceFactor.size(); performanceFactorElement++)
   {
      performanceFactor.at(performanceFactorElement).factor = 0;
      performanceFactor.at(performanceFactorElement).quantity = 0;
   }

   for (unsigned int network = 1; network <= networkQuantity; network++)
   {
      fileName = baseFileName;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << this->networkSize;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << network;
      fileName.append((MyStream).str());
      fileName.append(basePerfFileNameSuffix);

      std::cout << "Reading:" << fileName << " ";
      std::cout.flush();
      table = Support::readCSVfile(fileName.data());
      //if (table == NULL) exit(1);

      if (table != NULL)
      {
        for (line = 0; table[line][0] != Support::INVALID_IDENTIFICATION and table[line][Performances::File::Format::RUN] < runs; line++)
        {
          value = table[line][Performances::File::Format::PAIR_QUANTITY];

          dividendo = table[line][Performances::File::Format::TO_DESTINY_TRANSMISSION_QUANTITY];
          divisor = table[line][Performances::File::Format::CYCLE_SIZE];
          dividendo /= divisor;

          if ((this->*filter)(table[line], 0))
          {
            performanceFactor.at(value).factor += dividendo;
            performanceFactor.at(value).quantity++;
            data[value].push_back(dividendo);
          }

          delete [] table[line];
        }

        std::cout << "Read:" << line << std::endl;
        for (; table[line][0] != Support::INVALID_IDENTIFICATION; line++) delete [] table[line];
        delete [] table[line];
        delete [] table;
      }
   }

   fileName = baseFileName;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());

   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << this->networkSize;
   fileName.append((MyStream).str());
   fileName.append(basePerfFileNameSuffix);
   fileName.append(Support::GRAPHIC_SUFFIX);

   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Performance" << std::endl;

   for (unsigned int performanceFactorElement = 1; performanceFactorElement <= this->networkSize/2; performanceFactorElement++)
   {
      if (performanceFactor.at(performanceFactorElement).quantity != 0)
      {
         average = performanceFactor.at(performanceFactorElement).factor / performanceFactor.at(performanceFactorElement).quantity;
         standardDerivation = Statistic::std(data[performanceFactorElement], average, data[performanceFactorElement].size());
         confidence = Statistic::confidence(Support::CONFIDENCE_FACTOR, standardDerivation, data[performanceFactorElement].size());
         file << performanceFactorElement << Support::SEPARATOR << average << Support::SEPARATOR << standardDerivation << Support::SEPARATOR << confidence << Support::SEPARATOR << (average + confidence) << Support::SEPARATOR << (average - confidence) << Support::END_OF_INFORMATION << std::endl;
      }
   }
   file.close();
}

void Performance::allMinimumPathSizeHistogram(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector <unsigned int> * histogram)
{
   Support::OperationalSystem::Linux::Randomic::setSeed();
   std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION, routeTableLine;

   for (unsigned int run = 0; run < Support::MAXIMUM_RUNS; run++)
   {
      Support::OperationalSystem::Linux::Randomic::listOfIdentifiers(this->networkSize, listOfNodeIdentifiers);

      while (listOfNodeIdentifiers->size() > 0)
      {
         origin = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         destiny = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         routeTableLine = Networks::Routing::pathPosition(origin, destiny, this->networkSize);
         histogram->at(minimumRouteRouter->routeTable[routeTableLine].size())++;
      }

      delete listOfNodeIdentifiers;
      listOfNodeIdentifiers = NULL;
   }
}

void Performance::allMinimumPathSharedLinkHistogram(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector <unsigned int> * histogram, const char * histogramType)
{
   Support::OperationalSystem::Linux::Randomic::setSeed();
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION, routeTableLine, counter, shareFactorTotal;
   std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
   std::vector < std:: vector < unsigned int > > routes;
   std::vector < bool > visited (this->networkSize, false);
   std::vector < unsigned int > shareFactor;
   std::vector < unsigned char > shareFactorFromNode;

   for (unsigned int run = 0; run < Support::MAXIMUM_RUNS; run++)
   {
      Support::OperationalSystem::Linux::Randomic::listOfIdentifiers(this->networkSize, listOfNodeIdentifiers);

      routes.resize(0);
      while (listOfNodeIdentifiers->size() > 0)
      {
         origin = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         destiny = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         routeTableLine = Networks::Routing::pathPosition(origin, destiny, this->networkSize);
         routes.resize(routes.size() +1);
         for (unsigned int column = 0; column < minimumRouteRouter->routeTable[routeTableLine].size(); column++)
            routes.back().push_back(minimumRouteRouter->routeTable[routeTableLine][column]);
      }

      if (std::strcmp(histogramType, Support::ROUTE_PER_NEIGHBOR_SUFFIX) == 0)
      {
    	 shareFactorTotal = 0;
         for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
        	 shareFactorTotal+=routes.at(nodeLine).size();

         for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
         {
            shareFactor.resize(routes.at(nodeLine).size(),0);
            for (unsigned int nodeColumn = 0; nodeColumn < routes.at(nodeLine).size(); nodeColumn++)
            {
               for (unsigned int neighborLine = 0; neighborLine < routes.size(); neighborLine++)
               {
                  if (neighborLine == nodeLine) continue;
                  for (unsigned int neighborColumn = 0; neighborColumn < routes.at(neighborLine).size(); neighborColumn++)
                  {
                     if (this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn)))
                      	shareFactor[nodeColumn]++;
                     else if (routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn))
                    	shareFactor[nodeColumn]++;
                  }
               }
            }

            counter=0;
            for (unsigned int shareFactorElement=0; shareFactorElement < shareFactor.size(); shareFactorElement++)
            	counter+=shareFactor[shareFactorElement];
//            	if (shareFactor[shareFactorElement] > 0) counter++;
//            counter = (double)(100*counter)/(double)(shareFactor.size() * (shareFactorTotal - shareFactor.size()));
            counter = (double)(100*counter)/(double)((this->networkSize -2)*this->networkSize/2);
            if (histogram->size() < counter +1) histogram->resize(counter +1, 0);
            histogram->at(counter)++;
            shareFactor.clear();
         }
      }
      else if (std::strcmp(histogramType, Support::ROUTE_PER_LINK_SUFFIX) == 0)
      {
    	 shareFactorTotal = 0;
         for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
        	 shareFactorTotal+=routes.at(nodeLine).size() -1;

         for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
         {
            shareFactor.resize(routes.at(nodeLine).size() -1,0);
            for (unsigned int nodeColumn = 0; nodeColumn < routes.at(nodeLine).size() -1; nodeColumn++)
            {
               for (unsigned int neighborLine = 0; neighborLine < routes.size(); neighborLine++)
               {
                  if (neighborLine == nodeLine) continue;
                  shareFactorFromNode.resize(routes.at(neighborLine).size(),0);
                  for (unsigned int neighborColumn = 0; neighborColumn < routes.at(neighborLine).size() -1; neighborColumn++)
                  {
                     if (this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn))
                    	 or routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn)
                    	 or this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn +1))
                    	 or routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn +1)
                    	 or this->nodes[routes.at(nodeLine).at(nodeColumn +1)]->isNeighbor(routes.at(neighborLine).at(neighborColumn))
                    	 or routes.at(nodeLine).at(nodeColumn +1) == routes.at(neighborLine).at(neighborColumn)
                    	 or this->nodes[routes.at(nodeLine).at(nodeColumn +1)]->isNeighbor(routes.at(neighborLine).at(neighborColumn +1))
                    	 or routes.at(nodeLine).at(nodeColumn +1) == routes.at(neighborLine).at(neighborColumn +1))
                    	 shareFactorFromNode[neighborColumn]=1;
                  }
                  for (unsigned int shareFactorFromNodeElement = 0; shareFactorFromNodeElement < shareFactorFromNode.size(); shareFactorFromNodeElement++)
                	  shareFactor[nodeColumn] += shareFactorFromNode.at(shareFactorFromNodeElement);
                  shareFactorFromNode.clear();
               }
            }

            counter=0;
            for (unsigned int shareFactorElement=0; shareFactorElement < shareFactor.size(); shareFactorElement++)
            	counter+=shareFactor[shareFactorElement];
//            counter = (double)(100*counter)/(double)(shareFactorTotal - shareFactor.size());
            counter = (double)(100*counter)/(double)(shareFactor.size() * (shareFactorTotal - shareFactor.size()));
//            counter = (double)(100*counter)/(double)((this->networkSize -2)*(this->networkSize -1)/2);
            if (histogram->size() < counter +1) histogram->resize(counter +1, 0);
            histogram->at(counter)++;
            shareFactor.clear();
         }
      }
      else if (std::strcmp(histogramType, Support::ROUTE_PER_NODE_SUFFIX) == 0)
      {
         for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
         {
            for (unsigned int nodeColumn = 0; nodeColumn < routes.at(nodeLine).size(); nodeColumn++)
            {
               if (!visited.at(routes.at(nodeLine).at(nodeColumn)))
               {
                  visited.at(routes.at(nodeLine).at(nodeColumn)) = true;
                  for (unsigned int neighborLine = nodeLine +1; neighborLine < routes.size(); neighborLine++)
                  {
                     for (unsigned int neighborColumn = 0; neighborColumn < routes.at(neighborLine).size(); neighborColumn++)
                     {
                        if (routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn))
                           histogram->at(routes.at(nodeLine).at(nodeColumn))++;
                        if (this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn)))
                           histogram->at(routes.at(nodeLine).at(nodeColumn))++;
                     }
                  }
               }
            }
         }
      }

      delete listOfNodeIdentifiers;
      listOfNodeIdentifiers = NULL;
   }
}

void Performance::networkDegreeHistogram(std::vector <unsigned int> * histogram)
{
   for (unsigned int element = 0; element < this->networkSize; element++)
      histogram->at(this->nodes[element]->getNeighborsQuantity())++;
}

void Performance::allMinimumPathSharedLinkAverage(Networks::Routing::AllRoutesDiscover * minimumRouteRouter, std::vector < double > * linkAverage, std::vector < double > * pathSizeAverage, std::vector < double > * cliqueAverage)
{
   Support::OperationalSystem::Linux::Randomic::setSeed();
   unsigned int origin = Support::INVALID_IDENTIFICATION, destiny = Support::INVALID_IDENTIFICATION, routeTableLine;
   std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
   std::vector < std:: vector < unsigned int > > routes;

   for (unsigned int run = 0; run < Support::MAXIMUM_RUNS; run++)
   {
	   linkAverage->push_back(0);
	   pathSizeAverage->push_back(0);
	  cliqueAverage->push_back(0);

      Support::OperationalSystem::Linux::Randomic::listOfIdentifiers(this->networkSize, listOfNodeIdentifiers);

      routes.resize(0);
      while (listOfNodeIdentifiers->size() > 0)
      {
         origin = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         destiny = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
         routeTableLine = Networks::Routing::pathPosition(origin, destiny, this->networkSize);
         routes.resize(routes.size() +1);
         for (unsigned int column = 0; column < minimumRouteRouter->routeTable[routeTableLine].size(); column++)
            routes.back().push_back(minimumRouteRouter->routeTable[routeTableLine][column]);
      }

      for (unsigned int nodeLine = 0; nodeLine < routes.size(); nodeLine++)
      {
    	  pathSizeAverage->back() += routes.at(nodeLine).size();
    	  if (routes.at(nodeLine).size() == 2)
    		  cliqueAverage->back() += (double) 1;
    	  else if (routes.at(nodeLine).size() == 3)
    		  cliqueAverage->back() += (double) ((double)1/(double)2);
    	  else
    		  cliqueAverage->back() += (double) ((double)1/(double)3);
         for (unsigned int nodeColumn = 0; nodeColumn < routes.at(nodeLine).size() -1; nodeColumn++)
         {
            for (unsigned int neighborLine = nodeLine +1; neighborLine < routes.size(); neighborLine++)
            {
               for (unsigned int neighborColumn = 0; neighborColumn < routes.at(neighborLine).size() -1; neighborColumn++)
               {
                   if (this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn))
                  	 or routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn)
                  	 or this->nodes[routes.at(nodeLine).at(nodeColumn)]->isNeighbor(routes.at(neighborLine).at(neighborColumn +1))
                  	 or routes.at(nodeLine).at(nodeColumn) == routes.at(neighborLine).at(neighborColumn +1)
                  	 or this->nodes[routes.at(nodeLine).at(nodeColumn +1)]->isNeighbor(routes.at(neighborLine).at(neighborColumn))
                  	 or routes.at(nodeLine).at(nodeColumn +1) == routes.at(neighborLine).at(neighborColumn)
                  	 or this->nodes[routes.at(nodeLine).at(nodeColumn +1)]->isNeighbor(routes.at(neighborLine).at(neighborColumn +1))
                  	 or routes.at(nodeLine).at(nodeColumn +1) == routes.at(neighborLine).at(neighborColumn +1))
                	   linkAverage->back()++;
               }
            }
         }
      }

      linkAverage->back() /= routes.size();
      delete listOfNodeIdentifiers;
      listOfNodeIdentifiers = NULL;
   }
}

}
