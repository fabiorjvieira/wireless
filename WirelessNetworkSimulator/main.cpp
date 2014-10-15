/*
 * main.c
 *
 *  Created on: Aug 18, 2008
 *      Author: fjimenez
 */

#include "main.hpp"

/*comment*///Default values também
double Support::ALFA = 0.5;
double Support::QTD_EXAMPLES = 100;
double Support::CONFIDENCE_FACTOR = 0.95;
short Support::INITIAL_BUFFER_SIZE = 1;
short Support::FINAL_BUFFER_SIZE = 1;
short Support::BUFFER_SIZE = Support::INITIAL_BUFFER_SIZE;
bool Support::ROUTES_SORT_ORDER = Support::ASCENDING;
bool Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
double Support::MINIMUM_DISTANCE = 200;
std::string Support::NETWORK_FILE_NAME;
std::string Support::SECOND_NETWORK_FILE_NAME;
unsigned int Support::NETWORK_SIZE;
unsigned int Support::UPPER_LIMIT = 1;
unsigned int Support::LOWER_LIMIT = 1;
unsigned int Support::LAST_PARTIAL_ORDER_CYCLE_MEMBER = 1;
unsigned int Support::FIRST_PARTIAL_ORDER_CYCLE_MEMBER = 1;
unsigned int Support::MINIMUM_ROUTE_SIZE = 0;
unsigned int Support::MAXIMUM_ROUTE_SIZE = -1;
unsigned int Support::MAXIMUM_RUNS = 100;
std::string Support::LOCAL_OUTPUT_BASE_DIR = "./Result/";

void cluster(unsigned int networkSize, Networks::Node ** nodes, unsigned int * numberOfRadios, char * nodeFile)
{
   unsigned int ** quantities = new unsigned int * [7];
   quantities[0] = new unsigned int [networkSize];
   quantities[1] = new unsigned int [networkSize];
   quantities[2] = new unsigned int [networkSize];
   quantities[3] = new unsigned int [networkSize];
   quantities[4] = new unsigned int [networkSize];
   quantities[5] = new unsigned int [networkSize];
   quantities[6] = new unsigned int [networkSize];
   Performances::Statistic::neighborhoodQuantities(nodes, quantities, networkSize);

/*
   std::cout << "#Statistis File" << std::endl;
   std::cout << "#Format: x,y,z,n_neib,n_neib_neibo,radios" << std::endl;
   for (unsigned int node = 0; node < networkSize; node++)
   {
      std::cout << nodes[node]->getPosition().x << "," << nodes[node]->getPosition().y << "," << nodes[node]->getPosition().z << ",";
      std::cout << quantities[0][node] << "," << quantities[1][node] << "," << quantities[2][node] << "," << quantities[3][node] << ",";
      std::cout << quantities[4][node] << "," << quantities[5][node] << "," << quantities[6][node] << "," ;
      std::cout << numberOfRadios[node] << std::endl;
   }
*/

   double averageRadios = Performances::Statistic::average(numberOfRadios, networkSize);
   double stdRadios = Performances::Statistic::std(numberOfRadios, averageRadios, networkSize);
   double * averageQuantities = new double [7], * stdQuantities = new double [7];
   Networks::SpacialPosition averageNodePositions = Performances::Statistic::averageNodePosition(nodes, networkSize);
   Networks::SpacialPosition stdNodePositions = Performances::Statistic::stdNodePosition(nodes, averageNodePositions, networkSize);
   averageQuantities[0] = Performances::Statistic::average(quantities[0], networkSize);
   averageQuantities[1] = Performances::Statistic::average(quantities[1], networkSize);
   averageQuantities[2] = Performances::Statistic::average(quantities[2], networkSize);
   averageQuantities[3] = Performances::Statistic::average(quantities[3], networkSize);
   averageQuantities[4] = Performances::Statistic::average(quantities[4], networkSize);
   averageQuantities[5] = Performances::Statistic::average(quantities[5], networkSize);
   averageQuantities[6] = Performances::Statistic::average(quantities[6], networkSize);
   stdQuantities[0] = Performances::Statistic::std(quantities[0], averageQuantities[0], networkSize);
   stdQuantities[1] = Performances::Statistic::std(quantities[1], averageQuantities[1], networkSize);
   stdQuantities[2] = Performances::Statistic::std(quantities[2], averageQuantities[2], networkSize);
   stdQuantities[3] = Performances::Statistic::std(quantities[3], averageQuantities[3], networkSize);
   stdQuantities[4] = Performances::Statistic::std(quantities[4], averageQuantities[4], networkSize);
   stdQuantities[5] = Performances::Statistic::std(quantities[5], averageQuantities[5], networkSize);
   stdQuantities[6] = Performances::Statistic::std(quantities[6], averageQuantities[6], networkSize);

/*
   std::cout << "#Statistis File" << std::endl;
   std::cout << "#Format: x,y,z,n_neib,n_neib_neibo,radios" << std::endl;
   for (unsigned int node = 0; node < networkSize; node++)
   {
      std::cout << Performances::Statistic::scaling(nodes[node]->getPosition().x, averageNodePositions.x, stdNodePositions.x) << ",";
      std::cout << Performances::Statistic::scaling(nodes[node]->getPosition().y, averageNodePositions.y, stdNodePositions.y) << ",";
      std::cout << Performances::Statistic::scaling(nodes[node]->getPosition().z, averageNodePositions.z, stdNodePositions.z) << ",";
      std::cout << Performances::Statistic::scaling(quantities[0][node], averageQuantities[0], stdQuantities[0]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[1][node], averageQuantities[1], stdQuantities[1]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[2][node], averageQuantities[2], stdQuantities[2]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[3][node], averageQuantities[3], stdQuantities[3]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[4][node], averageQuantities[4], stdQuantities[4]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[5][node], averageQuantities[5], stdQuantities[5]) << ",";
      std::cout << Performances::Statistic::scaling(quantities[6][node], averageQuantities[6], stdQuantities[6]) << ",";
      std::cout << Performances::Statistic::scaling(numberOfRadios[node], averageRadios, stdRadios) << std::endl;
   }
*/

   unsigned char dataQuantity = 11;
   double ** data = new double * [dataQuantity];
   for (unsigned char element = 0; element < dataQuantity ; element++)
      data[element] = new double[networkSize];
   for (unsigned int node = 0; node < networkSize; node++)
   {
      data[0][node] = quantities[0][node];
      data[1][node] = quantities[1][node];
      data[2][node] = quantities[2][node];
      data[3][node] = quantities[3][node];
      data[4][node] = numberOfRadios[node];
      data[5][node] = quantities[4][node];
      data[6][node] = quantities[5][node];
      data[7][node] = quantities[6][node];
      data[8][node] = nodes[node]->getPosition().x;
      data[9][node] = nodes[node]->getPosition().y;
      data[10][node] = nodes[node]->getPosition().z;
   }
   double ** histogram = Performances::Statistic::histogram(data, networkSize, dataQuantity -7);

   double ** scaledData = new double * [dataQuantity];
   for (unsigned char element = 0; element < dataQuantity ; element++)
      scaledData[element] = new double[networkSize];
   for (unsigned int node = 0; node < networkSize; node++)
   {
      scaledData[0][node] = Performances::Statistic::scaling(quantities[0][node], averageQuantities[0], stdQuantities[0]);
      scaledData[1][node] = Performances::Statistic::scaling(quantities[1][node], averageQuantities[1], stdQuantities[1]);
      scaledData[2][node] = Performances::Statistic::scaling(quantities[2][node], averageQuantities[2], stdQuantities[2]);
      scaledData[3][node] = Performances::Statistic::scaling(quantities[3][node], averageQuantities[3], stdQuantities[3]);
      scaledData[4][node] = Performances::Statistic::scaling(numberOfRadios[node], averageRadios, stdRadios);
      scaledData[5][node] = Performances::Statistic::scaling(quantities[4][node], averageQuantities[4], stdQuantities[4]);
      scaledData[6][node] = Performances::Statistic::scaling(quantities[5][node], averageQuantities[5], stdQuantities[5]);
      scaledData[7][node] = Performances::Statistic::scaling(quantities[6][node], averageQuantities[6], stdQuantities[6]);
      scaledData[8][node] = Performances::Statistic::scaling(nodes[node]->getPosition().x, averageNodePositions.x, stdNodePositions.x);
      scaledData[9][node] = Performances::Statistic::scaling(nodes[node]->getPosition().y, averageNodePositions.y, stdNodePositions.y);
      scaledData[10][node] = Performances::Statistic::scaling(nodes[node]->getPosition().z, averageNodePositions.z, stdNodePositions.z);
   }
   double ** scaledHistogram = Performances::Statistic::histogram(scaledData, networkSize, dataQuantity -7);

   std::ofstream * dataFile;
   std::string manipulator;

   manipulator.clear();
   manipulator.append(nodeFile);
   manipulator.append(".data");
   dataFile = new std::ofstream(&manipulator[0], std::ios::out);
   for (unsigned int node = 0; node < networkSize; node++)
   {
      for (unsigned char statistic = 0; statistic < dataQuantity -1 ; statistic++)
         *dataFile << data[statistic][node] << ",";
      *dataFile << data[dataQuantity -1][node] << std::endl;
   }
   delete dataFile;

   manipulator.clear();
   manipulator.append(nodeFile);
   manipulator.append(".scaled.data");
   dataFile = new std::ofstream(&manipulator[0], std::ios::out);
   for (unsigned int node = 0; node < networkSize; node++)
   {
      for (unsigned char statistic = 0; statistic < dataQuantity -1 ; statistic++)
         *dataFile << scaledData[statistic][node] << ",";
      *dataFile << scaledData[dataQuantity -1][node] << std::endl;
   }
   delete dataFile;

   manipulator.clear();
   manipulator.append(nodeFile);
   manipulator.append(".data.histogram");
   dataFile = new std::ofstream(&manipulator[0], std::ios::out);
   *dataFile << histogram[0][0] << "," << histogram[1][0] << std::endl;
   for (unsigned int statistic = 1; histogram[0][statistic] != 0; statistic++)
      *dataFile << histogram[0][statistic] << "," << histogram[1][statistic] << std::endl;
   delete dataFile;

   manipulator.clear();
   manipulator.append(nodeFile);
   manipulator.append(".scaled.data.histogram");
   dataFile = new std::ofstream(&manipulator[0], std::ios::out);
   dataFile = new std::ofstream(&manipulator[0], std::ios::out);
   *dataFile << scaledHistogram[0][0] << "," << scaledHistogram[1][0] << std::endl;
   for (unsigned int statistic = 1; scaledHistogram[0][statistic] != 0; statistic++)
      *dataFile << scaledHistogram[0][statistic] << "," << scaledHistogram[1][statistic] << std::endl;
   delete dataFile;
}

int main(int numberOfArguments, char ** arguments)
{
   //Support::jointPerformanceResults("~/Documents/Tese/Doutorado/workspace/tmp/teste/Result/A/", "~/Documents/Tese/Doutorado/workspace/tmp/teste/", "*");
   //Support::jointPerformanceResults(arguments[1], arguments[2], arguments[3]);
   //return 0;
//   std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
//   unsigned int origin, destiny;
//   for (char iii = 0; iii < 3; iii++)
//   {
//      Support::OperationalSystem::Linux::Randomic::listOfIdentifiers(80, listOfNodeIdentifiers);
//      while (listOfNodeIdentifiers->size() > 0)
//      {
//         origin = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
//         destiny = Support::OperationalSystem::Linux::Randomic::randomicFromList(listOfNodeIdentifiers);
//         std::cout << origin << "," << destiny << std::endl;
//      }
//      std::cout << std::endl;
//   }

   Support::NETWORK_SIZE = atoi(arguments[1]);
   Support::NETWORK_FILE_NAME = arguments[2];
   Support::OperationalSystem::Linux::Randomic::setSeed();
   timeb startTime, endTime;
   ftime(&startTime);
   std::stringstream MyStream;
   std::string fileName, ruleName;
   std::string fileNameA, fileNameB, ruleNameA, ruleNameB, difference;
   unsigned char groupingColumnX, groupingColumnY;
   double incrementPerformance, minimumPerformance, maximumPerformance, runs;
   if (std::getenv("workspace") != NULL)
      Support::LOCAL_OUTPUT_BASE_DIR.insert(0,std::getenv("workspace"));

/*bacalhau*/
   switch (Support::NETWORK_SIZE)
   {
      case 60:
         Support::MINIMUM_DISTANCE = 231;
         break;
      case 80:
         Support::MINIMUM_DISTANCE = 200;
         break;
      case 100:
         Support::MINIMUM_DISTANCE = 178;
         break;
      case 120:
         Support::MINIMUM_DISTANCE = 162;
         break;
   }
#ifdef DOUBLEDENSE
   switch (Support::NETWORK_SIZE)
   {
      case 60:
         Support::MINIMUM_DISTANCE = 327;
         break;
      case 80:
         Support::MINIMUM_DISTANCE = 283;
         break;
      case 100:
         Support::MINIMUM_DISTANCE = 252;
         break;
      case 120:
         Support::MINIMUM_DISTANCE = 229;
         break;
   }
#endif /*DOUBLEDENSE*/
#ifdef QUADRIDENSE
   switch (Support::NETWORK_SIZE)
   {
      case 60:
         Support::MINIMUM_DISTANCE = 462;
         break;
      case 80:
         Support::MINIMUM_DISTANCE = 400;
         break;
      case 100:
         Support::MINIMUM_DISTANCE = 356;
         break;
      case 120:
         Support::MINIMUM_DISTANCE = 322;
         break;
   }
#endif /*QUADRIDENSE*/
#ifdef OCTODENSE
   switch (Support::NETWORK_SIZE)
   {
      case 60:
         Support::MINIMUM_DISTANCE = 654;
         break;
      case 80:
         Support::MINIMUM_DISTANCE = 566;
         break;
      case 100:
         Support::MINIMUM_DISTANCE = 504;
         break;
      case 120:
         Support::MINIMUM_DISTANCE = 458;
         break;
   }
#endif /*DOUBLEDENSE*/
/*bacalhau*/

//   Performances::Performance::search(arguments[1], atoi(arguments[2]), arguments[3]);
//   return 0;
/*
   fileName = Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX;
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, atoi(arguments[4]), atoi(arguments[5]));

   fileName = Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX;
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, atoi(arguments[4]), atoi(arguments[5]));

   fileName = ".each.1.neighbor.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".each.1.neighbor.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".each.2.neighbor.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".each.2.neighbor.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".each.3.neighbor.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".each.3.neighbor.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".each.4.neighbor.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".each.4.neighbor.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".each.5.neighbor.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".each.5.neighbor.routes", atoi(arguments[4]), atoi(arguments[5]));

   fileName = ".neighbor.2.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".neighbor.2.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".neighbor.3.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".neighbor.3.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".neighbor.4.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".neighbor.4.routes", atoi(arguments[4]), atoi(arguments[5]));
   fileName = ".neighbor.5.routes";
   fileName.append(arguments[3]);
   Performances::Performance::fix(arguments[1], arguments[2], 100, fileName.data(), ".neighbor.5.routes", atoi(arguments[4]), atoi(arguments[5]));
   return 0;
*/
   std::cout << std::endl;
   std::cout << "Program begins: "<< std::ctime(&startTime.time) << std::endl;

#ifdef STATISTICS
   Networks::Topology * topology;
   Networks::Network * network;
   Networks::Node ** nodes;
   Networks::Routing::AllRoutesDiscover * minimumRouteRouter;
   Performances::Performance * performance;
   std::vector <unsigned int> * pathHistogram = new std::vector <unsigned int> (Support::NETWORK_SIZE +1);
   std::vector <unsigned int> * nodeShareHistogram = new std::vector <unsigned int> (Support::NETWORK_SIZE);
   std::vector <unsigned int> * linkShareHistogram = new std::vector <unsigned int> (0);
   std::vector <unsigned int> * neighborShareHistogram = new std::vector <unsigned int> (0);
   std::vector <unsigned int> * degreeHistogram = new std::vector <unsigned int> (Support::NETWORK_SIZE);
   std::vector <double> * linkAverage = new std::vector <double> (0);
   std::vector <double> * pathSizeAverage = new std::vector <double> (0);
   std::vector <double> * cliqueAverage = new std::vector <double> (0);
   double avg, standardDerivation, confidence;

   for (unsigned int element = 0; element < pathHistogram->size(); element++)
	   pathHistogram->at(element) = 0;
   for (unsigned int element = 0; element < nodeShareHistogram->size(); element++)
	   nodeShareHistogram->at(element) = 0;
   for (unsigned int element = 0; element < linkShareHistogram->size(); element++)
	   linkShareHistogram->at(element) = 0;
   for (unsigned int element = 0; element < degreeHistogram->size(); element++)
	   degreeHistogram->at(element) = 0;

   for (unsigned int networkExample = 1; networkExample <= Support::QTD_EXAMPLES; networkExample++)
   {
      fileName = Support::NETWORK_FILE_NAME;
      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << Support::NETWORK_SIZE;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << networkExample;
      fileName.append((MyStream).str());

      fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << Support::NETWORK_SIZE;
      fileName.append((MyStream).str());

      fileName.append(Support::FILE_NAME_SEPARATOR);
      MyStream.str("");
      MyStream << networkExample;
      fileName.append((MyStream).str());

      topology =  new Networks::Topology(Support::NETWORK_SIZE);
      network = new Networks::Network(topology);
      nodes = network->loadNetwork(fileName.data(), Support::MINIMUM_DISTANCE);
      topology->discoverNeighborhood(nodes);
      topology->discoverNeighborsNeighborhood(nodes);

      minimumRouteRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
      minimumRouteRouter->loadRoutes(fileName.append(Support::MINIMUM_ROUTE_TABLE_SUFFIX).data());
      performance = new Performances::Performance(nodes,Support::NETWORK_SIZE);

      performance->allMinimumPathSizeHistogram(minimumRouteRouter, pathHistogram);
      performance->allMinimumPathSharedLinkHistogram(minimumRouteRouter, nodeShareHistogram, Support::ROUTE_PER_NODE_SUFFIX);
      performance->allMinimumPathSharedLinkHistogram(minimumRouteRouter, linkShareHistogram, Support::ROUTE_PER_LINK_SUFFIX);
      performance->allMinimumPathSharedLinkHistogram(minimumRouteRouter, neighborShareHistogram, Support::ROUTE_PER_NEIGHBOR_SUFFIX);
      performance->networkDegreeHistogram(degreeHistogram);
      performance->allMinimumPathSharedLinkAverage(minimumRouteRouter, linkAverage, pathSizeAverage, cliqueAverage);

      delete performance;
      delete minimumRouteRouter;
      delete network;
   }

   avg=0;
   for (unsigned int averageElement=0; averageElement < linkAverage->size(); averageElement++)
   	avg+=linkAverage->at(averageElement);
   avg /= linkAverage->size();
   standardDerivation = Performances::Statistic::std(*linkAverage, avg, linkAverage->size());
   confidence = Performances::Statistic::confidence(Support::CONFIDENCE_FACTOR, standardDerivation, linkAverage->size());
   std::cout << Support::NETWORK_SIZE << Support::SEPARATOR << avg << Support::SEPARATOR << standardDerivation << Support::SEPARATOR << confidence << Support::SEPARATOR;
   avg=0;
   for (unsigned int averageElement=0; averageElement < pathSizeAverage->size(); averageElement++)
   	avg+=pathSizeAverage->at(averageElement);
   avg /= pathSizeAverage->size();
   standardDerivation = Performances::Statistic::std(*pathSizeAverage, avg, pathSizeAverage->size());
   confidence = Performances::Statistic::confidence(Support::CONFIDENCE_FACTOR, standardDerivation, pathSizeAverage->size());
   std::cout << avg << Support::SEPARATOR << standardDerivation << Support::SEPARATOR << confidence << Support::SEPARATOR;
   avg=0;
   for (unsigned int averageElement=0; averageElement < cliqueAverage->size(); averageElement++)
   	avg+=cliqueAverage->at(averageElement);
   avg /= cliqueAverage->size();
   standardDerivation = Performances::Statistic::std(*cliqueAverage, avg, cliqueAverage->size());
   confidence = Performances::Statistic::confidence(Support::CONFIDENCE_FACTOR, standardDerivation, cliqueAverage->size());
   std::cout << avg << Support::SEPARATOR << standardDerivation << Support::SEPARATOR << confidence;
   std::cout << Support::END_OF_INFORMATION << std::endl;

   fileName = Support::NETWORK_FILE_NAME;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX);
   fileName.append(Support::PATH_SIZE_SUFFIX);
   fileName.append(Support::HISTOGRAM_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Histogram path size" << std::endl;
   for (unsigned int element = 0; element < pathHistogram->size(); element++)
	   file << element << Support::SEPARATOR << pathHistogram->at(element) << Support::END_OF_INFORMATION << std::endl;
   file.close();

   fileName = Support::NETWORK_FILE_NAME;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX);
   fileName.append(Support::DEGREE_SUFFIX);
   fileName.append(Support::HISTOGRAM_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   file.open(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Histogram node degree" << std::endl;
   for (unsigned int element = 0; element < degreeHistogram->size(); element++)
	   file << element << Support::SEPARATOR << degreeHistogram->at(element) << Support::END_OF_INFORMATION << std::endl;
   file.close();

   fileName = Support::NETWORK_FILE_NAME;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX);
   fileName.append(Support::ROUTE_PER_NODE_SUFFIX);
   fileName.append(Support::HISTOGRAM_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   file.open(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Histogram node/route sharing" << std::endl;
   for (unsigned int element = 0; element < nodeShareHistogram->size();)
   {
	  nodeShareHistogram->at(element) += nodeShareHistogram->at(element+1) + nodeShareHistogram->at(element+2) + nodeShareHistogram->at(element+3) + nodeShareHistogram->at(element+4);
      file << element << "-" << element+4 << Support::SEPARATOR << nodeShareHistogram->at(element) << Support::END_OF_INFORMATION << std::endl;
      element+=5;
   }
   file.close();

   fileName = Support::NETWORK_FILE_NAME;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX);
   fileName.append(Support::ROUTE_PER_LINK_SUFFIX);
   fileName.append(Support::HISTOGRAM_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   file.open(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Histogram link/route sharing" << std::endl;
   for (unsigned int element = 0; element < linkShareHistogram->size();)
   {
	  //if (linkShareHistogram->at(element) != 0)
         file << element << Support::SEPARATOR << linkShareHistogram->at(element) << Support::END_OF_INFORMATION << std::endl;
      element++;
   }
   file.close();

   fileName = Support::NETWORK_FILE_NAME;
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::DIRECTORY_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << Support::NETWORK_SIZE;
   fileName.append((MyStream).str());
   fileName.append(Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX);
   fileName.append(Support::ROUTE_PER_NEIGHBOR_SUFFIX);
   fileName.append(Support::HISTOGRAM_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   file.open(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }
   file << Support::COMMENTARY << "Histogram neighbor/route sharing" << std::endl;
   for (unsigned int element = 0; element < neighborShareHistogram->size();)
   {
	  //if (neighborShareHistogram->at(element) != 0)
         file << element << Support::SEPARATOR << neighborShareHistogram->at(element) << Support::END_OF_INFORMATION << std::endl;
      element++;
   }
   file.close();

   delete linkAverage;
   delete pathSizeAverage;
   delete cliqueAverage;
   delete pathHistogram;
   delete neighborShareHistogram;
   delete linkShareHistogram;
   delete nodeShareHistogram;
   delete degreeHistogram;

#endif /* STATISTICS */

#ifdef NETWORKING
   #ifndef MAIN_DEBUG
      if (numberOfArguments > 3) Support::LOWER_LIMIT = atoi(arguments[3]);
      if (numberOfArguments > 4) Support::UPPER_LIMIT = atoi(arguments[4]);
      if (numberOfArguments > 5)
      {
         switch (arguments[5][0])
         {
            case 'A':
               Support::ROUTES_SORT_ORDER = Support::DESCENDING;
               Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
               std::cout << std::endl << "DESCENDING BREADTH" << std::endl;
               break;
            case 'B':
               Support::ROUTES_SORT_ORDER = Support::DESCENDING;
               Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
               std::cout << std::endl << "DESCENDING DEPTH" << std::endl;
               break;
            case 'C':
               Support::ROUTES_SORT_ORDER = Support::ASCENDING;
               Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
               std::cout << std::endl << "ASCENDING BREADTH" << std::endl;
               break;
            case 'D':
               Support::ROUTES_SORT_ORDER = Support::ASCENDING;
               Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
               std::cout << std::endl << "ASCENDING DEPTH" << std::endl;
               break;
         }
         Support::LOCAL_OUTPUT_BASE_DIR += Support::DIRECTORY_NAME_SEPARATOR;
         Support::LOCAL_OUTPUT_BASE_DIR += arguments[5];
      }
      if (numberOfArguments > 6)
     {
         Support::INITIAL_BUFFER_SIZE = atoi(arguments[6]);
         Support::LOCAL_OUTPUT_BASE_DIR += Support::DIRECTORY_NAME_SEPARATOR;
         Support::LOCAL_OUTPUT_BASE_DIR += arguments[6];
     }
      if (numberOfArguments > 7) Support::FINAL_BUFFER_SIZE = atoi(arguments[7]);
      Support::BUFFER_SIZE = Support::INITIAL_BUFFER_SIZE;
     Support::LOCAL_OUTPUT_BASE_DIR += Support::DIRECTORY_NAME_SEPARATOR;
     if (std::getenv("jobId") != NULL) Support::LOCAL_OUTPUT_BASE_DIR += std::getenv("jobId");
   #endif /* MAIN_DEBUG */

   std::cout << "Processing:" << Support::NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Radio Range:" << Support::MINIMUM_DISTANCE << std::endl;
   std::cout << "ALFA:" << Support::ALFA << std::endl;

#if defined(PERFORMANCE_NEIGHBOR_N_EACH) || defined(PERFORMANCE_NEIGHBOR_N) || defined(ROUTING_NEIGHBOR_N_EACH) || defined(ROUTING_NEIGHBOR_N)
   std::cout << "Network Limits:" << Support::LOWER_LIMIT << " - " << Support::UPPER_LIMIT << std::endl;
#endif /* PERFORMANCE_NEIGHBOR_N_EACH or PERFORMANCE_NEIGHBOR_N */

#if defined(PERFORMANCE_ABR) || defined(PERFORMANCE_HR) || defined(PERFORMANCE_AR)
   std::cout << "Buffer Limits:" << Support::INITIAL_BUFFER_SIZE << " - " << Support::FINAL_BUFFER_SIZE << std::endl;
#endif /* PERFORMANCE_ABR or PERFORMANCE_HR or PERFORMANCE_AR */

#if defined(PERFORMANCE_HR)
   std::cout << "Last state:" << Support::LAST_PARTIAL_ORDER_CYCLE_MEMBER << std::endl;
#endif /* PERFORMANCE_HR */

#ifdef PERFORMANCE
   std::cout << "Ascending routes sort:" << (Support::ROUTES_SORT_ORDER == Support::ASCENDING) << std::endl;
   std::cout << "Depth routes enumeration:" << (Support::ROUTES_ENUMERATION_TYPE == Support::DEPTH) << std::endl;
#endif /* PERFORMANCE */

   Networks::Topology * topology =  new Networks::Topology(Support::NETWORK_SIZE);
   Networks::Network * network = new Networks::Network(topology);
   Networks::Node ** nodes = network->loadNetwork(Support::NETWORK_FILE_NAME.data(), Support::MINIMUM_DISTANCE);
   topology->discoverNeighborhood(nodes);
   topology->discoverNeighborsNeighborhood(nodes);
#endif /* NETWORKING */

#if defined(GRAPHICS_ALL_MINIMUM_PATH)
   fileName = arguments[3];
   ruleName = arguments[4];
   runs = atoi(arguments[5]);
   std::cout << "CONFIDENCE:" << Support::CONFIDENCE_FACTOR << std::endl;
   std::cout << "Processing:" << Support::NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Number of different examples:" << Support::QTD_EXAMPLES << std::endl;
   std::cout << "Routes:" << fileName << std::endl;
   std::cout << "Rule:" << ruleName << std::endl;
#endif /* GRAPHICS_ALL_MINIMUM_PATH */

#if defined(GRAPHICS) || defined(HISTOGRAM)
   fileName = arguments[3];
   ruleName = arguments[4];
   groupingColumnX = atoi(arguments[5]);
   groupingColumnY = atoi(arguments[6]);
   if (numberOfArguments > 7) Support::LOWER_LIMIT = atoi(arguments[7]);
   if (numberOfArguments > 8) Support::UPPER_LIMIT = atoi(arguments[8]);
   std::cout << "CONFIDENCE:" << Support::CONFIDENCE_FACTOR << std::endl;
   std::cout << "Processing:" << Support::NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Network Limits:" << Support::LOWER_LIMIT << " - " << Support::UPPER_LIMIT << std::endl;
   std::cout << "Number of different examples:" << Support::QTD_EXAMPLES << std::endl;
   std::cout << "Routes:" << fileName << std::endl;
   std::cout << "Rule:" << ruleName << std::endl;
   std::cout << "Grouping Columns:" << (int) groupingColumnX << "-" << (int) groupingColumnY << std::endl;
#endif /* GRAPHICS HISTOGRAM */

#if defined(GRAPHICS_JU)
   fileName = arguments[3];
   ruleName = arguments[4];
   minimumPerformance  = std::atof(arguments[5]);
   maximumPerformance = atof(arguments[6]);
   incrementPerformance = atof(arguments[7]);
   if (numberOfArguments > 8) Support::LOWER_LIMIT = atoi(arguments[8]);
   if (numberOfArguments > 9) Support::UPPER_LIMIT = atoi(arguments[9]);
   if (numberOfArguments > 10) Support::MINIMUM_ROUTE_SIZE = atoi(arguments[10]);
   if (numberOfArguments > 11) Support::MAXIMUM_ROUTE_SIZE = atoi(arguments[11]);
   std::cout << "CONFIDENCE:" << Support::CONFIDENCE_FACTOR << std::endl;
   std::cout << "Processing:" << Support::NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Network Limits:" << Support::LOWER_LIMIT << " - " << Support::UPPER_LIMIT << std::endl;
   std::cout << "Number of different examples:" << Support::QTD_EXAMPLES << std::endl;
   std::cout << "Routes:" << fileName << std::endl;
   std::cout << "Rule:" << ruleName << std::endl;
   std::cout << "Performance Limits:" << minimumPerformance << "-" << maximumPerformance << "-" << incrementPerformance << std::endl;
#endif /* GRAPHICS_JU */

#ifdef DOMINANCE
   Support::SECOND_NETWORK_FILE_NAME = arguments[3];
   fileNameA = arguments[4];
   ruleNameA = arguments[5];
   ruleNameB = arguments[6];
   difference = arguments[7];
   if (numberOfArguments > 8) Support::LOWER_LIMIT = atoi(arguments[8]);
   if (numberOfArguments > 9) Support::UPPER_LIMIT = atoi(arguments[9]);
   std::cout << "Processing:" << Support::NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Processing:" << Support::SECOND_NETWORK_FILE_NAME << " - Network size:" << Support::NETWORK_SIZE << std::endl;
   std::cout << "Network Limits:" << Support::LOWER_LIMIT << " - " << Support::UPPER_LIMIT << std::endl;
   std::cout << "Number of different examples:" << Support::QTD_EXAMPLES << std::endl;
   std::cout << "Route:" << fileNameA << std::endl;
   std::cout << "Rules:" << ruleNameA << "-" << ruleNameB << std::endl;
#endif /* DOMINANCE */

   std::cout << std::endl;

#ifdef MAIN_DEBUG
   void (Performances::Performance::*reOrderRule)(unsigned int & transmission, unsigned int & lastHole, unsigned int & firstHole, std::vector <char> & holes, std::vector < std::vector <unsigned int> > & neighbors, Performances::PartialOrder * partialOrder);

   //arguments[4][0] = '5';
   switch (arguments[4][0])
   {
      case '1':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::normalRule;
         std::cout << "DESCENDING BREADTH normalRule" << std::endl;
         break;
      case '2':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::normalRule;
         std::cout << "DESCENDING DEPTH normalRule" << std::endl;
         break;
      case '3':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::normalRule;
         std::cout << "ASCENDING BREADTH normalRule" << std::endl;
         break;
      case '4':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::normalRule;
         std::cout << "ASCENDING DEPTH normalRule" << std::endl;
         break;
      case '5':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::antecipationRule;
         std::cout << "DESCENDING BREADTH antecipationRule" << std::endl;
         break;
      case '6':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::antecipationRule;
         std::cout << "DESCENDING DEPTH antecipationRule" << std::endl;
         break;
      case '7':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::antecipationRule;
         std::cout << "ASCENDING BREADTH antecipationRule" << std::endl;
         break;
      case '8':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::antecipationRule;
         std::cout << "ASCENDING DEPTH antecipationRule" << std::endl;
         break;
      case '9':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::antecipationBufferedRule;
         std::cout << "DESCENDING BREADTH antecipationBufferedRule" << std::endl;
         break;
      case 'A':
         Support::ROUTES_SORT_ORDER = Support::DESCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::antecipationBufferedRule;
         std::cout << "DESCENDING DEPTH antecipationBufferedRule" << std::endl;
         break;
      case 'B':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::BREADTH;
         reOrderRule = &Performances::Performance::antecipationBufferedRule;
         std::cout << "ASCENDING BREADTH antecipationBufferedRule" << std::endl;
         break;
      case 'C':
         Support::ROUTES_SORT_ORDER = Support::ASCENDING;
         Support::ROUTES_ENUMERATION_TYPE = Support::DEPTH;
         reOrderRule = &Performances::Performance::antecipationBufferedRule;
         std::cout << "ASCENDING DEPTH antecipationBufferedRule" << std::endl;
         break;
   }
   Support::BUFFER_SIZE = atoi(arguments[5]);
   std::cout << "Buffer:" << Support::BUFFER_SIZE << std::endl;

   std::cout << std::endl;

//   for (unsigned int node = 0; node < Support::NETWORK_SIZE; node++) nodes[node]->printNode();
//   topology->printTopology();
//   for (unsigned int node = 0; node < Support::NETWORK_SIZE; node++) nodes[node]->printNeighborhood();
//   std::cout << std::endl;

   std::cout << "Routes:" << arguments[3] << std::endl;
   Performances::PartialOrder * partialOrder;
   std::vector < Performances::PartialOrder * > * partialOrderCycle;
   Networks::Routing::AllRoutesDiscover * testRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
   Performances::Performance * performance = new Performances::Performance(nodes,Support::NETWORK_SIZE);

   testRouter->loadRoutes(arguments[3]);
   testRouter->printTable(&testRouter->routeTable);
   Networks::Routing::AllRoutesDiscover::routeSort(&testRouter->routeTable, Support::ROUTES_SORT_ORDER);
   performance->loadRoutes(testRouter, Support::ROUTES_ENUMERATION_TYPE);
   performance->initialize();
   partialOrder = performance->initializePartialOrder();

   partialOrder = performance->partialOrderDynamics(partialOrder, reOrderRule);
//   ftime(&startTime);
//   ftime(&endTime);
//   std::cout << Support::COMMENTARY << "duration: " << ((long double) endTime.time) + ((long double) endTime.millitm/1000) - ((long double) startTime.time) - ((long double) startTime.millitm/1000) << " seconds" << std::endl;
//   partialOrder = performance->initializePartialOrder();
//   ftime(&startTime);
//   performance->simpleDinamics(partialOrder);
//   ftime(&endTime);
//   std::cout << Support::COMMENTARY << "duration: " << ((long double) endTime.time) + ((long double) endTime.millitm/1000) - ((long double) startTime.time) - ((long double) startTime.millitm/1000) << " seconds" << std::endl;
//   return 0;

   partialOrderCycle = performance->captureCycle(partialOrder, reOrderRule);

   std::cout << std::endl << "Cycle:" << std::endl;
   for (unsigned int cycleState = 0; cycleState < partialOrderCycle->size(); cycleState++)
   {
      std::cout << "history:" << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << transmission << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << partialOrderCycle->at(cycleState)->at(transmission) << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << partialOrderCycle->at(cycleState)->getBuffer()[transmission] << ",";
      std::cout << std::endl;
   }

   delete partialOrder;
   partialOrder = performance->initializePartialOrder();
   partialOrderCycle = performance->sortCycle(partialOrder, partialOrderCycle, reOrderRule);

   std::cout << std::endl << "Cycle sorted:" << std::endl;
   for (unsigned int cycleState = 0; cycleState < partialOrderCycle->size(); cycleState++)
   {
      std::cout << "history:" << cycleState << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << transmission << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << partialOrderCycle->at(cycleState)->at(transmission) << ",";
      std::cout << std::endl;
      for (unsigned transmission = 0; transmission < partialOrderCycle->at(cycleState)->size(); transmission++)
         std::cout << partialOrderCycle->at(cycleState)->getBuffer()[transmission] << ",";
      std::cout << std::endl;
   }

   std::cout << std::endl;

   delete partialOrder;

   //hibrida com o primeiro do ciclo
   std::cout << "hybridRule" << std::endl;
   for (unsigned int partialOrderCycleMember = 0; partialOrderCycleMember < partialOrderCycle->size(); partialOrderCycleMember++)
   {
      std::cout << partialOrderCycleMember << std::endl;
      partialOrder = partialOrderCycle->at(partialOrderCycleMember);
      partialOrder = performance->partialOrderDynamics(partialOrder, &Performances::Performance::antecipationBufferedRule);
   }

   for (unsigned int partialOrderCycleMember = 0; partialOrderCycleMember < partialOrderCycle->size(); partialOrderCycleMember++)
      delete partialOrderCycle->at(partialOrderCycleMember);
   delete partialOrderCycle;
   //delete partialOrder;
   delete testRouter;
   delete performance;
#endif /* MAIN_DEBUG */

#ifdef ROUTING
   /*comment*/
//   fase um - grafo para todos os menores caminhos que passam por um vizinho apenas
//   fase 2 - ate n caminhos por cada vizinho sem passar pela origem
//   fase 3 - o menor caminho
//   fase 4 - heuristica
//   fase 5 - desempenho

   Networks::Routing::AllRoutesDiscover * router = NULL;
#ifdef ROUTING_DIJKSTRA
   router = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
   router->nDijkstraRoutesDiscover(nodes, Support::NETWORK_FILE_NAME.data());
   delete router;

   fileName = Support::NETWORK_FILE_NAME;
   Networks::Routing::AllRoutesDiscover * dijkstraRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
   dijkstraRouter->loadRoutes(fileName.append(Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX).data());
   dijkstraRouter->routesOrganizer(Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::INFINIT_LIMIT);

   dijkstraRouter->minimumRouteDiscover(Support::NETWORK_FILE_NAME.data());
   dijkstraRouter->nRoutesThroughEachNeighbor(nodes, Support::NETWORK_FILE_NAME.data(), Support::INFINIT_LIMIT);
#endif /* ROUTING_DIJKSTRA */

#ifdef ROUTING_NEIGHBOR_N_EACH
   if (dijkstraRouter == NULL)
   {
      dijkstraRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
      dijkstraRouter->loadRoutes(fileName.append(Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX).data());
   }
   for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
   {
      dijkstraRouter->nRoutesThroughEachNeighbor(nodes, Support::NETWORK_FILE_NAME.data(), limit);

      fileName = Support::NETWORK_FILE_NAME;
      MyStream.str("");
      MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
      fileName.append(Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX);
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());


      router = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
      router->loadRoutes(fileName.data());

      fileName = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
      router->routesOrganizer(Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::INFINIT_LIMIT);
      delete router;
   }
#endif /* ROUTING_NEIGHBOR_N_EACH */

#ifdef ROUTING_DIJKSTRA
   delete dijkstraRouter;
#endif /* ROUTING_DIJKSTRA */

#ifdef ROUTING_NEIGHBOR_N
   fileName = Support::NETWORK_FILE_NAME;
   Networks::Routing::AllRoutesDiscover * neighborsRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
   neighborsRouter->loadRoutes(fileName.append(Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX).data());
   neighborsRouter->routesOrganizer(Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::INFINIT_LIMIT);
   for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
   {
      neighborsRouter->allnNeighborsRoutes(Support::NETWORK_FILE_NAME.data(),limit);
      MyStream.str("");
      MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
      fileName = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
      MyStream.str("");
      MyStream << Support::NETWORK_FILE_NAME << Support::FILE_NAME_SEPARATOR << Support::NAME_PARAMETER << Support::FILE_NAME_SEPARATOR << Support::NAME_PARAMETER << fileName;
      fileName = Support::NETWORK_FILE_NAME + fileName;
      Support::joinNetworkFiles(MyStream.str().data(), fileName.data(), Support::NETWORK_SIZE);
      //std::system(MyStream.str().data());
   }
   delete neighborsRouter;
#endif /* ROUTING_NEIGHBOR_N */

   /* Future!
      Networks::Routing::Heuristics::MWISHeuristic * heuristic = new Networks::Routing::Heuristics::MWISHeuristic();
      for (unsigned int origin = 0; origin < networkSize; origin++)
         for (unsigned int destiny = origin +1; destiny < networkSize; destiny++)
            heuristic->routing(nodes, router, origin, destiny, Support::NETWORK_FILE_NAME);
      delete heuristic;
   */

#endif /* ROUTING */

#ifdef PERFORMANCE
   Networks::Routing::AllRoutesDiscover * minimumRouteRouter = new Networks::Routing::AllRoutesDiscover(Support::NETWORK_SIZE);
   fileName = Support::NETWORK_FILE_NAME;
   minimumRouteRouter->loadRoutes(fileName.append(Support::MINIMUM_ROUTE_TABLE_SUFFIX).data());
   Performances::Performance * performance = new Performances::Performance(nodes,Support::NETWORK_SIZE);

#ifdef PERFORMANCE_NEIGHBOR_N_EACH
   for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
   {
      MyStream.str("");
      MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
      fileName = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
#ifdef PERFORMANCE_NR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::NR_PERF_SUFFIX, &Performances::Performance::normalDynamics);
#endif /* PERFORMANCE_NR */
#ifdef PERFORMANCE_AR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::AR_PERF_SUFFIX, &Performances::Performance::antecipationDynamics);
#endif /* PERFORMANCE_AR */
#ifdef PERFORMANCE_ABR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::ABR_PERF_SUFFIX, &Performances::Performance::antecipationBufferedDynamics);
#endif /* PERFORMANCE_ABR */
#ifdef PERFORMANCE_HR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::HR_PERF_SUFFIX, &Performances::Performance::hybridDynamics);
#endif /* PERFORMANCE_HR */
#ifdef PERFORMANCE_VIZ
      performance->allVIZ(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::VIZING_PERF_SUFFIX);
#endif /* PERFORMANCE_VIZ */
   }
#endif /* PERFORMANCE_NEIGHBOR_N_EACH */

#ifdef PERFORMANCE_NEIGHBOR_N
   for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
   {
      MyStream.str("");
      MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;
      fileName = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
      fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
#ifdef PERFORMANCE_NR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::NR_PERF_SUFFIX, &Performances::Performance::normalDynamics);
#endif /* PERFORMANCE_NR */
#ifdef PERFORMANCE_AR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::AR_PERF_SUFFIX, &Performances::Performance::antecipationDynamics);
#endif /* PERFORMANCE_AR */
#ifdef PERFORMANCE_ABR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::ABR_PERF_SUFFIX, &Performances::Performance::antecipationBufferedDynamics);
#endif /* PERFORMANCE_ABR */
#ifdef PERFORMANCE_HR
      performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::HR_PERF_SUFFIX, &Performances::Performance::hybridDynamics);
#endif /* PERFORMANCE_HR */
#ifdef PERFORMANCE_VIZ
      performance->allVIZ(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), fileName.data(), Support::VIZING_PERF_SUFFIX);
#endif /* PERFORMANCE_VIZ */
   }
#endif /* PERFORMANCE_NEIGHBOR_N */

#ifdef PERFORMANCE_ALL_MINIMUM_PATH
#ifdef PERFORMANCE_NR
   performance->allMinimumPathDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX, Support::NR_PERF_SUFFIX, &Performances::Performance::normalDynamics);
#endif /* PERFORMANCE_NR */
#ifdef PERFORMANCE_AR
   performance->allMinimumPathDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX, Support::AR_PERF_SUFFIX, &Performances::Performance::antecipationDynamics);
#endif /* PERFORMANCE_AR */
#ifdef PERFORMANCE_ABR
   performance->allMinimumPathDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX, Support::ABR_PERF_SUFFIX, &Performances::Performance::antecipationBufferedDynamics);
#endif /* PERFORMANCE_ABR */
#ifdef PERFORMANCE_HR
   performance->allMinimumPathDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX, Support::HR_PERF_SUFFIX, &Performances::Performance::hybridDynamics);
#endif /* PERFORMANCE_HR */
#ifdef PERFORMANCE_VIZ
   performance->allMinimumPathDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX, Support::VIZING_PERF_SUFFIX);
#endif /* PERFORMANCE_VIZ */
#endif /* PERFORMANCE_ALL_MINIMUM_PATH */

#ifdef PERFORMANCE_DIJKSTRA
#ifdef PERFORMANCE_NR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::NR_PERF_SUFFIX, &Performances::Performance::normalDynamics);
#endif /* PERFORMANCE_NR */
#ifdef PERFORMANCE_AR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::AR_PERF_SUFFIX, &Performances::Performance::antecipationDynamics);
#endif /* PERFORMANCE_AR */
#ifdef PERFORMANCE_ABR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::ABR_PERF_SUFFIX, &Performances::Performance::antecipationBufferedDynamics);
#endif /* PERFORMANCE_ABR */
#ifdef PERFORMANCE_HR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::HR_PERF_SUFFIX, &Performances::Performance::hybridDynamics);
#endif /* PERFORMANCE_HR */
#ifdef PERFORMANCE_VIZ
   performance->allVIZ(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX, Support::VIZING_PERF_SUFFIX);
#endif /* PERFORMANCE_VIZ */
#endif /* PERFORMANCE_DIJKSTRA */

#ifdef PERFORMANCE_NEIGHBOR_EACH
#ifdef PERFORMANCE_NR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::NR_PERF_SUFFIX, &Performances::Performance::normalDynamics);
#endif /* PERFORMANCE_NR */
#ifdef PERFORMANCE_AR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::AR_PERF_SUFFIX, &Performances::Performance::antecipationDynamics);
#endif /* PERFORMANCE_AR */
#ifdef PERFORMANCE_ABR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::ABR_PERF_SUFFIX, &Performances::Performance::antecipationBufferedDynamics);
#endif /* PERFORMANCE_ABR */
#ifdef PERFORMANCE_HR
   performance->allDynamics(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::HR_PERF_SUFFIX, &Performances::Performance::hybridDynamics);
#endif /* PERFORMANCE_HR */
#ifdef PERFORMANCE_VIZ
   performance->allVIZ(minimumRouteRouter, Support::NETWORK_FILE_NAME.data(), Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX, Support::VIZING_PERF_SUFFIX);
#endif /* PERFORMANCE_VIZ */
#endif /* PERFORMANCE_NEIGHBOR_EACH */

   delete performance;
   delete minimumRouteRouter;
#endif /* PERFORMANCE */

#ifndef NETWORKING
   bool (Performances::Performance::*filter)(unsigned int * values, unsigned int quantity) = &Performances::Performance::trueFilter;

   #ifdef GRAPHICS
      Performances::Performance * performanceGraphic = new Performances::Performance(NULL,Support::NETWORK_SIZE);

      if (fileName == Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->averageGroupedByXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
         }
      }
      else if (fileName == Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->averageGroupedByXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
         }
      }
      else if (fileName == Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX or fileName == Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX)
      {
         fileName.append(ruleName);
         performanceGraphic->averageGroupedByXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
      }
      delete performanceGraphic;
   #endif /* GRAPHICS */

   #ifdef GRAPHICS_JU
      Performances::Performance * performanceGraphic = new Performances::Performance(NULL,Support::NETWORK_SIZE);

      if (fileName == Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->ju(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), maximumPerformance, minimumPerformance, incrementPerformance, filter);
         }
      }
      else if (fileName == Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->ju(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), maximumPerformance, minimumPerformance, incrementPerformance, filter);
         }
      }
      else if (fileName == Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX or fileName == Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX)
      {
         fileName.append(ruleName);
         performanceGraphic->ju(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), maximumPerformance, minimumPerformance, incrementPerformance, filter);
      }
      delete performanceGraphic;
   #endif /* GRAPHICS_JU */

   #ifdef HISTOGRAM
      Performances::Performance * performanceGraphic = new Performances::Performance(NULL,Support::NETWORK_SIZE);

      if (fileName == Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->histogramXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
         }
      }
      else if (fileName == Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileName = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
            fileName.replace(fileName.find(Support::N), 3, MyStream.str().data());
            fileName.append(ruleName);
            performanceGraphic->histogramXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
         }
      }
      else if (fileName == Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX or fileName == Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX)
      {
         fileName.append(ruleName);
         performanceGraphic->histogramXYcolumns(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileName.data(), groupingColumnX, groupingColumnY, filter);
      }
      delete performanceGraphic;
   #endif /* HISTOGRAM */

#ifdef GRAPHICS_ALL_MINIMUM_PATH
   Performances::Performance * performanceGraphic = new Performances::Performance(NULL,Support::NETWORK_SIZE);

   if (fileName == Support::ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX)
   {
      if (ruleName == Support::ABR_PERF_SUFFIX or ruleName == Support::NR_PERF_SUFFIX)
      {
         fileName.append(ruleName);
         performanceGraphic->averageAllrouteQuantity(Support::NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, runs, fileName.data(), filter);
      }
   }
   delete performanceGraphic;
#endif /* GRAPHICS_ALL_MINIMUM_PATH */

   #ifdef DOMINANCE
      Performances::Performance * performanceTest = new Performances::Performance(NULL,Support::NETWORK_SIZE);

      if (fileNameA == Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileNameA = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
            fileNameA.replace(fileNameA.find(Support::N), 3, MyStream.str().data());
            fileNameA.append(ruleNameA);
            fileNameB = Support::NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX;
            fileNameB.replace(fileNameB.find(Support::N), 3, MyStream.str().data());
            fileNameB.append(ruleNameB);
            performanceTest->dominanceTest(difference.data(), Support::NETWORK_FILE_NAME.data(), Support::SECOND_NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileNameA.data(), fileNameB.data(), filter);
         }
      }
      else if (fileNameA == Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX)
      {
         for (unsigned int limit = Support::LOWER_LIMIT +1; limit <= Support::UPPER_LIMIT; limit++)
         {
            MyStream.str("");
            MyStream << Support::FILE_NAME_SEPARATOR << limit << Support::FILE_NAME_SEPARATOR;

            fileNameA = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
            fileNameA.replace(fileNameA.find(Support::N), 3, MyStream.str().data());
            fileNameA.append(ruleNameA);
            fileNameB = Support::NEIGHBOR_N_ROUTE_TABLE_SUFFIX;
            fileNameB.replace(fileNameB.find(Support::N), 3, MyStream.str().data());
            fileNameB.append(ruleNameB);
            performanceTest->dominanceTest(difference.data(), Support::NETWORK_FILE_NAME.data(), Support::SECOND_NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileNameA.data(), fileNameB.data(), filter);
         }
      }
      else if (fileNameA == Support::DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX or fileNameA == Support::NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX)
      {
         fileNameB = fileNameA;
         fileNameA.append(ruleNameA);
         fileNameB.append(ruleNameB);
         performanceTest->dominanceTest(difference.data(), Support::NETWORK_FILE_NAME.data(), Support::SECOND_NETWORK_FILE_NAME.data(), Support::QTD_EXAMPLES, fileNameA.data(), fileNameB.data(), filter);
      }
      delete performanceTest;
   #endif /* DOMINANCE */
#endif /* NETWORKING */

#ifdef NETWORKING
   delete network;
#endif /* NETWORKING */

   /*
      echo "set datafile commentschars '#';"
      echo "set datafile separator ',';"
      echo "set xrange [-0.1:3250];"
      echo "set yrange [-0.1:1.1];"
      echo "set ytic 0,0.1;"
      echo "set grid noxtics;"
      echo "set terminal fig color size 99 5;"

      echo "set ylabel 'desempenho'"
      echo "set xlabel 'pares origem/destino'"
      echo "set title 'todas as menores rotas';                  set output '${1}.${2}.dijkstra.all.routes.nr.perf.fig';    plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.dijkstra.all.routes.nr.perf' using ($6/$4) title 'Descendente Largura',    './Result/NormalRule/B/${1}/${2}/${1}.${2}.dijkstra.all.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',    './Result/NormalRule/C/${1}/${2}/${1}.${2}.dijkstra.all.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',    './Result/NormalRule/D/${1}/${2}/${1}.${2}.dijkstra.all.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'todas as menores rotas por cada vizinho'; set output '${1}.${2}.each.neighbor.routes.nr.perf.fig';   plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura',   './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',   './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',   './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 1 menores rotas por cada vizinho';     set output '${1}.${2}.each.1.neighbor.routes.nr.perf.fig'; plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.1.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura', './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.1.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade', './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.1.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura', './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.1.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 2 menores rotas por cada vizinho';     set output '${1}.${2}.each.2.neighbor.routes.nr.perf.fig'; plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.2.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura', './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.2.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade', './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.2.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura', './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.2.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 3 menores rotas por cada vizinho';     set output '${1}.${2}.each.3.neighbor.routes.nr.perf.fig'; plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.3.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura', './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.3.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade', './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.3.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura', './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.3.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 4 menores rotas por cada vizinho';     set output '${1}.${2}.each.4.neighbor.routes.nr.perf.fig'; plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.4.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura', './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.4.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade', './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.4.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura', './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.4.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 5 menores rotas por cada vizinho';     set output '${1}.${2}.each.5.neighbor.routes.nr.perf.fig'; plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.each.5.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Largura', './Result/NormalRule/B/${1}/${2}/${1}.${2}.each.5.neighbor.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade', './Result/NormalRule/C/${1}/${2}/${1}.${2}.each.5.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Largura', './Result/NormalRule/D/${1}/${2}/${1}.${2}.each.5.neighbor.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 2 menores rotas de todos os vizinho';  set output '${1}.${2}.neighbor.2.routes.nr.perf.fig';      plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.neighbor.2.routes.nr.perf' using ($6/$4) title 'Descendente Largura',      './Result/NormalRule/B/${1}/${2}/${1}.${2}.neighbor.2.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',      './Result/NormalRule/C/${1}/${2}/${1}.${2}.neighbor.2.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',      './Result/NormalRule/D/${1}/${2}/${1}.${2}.neighbor.2.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 3 menores rotas de todos os vizinho';  set output '${1}.${2}.neighbor.3.routes.nr.perf.fig';      plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.neighbor.3.routes.nr.perf' using ($6/$4) title 'Descendente Largura',      './Result/NormalRule/B/${1}/${2}/${1}.${2}.neighbor.3.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',      './Result/NormalRule/C/${1}/${2}/${1}.${2}.neighbor.3.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',      './Result/NormalRule/D/${1}/${2}/${1}.${2}.neighbor.3.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 4 menores rotas de todos os vizinho';  set output '${1}.${2}.neighbor.4.routes.nr.perf.fig';      plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.neighbor.4.routes.nr.perf' using ($6/$4) title 'Descendente Largura',      './Result/NormalRule/B/${1}/${2}/${1}.${2}.neighbor.4.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',      './Result/NormalRule/C/${1}/${2}/${1}.${2}.neighbor.4.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',      './Result/NormalRule/D/${1}/${2}/${1}.${2}.neighbor.4.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"
      echo "set title 'as 5 menores rotas de todos os vizinho';  set output '${1}.${2}.neighbor.5.routes.nr.perf.fig';      plot './Result/NormalRule/A/${1}/${2}/${1}.${2}.neighbor.5.routes.nr.perf' using ($6/$4) title 'Descendente Largura',      './Result/NormalRule/B/${1}/${2}/${1}.${2}.neighbor.5.routes.nr.perf' using ($6/$4) title 'Descendente Profundidade',      './Result/NormalRule/C/${1}/${2}/${1}.${2}.neighbor.5.routes.nr.perf' using ($6/$4) title 'Ascendente Largura',      './Result/NormalRule/D/${1}/${2}/${1}.${2}.neighbor.5.routes.nr.perf' using ($6/$4) title 'Ascendente Profundidade'"

      echo "set datafile commentschars '#';"
      echo "set datafile separator ',';"
      echo "set xrange [-0.1:30];"
      echo "set yrange [-0.1:1.1];"
      echo "set ytic 0,0.01;"
      echo "set xtic 0,1;"
      echo "set terminal fig color size 99 5;"
      echo "set ylabel 'desempenho'"
      echo "set xlabel '?'"

      echo "plot '?' using 1:2:3 with errorbars, '' using 1:2 with lines title 'titulo'"
   */

   /*comment*///Ce n'est pas necessaire!
//   delete topology;
//   for (unsigned int nodesMember = 0; nodesMember < Support::NETWORK_SIZE; nodesMember++)
//      delete nodes[nodesMember];
//   delete [] nodes;

/*
   int minimumRadioQuantity = std::Support::UPPER_LIMIT;
   int maximumRadioQuantity = std::atoi(arguments[5]);
   Support::OperationalSystem::Linux::Randomic::setSeed();
   unsigned int * numberOfRadios = new unsigned int[Support::NETWORK_SIZE];
   for (unsigned int node = 0; node < Support::NETWORK_SIZE; node++)
      numberOfRadios[node] = Support::OperationalSystem::Linux::Randomic::getRandomic(maximumRadioQuantity - minimumRadioQuantity +1) +1;
*/

   ftime(&endTime);
   std::cout << std::endl << Support::COMMENTARY << "Program ends: "<< std::ctime(&endTime.time);
   std::cout << Support::COMMENTARY << "Program duration: " << ((long double) endTime.time) + ((long double) endTime.millitm/1000) - ((long double) startTime.time) - ((long double) startTime.millitm/1000) << " seconds" << std::endl;
   std::cout << std::endl;
   return 0;
}
