## Assumptions

- each customer process has a local variable called customerName of type `string`
- assume only the tickets are serviced FCFS, boarding can be done in any order. The reason FCFS is needed for the tickets is to make sure early arrivals get a ticket, but once a passenger gets a ticket it doesn't matter what order they get on the bus because they're guaranteed a seat.
- break works the same as in C/C++

## Datatypes

To declare the semaphores and their initial values I'm using the following syntax

```
BinarySemaphore <name>(<initial value>)
```
to declare a binary semaphore with name of `<name>` and an initial value of `<inital value>`

Similarly a non-binary semaphore is declared with the syntax

```
NonBinarySemaphore <name>(<initial value>)
```
where the new semaphore has a name of `<name>` and an initial value of `<initial value>`.

E.g.

```
BinarySemaphore example(0);
```
This declares a binary semaphore called example with an initial value of 0


For problem1 I've defined two ticket types via an enumerated value data type

```
enumerated TicketType {NEXT_BUS, CURRENT_BUS}
```

In the problem a ticket is specified as having a departure time, a seat number and the customer's name.
The datatype of departureTime doesn't really matter for this algorithm, just assume it is capable of representing
time to at least the resolution of an hour.

```
record Ticket
  Time departureTime = 12 AM
  int seatNumber = 0
  string customerName = ""
end
```

## Functions

```
Ticket printTicket(TicketType type)
```

This function takes in a ticket type argument and returns a constructed Ticket object.
The departureTime field is taken from the shared variable departureTime if the type is CURRENT_BUS. Otherwise
3 hours is added to the shared departureTime variable and stored in the field.
The seatNumber field is determined by an unspecified algorithm, but guarantees that
the seatNumbers assigned for a bus are unique.
The customerName is left unspecified. It is assumed that the `takeTicket` method fills this in.
Every call of this function should have a following `takeTicket` call.
This function is only called by the TicketAgent thread

```
Ticket takeTicket()
```

This function copies the value of the shared ticket variable to the local Ticket object. While doing
so it uses the calling processes customerName to fill in the customerName field of the Ticket object it
is creating. Every call of this should be preceded by a `printTicket` call

```
bool isTimeToDepart(Time departureTime)
```

This function returns true if the time is at or has passed the specified depatureTime.

```
Time nextDepartureTime(Time departureTime)
```
given a departureTime this function computes and returns the departureTime after this one (i.e. 3 hours afterwards)

```
load()
```
Does anything necessary for the bus to allow boarding of passengers.

```
depart()
```
Does anything necessary while the bus is heading to NYC

```
ride()
```
Does anything necessary for passengers who are completely seated in the bus.

```
board()
```
Set the passenger to the boarded state.

## Shared Variables

```
BinarySemaphore ticketQueue(0);


NonBinarySemaphore ticketReady(0);
BinarySemaphore ticketReceived(0);

BinarySemaphore mutex(1);
Time departureTime = 12 AM;
int ticketsSold = 0;
int nextBusTickets = 0;
TicketType ticketType = CURRENT_BUS;
Ticket ticket;

BinarySemaphore nextBusQueue(0);
BinarySemaphore busBoardable(0);

BinarySemaphore gateEmpty(1);
```

## Ticket Agent

```
while(True)
{
  wait(ticketQueue);
  wait(mutex);
  if(ticketsSold == 60)
  {
    nextBusTickets++;
    ticketType = NEXT_BUS;
  }
  else
  {
    ticketsSold++;
    ticketType = CURRENT_BUS;
  }
  ticket = printTicket(ticketType);

  signal(ticketReady);
  wait(ticketReceived);
  signal(mutex);
}
```

## Passengers

```
signal(ticketQueue);
wait(ticketReady)

/* To get to this point ticket agent must hold
 * mutex meaning no bus thread can be changing the ticket variables
 * additionally every other passenger will be blocked on the ticketReady semaphore.
 * The agent is blocked waiting for this thread to finish with the shared data
 * meaning it is safe for this thread to access the ticket shared variables
 * without explicilty holding the mutex */
local Ticket ticket = takeTicket();
local TicketType type = ticketType;

signal(ticketReceived);
if(type == NEXT_BUS)
{
  wait(nextBusQueue);
}

/* Passengers may block here if they are released 
 * from the nextBusQueue but the next bus hasn't
 * finished pulling up to the gate to allow boarders
 */
wait(busBoardable);
signal(busBoardable);

/* Serializes the boarding of passengers. But 
 * don't enforce a FCFS boarding order. */
wait(canBoard);
board();
boarded++;
signal(canBoard);

ride()
```

## Buses

```
wait(gateEmpty);
load();

signal(busBoardable);

while(True)
{
  wait(mutex);
  if(isTimeToDepart(departureTime))
  {
    break;
  }
  signal(mutex);
}

// wait until all passengers who were sold tickets to the bus have boarded
while(True)
{
  wait(canBoard);
    if(ticketsSold == boarded)
    {
      signal(canBoard);
      break;
    }
  signal(canBoard);
}

wait(busBoardable);

// release all passengers to the gate that
// were waiting for the next bus
for(int i=0; i<nextBusTickets; i++)
{
  signal(nextBusQueue);
}

// reset the ticket variables for the next bus
ticketsSold = nextBusTickets;
nextBusTickets = 0;
ticketType = CURRENT_BUS;
departureTime = nextDepartureTime(departureTime);
signal(mutex);

signal(gateEmpty);

depart();
```
