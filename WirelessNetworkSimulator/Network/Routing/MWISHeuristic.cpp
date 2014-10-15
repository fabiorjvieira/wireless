/*
 * MWISHeuristic.cpp
 *
 *  Created on: Feb 7, 2009
 *      Author: fjimenez
 */

#include "MWISHeuristic.h"

namespace Networks {

namespace Routing {

namespace Heuristics {

MWISHeuristic::MWISHeuristic() {
   // TODO Auto-generated constructor stub

}

MWISHeuristic::~MWISHeuristic() {
   // TODO Auto-generated destructor stub
}

void MWISHeuristic::routing(Networks::Node ** nodes, Networks::Routing::AllRoutesDiscover * router, unsigned int origin, unsigned int destiny, char * baseFileName)
{
   std::string fileName = baseFileName;
   std::stringstream MyStream;

   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   MyStream << origin;
   fileName.append((MyStream).str());
   fileName.append(Support::FILE_NAME_SEPARATOR);
   MyStream.str("");
   (MyStream) << destiny;
   fileName.append((MyStream).str());

   fileName.append(Support::MWIS_ROUTE_TABLE_SUFFIX);
   std::cout << "Creating:" << fileName << std::endl;
   std::ofstream file(fileName.data(), std::ios::out);
   if (not file)
   {
      std::cerr << fileName << " not created!" << std::endl;
      exit(1);
   }

   //Networks::Node ** nodes = router->nodes;
   unsigned int  lineQuantity = router->routeTable.size();
   std::vector < std::vector <unsigned int> > * routeTable = & router->routeTable;
   unsigned int * activeLines = new unsigned int[lineQuantity], bucketMember;
   long int lastActiveLine = 0, actualActiveLine = 0, pathNodeMember;
   unsigned int networkSize = router->networkSize;
   unsigned int * maximumWeightedIndependentSet;

   long int lastPathNodeQueue = 0;
   unsigned int * pathNodeQueue = new unsigned int[networkSize];
   Networks::Routing::BucketNextHop ** pathNode = new Networks::Routing::BucketNextHop * [networkSize];
   /*comment*///allocate too less memory
   for (unsigned int line = 0; line < networkSize; line++) pathNode[line] = NULL;

   std::cout << std::endl << Support::COMMENTARY << "MWIS routing " << origin << "-" << destiny << std::endl;
   file  << Support::COMMENTARY << "MWIS routing table" << std::endl;

   /*comment*///identify origin to destiny lines and hops quantity
   for (unsigned int line = 0; line < lineQuantity; line++)
      if ((*routeTable)[line][0] == origin)
         if ((*routeTable)[line].back() == destiny)
            activeLines[lastActiveLine++] = line;
   lastActiveLine--;

   /*comment*///build BucketNextHop structure for witch path in the current column of the route table
   for (unsigned int column = 0; column < router->maximumColumnQuantity -1 and lastPathNodeQueue >= 0; column++)
   {
      actualActiveLine = 0;
      lastPathNodeQueue = -1;
      for (unsigned int line; actualActiveLine <= lastActiveLine; actualActiveLine++)
      {
         line = activeLines[actualActiveLine];
         if (pathNode[(*routeTable)[line][column]] == NULL)
         {
            pathNodeQueue[++lastPathNodeQueue] = (*routeTable)[line][column];
            pathNode[(*routeTable)[line][column]] = new Networks::Routing::BucketNextHop;
            pathNode[(*routeTable)[line][column]]->lastNextHops = 0;
            pathNode[(*routeTable)[line][column]]->nextHopsQueue = new unsigned int [networkSize];
            pathNode[(*routeTable)[line][column]]->nextHops = new Networks::Routing::NextHop [networkSize];
            for (unsigned int deep = 0; deep < networkSize; deep++)
            {
               pathNode[(*routeTable)[line][column]]->nextHops[deep].hopCount = NULL;
               pathNode[(*routeTable)[line][column]]->nextHops[deep].lastHopCount = 0;
            }
            pathNode[(*routeTable)[line][column]]->nextHopsQueue[pathNode[(*routeTable)[line][column]]->lastNextHops] = (*routeTable)[line][column +1];
            pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].hopCount = new unsigned int [lastActiveLine +1];
            pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].routeTableLine = new unsigned int [lastActiveLine +1];
         }
         else if (pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].hopCount == NULL)
         {
               pathNode[(*routeTable)[line][column]]->nextHopsQueue[++pathNode[(*routeTable)[line][column]]->lastNextHops] = (*routeTable)[line][column +1];
               pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].hopCount = new unsigned int [lastActiveLine +1];
               pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].routeTableLine = new unsigned int [lastActiveLine +1];
         }
         else pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].lastHopCount++;
         pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].hopCount[pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].lastHopCount] = (*routeTable)[line].size() - column -1;
         pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].routeTableLine[pathNode[(*routeTable)[line][column]]->nextHops[(*routeTable)[line][column +1]].lastHopCount] = line;
         if ((*routeTable)[line][column +1] == destiny)
         {
            activeLines[actualActiveLine] = activeLines[lastActiveLine--];
            actualActiveLine--;
         }
      }

      /*debug*/
      pathNodeMember = 0;
      std::cout << std::endl << "Column:" << column << std::endl;
      for (unsigned int line; pathNodeMember <= lastPathNodeQueue; pathNodeMember++)
      {
         line = pathNodeQueue[pathNodeMember];
         std::cout << "Origin:" << line << std::endl;
         bucketMember = 0;
         for (unsigned int pathMember; bucketMember <= pathNode[line]->lastNextHops; bucketMember++)
         {
            pathMember = pathNode[line]->nextHopsQueue[bucketMember];
            std::cout << "Possible destiny:" << pathMember << " Hops:";
            for (unsigned int hopMembers = 0; hopMembers <= pathNode[line]->nextHops[pathMember].lastHopCount; hopMembers++)
               std::cout << pathNode[line]->nextHops[pathMember].hopCount[hopMembers] << Support::SEPARATOR;
            std::cout << std::endl;
         }
      }
      /*debug*/

      /*comment*///search for the maximum weighted independent set of neighbors, for witch member of pathNodeQueue
      /*comment*///mark chosen ones and eliminate others from activeLines
      /*comment*///chose best ones and remove others (blacklist 1) (blacklist 2 must be include in the choose of MWIS)
      //made a list of no chosed = blacklist 1
      //par groupe de deux, trouvez les meme voisins
      pathNodeMember = 0;
      lastActiveLine = -1;
      for (unsigned int line; pathNodeMember <= lastPathNodeQueue; pathNodeMember++)
      {
         line = pathNodeQueue[pathNodeMember];
         maximumWeightedIndependentSet = MWISHeuristic::maximumWeightedIndependentSet(pathNode[line],nodes);
         for (unsigned int maximumWeightedIndependentSetMember = 0; maximumWeightedIndependentSetMember <= pathNode[line]->lastNextHops and maximumWeightedIndependentSet[maximumWeightedIndependentSetMember] != Support::INVALID_IDENTIFICATION; maximumWeightedIndependentSetMember++)
         {
            if (maximumWeightedIndependentSet[maximumWeightedIndependentSetMember] == destiny)
            {
               for (unsigned int hopCountMember = 0; hopCountMember <= pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].lastHopCount; hopCountMember++)
               {
                  file << (*routeTable)[pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].routeTableLine[hopCountMember]][0];
                  for (unsigned int pathMember = 1; pathMember < (*routeTable)[pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].routeTableLine[hopCountMember]].size(); pathMember++)
                     file << Support::SEPARATOR << (*routeTable)[pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].routeTableLine[hopCountMember]][pathMember];
                  file << Support::END_OF_INFORMATION << std::endl;
               }
            }
            else
            for (unsigned int hopCountMember = 0; hopCountMember <= pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].lastHopCount; hopCountMember++)
               activeLines[++lastActiveLine] = pathNode[line]->nextHops[maximumWeightedIndependentSet[maximumWeightedIndependentSetMember]].routeTableLine[hopCountMember];
         }
         delete [] maximumWeightedIndependentSet;
      }

      /*comment*///free memory
      pathNodeMember = 0;
      for (unsigned int line; pathNodeMember <= lastPathNodeQueue; pathNodeMember++)
      {
         line = pathNodeQueue[pathNodeMember];
         bucketMember = 0;
         for (unsigned int pathMember; bucketMember <= pathNode[line]->lastNextHops; bucketMember++)
         {
            pathMember = pathNode[line]->nextHopsQueue[bucketMember];
            delete [] pathNode[line]->nextHops[pathMember].hopCount;
            delete [] pathNode[line]->nextHops[pathMember].routeTableLine;
         }
         delete [] pathNode[line]->nextHops;
         delete [] pathNode[line]->nextHopsQueue;
         delete pathNode[line];
         pathNode[line] = NULL;
      }
   }

   file.close();
   delete [] pathNode;
   delete [] pathNodeQueue;
   delete [] activeLines;
}

unsigned int * MWISHeuristic::maximumWeightedIndependentSet(Networks::Routing::BucketNextHop * nextHops, Networks::Node ** nodes)
{
   unsigned int candidateSetSize = nextHops->lastNextHops +1;
   unsigned int * candidates = new unsigned int [candidateSetSize];
   double candidatesScore[2] = {0,0}, bestCandidatesScore[2] = {0,0};
   unsigned int * bestCandidatesIdentifications = new unsigned int [candidateSetSize];
   long int lastCandidate = 0, lastBestCandidate = -1;
   unsigned int next, nextCandidateIdentification, currentCandidateIdentification, lastCandidateIdentification;
   bool neighbor;

   for (unsigned int current = 0; current < candidateSetSize -1; current++)
   {
      lastCandidate = 0;
      candidates[lastCandidate] = current;
      next = current +1;
      MWISHeuristic::scoreHeuristics(candidatesScore, nextHops->nextHops[nextHops->nextHopsQueue[0]], Support::INCLUDE);
      /*debug*///std::cout << current << ";" <<std::endl;
      while (lastCandidate >= 0)
      {
         if (next < candidateSetSize)
         {
            nextCandidateIdentification = nextHops->nextHopsQueue[next];
            neighbor = false;
            for (long int candidateMember = 0; not neighbor and candidateMember <= lastCandidate; candidateMember++)
            {
               /*comment*///to do: colocar backlist 1 e 2 infuenciando a escolha dos vizinhos!!!
               currentCandidateIdentification = nextHops->nextHopsQueue[candidates[candidateMember]];
               neighbor = nodes[currentCandidateIdentification]->isNeighbor(nextCandidateIdentification);
            }
            if (not neighbor)
            {
               MWISHeuristic::scoreHeuristics(candidatesScore, nextHops->nextHops[nextCandidateIdentification], Support::INCLUDE);
               candidates[++lastCandidate] = next;
            }
            next++;
         }
         else
         {
            /*debug* /
            for (long int candidateMember = 0; candidateMember <= lastCandidate -1; candidateMember++)
               std::cout << candidates[candidateMember] << Support::SEPARATOR;
            std::cout << candidates[lastCandidate] << Support::END_OF_INFORMATION;
            std::cout << std::endl;
            / *debug*/

            /*comment*///compare with the best one
            if (MWISscore(bestCandidatesScore,Support::ALFA) < MWISscore(candidatesScore,Support::ALFA) or (MWISscore(bestCandidatesScore,Support::ALFA) == MWISscore(candidatesScore,Support::ALFA) and lastBestCandidate < lastCandidate))
            {
               bestCandidatesScore[Support::SUM] = candidatesScore[Support::SUM];
               bestCandidatesScore[Support::MINIMUM] = candidatesScore[Support::MINIMUM];
               lastBestCandidate = lastCandidate;
               for (long int candidateMember = 0; candidateMember <= lastCandidate; candidateMember++)
                  bestCandidatesIdentifications[candidateMember] = nextHops->nextHopsQueue[candidates[candidateMember]];
            }

            lastCandidateIdentification = nextHops->nextHopsQueue[candidates[lastCandidate]];
            MWISHeuristic::scoreHeuristics(candidatesScore, nextHops->nextHops[lastCandidateIdentification], Support::REMOVE);
            lastCandidate--;
            if (lastCandidate > 0)
            {
               next = candidates[lastCandidate] +1;
               lastCandidate--;
            }
         }
      }
   }
   MWISHeuristic::scoreHeuristics(candidatesScore, nextHops->nextHops[nextHops->nextHopsQueue[nextHops->lastNextHops]], Support::INCLUDE);
   if (MWISscore(bestCandidatesScore,Support::ALFA) < MWISscore(candidatesScore,Support::ALFA) or (MWISscore(bestCandidatesScore,Support::ALFA) == MWISscore(candidatesScore,Support::ALFA) and lastBestCandidate < lastCandidate))
   {
      bestCandidatesScore[Support::SUM] = candidatesScore[Support::SUM];
      bestCandidatesScore[Support::MINIMUM] = candidatesScore[Support::MINIMUM];
      lastBestCandidate = 0;
      bestCandidatesIdentifications[0] = nextHops->nextHopsQueue[nextHops->lastNextHops];
   }

   if (lastBestCandidate < candidateSetSize -1) bestCandidatesIdentifications[lastBestCandidate +1] = Support::INVALID_IDENTIFICATION;

   delete [] candidates;
   return bestCandidatesIdentifications;
}

void MWISHeuristic::scoreHeuristics(double candidatesScore[2], Networks::Routing::NextHop nextHop, bool action)
{
   unsigned int minimum = Support::INVALID_IDENTIFICATION;
   if (action == Support::INCLUDE)
   {
      for (unsigned int nextHopMember = 0; nextHopMember <= nextHop.lastHopCount; nextHopMember++)
      {
         candidatesScore[Support::SUM] += 1/(double)(nextHop.hopCount[nextHopMember]);
         if (minimum > nextHop.hopCount[nextHopMember])
            minimum = nextHop.hopCount[nextHopMember];
      }
      candidatesScore[Support::MINIMUM] += 1/(double)minimum;
   }
   else
   {
      for (unsigned int nextHopMember = 0; nextHopMember <= nextHop.lastHopCount; nextHopMember++)
      {
            candidatesScore[Support::SUM] -= 1/(double)(nextHop.hopCount[nextHopMember]);
            if (minimum > nextHop.hopCount[nextHopMember])
               minimum = nextHop.hopCount[nextHopMember];
      }
      candidatesScore[Support::MINIMUM] -= 1/(double)minimum;
   }
}

}

}

}

