/*
 * Support.hpp
 *
 *  Created on: Jan 26, 2009
 *      Author: fjimenez
 */

#ifndef SUPPORT_HPP_
#define SUPPORT_HPP_

#include <algorithm>
#include <iostream>
#include <ctime>
#include <sys/timeb.h>
#include <cmath>
#include <sstream>

#include "executable.hpp"
#include "Randomic.h"

namespace Support
{
   /*comment*///parametros do algoritmo
   /*comment*///fator de equilibrio na ponderacao do conjunto independente maximo
   extern double ALFA;
   /*comment*///numero de exemplos de uma rede, dado um tamanho fixo
   extern double QTD_EXAMPLES;
   /*comment*///confianca
   extern double CONFIDENCE_FACTOR;
   /*comment*///tamanho do buffer dos vertices
   extern short INITIAL_BUFFER_SIZE;
   extern short FINAL_BUFFER_SIZE;
   extern short BUFFER_SIZE;
   /*comment*///ordem das rotas
   extern bool ROUTES_SORT_ORDER;
   /*comment*///forma de enumeracao das transmissoes
   extern bool ROUTES_ENUMERATION_TYPE;
   /*comment*///alcance do radio
   extern double MINIMUM_DISTANCE;
   /*comment*///nome do arquivo que contem o grafo da rede no formato grafix
   extern std::string NETWORK_FILE_NAME;
   extern std::string SECOND_NETWORK_FILE_NAME;
   /*comment*///quantidade de membros da rede
   extern unsigned int NETWORK_SIZE;
   /*comment*///limites de rotas por vizinhos e limite de rotas
   extern unsigned int UPPER_LIMIT;
   extern unsigned int LOWER_LIMIT;
   /*comment*///quantidade de partial Order Cycle Members que iniciam o algoritmo hibrido
   extern unsigned int LAST_PARTIAL_ORDER_CYCLE_MEMBER;
   extern unsigned int FIRST_PARTIAL_ORDER_CYCLE_MEMBER;
   extern unsigned int MAXIMUM_RUNS;

   extern unsigned int MINIMUM_ROUTE_SIZE;
   extern unsigned int MAXIMUM_ROUTE_SIZE;

   extern std::string LOCAL_OUTPUT_BASE_DIR;

   /*comment*///ruim como escolher uma œnica rota
   static const double DEAD_VALUE = 1/3;

   /*comment*///ordem das rotas
   static const bool ASCENDING = true;
   static const bool DESCENDING = false;

   /*comment*///forma de enumeracao das transmissoes
   static const bool DEPTH = true;
   static const bool BREADTH = false;

   static const bool REMOVE = false;
   static const bool INCLUDE = true;
   static const unsigned int MAX_READS = 1024*256;
   static const unsigned int INVALID_IDENTIFICATION = -1;
   static const unsigned int CIRCULAR_POSISSION_LIMIT = (unsigned int) pow(2,16);
   static const unsigned char REALLOCATION_FACTOR = 2;

   /*comment*///posicao no array, da soma dos pesos dos elementos do conjunto ponderado maximo
   static const unsigned char SUM = 0;
   /*comment*///posicao no array, do menor peso dos elementos do conjunto ponderado maximo
   static const unsigned char MINIMUM = SUM +1;
   /*comment*///numero maximo de rotas por cada vizinho, hard limit, nao e condicao de parada, mas estoura a memoria se ultrapassado (coloque valor alto, mas olha a memoria!)
   //static const unsigned char MAXIMUM_ROUTES_FACTOR = 10;
   static const unsigned int INFINIT_LIMIT = -1;

   static const unsigned int IN_EDGE = 1;//'<';
   static const unsigned int NO_EDGE = 0;//' ';
   static const unsigned int OUT_EDGE = -1;//'>';

   static const char EMPTY_LAYER = ' ';
   static const char NEIGHBOR_IN_LAYER = 'X';

   static const char END_OF_INFORMATION[2] = ";";
   static const char SEPARATOR[2] = ",";
   static const char END_OF_LINE[2] = "\0";
   static const char COMMENTARY[2] = "#";
   static const char NAME_PARAMETER[2] = "*";

   static const char FILE_NAME_SEPARATOR[2] = ".";
   static const char DIRECTORY_NAME_SEPARATOR[2] = "/";
   static const char N[4] = ".n.";

   static const char ALL_ROUTE_TABLE_SUFFIX [12] = ".all.routes";
   static const char DIJKSTRA_ALL_ROUTE_TABLE_SUFFIX [21] = ".dijkstra.all.routes";
   static const char ALL_MINIMUM_PATH_ROUTE_TABLE_SUFFIX [31] = ".all.minimum.path.routes";

   static const char NEIGHBOR_EACH_ROUTE_TABLE_SUFFIX [22] = ".each.neighbor.routes";
   static const char NEIGHBOR_N_EACH_ROUTE_TABLE_SUFFIX [24] = ".each.n.neighbor.routes";
   static const char NEIGHBOR_N_ROUTE_TABLE_SUFFIX [19] = ".neighbor.n.routes";
   static const char MINIMUM_ROUTE_TABLE_SUFFIX[12] = ".min.routes";

   static const char MWIS_ROUTE_TABLE_SUFFIX [14] = ".mwis.routes";

   static const char NR_PERF_SUFFIX [10] = ".nr.perf";
   static const char AR_PERF_SUFFIX [10] = ".ar.perf";
   static const char ABR_PERF_SUFFIX [10] = ".abr.perf";
   static const char HR_PERF_SUFFIX [10] = ".hr.perf";
   static const char VIZING_PERF_SUFFIX [10] = ".viz.perf";

   static const char GRAPHIC_SUFFIX [34] = ".averageGroupedBy.graphic";
   static const char GRAPHIC_JU_SUFFIX [100] = ".histogram.cumulative.normalized.graphic";
   static const char HISTOGRAM_SUFFIX [34] = ".histogram.graphic";
   static const char DOMINANCE_SUFFIX [11] = ".dominance";
   static const char DEGREE_SUFFIX [30] = ".node.degree";
   static const char PATH_SIZE_SUFFIX [11] = ".path.size";
   static const char ROUTE_PER_NODE_SUFFIX [31] = ".per.node.routes.size";
   static const char ROUTE_PER_LINK_SUFFIX [31] = ".per.link.routes.size";
   static const char ROUTE_PER_NEIGHBOR_SUFFIX [31] = ".per.neighbor.routes.size";

   #define circularPosition(position, size) (position) % (size)

   class DebugFile : public std::ofstream
   {
   public:
      std::string name;
      unsigned int limit;
      unsigned int limitCounter;

      DebugFile(std::string name, unsigned int limit):std::ofstream(name.data(), std::ios::out)
      {
         this->name = name;
         this->limit = limit;
         this->limitCounter = 0;
         if (not *this)
         {
            std::cerr << this->name << " not created!" << std::endl;
            exit(1);
         }
      }
      DebugFile & operator ++(int val)
      {
         if (this->limitCounter > this->limit)
         {
            this->limitCounter = 0;
            this->seekp(std::ios::ios_base::beg);
         }
         this->limitCounter++;
         return (*this);
      }
   };

   static unsigned int ** readCSVfile(const char * fileName)
   {
      std::ifstream file(fileName, std::ios::in);
      char in[Support::MAX_READS];
      char value[Support::MAX_READS];
      unsigned int subPosition, columnSize = 0;
      unsigned int tableLineQuantity = 0, tableColumnQuantity = 0;
      unsigned int ** table;

      unsigned int bytesRead, position;

      if (not file)
      {
         std::cerr << fileName << " not found!" << std::endl;
         return NULL; //exit(1);
      }

      /*comments*///computing maximum column size and line quantity
      file.getline(in, Support::MAX_READS);
      bytesRead = file.gcount();
      while(bytesRead > 3)
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
               while (position < bytesRead and in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
               position++;
               subPosition = 0;
               columnSize++;
            }
            while (position < bytesRead and in[position -1] != Support::END_OF_INFORMATION[0]);
            tableLineQuantity++;
            if (tableColumnQuantity < columnSize) tableColumnQuantity = columnSize;
         }
         if (not file.eof())
         {
            file.getline(in, Support::MAX_READS);
            bytesRead = file.gcount();
         }
         else break;
      }
      file.close();

      /*comments*///Allocation of routes
      table = new unsigned int * [tableLineQuantity +1];
      for (unsigned int line = 0; line <= tableLineQuantity; line++)
      {
         table[line] = new unsigned int [tableColumnQuantity +1];
         for (unsigned int column = 0; column <= tableColumnQuantity; column++)
            table[line][column] = Support::INVALID_IDENTIFICATION;
      }

      /*comments*///load routes
      file.open(fileName, std::ios::in);
      for (unsigned int line = 0; line < tableLineQuantity; line++)
      {
         do file.getline(in, Support::MAX_READS);
         while (in[0] == Support::COMMENTARY[0]);
         bytesRead = file.gcount();

         position = 0;
         columnSize=0;
         do
         {
            subPosition = 0;
            do
             value[subPosition++] = in[position++];
            while (position < bytesRead and in[position] != Support::SEPARATOR[0] and in[position] != Support::END_OF_INFORMATION[0]);
            value[subPosition] = Support::END_OF_LINE[0];
            table[line][columnSize] = atoi(value);
            columnSize++;
            position++;
            subPosition = 0;
         }
         while (position < bytesRead and in[position -1] != Support::END_OF_INFORMATION[0]);
      }
      file.close();

      return table;
   }

   static void writeCSVfile(char * baseFileName, unsigned int ** table)
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

      for (unsigned int line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
      {
         file << table[line][0] ;
         for (unsigned int column = 1; table[line][column] != Support::INVALID_IDENTIFICATION; column++)
            file << Support::SEPARATOR << table[line][column];
         file << Support::END_OF_INFORMATION << std::endl;
      }

      file.close();
   }

   static void fixWriteCSVfile(char * baseFileName, unsigned int ** table)
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

/*
      0 minFile << oneRouteRouter->routeTable[0][0] << Support::SEPARATOR <<
      1 oneRouteRouter->routeTable[0][oneRouteRouter->routeTableColumnQuantity -1] << Support::SEPARATOR <<
      2 minimumRouteTableLineHopCount[origin][destiny] << Support::SEPARATOR <<
      3 origin << Support::SEPARATOR <<
      4 destiny << Support::SEPARATOR <<
      5 this->cycleSize << Support::SEPARATOR <<
      6 this->toDestinyTransmissionQuantity << std::endl;

  0   0 file << origin << Support::SEPARATOR <<
  1   1 destiny << Support::SEPARATOR <<
  2   2 minimumRouteTableLineHopCount[origin][destiny] << Support::SEPARATOR <<
    X 3 origin << Support::SEPARATOR <<
    X 4 destiny << Support::SEPARATOR <<
  3   5 this->cycleSize << Support::SEPARATOR <<
  4   6 this->toDestinyTransmissionQuantity << std::endl;
  5     this->toDestinyUniqueTransmissionQuantity
*/

      for (unsigned int line = 0; table[line][0] != Support::INVALID_IDENTIFICATION; line++)
         file << table[line][3] << Support::SEPARATOR
              << table[line][4] << Support::SEPARATOR
              << table[line][2] << Support::SEPARATOR
              << table[line][5] << Support::SEPARATOR
              << table[line][6] << Support::SEPARATOR
              << 0 << Support::END_OF_INFORMATION << std::endl;

      file.close();
   }

   static void joinNetworkFiles(const char * baseFileName, const char * outFileName, unsigned int quantity)
   {
      char in[Support::MAX_READS];
      int bytesRead;
      std::string fileName = outFileName;
      std::string inFileName;
      std::stringstream MyStream;
      std::ifstream inFile("", std::ios::in);
      std::cout << "Creating:" << fileName << std::endl;
      std::ofstream file(fileName.data(), std::ios::out);
      if (not file)
      {
         std::cerr << fileName << " not created!" << std::endl;
         exit(1);
      }

      for (unsigned int fileElement = 0; fileElement < quantity; fileElement++)
      {
         for (unsigned int fileNextElement = fileElement +1; fileNextElement < quantity; fileNextElement++)
         {
            inFileName = baseFileName;
            MyStream.str("");
            MyStream << fileElement;
            inFileName.replace(inFileName.find(Support::NAME_PARAMETER), 1, MyStream.str().data());
            MyStream.str("");
            MyStream << fileNextElement;
            inFileName.replace(inFileName.find(Support::NAME_PARAMETER), 1, MyStream.str().data());
            inFile.open(inFileName.data(), std::ios::in);
            if (not inFile)
            {
               std::cerr << inFileName << " not found!" << std::endl;
               exit(1);
            }

            inFile.getline(in, Support::MAX_READS);
            bytesRead = inFile.gcount();
            while(bytesRead > 0)
            {
               file << in << std::endl;
               inFile.getline(in, Support::MAX_READS);
               bytesRead = inFile.gcount();
            }
            inFile.close();
         }
      }

      file.close();
   }

   static std::vector < std::string > * shell(const char * command, bool output)
   {
      std::cout << "[" << command << "]" << std::endl;
      std::vector < std::string > * result =  new std::vector < std::string > ();
      FILE *fpipe;
      char line[2048];

      if ( !(fpipe = (FILE*)popen(command,"r")) )
      {
         std::cout << "Problems with pipe" << std::endl;
         exit(0);
      }
      else
      {
         sleep(1);
         while (fgets(line, sizeof line, fpipe))
         {
            result->push_back(line);
            result->back().resize(result->back().size() -1);
            if (output) std::cout << "[" << result->back() << "]" << std::endl;
         }
      }
      return result;
   }

   static void jointPerformanceResults(const char * inDirectoryName, const char * outDirectoryName, const char * fileName)
   {
      //find Result/B/1/ -type f | rev | cut -d "/" -f 1 | sort -u | rev
      //find Result/B/1/ -name
      //grep -rL "^100,1" Result/B/1/*
      //grep -n "^[0-9]*,1," Result/B/1/131892/80.27.all.minimum.path.routes.abr.perf | tail -n1 | cut -d ":" -f 1
      //head -n

      unsigned int lastRun;
      std::stringstream MyStream;
      std::string listCommand, fullFileName;
      listCommand = "find ";
      listCommand += inDirectoryName;
      listCommand += " -type f -name '";
      listCommand += fileName;
      listCommand += "' | rev | cut -d '/' -f 1 | sort -u | rev";

      std::vector < std::string > * fileList = Support::shell(listCommand.data(), true);
      std::vector < std::string > * filesCopy;
      std::vector < std::string > * lastLine;

      for (unsigned int file = 0; file < fileList->size(); file++)
      {
         listCommand += outDirectoryName;
         listCommand += fileList->at(file);
         listCommand += ".tmp";
         std::cout << listCommand << std::endl;
         remove(listCommand.data());
         listCommand = "find ";
         listCommand += inDirectoryName;
         listCommand += " -name " + fileList->at(file);
         filesCopy = Support::shell(listCommand.data(), true);
         for (unsigned int fileCopy = 0; fileCopy < filesCopy->size(); fileCopy++)
         {
            listCommand = "grep -n '^[0-9]*,1,' " + filesCopy->at(fileCopy) + " | tail -n1 | cut -d ':' -f 1";
            lastLine = Support::shell(listCommand.data(), true);
            if (lastLine->size() > 0)
            {
               listCommand = "head -n " + lastLine->at(0) + " " + filesCopy->at(fileCopy) + " >> ";
               listCommand += outDirectoryName;
               listCommand += fileList->at(file);
               listCommand += ".tmp";
               std::cout << listCommand << std::endl;
               system(listCommand.data());
            }
            delete lastLine;
            listCommand = filesCopy->at(fileCopy).substr(0,filesCopy->at(fileCopy).find_last_of(Support::DIRECTORY_NAME_SEPARATOR));
            remove(listCommand.data());
         }

         listCommand = "find ";
         listCommand += outDirectoryName;
         listCommand += " -depth 1 -name ";
         listCommand += fileList->at(file);
         lastLine = Support::shell(listCommand.data(), true);
         if (lastLine->size() == 0) lastRun = 0;
         else
         {
            delete lastLine;
            listCommand = "tail -n1 ";
            listCommand += outDirectoryName;
            listCommand += fileList->at(file);
            listCommand += " | cut -d ',' -f 1";
            lastLine = Support::shell(listCommand.data(), true);
            lastRun = std::atoi(lastLine->at(0).data()) +1;
         }
         delete lastLine;

         listCommand = "cat ";
         listCommand += outDirectoryName;
         listCommand += fileList->at(file);
         listCommand += ".tmp | grep -v Performance";
         lastLine = Support::shell(listCommand.data(), false);
         for (unsigned int line = 0; line < lastLine->size(); line++)
         {
            MyStream.str("");
            MyStream << "echo '" << lastRun << lastLine->at(line).substr(lastLine->at(line).find_first_of(Support::SEPARATOR));
            listCommand = MyStream.str();
            listCommand += "' >> ";
            listCommand += outDirectoryName;
            listCommand += fileList->at(file);
            //std::cout << listCommand << std::endl;
            system(listCommand.data());
            //std::cout << lastLine->at(line).substr(lastLine->at(line).find_first_of(Support::SEPARATOR)).substr(0,3) << std::endl;
            if (lastLine->at(line).substr(lastLine->at(line).find_first_of(Support::SEPARATOR)).substr(0,3).compare(",1,") == 0) lastRun++;
         }

         delete lastLine;
         delete filesCopy;
         listCommand = "rm -f ";
         listCommand += outDirectoryName;
         listCommand += fileList->at(file);
         listCommand += ".tmp";
         system(listCommand.data());
         std::cout << listCommand << std::endl << std::endl;
      }
      delete fileList;
   }

}

#endif /* SUPPORT_HPP_ */
