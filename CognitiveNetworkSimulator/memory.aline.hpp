//#define _POSIX_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>

char START_DATA = '#';
char END_DATA = '*';
char CHAR_FILLER = '0';
char BINARY_FILLER = 0;

std::string COGNATOR_COGNATOR_COMM = "0";
std::string COGNATOR_SIMULATOR_COMM = "1";

std::string COGNATOR_PREFIX = "0";
std::string SIMULATOR_PREFIX = "1";

std::string COGNITIVE_QUEUE_PREFIX = "CO";
std::string SENSORING_QUEUE_PREFIX = "SE";
std::string COGNITIVE_SUCCESS_QUEUE_PREFIX = "CS";
std::string COGNITIVE_REQUEST_QUEUE_PREFIX = "CR";

unsigned short int TAG_SIZE = 24;
unsigned short int KEY_SIZE = 21;
unsigned short int INTERVAL_SIZE = 7;
unsigned short int CHANNEL_SIZE = 4;
unsigned short int DELTA_INTERVAL_SIZE = 4;
unsigned short int THROUGHPUT_SIZE = 3;
unsigned short int AGENT_TYPE_SIZE = 2;
unsigned short int AGENT_ID_SIZE = 3;
unsigned short int RISK_SIZE = 3;

struct IPCkit
{
   key_t memoryKey;
   size_t memorySize;
   std::stringstream semaphoreSimName, semaphoreCogName;

   sem_t * semaphoreSim;
   sem_t * semaphoreCog;
   char * sharedMemory;
};

IPCkit * init(IPCkit * ipcKit)
{
   //Getting shared memory
   int memoryId = shmget(ipcKit->memoryKey, ipcKit->memorySize, IPC_CREAT | 0666);
   if (memoryId < 0)
   {
      std::cout << "Error getting shared memory id" << std::endl;
      exit(1);
   }
   //Attached shared memory
   ipcKit->sharedMemory = (char *) shmat(memoryId, NULL, 0);
   if (ipcKit->sharedMemory == (char *) -1)
   {
      std::cout << "Error attaching shared memory id" << std::endl;
      exit(1);
   }

   ipcKit->semaphoreSim = sem_open(ipcKit->semaphoreSimName.str().data(), O_CREAT, S_IRWXU, 0);
   ipcKit->semaphoreCog = sem_open(ipcKit->semaphoreCogName.str().data(), O_CREAT, S_IRWXU, 0);

   return ipcKit;
}

void term(IPCkit * ipcKit)
{
   sem_close(ipcKit->semaphoreCog);
   sem_close(ipcKit->semaphoreSim);
   shmdt(ipcKit->sharedMemory);
}

void reset(IPCkit * ipcKit)
{
   sem_unlink(ipcKit->semaphoreSimName.str().data());
   sem_unlink(ipcKit->semaphoreCogName.str().data());
}

void release(IPCkit * ipcKit)
{
   reset(ipcKit);
   delete ipcKit;
}

IPCkit * get(key_t memoryKey, const char * semaphoreSimName, const char * semaphoreCogName, size_t memorySize)
{
   IPCkit * ipcKit = new IPCkit;

   //Getting shared memory
   int memoryId = shmget(memoryKey, memorySize, IPC_CREAT | 0666);
   if (memoryId < 0)
   {
      std::cout << "Error getting shared memory id" << std::endl;
      exit(1);
   }
   //Attached shared memory
   ipcKit->sharedMemory = (char *) shmat(memoryId, NULL, 0);
   if (ipcKit->sharedMemory == (char *) -1)
   {
      std::cout << "Error attaching shared memory id" << std::endl;
      exit(1);
   }

   ipcKit->semaphoreSim = sem_open(semaphoreSimName, O_CREAT);
   ipcKit->semaphoreCog = sem_open(semaphoreCogName, O_CREAT);

   return ipcKit;
}

unsigned long long int byteToInt(const std::string & binary)
{
   unsigned long long int integer = 0;
   for (unsigned char byteIndex = 0; byteIndex < binary.size(); byteIndex++)
      integer = integer*256 + (unsigned char) binary.at(byteIndex);
   return integer;
}

//Aline
unsigned long long int byteToInt(unsigned char * binary, int size)
{
   unsigned long long int integer = 0;

   for (unsigned char byteIndex = 0; byteIndex < size; byteIndex++)
      integer = integer*256 + (unsigned char) binary[byteIndex];
   return integer;
}

std::string intToByte(unsigned long long int integer)
{
   std::string binary;
   unsigned char byte;

   do
   {
      byte = integer % 256;
      binary.insert(binary.begin(),byte);
      integer = (integer - byte) / 256;
   }
   while (integer != 0);

   return binary;
}

std::string intToAlpha(unsigned long long int integer)
{
	std::ostringstream alpha;
	alpha.str("");
	alpha << integer;
	return alpha.str();
}

void oldSimulator(IPCkit * ipcKit, unsigned long int maxInterval)
{
   //Loop
   unsigned long int X = 0;
   //Shared memory I/O ports
   char * sharedMemory = ipcKit->sharedMemory, sharedMemoryChar;
   std::stringstream readWriteOn;

   //Write on the shared memory segment
   sem_wait(ipcKit->semaphoreSim);
   readWriteOn.str("");
   readWriteOn << START_DATA << "Hello World" << X << END_DATA;
   memcpy(ipcKit->sharedMemory, readWriteOn.str().data(), readWriteOn.str().size());
   sem_post(ipcKit->semaphoreCog);

   while (X < maxInterval)
   {
      sem_wait(ipcKit->semaphoreSim);

      //Reading from the shared memory segment.
      sharedMemoryChar = ' ';
      readWriteOn.str("");
      sharedMemory = ipcKit->sharedMemory;
      do
      {
         memcpy(&sharedMemoryChar, sharedMemory++, 1);
         readWriteOn << sharedMemoryChar;
      }
      while (sharedMemoryChar != END_DATA);

      //Write on the shared memory segment
      std::cout << readWriteOn.str() << std::endl;
      readWriteOn.str("");
      readWriteOn << START_DATA << "Hello World" << X << END_DATA;
      memcpy(ipcKit->sharedMemory, readWriteOn.str().data(), readWriteOn.str().size());

      sem_post(ipcKit->semaphoreCog);
      X++;
   }
}

void queueReader(IPCkit * ipcKit, std::string queueTag)
{
   //Shared memory I/O ports
   char readFrom[TAG_SIZE], * sharedMemory = ipcKit->sharedMemory;
   std::string parser;
   std::stringstream startTag, endTag;

   memcpy(&readFrom, sharedMemory, TAG_SIZE);
   //test start tag - 24 bytes (TAG_SIZE)
   startTag << queueTag << START_DATA << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << ipcKit->memoryKey;
   endTag << queueTag << END_DATA << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << ipcKit->memoryKey;
   if (startTag.str().compare(readFrom) == 0)
   {
      sharedMemory += TAG_SIZE;
      memcpy(&readFrom, sharedMemory, TAG_SIZE);
      do
      {
         parser = readFrom; //(readFrom, TAG_SIZE);
         std::cout << byteToInt(parser.substr(0,7)) << std::endl;
         std::cout << parser.substr(7,2) << " " << parser.substr(9,3) << std::endl;
         std::cout << parser.substr(12,2) << " " << parser.substr(14,3) << std::endl;
         std::cout << byteToInt(parser.substr(17,4)) << std::endl;
         std::cout << byteToInt(parser.substr(21,3)) << " " << parser.substr(21,3) << std::endl;
         sharedMemory += TAG_SIZE;
         memcpy(&readFrom, sharedMemory, TAG_SIZE);
      }
      //test end tag - 24 bytes (TAG_SIZE)
      while (endTag.str().compare(readFrom) != 0);
   }
   else std::cout << "Start tag is missing. Queue ignored." << std::endl;
}


void oldCognator(IPCkit * ipcKit, unsigned long int maxInterval)
{
   /*
    * ler a fila de pedidos cognitivos, sensoreados e sucesso
    * verificar a primeira rodada, quando nao ha fila de sucesso
    */
   //Loop
   unsigned long int X = 0;
   //Shared memory I/O ports
   char * sharedMemory = ipcKit->sharedMemory, sharedMemoryChar;
   std::stringstream readWriteOn;

   while (X < maxInterval)
   {
      sem_wait(ipcKit->semaphoreCog);

      //Reading from the shared memory segment.
      sharedMemoryChar = ' ';
      readWriteOn.str("");
      sharedMemory = ipcKit->sharedMemory;
      do
      {
         memcpy(&sharedMemoryChar, sharedMemory++, 1);
         readWriteOn << sharedMemoryChar;
      }
      while (sharedMemoryChar != END_DATA);

      //process

      //Write on the shared memory segment
      std::cout << readWriteOn.str() << std::endl;
      readWriteOn.str("");
      readWriteOn << START_DATA << "Bye World" << X << END_DATA;
      memcpy(ipcKit->sharedMemory, readWriteOn.str().data(), readWriteOn.str().size());

      sem_post(ipcKit->semaphoreSim);
      X++;
   }
}

void cognator(IPCkit * ipcKit, unsigned long int maxInterval)
{
   //Loop
   unsigned long int X = 0;
   //Shared memory I/O ports
   std::stringstream readWriteOn, startTag, endTag;

   while (X < maxInterval)
   {
      sem_wait(ipcKit->semaphoreCog);

      //Reading from the shared memory segment.
      queueReader(ipcKit, COGNITIVE_REQUEST_QUEUE_PREFIX);
      queueReader(ipcKit, SENSORING_QUEUE_PREFIX);
      queueReader(ipcKit, COGNITIVE_SUCCESS_QUEUE_PREFIX);

      //process

      //Write on the shared memory segment
      readWriteOn.str("");
      readWriteOn << COGNITIVE_QUEUE_PREFIX << START_DATA << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << ipcKit->memoryKey;
      readWriteOn << COGNITIVE_QUEUE_PREFIX << START_DATA << std::setw(KEY_SIZE) << std::setfill(CHAR_FILLER) << ipcKit->memoryKey;
      memcpy(ipcKit->sharedMemory, readWriteOn.str().data(), readWriteOn.str().size());

      sem_post(ipcKit->semaphoreSim);
      X++;
   }
}

int example(int argc, const char **argv)
{
   IPCkit * ipcKit;
   unsigned long int Z;

   if (argc > 3)
   {
      ipcKit = new IPCkit;
      ipcKit->memoryKey = atoi(argv[1]);
      ipcKit->semaphoreSimName << SIMULATOR_PREFIX << std::setw(KEY_SIZE) << std::setfill('0') << ipcKit->memoryKey;
      ipcKit->semaphoreCogName << COGNATOR_PREFIX << std::setw(KEY_SIZE) << std::setfill('0') << ipcKit->memoryKey;
      ipcKit->memorySize = atoi(argv[2]);
      Z = atoi(argv[3]);
   }

   if (argc == 5)
   {
      ipcKit = init(ipcKit);

      if (COGNATOR_PREFIX.compare(argv[4]) == 0) oldCognator(ipcKit, Z);
      else if (SIMULATOR_PREFIX.compare(argv[4]) == 0) oldSimulator(ipcKit, Z);

      term(ipcKit);
   }
   else if (argc == 4)
   {
      ipcKit = get(ipcKit->memoryKey, ipcKit->semaphoreSimName.str().data(), ipcKit->semaphoreCogName.str().data(), ipcKit->memorySize);
      cognator(ipcKit, Z);
   }

   return 0;
}

std::vector < std::string > split(std::string inputString, std::string delimiter)
{
   std::vector < std::string > arrayOfStrings;
   size_t fieldStartPosition = 0, fieldStopPosition = 0;

   while (std::string::npos != fieldStopPosition)
   {
      fieldStopPosition = inputString.find(delimiter, fieldStopPosition + delimiter.size());
      arrayOfStrings.push_back(inputString.substr(fieldStartPosition, fieldStopPosition - fieldStartPosition));
      fieldStartPosition = fieldStopPosition + delimiter.size();
   }

   return arrayOfStrings;
}
