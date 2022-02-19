
# ThreadsAndProcesses
The program reads a configuration file (.txt file) and extract a number (ranging 0-128) which would be used to create a number of workers (threads or child processes depending on the program that is run)

## Running a1-procs.c
1. Create a configuration file of type .txt.
2. Inside the configuration file, write a number ranging from 0-128 and save the file.
3. Compile a1-procs.c using either:  
+ clang -Wall a1-procs.c -o a1-procs 
+ make
4. Run the program using:
+ ./a1-procs {config.txt}
+ config.txt would be replaced by the name of your configuration file
5. After run, the number of child processes specified by the configuration file would be created (each doing "work", which is just an infinite loop).
6. Change the number in the configuration file and save.
7. Send a signal from the terminal using:
+ kill -HUP {parent process id}
+ The parent process id can be found in the print statement at the top stating I am {parent process id}.
+ This signal would update the current number of child process running (removing/creating/remain the same)
8. To exit the program, send a signal from the terminal using either:
+ kill -INT {parent process id}
+ If the program is run from terminal, click on the terminal that is running the program and press "CTRL + C".

## Running a1-threads.c
1. Create a configuration file of type .txt.
2. Inside the configuration file, write a number ranging from 0-128 and save the file.
3. Compile a1-threads.c using either:  
+ clang -Wall a1-threads.c -o a1-threads 
+ make
4. Run the program using:
+ ./a1-threads {config.txt}
+ config.txt would be replaced by the name of your configuration file
5. After run, the number of threads (excluding the main threads) specified by the configuration file would be created (each doing "work", which is just an infinite loop).
6. Change the number in the configuration file and save.
7. Send a signal from the terminal using:
+ kill -HUP {parent process id}
+ The parent process id can be found in the print statement at the top stating I am {parent process id}.
+ This signal would update the current number of child process running (removing/creating/remain the same)
8. To exit the program, send a signal from the terminal using either:
+ kill -INT {parent process id}
+ If the program is run from terminal, click on the terminal that is running the program and press "CTRL + C".
