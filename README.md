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

semaphore busLoading(1); // allows a single bus thread to run at a time
semaphore busBoardable(0); // signaled by bus threads

semaphore canBoard(1); // boarding mutex. Only one passenger can board at a time, but order doesn't matter
```

## Ticket Agent

```
while(True)
{
  wait(ticketsForSale); // wait for the tickets for sale
  wait(customerArrived); // wait for a customer to wake up the agent
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

wait(busBoardable);
signal(busBoardable);

canBoard.wait();
board();
canBoard.signal();

```

## Buses

```
// bus arrives and lets passengers on
wait(busLoading);
load();

signal(ticketsForSale);
signal(busBoardable);

waitForDepartureTime(); // busy wait until departure time is here

wait(ticketsForSale); // turn off ticket sales

// TODO: Wait until every ticket has boarded

wait(busBoardable);
signal(busLoading);

// TODO: Release waiting passengers
// TODO: Reset ticket variables
```
