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
time departureTime = 12 AM; // don't care about the internal representation just assume it starts by representing 12 AM (i.e. midnight)

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
departureTime = departureTime + 3 hours;
signal(ticketMutex)

// release the next bus
signal(gateEmpty);
// let the ticket agent sell tickets again
signal(ticketsForSale);

depart()
```

## Problem 2

### Assumptions

- all tickets are the same (i.e. children and adult tickets are the same), however, ticket agent will only sell tickets to customers based on Rule*. This means tickets for children and adults don't need to be distinguished and the tickets received by the family head can be distributed, in whatever manner they choose.
- All passenger threads have a unique name
- each family head creates and shares the following between itself and its family members
  - int familySize which is equal to the number of family members (including this family head)
  - familyTickets array which is the same size as familySize. Each index in the array is unique to a family member (based on family member name) and stores the ticket received from the ticket agent for that family member
  - semaphore familyTicketsReceived(0) which is signaled by the familyHead when the tickets are received
  - shared waitForNextBus boolean
- each group head creates and shares the following between itself and its family members
  - groupSize which is equal to the number of group members (including this group head)
  - groupTickets array which is the same size as groupSize. Each index in the array is unique to a group member (based on group member name) and stores the ticket received from the ticket agent for that group member
  - semaphore groupTicketsReceived(0) which is signaled by the group head when the tickets are received
  - shared waitForNextBus boolean
- Family head also contains a local boolean called haveChildren which is true when one or more of the family's group are children
- single passenger threads have a a boolean isFemale which is true when the passenger is a female

### Shared Variables

```
semaphore wakeTicketAgent(0); // waited on in ticket agent thread. Signaled from customer and bus threads

semaphore ticketModeMutex(1)
time departureTime = 12 AM; // I don't really care about the actual representation of time just assume departureTime starts at 12 AM 
```

### Ticket Agent

```
while(True)
{
  // either a passenger or the bus can wake the ticket agent
  // bus wakes the agent when its within 20 minutes of departure
  // passengers wake agent when they arrive and wish to purchase a ticket
  wait(wakeTicketAgent);
  
  wait(ticketModeMutex);
  
  if(within20MinutesOfDeparture(depatureTime))
  {
    signal(ticketModeMutex);
  
    // TODO: start selling tickets to the ticket waiting line customers
    
    // TODO: If there is a customer in line sell a ticket to the main line without Rule*
    
    // Loop here until the bus has departed
    local bool waiveRule = True; // used to break out of loop when bus has departed
    while(waiveRule) // basically while(True) except if the departure time changes the variable will become False
    {
      // sleep here just like in the main loop
      wait(wakeTicketAgent);
      wait(ticketModeMutex);
      if(within20MinutesOfDeparture(departureTime))
      {
        // TODO: sell tickets without rules* to main line
        // no more people should be going to ticket waiting line
    
        signal(ticketModeMutex);
      }
      else
      {
        signal(ticketModeMutex);
        waiveRule = False; // exit this loop
        
        // TODO: sell the ticket with Rule*
        // this has to be here so a customer isn't missed
      }
    }
  }
  else
  {
    signal(ticketModeMutex);

    // TODO: sell tickets with Rule*
    wait(ticketsForSale)
    wait(ticketMutex)
    // if this is a group/family check if children
      // if space for familySize/groupSize
        // sell them familySize/groupSize tickets
      // else
        // send them to the waiting line
    // else if this is a single female
      // if they fit
        // sell them a ticket
      // else
        // send them to the waiting line
    // else
      // send them to the waiting line
  }
}
```

### Family Head Passenger

```

```

### Family Member Passenger

```

```

### Group Head Passenger

```

```

### Group Member Passenger

```

```

### Single Passenger

```

```

### Bus

```
local bool allAboard = False;

// bus arrives and lets passengers on
wait(gateEmpty);
load();

signal(busBoardable);

waitFor20MinutesBeforeDeparture();

signal(wakeTicketAgent); // TODO: Double check this semaphore name matches

waitForDepartureTime();

// TODO: Think of a better way to do this
while(!allAboard)
{
  wait(boardedMutex);
  if(ticketsSold == boarded)
    allAboard = true;
  signal(boardedMutex);
}

wait(busBoardable);

wait(ticketMutex);
for(int i=0; i<nextBusTickets; i++)
{
  signal(nextBus);
}

// reset the ticket counters for the next bus
ticketsSold = nextBusTickets;
nextBusTickets = 0;
busSoldOut = False;
signal(ticketMutex);

//release the next bus
signal(gateEmpty);
// let the ticket agent sell tickets again
signal(ticketsForSale);

depart();
```
