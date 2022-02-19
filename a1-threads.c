// -------------------------------------------
// Name: Meng Zhong Kuan
// Student Number: 7895449
// Course: COMP3430, Section: A01
// Instructor: Robert Guderian
// Assignment: #1, Question: #2.1
//
// Purpose of program
// - Read a configuration file (txt file) to decide how many threads should be used
// - On receiving a SIGHUP, re-read configuration file and change the number of current
//   workers appropriately (creating or removing processes)
// - On receiving a SIGINT, workers should cleanly exit
//   - Child threads:
//   	- Cleaning any resources
//   	- Print a message saying that it got the signal and is quitting
//   - Main thread
//      - Cleaning any resources
//      - Wait for all child threads to exit before it does
//  -----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#define maxThreads 130 // maximum 

char *filename;               // filename of the configuration
int workerAliveCounter = 0;   // number of threads alive
int changeNumThread    = 0;   // flag to change number of threads in main
int threadActive[maxThreads]; // keeps track of active thread, can only be changed by main thread
pthread_t my_thread[maxThreads]; 
// ----------------------------------------------------
// ReadConfigFile
//
// PURPOSE:
//  - Read the configuration
// 
// OUTPUT:
//  - Return the number value in the configuration file
//  ---------------------------------------------------
int ReadConfigFile(void)
{
	assert(filename != NULL);
	int numThreads = 0;
	FILE *fp       = fopen(filename, "r");

	// Reading of config file 
	printf("Reading config file\n");
	if(fp == NULL)
	{
		printf("Error: file %s cannot be opened\n", filename);
		exit(EXIT_FAILURE);
	}
	fscanf(fp, "%d", &numThreads);

	// Value checking
	if(numThreads < 0 || numThreads >= maxThreads)
	{
		printf("Value in config file must be a positive integer less than %d\n", maxThreads);
		exit(EXIT_FAILURE);
	}
	
	printf("Changing setting to %d\n", numThreads);
	return numThreads;
}

// ----------------------------------------------------
// ReReadConfigFile
//
// PURPOSE:
//  - Change a flag to re-read configuration file and number of threads
//    after receiving SIGHUP signal
//  - Actual work done in main
//  ---------------------------------------------------
void ReReadConfigFile(__attribute__((unused)) int arg)
{
	changeNumThread = 1;
}

// ---------------------------------------------------
// ExitThreads
//
// PURPOSE:
//  - Exit all threads
// ---------------------------------------------------
void ExitThreads(__attribute__((unused)) int arg)
{
	while(workerAliveCounter > 0)
	{
		threadActive[--workerAliveCounter] = 0; //Deactivate all threads
		pthread_join(my_thread[workerAliveCounter], NULL);
	}
	printf("Main Thread is exiting\n");
	printf("Program has run successfully\n");
	exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------------------
// ThreadWork
//
// PURPOSE:
//  - Print some entry and exit messages
//  - Idle around until it receives termination message (threadActive[threadNum] = 0) from main
//  ------------------------------------------------------------- ---------------------------

void* ThreadWork(void* threadNumPtr)
{
	int threadNum = 0;
	threadNum     = *((int*)threadNumPtr);
	printf("Thread %d has started\n", threadNum);
	while(threadActive[threadNum] != 0); // Wait for main thread to close it
	free(threadNumPtr);
	printf("Thread %d says goodbye\n", threadNum);
	return NULL;
}

int main(int argc, char *argv[])
{

	int numThreads           = 0; //Extracted number of threads to create/destroy/remain
	int pid                  = 0;
	void *threadNumPtr       = NULL; 
	// Ensure argument is supplied
	if(argc != 2)
	{
		printf("There should be 1 argument provided\n");
		exit(EXIT_FAILURE);
	}
	assert(argv[1] != NULL);

	// Read the configuration file and extract the value
	filename        = argv[1];
	pid             = getpid();
	changeNumThread = 1;	

	printf("I am %d\n", pid);
	
	signal(SIGHUP, ReReadConfigFile);
	signal(SIGINT, ExitThreads);
	// Main thread task 
	while(1)
	{
		// Read / Re-read configuration file and alter the number of threads
		if(changeNumThread != 0)
		{
			numThreads = ReadConfigFile(); 
			// First time reading config or reading in 0 in config or more threads to create after re-read
			if(workerAliveCounter == 0 || workerAliveCounter < numThreads) 			
			{
				// Create a number of worker threads based on extracted value (numThreads)
				while(workerAliveCounter < numThreads)
				{
					threadNumPtr = malloc(sizeof(int));		
					printf("Starting Thread %d\n", workerAliveCounter);
					*((int*)threadNumPtr) = workerAliveCounter;
					pthread_create(&my_thread[workerAliveCounter], NULL, ThreadWork, threadNumPtr);	
					threadActive[workerAliveCounter++] = 1;			
				}
				
			}	
			// Thread number remains the same
			else if(numThreads == workerAliveCounter)
			{
				printf("The number of worker threads remain the same\n");
			}
			// Delete excess threads
			else if(numThreads < workerAliveCounter)
			{		 
				while(workerAliveCounter > numThreads)
				{
					threadActive[--workerAliveCounter] = 0;
					printf("Stopping Thread %d\n", workerAliveCounter);
				}
			}	
			changeNumThread = 0; // End of change to number of worker threads	
		}

	}
		
}
