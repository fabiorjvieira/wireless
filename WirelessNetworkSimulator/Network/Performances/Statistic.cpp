/*
 * Statistic.cpp
 *
 *  Created on: Nov 29, 2008
 *      Author: fjimenez
 */

#include "Statistic.h"

namespace Performances {

unsigned int Statistic::edgeQuantity(Networks::Node ** nodes, unsigned int networkSize)
{
   unsigned int edgeQuantity = 0;

   for (unsigned int node = 0; node < networkSize; node++)
   {
      for(unsigned int neighbor = node +1; neighbor < networkSize; neighbor++)
      {
         if (nodes[node]->isNeighbor(neighbor)) edgeQuantity++;
      }
   }

   return edgeQuantity;
}

void Statistic::neighborhoodQuantities(Networks::Node ** nodes, unsigned int ** quantities, unsigned int networkSize)
{
   unsigned int * neighbors             = new unsigned int [networkSize],
                * neighborsOfNeighbor   = new unsigned int [networkSize];
   for (unsigned int node = 0; node < networkSize; node++)
   {
      neighbors[node] = Support::INVALID_IDENTIFICATION;
      neighborsOfNeighbor[node] = Support::INVALID_IDENTIFICATION;
   }


   for (unsigned int node = 0; node < networkSize; node++)
   {
      quantities[0][node] = 0;
      quantities[1][node] = 0;
      quantities[2][node] = 0;
      quantities[3][node] = 0;
      quantities[4][node] = 0;
      quantities[5][node] = 0;
      quantities[6][node] = 0;
      for(unsigned int neighbor = 0; neighbor < networkSize; neighbor++)
      {
         if (neighbor != node and nodes[node]->isNeighbor(neighbor))
         {
            //vizinhos
            quantities[0][node]++;
            for (unsigned int neighborOfneighbor = 0; neighborOfneighbor < networkSize; neighborOfneighbor++)
            {
               if (nodes[node]->getNeighborhood()[neighbor][neighborOfneighbor] and neighborOfneighbor != node and neighborOfneighbor != neighbor)
               {
                  if (neighborsOfNeighbor[neighborOfneighbor] != node)
                  {
                     //vizinhos + vizinhos dos vizinhos.
                     quantities[4][node]++;
                     neighborsOfNeighbor[neighborOfneighbor] = node;
                  }
                  if (nodes[node]->isNeighbor(neighborOfneighbor))
                  {
                     if (neighbors[neighborOfneighbor] != node)
                     {
                        //vizinhos.
                        quantities[5][node]++;
                        neighbors[neighborOfneighbor] = node;
                     }
                  }
                  else
                  {
                     //acho que tem o acerto 'e
                     //arestas entre vizinhos e os vizinhos dos vizinhos
                     quantities[3][node]++;
                     if (neighbors[neighborOfneighbor] != node)
                     {
                        //vizinhos dos vizinhos.
                        quantities[1][node]++;
                        neighbors[neighborOfneighbor] = node;
                     }
                  }
               }
            }
            for (unsigned int neighborOfneighbor = neighbor+1; neighborOfneighbor < networkSize; neighborOfneighbor++)
            {
               if (nodes[node]->getNeighborhood()[neighbor][neighborOfneighbor] and neighborOfneighbor != node and neighborOfneighbor != neighbor)
               {
                  //arestas entre vizinhos + entre vizinhos e vizinhos dos vizinhos
                  quantities[6][node]++;
                  //arestas entre vizinhos
                  if (nodes[node]->isNeighbor(neighborOfneighbor)) quantities[2][node]++;
                  //arestas entre vizinhos e os vizinhos dos vizinhos
                  //else quantities[3][node]++;
               }
            }
         }
      }
   }

   delete [] neighbors;
   delete [] neighborsOfNeighbor;
}

Networks::SpacialPosition Statistic::averageNodePosition(Networks::Node ** nodes, unsigned int networkSize)
{
   Networks::SpacialPosition average;
   average.x = 0; average.y = 0; average.z = 0;

   for (unsigned int node = 0; node < networkSize; node++)
   {
      average.x += nodes[node]->getPosition().x;
      average.y += nodes[node]->getPosition().y;
      average.z += nodes[node]->getPosition().z;
   }
   average.x /= networkSize;
   average.y /= networkSize;
   average.z /= networkSize;

   return average;
}

Networks::SpacialPosition Statistic::stdNodePosition(Networks::Node ** nodes, Networks::SpacialPosition average, unsigned int networkSize)
{
   Networks::SpacialPosition std;
   std.x = 0; std.y = 0; std.z = 0;

   for (unsigned int node = 0; node < networkSize; node++)
   {
      std.x += std::pow(average.x - nodes[node]->getPosition().x,2);
      std.y += std::pow(average.y - nodes[node]->getPosition().y,2);
      std.z += std::pow(average.z - nodes[node]->getPosition().z,2);
   }
   std.x = std::sqrt(std.x/(networkSize -1));
   std.y = std::sqrt(std.y/(networkSize -1));
   std.z = std::sqrt(std.z/(networkSize -1));

   return std;
}

double Statistic::average(double * values, unsigned int size)
{
   double average = 0;

   for (unsigned int element = 0; element < size; element++)
      average += values[element];

   return average/size;
}

double Statistic::average(unsigned int * values, unsigned int size)
{
   double average = 0;

   for (unsigned int element = 0; element < size; element++)
      average += values[element];

   return average/size;
}

double Statistic::std(double * values, double average, unsigned int size)
{
   double std = 0;

   for (unsigned int element = 0; element < size; element++)
      std += std::pow(average - values[element],2);
   std /= (size -1);

   return std::sqrt(std);
}

double Statistic::std(unsigned int * values, double average, unsigned int size)
{
   double std = 0;

   for (unsigned int element = 0; element < size; element++)
      std += std::pow(average - values[element],2);
   std /= (size -1);

   return std::sqrt(std);
}

double Statistic::scaling(double value, double average, double std)
{
   if (std > 0)
      return (value - average)/std;
   else return 0;
}

double ** Statistic::histogram(double ** values, unsigned int size, unsigned int length)
{
   double * distances = new double[size*(size -1)], ** histogram = new double *  [2];
   histogram[0] = new double [size*(size-1)]; histogram[1] = new double [size*(size-1)];
   unsigned int counter = 0;

   for (unsigned int element = 0; element < size; element++)
   {
      for (unsigned int subElement = element +1; subElement < size; subElement++)
      {
         distances[counter] = 0;
         for (unsigned int dimension = 0; dimension < length; dimension++ )
         {
            distances[counter] += std::pow(values[dimension][element] - values[dimension][subElement], 2);
         }
         distances[counter] = std::sqrt(distances[counter]);
         counter++;
      }
   }

   counter = 0;
   histogram[0][counter] = 0; histogram[1][counter] = 0;
   for (unsigned int element = 0; element < size*(size -1); element++)
      if (distances[element] == 0)
         histogram[1][counter]++;
   counter++;

   for (unsigned int element = 0; element < size*(size -1); element++)
   {
      if (distances[element] != 0)
      {
         histogram[0][counter] = distances[element]; histogram[1][counter] = 1;
         for (unsigned int subElement = element +1; subElement < size; subElement++)
         {
            if (distances[subElement] == distances[element])
            {
               histogram[1][counter]++;
               distances[subElement] = 0;
            }
         }
         distances[element] = 0;
         counter++;
      }
   }
   histogram[0][counter] =  0;

   delete [] distances;
   return histogram;
}

double Statistic::std(std::vector <double> values, double average, unsigned int size)
{
   if (size == 1) return 0;
   double std = 0;

   for (unsigned int element = 0; element < size; element++)
      std += std::pow(average - values[element],2);
   std /= (size -1);

   return std::sqrt(std);
}

double Statistic::confidence(double confidenceFactor, double std, unsigned int size)
{
   return (invstudenttdistribution(size, confidenceFactor) * std)/std::sqrt((double)size);
}

}

/*
 * Pr�
 * PCA, zerar media, normaliza a disper��o, histograma
 * qtd_vizinhos/qtd_radios|qtd_freq_ort, destino: dentro do cluster/fora/por dentro
 * P�s
 * retirar elementos que n�o s�o membros do cluster, histograma
 */
