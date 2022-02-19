// -------------------------------------
// Name: Meng Zhong Kuan
// Student Number: 7895449
// Course: COMP3430, Section: A01
// Instructor: Robert Guderian
// Assignment: #1, Question: #2.1
// 
// Purpose of program
// - Read a configuation file (txt file) to decide how many processes should be used
// - On receiving a SIGHUP, re-read configuration file and change the number of current
//   workers appropriately (creating or removing processes)
// - On receving a SIGINT, workers should cleanly exit
//   - Child process:
//     - Cleaning any resources
//     - Print a message saying that it got the signal and is quitting
//   - Main process:
//     - Cleanning any resources
//     - Wait for all child proccesses to exit before it does.
// -----------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#define maxChildProcess 130 //maximum child process

char *filename;                 // filename of the configuration file
int childAliveCounter;          // counting the number of child still alive
int globalPid[maxChildProcess]; // pid of child processes
int changeChild;                // 0 to not modify (create/delete/do nothing) child process, 1 to modify child process 

// -----------------------------------------------------
// readConfigFile
//
// PURPOSE:
//  - Read the configuration file
//
// OUTPUT:
//  - Return the number value in the configuration file
//  ----------------------------------------------------
int readConfigFile(void)
{
	assert(filename != NULL);

	int numProcesses = 0;
	FILE *fp         = fopen(filename, "r");
	
	// Reading of config file	
	printf("Reading config file\n");
	if(fp == NULL)
	{
		printf("Error: file %s cannot be opened\n", filename);
		exit(EXIT_FAILURE);
	}		
	fscanf(fp, "%d", &numProcesses);			
	
	// Value checking
	if(numProcesses < 0 || numProcesses >= maxChildProcess)
	{
		printf("Value in config file must be a positive integer less than 130\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Changing setting to %d\n", numProcesses);
	return numProcesses;
}

// ------------------------------------------------------------------------
// reReadConfigFile
//
// PURPOSE:
//  - Changing a flag to re-read configuration file and number of workers 
//    after receiving SIGHUP signal.
//  - Actual work done in main
//  -----------------------------------------------------------------------
void reReadConfigFile(__attribute__((unused)) int arg)
{
	changeChild = 1;		
}

// ------------------------------------------------------
// childCleanExit
//
// PURPOSE:
//  -  Cleanly exit upon receiving SIGINT 
// ------------------------------------------------------
void childCleanExit(__attribute__((unused)) int arg)
{		
	printf("%d is exiting\n", getpid());
	kill(getppid(), SIGUSR1);
	exit(EXIT_SUCCESS);					
}

// -----------------------------------------------------
// responseToChildQuit
//
// PURPOSE: 
//  - Decrease childAliveCounter
//  - Wait for resource collection
//  ---------------------------------------------------
void responseToChildQuit(__attribute__((unused)) int arg)
{
	childAliveCounter--;	
	wait(NULL);	
}

// ---------------------------------------------------
// exitAllChild
//
// PURPOSE:
//  - Send signal SIGINT to all child to cleanly exit
//  ---------------------------------------------------
void exitAllChild(__attribute__((unused)) int arg)
{
	int i = childAliveCounter;
	
	// Send signal and wait for childs to exit	
	while(i > 0)
	{	
		kill(globalPid[i], SIGINT);
		waitpid(globalPid[i], NULL, 0);
		i--;
	}
		
	// Printing exit and success
	printf("%d is exiting\n", globalPid[0]);
	printf("Program has been run successfully\n");
	exit(EXIT_SUCCESS);
	printf("Exit all child\n");
}

int main(int argc, char *argv[])
{
	int numProcesses        = 0;
	int pid                 = getpid();
	int i                   = 0;
	int extraProcesses      = 0;
	int excessProcesses     = 0;
	int childAliveStartLoop = 0; // keeps track of the amount of children alive at the start of the loop
	
	// Ensure argument is supplied
	if(argc != 2) 
	{
		printf("There should be 1 argument provided\n");
		exit(EXIT_FAILURE);
	}
	assert(argv[1] != NULL);
	
	// Read the configuration file and extract the value
	filename                     = argv[1]; 
	pid                          = getpid();
	globalPid[childAliveCounter] = pid; // Store parent PID in the first slot
	changeChild                  = 1; // Change child	
	printf("I am %d\n", pid);
	
	
	while(1){
		// Read or re-read configuration file and alter the thread numbers.
		if(changeChild != 0)
		{	
			pid = globalPid[0]; // Reset to parent PID
			numProcesses = readConfigFile();
			// Print same process number if processes are the same
			if(numProcesses == childAliveCounter)
			{
				printf("The number of child processes remains the same\n");
			}	
			// Delete excess processes
			else if(numProcesses < childAliveCounter)
			{
				excessProcesses = childAliveCounter - numProcesses;	
				childAliveStartLoop = childAliveCounter;
				for(i = 0; i < excessProcesses; i++)
				{	
					printf("%d is being killed\n", globalPid[childAliveStartLoop-i]);
					kill(globalPid[childAliveStartLoop-i], SIGINT);
					waitpid(globalPid[childAliveStartLoop-i], NULL, 0); // wait for the child to exit before deleting another	
				}
			}	
			// First time reading config or  re-reading and creating extra processes
			else if(childAliveCounter == 0 || numProcesses > childAliveCounter) // First time creating child processes
			{
				extraProcesses = numProcesses - childAliveCounter;
				i              = 0;
				// Create a number of child processes based on extracted value (numProcesses)
				while(i < extraProcesses && pid != 0) 
				{
					pid = fork();
					if(pid != 0) //If it is the parent process
					{
						globalPid[++childAliveCounter] = pid;
						printf("%d is starting\n", pid);
						i++;
					}	
				}	
			}
			// Pipe Error
			if(pid < 0) 
			{
				printf("Pipe Error\n");
				exit(EXIT_FAILURE);
			}
			// Child Process
			else if(pid == 0) 
			{
				signal(SIGINT, childCleanExit); // Cleanly exit using childCleanExit upon receiving SIGINT		
				while(1); // "Work" done by child workers
			}
			// Parent Process
			else 
			{	
				signal(SIGHUP, reReadConfigFile); // Re-read configuration file using readConfigFile upon receiving SIGHUP
				signal(SIGUSR1, responseToChildQuit); // Decrease the child counter and wait for resource collection
				signal(SIGINT, exitAllChild); // Exit all child
				changeChild = 0;
			}
		}
	}
}
