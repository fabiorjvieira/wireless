/*
 * main.cpp
 *
 *  Created on: Aug 1, 2014
 *      Author: fabiorjvieira
 */

#include "main.hpp"
//g++ main.cpp -o simulator.bin -lpthread
//unique identifier for thread comm JOB_ID = 69
//maximum interval = 1000
//shared memory size 1024 memory segments (depends on the size of the SO segments)
//simulator.bin examples/example01.Agents examples/configuration01.WirelessParameters examples/licensedEvents01.Queue examples/cognitiveRequests01.Queue 1000 69 1024

int cogReq(int argc, const char **argv)
{
   AgentList * agentList;
   std::string agentListFileName = argv[1];
   std::string cognitiveRequestQueueFileName = argv[2];
   double intervalExpectedValue = atof(argv[3]);
   double deltaIntervalExpectedValue  = atof(argv[4]);
   unsigned int maxInterval = atoi(argv[5]);

   agentList = loadAgents(agentListFileName);
   saveCognitiveRequestQueue(cognitiveRequestQueueFileName, agentList->cognitiveAntennas, agentList->cognitiveClients, maxInterval, intervalExpectedValue, deltaIntervalExpectedValue);

   return 0;
}

int main(int argc, const char **argv)
{
   //return cogReq(argc, argv);
   
   //memory
   IPCkit * ipcKit = new IPCkit;
   char * sharedMemory;

   //file names
   std::string agentListFileName;
   std::string wirelessParametersFileName;
   std::string licensedQueueFileName;
   std::string cognitiveRequestQueueFileName;

   //agents
   AgentList * agentList;
   //parameters
   WirelessParameters * wirelessParametersList;
   //queues
   LicensedQueue licensedQueue;
   CognitiveRequestQueue cognitiveRequestQueue;
   CognitiveQueue cognitiveQueue;
   SensoringQueue sensoringQueue;
   SuccessQueue cognitiveSuccessQueue;
   SuccessQueue licensedSuccessQueue;
   //filePointer
   std::streampos cognitiveRequestQueueFilePosition;
   std::streampos licensedQueueFilePosition;

   //last interval
   unsigned long long int maxInterval;
   //current interval
   unsigned long long int interval;

   if (argc != 8)
   {
      std::cout << "Seven arguments are necessary." << std::endl;
      std::cout << "Agent list file." << std::endl;
      std::cout << "Wireless parameter file." << std::endl;
      std::cout << "Licensed event queue file." << std::endl;
      std::cout << "Cognitive requests file." << std::endl;
      std::cout << "Last interval." << std::endl;
      std::cout << "Semaphore key (JOB_ID)." << std::endl;
      std::cout << "Memory size in segments." << std::endl;
      return -1;
   }

   //memory init
   ipcKit->memoryKey = atoi(argv[6]);
   ipcKit->memorySize = atoi(argv[7]);
   ipcKit->semaphoreSimName << SIMULATOR_PREFIX << std::setw(KEY_SIZE) << std::setfill('0') << ipcKit->memoryKey;
   ipcKit->semaphoreCogName << COGNATOR_PREFIX << std::setw(KEY_SIZE) << std::setfill('0') << ipcKit->memoryKey;

   //release/initialize memory/semaphores
   reset(ipcKit);
   ipcKit = init(ipcKit);

   //load parameters
   agentListFileName = argv[1];
   wirelessParametersFileName = argv[2];
   licensedQueueFileName = argv[3];
   cognitiveRequestQueueFileName = argv[4];
   maxInterval = atoi(argv[5]);

   //load agent list
   agentList = loadAgents(agentListFileName);
   //load wireless parameters
   wirelessParametersList = loadWirelessParametersList(wirelessParametersFileName);

   interval=0;
   //clear and load the first licensed events(interval)
   licensedQueue.events.clear();
   loadLicensedQueue(licensedQueueFileName, licensedQueueFilePosition, licensedQueue, *agentList, interval);
   //clear and load the first cognitive request(interval+1)
   cognitiveRequestQueue.request.clear();
   loadCognitiveRequestQueue(cognitiveRequestQueueFileName, cognitiveRequestQueueFilePosition, cognitiveRequestQueue, *agentList, interval+1);

   while (interval <= maxInterval)
   {
      //clear and calculates success queue(interval) based on cognitive queue(interval) and licensed queue(interval)
      cognitiveSuccessQueue.events.clear();
      cognitiveSuccessQueue.success.clear();
      cognitiveSuccessQueueCalculus(cognitiveSuccessQueue, licensedQueue, cognitiveQueue, interval, wirelessParametersList);

      if (interval < maxInterval)
      {
         //clear and calculates sensor queue(interval) based on cognitive queue(interval) and licensed queue(interval)
         sensoringQueue.events.clear();
         sensoringQueue.risk.clear();
         sensoringQueueCalculus(agentList, sensoringQueue, cognitiveQueue, licensedQueue, interval, wirelessParametersList);

         //write into shared memory cognitive success queue(interval), cognitive requests(interval+1) and sensor queue(interval)
         sharedMemory = ipcKit->sharedMemory;
         sharedMemory = writeSensoringQueue(ipcKit->memoryKey, sharedMemory, sensoringQueue, interval);
         sharedMemory = writeCognitiveSuccessQueue(ipcKit->memoryKey, sharedMemory, cognitiveSuccessQueue, interval);
         sharedMemory = writeCognitiveRequestQueue(ipcKit->memoryKey, sharedMemory, cognitiveRequestQueue, interval+1);

         //release cognitive module
         sem_post(ipcKit->semaphoreCog);
      }

//the beginning of parallel tasks
      //save current sensoring queue(interval)
      saveSensoringQueue(sensoringQueueFileName, sensoringQueue, interval);
      //save previous cognitive queue(interval-1)
      saveCognitiveQueue(cognitiveQueueFileName, cognitiveQueue, interval-1);

      //clear and calculates success queue(interval) based on cognitive queue(interval) and licensed queue(interval)
      licensedSuccessQueue.events.clear();
      licensedSuccessQueue.success.clear();
      licensedSuccessQueueCalculus(licensedSuccessQueue, licensedQueue, cognitiveQueue, interval, wirelessParametersList);
      saveSuccessQueue(licensedSuccessQueueFileName,licensedSuccessQueue, interval);

      //clear and load the next licensed events(interval+1)
      licensedQueue.events.clear();
      loadLicensedQueue(licensedQueueFileName, licensedQueueFilePosition, licensedQueue, *agentList, interval+1);

      //clear and load the next cognitive request(interval+2)
      cognitiveRequestQueue.request.clear();
      loadCognitiveRequestQueue(cognitiveRequestQueueFileName, cognitiveRequestQueueFilePosition, cognitiveRequestQueue, *agentList, interval+2);

     if (interval < maxInterval)
     {
         //clear cognitive queue
         cognitiveQueue.events.clear();
//the end of parallel tasks

         //waiting for cognitive module
         std::cout << "Wating for cognitive module..." << std::endl;
         sem_wait(ipcKit->semaphoreSim);

         //read from shared memory cognitive queue(interval+1)
         readCognitiveQueue(ipcKit->memoryKey, ipcKit->sharedMemory, cognitiveQueue, agentList, interval+1);
      }
     interval+=1;
   }

   //release/terminate memory/semaphores
   term(ipcKit);
   release(ipcKit);
   return 0;
}
