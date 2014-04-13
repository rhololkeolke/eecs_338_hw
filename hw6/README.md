# EECS 338 HW6

Devin Schwab (dts34)
04/13/14

# Compiling

To compile just use the `make` command. It will create a set of binaries called `mother`, `tina` and `judy.

To clean up the build use the command `make clean`

# Running

To run the RPC server type `./mother`. This will execute the mother server.

The RPC client programs, tina and judy, take a command line argument specifying
the location of the mother program. So for instance if mother is running on eecslinab3 then you
would start tina with the command `./tina eecslinab3.case.edu` and judy with the command `./judy eecslinab3.case.edu`.
I was not sure which servers you would be running things on which is why I left the RPC server
as an argument.

# Results

The output of the programs are found in the `mother_log.txt`, `judy_log.txt` and `tina_log.txt` files
included with this submission. In this case mother was run on eecslinab3.case.edu, judy
was run on eecslinab4.case.edu and tina was run on eecslinab7.case.edu

