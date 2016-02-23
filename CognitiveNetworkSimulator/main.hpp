/*
 * main.hpp
 *
 *  Created on: Aug 1, 2014
 *      Author: fabiorjvieira
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <cfloat>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <iomanip>

#include "memory.hpp"

static const char FIELD_SEPARATOR = ' ';

static const double BOLTZMANN = 1.3803e-23;
static const double REFERENCE_TEMPERATURE = 290.0; //kelvin
static const double NOISE_FIGURE = 0.031622776602; //15dBm

static const double COGNITIVE_SNR = 25.0; //1% of loss with 54Mbs
static const double PASSIVE_PRIMARY_SNR = 25.0; //quality
static const double ACTIVE_PRIMARY_SNR = 25.0; //1% of loss with 54Mbs

std::string cognitiveQueueFileName = "./cognitiveEvents.Queue";
std::string cognitiveSuccessQueueFileName = "./cognitiveSuccess.Queue";
std::string licensedSuccessQueueFileName = "./licensedSuccess.Queue";
std::string sensoringQueueFileName = "./sensoring.Queue";

float noiseFloor(long double bandWidth)
{
   float thermalNoise = BOLTZMANN * REFERENCE_TEMPERATURE * bandWidth;
   float noiseFloor = thermalNoise; //* NOISE_FIGURE; // noiseFigure; //perda dentro do receptor nao ideal
   noiseFloor = pow(100,-3);
   return noiseFloor;
}

float dBtoSNR(long double snrThreshold)
{
   float value = pow(10,snrThreshold/ (float)10);
   return value;
}

//primitive type
struct Position
{
   float x, y, z;
};

enum AgentType{AT,AW,AH,AC,PC,CA,CC,IV};
/*
   AT - Licensed Antenna TV
   AW - Licensed Antenna WIFI
   AH - Licensed Antenna HYBRID (TV + WIFI)
   AC - Licensed Active Client
   PC - Licensed Passive Client
   CA - Cognitive Antenna
   CC - Cognitive Client
   IV - Invalid
*/

char * AgentTypeChar[IV +1] = {"AT","AW","AH","AC","PC","CA","CC", "IV"};

AgentType char2AgentType(char typeChar[3])
{
   if (strcmp(typeChar, AgentTypeChar[AT]) == 0)
      return AT;
   else if (strcmp(typeChar, AgentTypeChar[AW]) == 0)
      return AW;
   else if (strcmp(typeChar, AgentTypeChar[AH]) == 0)
      return AH;
   else if (strcmp(typeChar, AgentTypeChar[AC]) == 0)
      return AC;
   else if (strcmp(typeChar, AgentTypeChar[PC]) == 0)
      return PC;
   else if (strcmp(typeChar, AgentTypeChar[CA]) == 0)
      return CA;
   else if (strcmp(typeChar, AgentTypeChar[CC]) == 0)
      return CC;
   else return IV;
}

struct WirelessParameters
{
   float snrThreshold;
   float pathLoss;
   float transmissionPower;
   unsigned long int channels;
   unsigned long int carrier;
   unsigned long int bandWidth;
   AgentType type;
};

//Agents
struct Agent
{
   unsigned short int identification = 0; //000, 001, 002, ...
   Position position;
   AgentType type = IV;
};

Agent INVALID_AGENT;
//INVALID_AGENT.identification = 0;
//INVALID_AGENT.type = IV;

Agent GENERIC_COGNITIVE_AGENT;
Agent GENERIC_LICENSED_AGENT;

struct AgentList
{
   std::vector < Agent > cognitiveClients;
   std::vector < Agent > cognitiveAntennas;
   std::vector < Agent > licensedPassiveClients;
   std::vector < Agent > licensedActiveClients;
   std::vector < Agent > licensedAntennas;
};

//Events and Queues
unsigned long long int INVALID_INTERVAL = -1;

struct Event
{
   unsigned long long int interval;
   Agent * transmitter;
   Agent * receiver;
   unsigned long int channel;
};

struct CognitiveQueue
{
   std ::vector < Event > events;
};

struct LicensedQueue
{
   std ::vector < Event > events;
};

bool SUCCEDED = 1;

struct Success
{
   bool success, licensed, cognitive;
};

struct SuccessQueue
{
   std ::vector < Event > events;
   std ::vector < Success > success;
};

struct SensoringQueue
{
   std ::vector < Event > events;
   std ::vector < float > risk;
};

struct CognitiveRequest
{
   unsigned long long int interval;
   Agent * transmitter;
   Agent * receiver;
   unsigned long int deltaInterval;
   unsigned long int minimumThroughput;
};

struct CognitiveRequestQueue
{
   std ::vector < CognitiveRequest > request;
};

/*wireless parameters file structure
TYPE pathLoss snrThreshold(dBm) transmissioPower channels carrier(Hz) bandWidth(Hz)
*/
WirelessParameters * loadWirelessParametersList(std::string fileName)
{
   std::ifstream file;
   std::stringstream endOfLine;
   char character[1];
   WirelessParameters * wirelessParametersList = new WirelessParameters[IV];
   AgentType agentType;
   char type[3];

   endOfLine << std::endl;

   file.open(fileName.data(), std::ios::in);
   if (not file)
    {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }
   std::cout << "#Reading file:" << fileName << std::endl;

   do
   {
      type[0]=AgentTypeChar[IV][0];type[1]=AgentTypeChar[IV][1];type[2]=endOfLine.str().data()[0];
      file >> type;
      agentType = char2AgentType(type);

      if (agentType != IV)
      {
         wirelessParametersList[agentType].type = agentType;
         file >> wirelessParametersList[agentType].snrThreshold;
         file >> wirelessParametersList[agentType].pathLoss;
         file >> wirelessParametersList[agentType].transmissionPower;
         file >> wirelessParametersList[agentType].channels;
         file >> wirelessParametersList[agentType].carrier;
         file >> wirelessParametersList[agentType].bandWidth;
      }

      while (file.peek() != endOfLine.str().data()[0] and !file.eof())
         file.read(character,1);
   }
   while (!file.eof());

   file.close();

   return wirelessParametersList;
}

/*agent file structure
TYPE(3 characters) IDENTIFICATION(3 digits) X(Cartesian coord) Y(Cartesian coord) Z(Cartesian coord)
*/
AgentList * loadAgents(std::string fileName)
{
   std::ifstream file;
   std::stringstream endOfLine;
   char character[1];
   AgentList * agentList = new AgentList;
   Agent agent;
   char type[3];

   endOfLine << std::endl;

   file.open(fileName.data(), std::ios::in);
   if (not file)
    {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }
   std::cout << "#Reading file:" << fileName << std::endl;

   do
   {
      type[0]=AgentTypeChar[IV][0];type[1]=AgentTypeChar[IV][1];type[2]=endOfLine.str().data()[0];
      file >> type;
      agent.type = char2AgentType(type);

      if (agent.type != IV)
      {
         file >> agent.identification;
         file >> agent.position.x;
         file >> agent.position.y;
         file >> agent.position.z;

         switch (agent.type)
         {
            case AT:
            case AW:
            case AH:
               agentList->licensedAntennas.push_back(agent);
               break;
            case AC:
               agentList->licensedActiveClients.push_back(agent);
               break;
            case PC:
               agentList->licensedPassiveClients.push_back(agent);
               break;
            case CA:
               agentList->cognitiveAntennas.push_back(agent);
               break;
            case CC:
               agentList->cognitiveClients.push_back(agent);
               break;
         }
      }

      while (file.peek() != endOfLine.str().data()[0] and !file.eof())
         file.read(character,1);
   }
   while (!file.eof());

   file.close();

   return agentList;
}

/*Licensed Queue file structure
INTERVAL(max 6 digits) TRANSMITTER(TYPE + IDENTIFICATION) RECEIVER(TYPE + IDENTIFICATION) CHANNEL(4 digits)
*/
void loadLicensedQueue(std::string fileName, std::streampos & filePosition, LicensedQueue & licensedQueue, AgentList & agentList, unsigned long long int interval)
{
   std::ifstream file;
   std::stringstream endOfLine;
   char character[1];
   Agent agent;
   Event event;
   char type[3];

   endOfLine << std::endl;

   file.open(fileName.data(), std::ios::in);
   if (not file)
    {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }
   std::cout << "#Reading file:" << fileName << std::endl;
   file.seekg(filePosition);
   event.interval = INVALID_INTERVAL;
   file >> event.interval;

   while (interval == event.interval and !file.eof())
   {
         file >> type;
         agent.type = char2AgentType(type);
         file >> agent.identification;

         switch (agent.type)
         {
            case AT:
            case AW:
            case AH:
               event.transmitter = & agentList.licensedAntennas.at(agent.identification);
               break;
            case CA:
               event.transmitter = & agentList.cognitiveAntennas.at(agent.identification);
               break;
         }

         file >> type;
         agent.type = char2AgentType(type);
         file >> agent.identification;

         switch (agent.type)
         {
            case AC:
               event.receiver = & agentList.licensedActiveClients.at(agent.identification);
               break;
            case PC:
               event.receiver = & agentList.licensedPassiveClients.at(agent.identification);
               break;
            case CC:
               event.receiver = & agentList.cognitiveClients.at(agent.identification);
               break;
            case IV:
               event.receiver = & INVALID_AGENT;
               break;
         }

         file >> event.channel;
         licensedQueue.events.push_back(event);

      while (file.peek() != endOfLine.str().data()[0] and !file.eof())
         file.read(character,1);

      filePosition = file.tellg();
      event.interval = INVALID_INTERVAL;
      file >> event.interval;
   }

   file.close();
}

/*Cognitive Request Queue file structure
INTERVAL TRANSMITTER RECEIVER DELTA_INTERVAL THROUGHPUT
*/
void loadCognitiveRequestQueue(std::string fileName, std::streampos & filePosition, CognitiveRequestQueue & cognitiveRequestQueue, AgentList & agentList, unsigned long long int interval)
{
   std::ifstream file;
   std::stringstream endOfLine;
   char character[1];
   Agent agent;
   CognitiveRequest request;
   char type[3];

   endOfLine << std::endl;

   file.open(fileName.data(), std::ios::in);
   if (not file)
    {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }
   std::cout << "#Reading file:" << fileName << std::endl;
   file.seekg(filePosition);
   request.interval = INVALID_INTERVAL;
   file >> request.interval;

   while (interval == request.interval and !file.eof())
   {
      file >> type;
      agent.type = char2AgentType(type);
      file >> agent.identification;

      switch (agent.type)
      {
      case AT:
      case AW:
      case AH:
         request.transmitter = & agentList.licensedAntennas.at(agent.identification);
           break;
      case CA:
         request.transmitter = & agentList.cognitiveAntennas.at(agent.identification);
         break;
      }

      file >> type;
      agent.type = char2AgentType(type);
      file >> agent.identification;

      switch (agent.type)
      {
         case AC:
            request.receiver = & agentList.licensedActiveClients.at(agent.identification);
            break;
         case PC:
            request.receiver = & agentList.licensedPassiveClients.at(agent.identification);
            break;
         case CC:
            request.receiver = & agentList.cognitiveClients.at(agent.identification);
            break;
      }

      file >> request.deltaInterval;
      file >> request.minimumThroughput;
      cognitiveRequestQueue.request.push_back(request);

      while (file.peek() != endOfLine.str().data()[0] and !file.eof())
         file.read(character,1);

      filePosition = file.tellg();
      request.interval = INVALID_INTERVAL;
      file >> request.interval;
   }

   file.close();
}

void saveSuccessQueue(std::string fileName, SuccessQueue & successQueue, unsigned long long int interval)
{
  std::ofstream file;

  file.open(fileName.data(), std::ios::app);
  if (not file)
   {
    std::cerr << fileName << " not found!" << std::endl;
    exit(1);
  }
  std::cout << "#Writing file:" << fileName << std::endl;

  for (unsigned int successEventIndex = 0; successEventIndex < successQueue.events.size() and interval == successQueue.events.at(successEventIndex).interval; successEventIndex++)
  {
    file << successQueue.events.at(successEventIndex).interval << FIELD_SEPARATOR;
    file << AgentTypeChar[successQueue.events.at(successEventIndex).transmitter->type] << FIELD_SEPARATOR;
    file << successQueue.events.at(successEventIndex).transmitter->identification << FIELD_SEPARATOR;
    file << AgentTypeChar[successQueue.events.at(successEventIndex).receiver->type] << FIELD_SEPARATOR;
    file << successQueue.events.at(successEventIndex).receiver->identification << FIELD_SEPARATOR;
    file << successQueue.events.at(successEventIndex).channel << FIELD_SEPARATOR;
    file << successQueue.success.at(successEventIndex).licensed << FIELD_SEPARATOR;
    file << successQueue.success.at(successEventIndex).cognitive << FIELD_SEPARATOR;
    file << successQueue.success.at(successEventIndex).success << std::endl;
  }

  file.close();
}

void saveSensoringQueue(std::string fileName, SensoringQueue & sensoringQueue, unsigned long long int interval)
{
  std::ofstream file;

  file.open(fileName.data(), std::ios::app);
  if (not file)
   {
    std::cerr << fileName << " not found!" << std::endl;
    exit(1);
  }
  std::cout << "#Writing file:" << fileName << std::endl;

  for (unsigned int sensoringEventIndex = 0; sensoringEventIndex < sensoringQueue.events.size() and interval == sensoringQueue.events.at(sensoringEventIndex).interval; sensoringEventIndex++)
  {
//    file << sensoringQueue.events.at(sensoringEventIndex).interval << FIELD_SEPARATOR;
//    file << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).transmitter->type] << FIELD_SEPARATOR;
//    file << sensoringQueue.events.at(sensoringEventIndex).transmitter->identification << FIELD_SEPARATOR;
//    //cycle-stationary
//    file << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).receiver->type] << FIELD_SEPARATOR;
//    file << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << FIELD_SEPARATOR;
//    file << sensoringQueue.events.at(sensoringEventIndex).channel << FIELD_SEPARATOR;
//    file << sensoringQueue.risk.at(sensoringEventIndex) << std::endl;
	  file << std::setw(INTERVAL_SIZE) << std::setfill(CHAR_FILLER) << std::right << intToAlpha(sensoringQueue.events.at(sensoringEventIndex).interval);
	  file << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).transmitter->type];
	  file << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << intToAlpha(sensoringQueue.events.at(sensoringEventIndex).transmitter->identification);
      //cycle-stationary
	  file << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).receiver->type];
	  file << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << CHAR_FILLER;
	  file << std::setw(CHANNEL_SIZE) << std::setfill(CHAR_FILLER) << std::right << intToAlpha(sensoringQueue.events.at(sensoringEventIndex).channel);
	  file << std::setw(RISK_SIZE) << std::setfill(CHAR_FILLER) << std::left << intToAlpha(sensoringQueue.risk.at(sensoringEventIndex)) << std::endl;
  }

  file.close();
}

void saveCognitiveQueue(std::string fileName, CognitiveQueue & cognitiveQueue, unsigned long long int interval)
{
      std::ofstream file;

      file.open(fileName.data(), std::ios::app);
      if (not file)
       {
         std::cerr << fileName << " not found!" << std::endl;
         exit(1);
      }
      std::cout << "#Writing file:" << fileName << std::endl;

      for (unsigned int cognitiveEventIndex = 0; cognitiveEventIndex < cognitiveQueue.events.size() and interval == cognitiveQueue.events.at(cognitiveEventIndex).interval; cognitiveEventIndex++)
      {
         file << cognitiveQueue.events.at(cognitiveEventIndex).interval << FIELD_SEPARATOR;
         file << AgentTypeChar[cognitiveQueue.events.at(cognitiveEventIndex).transmitter->type] << FIELD_SEPARATOR;
         file << cognitiveQueue.events.at(cognitiveEventIndex).transmitter->identification << FIELD_SEPARATOR;
         file << AgentTypeChar[cognitiveQueue.events.at(cognitiveEventIndex).receiver->type] << FIELD_SEPARATOR;
         file << cognitiveQueue.events.at(cognitiveEventIndex).receiver->identification << FIELD_SEPARATOR;
         file << cognitiveQueue.events.at(cognitiveEventIndex).channel << FIELD_SEPARATOR;
      }

      file.close();
}

std::string riskToString(float risk)
{
    std::stringstream riskString;
	riskString << roundf(risk * 100) / 100;
	size_t position = riskString.str().find(".",0);

	if (position == std::string::npos)
		riskString.str(riskString.str().substr(0,1));
	else
	{
		riskString.str(riskString.str().substr(0,4));
		riskString.str().replace(position, 1, "");
	}

    return riskString.str();
}

char * writeSensoringQueue(key_t memoryKey, char * sharedMemory, SensoringQueue & sensoringQueue, unsigned long long int interval)
{
   //Shared memory I/O ports
   std::stringstream writeOn;

   //start tag
   writeOn << SENSORING_QUEUE_PREFIX << START_DATA;
   writeOn << std::setw(KEY_SIZE) << std::right << std::setfill(CHAR_FILLER) << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();
   for (unsigned int sensoringEventIndex = 0; sensoringEventIndex < sensoringQueue.events.size() and interval == sensoringQueue.events.at(sensoringEventIndex).interval; sensoringEventIndex++)
   {
      //Write on the shared memory segment 24 bytes (TAG_SIZE)
      writeOn.str("");
      writeOn << std::setw(INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(sensoringQueue.events.at(sensoringEventIndex).interval);
      writeOn << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).transmitter->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << sensoringQueue.events.at(sensoringEventIndex).transmitter->identification;
      //cycle-stationary
      writeOn << AgentTypeChar[sensoringQueue.events.at(sensoringEventIndex).receiver->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << CHAR_FILLER;// << sensoringQueue.events.at(sensoringEventIndex).receiver->identification;
      writeOn << std::setw(CHANNEL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(sensoringQueue.events.at(sensoringEventIndex).channel);
      ///*debug*/std::cout << std::setw(RISK_SIZE) << std::setfill(CHAR_FILLER) << std::left << riskToString(sensoringQueue.risk.at(sensoringEventIndex)) << std::endl;
      writeOn << std::setw(RISK_SIZE) << std::setfill(CHAR_FILLER) << std::left << riskToString(sensoringQueue.risk.at(sensoringEventIndex));
      memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
      sharedMemory += writeOn.str().size();
   }
   //end tag
   writeOn << SENSORING_QUEUE_PREFIX << END_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();

   return sharedMemory;
}

char * writeCognitiveSuccessQueue(key_t memoryKey, char * sharedMemory, SuccessQueue & cognitiveSuccessQueue, unsigned long long int interval)
{
   //Shared memory I/O ports
   std::stringstream writeOn;

   //start tag
   writeOn << COGNITIVE_SUCCESS_QUEUE_PREFIX << START_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();
   for (unsigned int cognitiveSuccessEventIndex = 0; cognitiveSuccessEventIndex < cognitiveSuccessQueue.events.size() and interval == cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).interval; cognitiveSuccessEventIndex++)
   {
      //Write on the shared memory segment 24 bytes (TAG_SIZE)
      writeOn.str("");
      writeOn << std::setw(INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).interval);
      writeOn << AgentTypeChar[cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).transmitter->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).transmitter->identification;
      writeOn << AgentTypeChar[cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).receiver->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).receiver->identification;
      writeOn << std::setw(CHANNEL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveSuccessQueue.events.at(cognitiveSuccessEventIndex).channel);
      writeOn << cognitiveSuccessQueue.success.at(cognitiveSuccessEventIndex).licensed;
      writeOn << cognitiveSuccessQueue.success.at(cognitiveSuccessEventIndex).cognitive;
      writeOn << cognitiveSuccessQueue.success.at(cognitiveSuccessEventIndex).success;
      memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
      sharedMemory += writeOn.str().size();
   }
   //end tag
   writeOn << COGNITIVE_SUCCESS_QUEUE_PREFIX << END_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();

   return sharedMemory;
}

char * writeCognitiveRequestQueue(key_t memoryKey, char * sharedMemory, CognitiveRequestQueue & cognitiveRequestQueue, unsigned long long int interval)
{
   //Shared memory I/O ports
   std::stringstream writeOn;

   //start tag
   writeOn << COGNITIVE_REQUEST_QUEUE_PREFIX << START_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();
   for (unsigned int cognitiveRequestIndex = 0; cognitiveRequestIndex < cognitiveRequestQueue.request.size() and interval == cognitiveRequestQueue.request.at(cognitiveRequestIndex).interval; cognitiveRequestIndex++)
   {
      //Write on the shared memory segment 24 bytes (TAG_SIZE)
      writeOn.str("");
      writeOn << std::setw(INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).interval);
      writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->identification;
      writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->identification;
      writeOn << std::setw(DELTA_INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).deltaInterval);
      writeOn << std::setw(THROUGHPUT_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).minimumThroughput);
      memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
      sharedMemory += writeOn.str().size();
   }
   //end tag
   writeOn << COGNITIVE_REQUEST_QUEUE_PREFIX << END_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();

   return sharedMemory;
}

char * saveCognitiveRequestQueue(key_t memoryKey, char * sharedMemory, CognitiveRequestQueue & cognitiveRequestQueue, unsigned long long int interval)
{
   //Shared memory I/O ports
   std::stringstream writeOn;

   //start tag
   writeOn << COGNITIVE_REQUEST_QUEUE_PREFIX << START_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   sharedMemory += writeOn.str().size();
   for (unsigned int cognitiveRequestIndex = 0; cognitiveRequestIndex < cognitiveRequestQueue.request.size() and interval == cognitiveRequestQueue.request.at(cognitiveRequestIndex).interval; cognitiveRequestIndex++)
   {
      //Write on the shared memory segment 24 bytes (TAG_SIZE)
      writeOn.str("");
      writeOn << std::setw(INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).interval);
      writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).transmitter->identification;
      writeOn << AgentTypeChar[cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->type];
      writeOn << std::setw(AGENT_ID_SIZE) << std::setfill(CHAR_FILLER) << std::right << cognitiveRequestQueue.request.at(cognitiveRequestIndex).receiver->identification;
      writeOn << std::setw(DELTA_INTERVAL_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).deltaInterval);
      writeOn << std::setw(THROUGHPUT_SIZE) << std::setfill(BINARY_FILLER) << std::right << intToByte(cognitiveRequestQueue.request.at(cognitiveRequestIndex).minimumThroughput);
      memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
      sharedMemory += writeOn.str().size();
   }
   //end tag
   writeOn << COGNITIVE_REQUEST_QUEUE_PREFIX << END_DATA;
   writeOn << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(sharedMemory, writeOn.str().data(), writeOn.str().size());
   return sharedMemory;
}

char * readCognitiveQueue(key_t memoryKey, char * sharedMemory, CognitiveQueue & cognitiveQueue, AgentList * agentList, unsigned long long int interval)
{
   //Shared memory I/O ports
   char readFrom[TAG_SIZE];
   std::string parser;
   Event event;
   std::stringstream tag;

   tag << COGNITIVE_QUEUE_PREFIX << START_DATA;
   tag << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
   memcpy(&readFrom, sharedMemory, TAG_SIZE);
   //test start tag - 24 bytes (TAG_SIZE)
   if (tag.str().compare(readFrom) == 0)
   {
      tag.str("");
      tag << COGNITIVE_QUEUE_PREFIX << END_DATA;
      tag << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << std::right << memoryKey;
      sharedMemory += TAG_SIZE;
      memcpy(&readFrom, sharedMemory, TAG_SIZE);
      do
      {
         parser = readFrom; //(readFrom, TAG_SIZE);
         event.interval = byteToInt(parser.substr(0,INTERVAL_SIZE));
         if (interval == event.interval)
         {
            if (parser.substr(7,AGENT_TYPE_SIZE).compare(AgentTypeChar[CA]) == 0)
            {
               event.transmitter = & agentList->cognitiveAntennas.at(atoi(parser.substr(9,AGENT_ID_SIZE).data()));
               if (parser.substr(12,AGENT_TYPE_SIZE).compare(AgentTypeChar[CC]) == 0)
               {
                  event.receiver = & agentList->cognitiveClients.at(atoi(parser.substr(14,AGENT_ID_SIZE).data()));
                  event.channel = byteToInt(parser.substr(17,CHANNEL_SIZE));
                  cognitiveQueue.events.push_back(event);
               }
               else std::cout << "Receiver type != CC. Event ignored." << std::endl;
            }
            else std::cout << "Transmitter type != CA. Event ignored." << std::endl;
         }
         else std::cout << "Wrong interval. Queue ignored." << std::endl;
         sharedMemory += TAG_SIZE;
         memcpy(&readFrom, sharedMemory, TAG_SIZE);
      }
      //test end tag - 24 bytes (TAG_SIZE)
      while (tag.str().compare(readFrom) != 0);
   }
   else std::cout << "Start tag is missing. Cognitive queue ignored." << std::endl;

   return sharedMemory;
}

float interference(Agent * transmitter, Agent * receiver, Agent * interfering, WirelessParameters * wirelessParametersList)
{
   float interference = 0, distance, linkSize;

   distance = sqrt(pow(receiver->position.x - interfering->position.x, 2) + pow(receiver->position.y - interfering->position.y,2) + pow(receiver->position.z - interfering->position.z,2));
   linkSize = sqrt(pow(receiver->position.x - transmitter->position.x, 2) + pow(receiver->position.y - transmitter->position.y,2) + pow(receiver->position.z - transmitter->position.z,2));
   if (distance < 1) distance = 1;
   if (linkSize < 1) linkSize = 1;
   interference = (dBtoSNR(wirelessParametersList[receiver->type].snrThreshold) *
		             (noiseFloor(wirelessParametersList[receiver->type].bandWidth) + (wirelessParametersList[interfering->type].transmissionPower/pow(distance,wirelessParametersList[interfering->type].pathLoss)))) /
		          (wirelessParametersList[transmitter->type].transmissionPower/pow(linkSize,wirelessParametersList[transmitter->type].pathLoss));

   return interference;
}

void licensedSuccessQueueCalculus(SuccessQueue & licensedSuccessQueue, LicensedQueue & licensedQueue, CognitiveQueue & cognitiveQueue, unsigned long long int interval, WirelessParameters * wirelessParametersList)
{
   Agent * transmitterA, * receiverA, * transmitterB, * receiverB;
   unsigned short int channelA, channelB;
   float transmiterInterferenceL, receiverInterferenceL, transmiterInterferenceC, receiverInterferenceC;
   Success success;

   for (unsigned int licensedEventIndexA = 0; licensedEventIndexA < licensedQueue.events.size() and interval == licensedQueue.events.at(licensedEventIndexA).interval; licensedEventIndexA++)
   {
      receiverA = licensedQueue.events.at(licensedEventIndexA).receiver;
      if (receiverA->type == IV) continue;

      transmiterInterferenceL = 0;
      receiverInterferenceL = 0;
      transmiterInterferenceC = 0;
      receiverInterferenceC = 0;
      transmitterA = licensedQueue.events.at(licensedEventIndexA).transmitter;
      channelA = licensedQueue.events.at(licensedEventIndexA).channel;
      success.success = true;
      success.cognitive = false;
      success.licensed = false;

      //Interference from Licensed Network
      for (unsigned int licensedEventIndexB = 0; licensedEventIndexB < licensedEventIndexA and interval == licensedQueue.events.at(licensedEventIndexB).interval and success.success; licensedEventIndexB++)
      {
         transmitterB = licensedQueue.events.at(licensedEventIndexB).transmitter;
         receiverB = licensedQueue.events.at(licensedEventIndexB).receiver;
         //if it is a license passive communication (unidirectional, from antenna to client), then ignore it since there is always a beacon on the same channel that will be take into account (receiver = invalid)
         if ((transmitterB->type == AT and receiverB->type != IV) or (transmitterB->type == AH and (receiverB->type != IV or receiverB->type != AC))) continue;

         channelB = licensedQueue.events.at(licensedEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
            receiverInterferenceL += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
            if (receiverA->type == AC) transmiterInterferenceL += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
            if (receiverB->type == AC)
            {
               receiverInterferenceL += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
                if (receiverA->type == AC) transmiterInterferenceL += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
            }
         }
         if (receiverInterferenceL > 1 or transmiterInterferenceL > 1)
         {
            success.success = false;
            success.licensed = true;
         }
      }
      for (unsigned int licensedEventIndexB = licensedEventIndexA +1; licensedEventIndexB < licensedQueue.events.size() and interval == licensedQueue.events.at(licensedEventIndexB).interval and success.success; licensedEventIndexB++)
      {
         transmitterB = licensedQueue.events.at(licensedEventIndexB).transmitter;
         receiverB = licensedQueue.events.at(licensedEventIndexB).receiver;
         //if it is a license passive communication (unidirectional, from antenna to client), then ignore it since there is always a beacon on the same channel that will be take into account (receiver = invalid)
         if ((transmitterB->type == AT and receiverB->type != IV) or (transmitterB->type == AH and (receiverB->type != IV or receiverB->type != AC))) continue;

         channelB = licensedQueue.events.at(licensedEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
            receiverInterferenceL += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
            if (receiverA->type == AC) transmiterInterferenceL += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
            if (receiverB->type == AC)
            {
               receiverInterferenceL += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
                if (receiverA->type == AC) transmiterInterferenceL += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
            }
         }
         if (receiverInterferenceL > 1 or transmiterInterferenceL > 1)
         {
            success.success = false;
            success.licensed = true;
         }
      }

      //Interference from Cognitive Network
      for (unsigned int cognitiveEventIndexB = 0; cognitiveEventIndexB < cognitiveQueue.events.size() and interval == cognitiveQueue.events.at(cognitiveEventIndexB).interval and success.success; cognitiveEventIndexB++)
      {
         transmitterB = cognitiveQueue.events.at(cognitiveEventIndexB).transmitter;
         receiverB = cognitiveQueue.events.at(cognitiveEventIndexB).receiver;
         channelB = cognitiveQueue.events.at(cognitiveEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
            receiverInterferenceC += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
            if (receiverA->type == AC) transmiterInterferenceC += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
            receiverInterferenceC += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
            if (receiverA->type == AC) transmiterInterferenceC += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
         }
         if (receiverInterferenceC > 1 or transmiterInterferenceC > 1)
         {
            success.success = false;
            success.cognitive = true;
         }
      }

      if (success.success and (receiverInterferenceC + receiverInterferenceL > 1 or transmiterInterferenceC + transmiterInterferenceL > 1))
         success.success = false;

      licensedSuccessQueue.events.push_back(licensedQueue.events.at(licensedEventIndexA));
      licensedSuccessQueue.success.push_back(success);
   }
}

void cognitiveSuccessQueueCalculus(SuccessQueue & cognitiveSuccessQueue, LicensedQueue & licensedQueue, CognitiveQueue & cognitiveQueue, unsigned long long int interval, WirelessParameters * wirelessParametersList)
{
   Agent * transmitterA, * receiverA, * transmitterB, * receiverB;
   unsigned short int channelA, channelB;
   float transmiterInterferenceL, receiverInterferenceL, transmiterInterferenceC, receiverInterferenceC;
   Success success;

   for (unsigned int cognitiveEventIndexA = 0; cognitiveEventIndexA < cognitiveQueue.events.size() and interval == cognitiveQueue.events.at(cognitiveEventIndexA).interval; cognitiveEventIndexA++)
   {
      transmiterInterferenceC = 0;
      receiverInterferenceC = 0;
      transmiterInterferenceL = 0;
      receiverInterferenceL = 0;
      transmitterA = cognitiveQueue.events.at(cognitiveEventIndexA).transmitter;
      receiverA = cognitiveQueue.events.at(cognitiveEventIndexA).receiver;
      channelA = cognitiveQueue.events.at(cognitiveEventIndexA).channel;
      success.success = true;
      success.cognitive = false;
      success.licensed = false;

      //Interference from Cognitive Network
      for (unsigned int cognitiveEventIndexB = 0; cognitiveEventIndexB < cognitiveEventIndexA and interval == cognitiveQueue.events.at(cognitiveEventIndexB).interval and success.success; cognitiveEventIndexB++)
      {
         transmitterB = cognitiveQueue.events.at(cognitiveEventIndexB).transmitter;
         receiverB = cognitiveQueue.events.at(cognitiveEventIndexB).receiver;
         channelB = cognitiveQueue.events.at(cognitiveEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
             receiverInterferenceC += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
             transmiterInterferenceC += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
              receiverInterferenceC += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
             transmiterInterferenceC += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
         }
         if (receiverInterferenceC > 1 or transmiterInterferenceC > 1)
         {
            success.success = false;
            success.cognitive = true;
         }
      }
      for (unsigned int cognitiveEventIndexB = cognitiveEventIndexA +1; cognitiveEventIndexB < cognitiveQueue.events.size() and interval == cognitiveQueue.events.at(cognitiveEventIndexB).interval and success.success; cognitiveEventIndexB++)
      {
         transmitterB = cognitiveQueue.events.at(cognitiveEventIndexB).transmitter;
         receiverB = cognitiveQueue.events.at(cognitiveEventIndexB).receiver;
         channelB = cognitiveQueue.events.at(cognitiveEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
             receiverInterferenceC += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
             transmiterInterferenceC += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
             receiverInterferenceC += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
             transmiterInterferenceC += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
         }
         if (receiverInterferenceC > 1 or transmiterInterferenceC > 1)
         {
            success.success = false;
            success.cognitive = true;
         }
      }

      //Interference from Licensed Network
      for (unsigned int licensedEventIndexB = 0; licensedEventIndexB < licensedQueue.events.size() and interval == licensedQueue.events.at(licensedEventIndexB).interval and success.success; licensedEventIndexB++)
      {
         transmitterB = licensedQueue.events.at(licensedEventIndexB).transmitter;
         receiverB = licensedQueue.events.at(licensedEventIndexB).receiver;
         //if it is a license passive communication (unidirectional, from antenna to client), then ignore it since there is always a beacon on the same channel that will be take into account (receiver = invalid)
         if ((transmitterB->type == AT and receiverB->type != IV) or (transmitterB->type == AH and (receiverB->type != IV or receiverB->type != AC))) continue;
         channelB = licensedQueue.events.at(licensedEventIndexB).channel;
         if (transmitterA != transmitterB and channelA == channelB)
         {
            receiverInterferenceL += interference(transmitterA, receiverA, transmitterB, wirelessParametersList);
            transmiterInterferenceL += interference(receiverA, transmitterA, transmitterB, wirelessParametersList);
            if (receiverB->type == AC)
            {
               receiverInterferenceL += interference(transmitterA, receiverA, receiverB, wirelessParametersList);
                transmiterInterferenceL += interference(receiverA, transmitterA, receiverB, wirelessParametersList);
            }
         }
         if (receiverInterferenceL > 1 or transmiterInterferenceL > 1)
         {
            success.success = false;
            success.licensed = true;
         }
      }

      if (success.success and (receiverInterferenceC + receiverInterferenceL > 1 or transmiterInterferenceC + transmiterInterferenceL > 1))
         success.success = false;

      cognitiveSuccessQueue.events.push_back(cognitiveQueue.events.at(cognitiveEventIndexA));
      cognitiveSuccessQueue.success.push_back(success);
   }
}

float risk(Agent * sensor, Agent * transmitter, WirelessParameters * wirelessParametersList)
{
   float risk, distance, fromCognitveAntennaToLicensedReceiver, fromCognitveClientToLicensedReceiver, maximumLinkSize, maximumSensorLinkSize;

   distance = sqrt(pow(sensor->position.x - transmitter->position.x, 2) + pow(sensor->position.y - transmitter->position.y,2) + pow(sensor->position.z - transmitter->position.z,2));
   maximumLinkSize = pow((wirelessParametersList[transmitter->type].transmissionPower / dBtoSNR(wirelessParametersList[transmitter->type].snrThreshold)) / noiseFloor(wirelessParametersList[transmitter->type].bandWidth),1/wirelessParametersList[transmitter->type].pathLoss);
   maximumSensorLinkSize = pow((wirelessParametersList[sensor->type].transmissionPower/ dBtoSNR(wirelessParametersList[sensor->type].snrThreshold)) / noiseFloor(wirelessParametersList[sensor->type].bandWidth),1/wirelessParametersList[sensor->type].pathLoss);
   fromCognitveAntennaToLicensedReceiver = distance - maximumLinkSize;
   fromCognitveClientToLicensedReceiver = distance - maximumLinkSize - maximumSensorLinkSize;
   if (distance < 1) distance = 1;
   if (fromCognitveAntennaToLicensedReceiver < 1) fromCognitveAntennaToLicensedReceiver = 1;
   if (fromCognitveClientToLicensedReceiver < 1) fromCognitveClientToLicensedReceiver = 1;

   risk = (dBtoSNR(wirelessParametersList[transmitter->type].snrThreshold) *
             (noiseFloor(wirelessParametersList[transmitter->type].bandWidth) +
             (wirelessParametersList[sensor->type].transmissionPower * ((1/pow(fromCognitveAntennaToLicensedReceiver,wirelessParametersList[sensor->type].pathLoss)) +
                (1/pow(fromCognitveClientToLicensedReceiver,wirelessParametersList[sensor->type].pathLoss)))))) /
          (wirelessParametersList[transmitter->type].transmissionPower/pow(maximumLinkSize,wirelessParametersList[transmitter->type].pathLoss));
   if (risk > 1) risk = 1;
   else if (risk < 0) std::cout << "Risk under zero!!!" << std::endl;

   return risk;
}

void sensoringQueueCalculus(AgentList * agentList, SensoringQueue & sensoringQueue, CognitiveQueue & cognitiveQueue, LicensedQueue & licensedQueue, unsigned long long int interval, WirelessParameters * wirelessParametersList)
{
   unsigned long long int numberOfChannels = 0;
   unsigned long int channel;
   float riskValue, maximumRiskValue;
   Event sensor;
   Agent * transmitter, * maximumRiskAgent;

   //frequency spectrum |-TV-|-WIFI-|-COG-|
   numberOfChannels += wirelessParametersList[AT].channels;
   numberOfChannels += wirelessParametersList[AW].channels;
   numberOfChannels += wirelessParametersList[CA].channels;
   sensor.interval = interval;

   for (unsigned int cognitiveAntennaIndex = 0; cognitiveAntennaIndex < agentList->cognitiveAntennas.size(); cognitiveAntennaIndex++)
   {
      sensor.transmitter = & agentList->cognitiveAntennas.at(cognitiveAntennaIndex);
      for (sensor.channel = 0; sensor.channel < numberOfChannels; sensor.channel++)
      {
         //Cognitive network
         maximumRiskValue = 0;
         GENERIC_COGNITIVE_AGENT.type = CA;
         maximumRiskAgent = & GENERIC_COGNITIVE_AGENT;
         for (unsigned int cognitiveEventIndex = 0; cognitiveEventIndex < cognitiveQueue.events.size() and interval == cognitiveQueue.events.at(cognitiveEventIndex).interval; cognitiveEventIndex++)
         {
            transmitter = cognitiveQueue.events.at(cognitiveEventIndex).transmitter;
            channel = cognitiveQueue.events.at(cognitiveEventIndex).channel;
            if (transmitter != sensor.transmitter and sensor.channel == channel)
            {
               //risk of the communication from transmitter to receiver
               riskValue = risk(sensor.transmitter, transmitter, wirelessParametersList);
               //sensoringQueue.events.push_back(sensor); ???por que fiz isso, para a seguna fase?
               //sensoringQueue.risk.push_back(riskValue);
               if (riskValue > maximumRiskValue)
			   {
            	   maximumRiskValue = riskValue;
            	   maximumRiskAgent = transmitter;
			   }
               //risk of the communication from receiver to transmitter
               sensor.receiver = cognitiveQueue.events.at(cognitiveEventIndex).receiver;
               riskValue = risk(sensor.transmitter, sensor.receiver, wirelessParametersList);
               if (riskValue > maximumRiskValue)
			   {
            	   maximumRiskValue = riskValue;
            	   maximumRiskAgent = sensor.receiver;
			   }
            }
         }
         sensor.receiver = maximumRiskAgent;
         sensoringQueue.events.push_back(sensor);
         sensoringQueue.risk.push_back(maximumRiskValue);

         //Licensed network
         maximumRiskValue = 0;
         GENERIC_LICENSED_AGENT.type = AT;
         maximumRiskAgent = & GENERIC_LICENSED_AGENT;
         for (unsigned int licensedEventIndex = 0; licensedEventIndex < licensedQueue.events.size() and interval == licensedQueue.events.at(licensedEventIndex).interval; licensedEventIndex++)
         {
            transmitter = licensedQueue.events.at(licensedEventIndex).transmitter;
            sensor.receiver = licensedQueue.events.at(licensedEventIndex).receiver;
            //if it is a license passive communication (unidirectional, from antenna to client), then ignore it since there is always a beacon on the same channel that will be take into account (receiver = invalid)
            if ((transmitter->type == AT and sensor.receiver->type != IV) or (transmitter->type == AH and (sensor.receiver->type != IV or sensor.receiver->type != AC))) continue;
            channel = licensedQueue.events.at(licensedEventIndex).channel;
            if (transmitter != sensor.transmitter and sensor.channel == channel)
            {
               //risk of the communication from an active receiver to its transmitter
               if (sensor.receiver->type == AC)
               {
                  riskValue = risk(sensor.transmitter, sensor.receiver, wirelessParametersList);
                  if (riskValue > maximumRiskValue)
				  {
                	  maximumRiskValue = riskValue;
                      maximumRiskAgent = sensor.receiver;
				  }
               }
               //risk of the communication from transmitter to receiver
               riskValue = risk(sensor.transmitter, transmitter, wirelessParametersList);
               if (riskValue > maximumRiskValue)
               {
                  maximumRiskValue = riskValue;
                  maximumRiskAgent = transmitter;
               }
            }
         }
         sensor.receiver = maximumRiskAgent;
         sensoringQueue.events.push_back(sensor);
         sensoringQueue.risk.push_back(maximumRiskValue);
      }
   }
}

#endif /* MAIN_HPP_ */
