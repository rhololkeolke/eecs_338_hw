eecs_338_hw2
============

Homework 2 for EECS 338 - Operating Systems

## Shared Variables

```
semaphore customerArrived(0); // signaled by customer threads blocked on by Ticket Agent
non-binary semaphore ticketReady(0); // signaled by ticket agent, blocked on by customer
semaphore ticketReceived(0); // signaled by customer threads blocked on by Ticket Agent
semaphore ticketsForSale(1); // signaled by bus threads when new bus is ready to board and turned off when bus is readying for departure

semaphore ticketMutex(1); // mutex for the ticket related variables
int ticketsSold = 0; // number of people riding this bus
int nextBusTickets = 0; // number of people waiting for the next bus
bool busSoldOut = False; // True when the current bus's tickets are sold out

semaphore nextBus; // represents waiting area for those who missed the bus

semaphore gateEmpty(1); // allows a single bus thread to run at a time
semaphore busBoardable(0); // signaled by bus threads

semaphore canBoard(1); // boarding mutex. Only one passenger can board at a time, but order doesn't matter

semaphore boardedMutex(1); // mutex for boarding count
int boarded = 0;
```

## Ticket Agent

```
while(True)
{
  wait(customerArrived); // wait for a customer to wake up the agent
  wait(ticketsForSale); // wait for the tickets for sale
  wait(ticketMutex);
  if(ticketsSold == 60)
  {
    nextBusTickets++;
    busSoldOut = True; // inefficient because updates every time missed
  }
  else
  {
    ticketsSold++;
  }
  printTicket();
  signal(ticketMutex);
  signal(ticketReady); // tell the customer to grab the ticket
  wait(ticketReceived);
  signal(ticketsForSale);
}
```

## Passengers

```
signal(customerArrived); // wake up the ticket agent
wait(ticketReady); // wait for the ticket agent to print the ticket
wait(ticketMutex)
Ticket ticket = takeTicket(); // get the ticket from the ticketAgent
if(busSoldOut)
{
  // this customer missed the bus
  // release the mutex and take the ticket
  // then wait for the next bus
  signal(ticketMutex);
  signal(ticketReceived);
  wait(nextBus);
}
else
{
  // this customer made it on this bus
  // board as soon as possible
  signal(ticketMutex);
  signal(ticketReceived);
}

// Passengers can block here if they were
// released from waiting for the nextBus,
// but the bus hasn't finished reinitializing
// all of the ticket counters
wait(busBoardable);
signal(busBoardable);

// serializes boarding of passsengers
// assuming that boarding order doesn't matter
// only ticket sale order matters
wait(canBoard);
board();
wait(boardedMutex);
boarded++
signal(boardedMutex);
signal(canBoard);

```

## Buses

```
local bool allAboard = False;

// bus arrives and lets passengers on
wait(gateEmpty);
load();

signal(busBoardable);

waitForDepartureTime(); // busy wait until departure time is here

wait(ticketsForSale); // turn off ticket sales

// busy wait until all passengers who were sold tickets
// to this bus have boarded
while(!allAboard)
{
  wait(boardedMutex);
    if(ticketsSold == boarded)
      allAboard = True;
  signal(boardedMutex);
}

wait(busBoardable);

wait(ticketMutex)
for(int i=0; i<nextBusTickets; i++)
{
  signal(nextBus); // release any waiting passengers
}

// reset the ticket counters for the next bus
ticketsSold = nextBusTickets;
nextBusTickets = 0;
busSoldOut = False;
signal(ticketMutex)

// release the next bus
signal(gateEmpty);
// let the ticket agent sell tickets again
signal(ticketsForSale);

depart()
```
