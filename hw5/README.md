# EECS 338 HW 5

Devin Schwab  
dts34  

# Compiling

To compile just use the `make` command. It will create a binary called main.
To clean up the build use the command `make clean`.

# Running

To run the program type `./main`. This will execute the program with
the default settings of 20 molecules (i.e. 20 oxygen and 40 hydrogen for 60 total threads).

The program supports command line arguments that modify its execution. To
see a list of the arguments available type `./main --h`, however, the list
is also included below for convenience.

`--molecules (-m)`: Number of molecules to make this run (defaults to 20)
`--hydrogen (-h)`: Number of hydrogen threads to spawn at a time (defaults to 5)
`--oxygen (-o)`: Number of oxygen threads to spawn at a time (defaults to 3)
`--seed (-s)`: Seed for random number generator (defaults to current time)

# Results

The output of the program running with the default settings is contained within `output.txt`. This was obtained
with the command `./main > output.txt`. A version executed with script is contained within `script_output.txt`.
They should be identical, however, in previous assignments certain system calls did not work in script so I have
provided both versions.
