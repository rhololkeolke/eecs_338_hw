# EECS 338 HW 4

Devin Schwab  
dts34  
3/23/14  

## Building

To compile use the default make target. To clean run `make clean`.

## Running

To run execute the `main` executable. This is the process that initializes
the shared variables and then starts the agent process, bus processes and customer processes.

By default the number of customer processes started will be 18 and the total number of buses
will be 3. The customer process will be launched in groups of 6 and the busses will be launched
in groups of 1. The groups will launch with a randomized interval. The seed for the random interval
will come from the current time.

The main executable takes a number of command line arguments. These parameters control options
such as number of processes and random seed. They exist to make testing in different scenarios
easier than recompiling.

To get a list of the paramters you can run `./main -h`, however, the parameters are also listed here for
convenience.

`--totc (-C)`: Total number of customer processes for the run  
`--totb (-B)`: Total number of bus processes for the run  
`--numc (-c)`: Number of customer processes to spawn each spawn time  
`--numb (-b)`: Number of bus processes to spawn each spawn time  
`--seed (-s)`: Seed for random number generator  
`-h`: Displays a list of arguments  

