## Assumptions

- same assumptions as in problem unless otherwise noted
- There is no difference between a group ticket, a family ticket, a childs ticket, a man's ticket or a female's ticket once purchased. The differences are determined at purchase time. This means the ticket datatype can remain the same as from problem 1.
    - The ticket agent when printing the ticket will assign seats in first and second class based on the type of passenger purchasing. After that point the seat number implicitly determines what class the ticket is in.
- The only difference between groups and families is that families may have children which give them special ticket purchasing priority
- Each passenger thread has certain local variables that are properties of that thread. These properties are set by an unknown process when it begins

The local variables in each process are assumed to be

```
bool isHead
PassengerType l_passengerType
bool l_hasChildren
int l_numPassengers
```

`isHead` is True when the passenger thread is a family or a group head. `passengerType` is `MALE` when a single male passenger, `FEMALE` when a single female passenger, `GROUP` when a group or family. `hasChildren` can only be true in family threads and is True when the family has children associated with it. `numPassengers` specifies how many passengers are in this group/family. For single passengers `numPassengers = 1`.


- Each passenger thread has a set of shared semaphores and variables that are shared only within that family/group. For a single passenger shared variables and semaphores are also used in order to make the solution the same for all passenger thread types. However, in the single passenger no one else can access the shared variables so for all regular purposes they can be considered local.
 
```
NonBinarySemaphore distributeTickets(0);
Ticket s_tickets[numPassengers];
TicketType s_ticketType = CURRENT_BUS;
```

`distributeTickets` serializes access to the ticket array. The semaphore is non-binary because when the family head gets updated tickets after having to wait it needs to be guaranteed access to the ticket array and not starved. It does this by joining the non-binary semaphore queue. Additionally the ticket array can only be updated after every family member has read the information and reacted in order to prevent bad states.

`s_tickets` is an array of type Ticket of size `numPassengers`. `s_ticketType` is the current type of the tickets in the `s_tickets` array. Only when `s_ticketType` is `CURRENT_BUS` or `NEXT_BUS` is the information in the `s_tickets` array safe.

- The ticket agent only ever interacts with family heads, group heads and single passengers. Family heads and group heads distribute their tickets among the processes within that family/group.

## Datatypes

Unless otherwise stated assume the same datatypes from problem1.

`TicketType` has been updated to have the following definition

```
enumerated TicketType {CURRENT_BUS, NEXT_BUS, WAIT_FOR_TICKET, SOLD_OUT}
```

`CURRENT_BUS` and `NEXT_BUS` retain the same meaning as in problem1. `WAIT_FOR_TICKET` tells the customers that they need to step aside and wait to purchase a ticket due to Rule*. `SOLD_OUT` tells the customer that both the current bus and the next bus are completely booked.

`PassengerType` specifies which category of passengers this thread is in.

```
enumerated PassengerType {MALE, FEMALE, GROUP}
```
When a single male passenger the type is `MALE`, when a single female passenger the type is `FEMALE`. When a family or group the type is `GROUP`.

## Functions

Assume the same functions from problem1 (although not all functions from problem 1 are used in this problem)

```
printTickets(Ticket[] tickets, int numTickets)
```
Same as in problem1 except now instead of returning a single ticket it creates `numTickets` Ticket objects and stores them in the first `numTickets` indices in the specified tickets array. It assumes that the specified array is big enough to hold that many tickets

```
takeTicket(Ticket[] tickets, string customerName)
```
This function looks up the ticket index in the array based on the customersName and how many other people are in the family group (for single passengers there are 0 other people). It then fills in its own name in the ticket. The function used to get the index from the name is guaranteed to return a number between [0, numTickets) and be a unique index within the array (i.e. no two customerNames within a family/group will try to access the same index). How this function achieves the mapping is not specified as it is a sequential programming problem.

```
bool 20MinutesBefore(Time departureTime)
```
This function returns True when the specified departureTime is 20 minutes or less away.

```
balk()
```
This function ends the process.

# Shared Variables

```
BinarySemaphore ticketEvent(0);
NonBinarySemaphore mutex(1);
Time departureTime = 12 AM;
int numWaiting = 0;
int ticketsSold = 0
int nextBusTickets = 0;

BinarySemaphore ticketInfoRequest(0);
BinarySemaphore ticketInfoProvided(0);
int numPassengers = 0;
PassengerType passengerType = MALE;
bool hasChildren = False;

BinarySemaphore ticketReady(0);
BinarySemaphore ticketReceived(0);
TicketType ticketType = CURRENT_BUS;
Ticket tickets[60];


NonBinarySemaphore waitingQueue(0);

```

# Ticket Agent

```
while(True)
{
    wait(ticketEvent);
    
    wait(mutex);
    if(20MinutesBefore(depatureTime))
    {
        /* If there are customers in the ticket waiting queue
         * then this is the first time this section has been hit
         * for this bus. Therefore drain the ticket waiting queue.
         */
         if(numWaiting > 0)
         {
            // while there are still people waiting
            while(numWaiting > 0)
            {
                // sell without Rule*
                signal(waitingQueue);
                signal(ticketInfoRequest) // tell the customer you want his/her info (e.g. family size)
                wait(ticketInfoProvided) // wait until the customer has provided the info
                
                // if there is no room on this bus
                if(ticketsSold + numPassengers > 60)
                {
                    // and there is no room on the next bus
                    if(nextBusTickets + numPassengers > 60)
                    {
                        // tell them to give up
                        ticketType = SOLD_OUT;
                    }
                    else
                    {
                        // otherwise have them wait for the next bus
                        nextBusTickets = nextBusTickets + numPassengers;
                        ticketType = NEXT_BUS;
                    }
                }
                else
                {
                    // there is still space so give them a ticket
                    ticketsSold = ticketsSold + numPassengers;
                    ticketType = CURRENT_BUS;
                }
                
                if(ticketType != SOLD_OUT)
                {
                    // print out numPassengers tickets and store them in the tickets array
                    printTickets(tickets, numPassengers);
                }
                
                signal(ticketsReady);
                wait(ticketsReceived);
                
                numWaiting--;
            }
         }
         else
         {
            // the waiting queue has already been drained for this bus
            // sell a ticket without Rule*
            // almost the same as above, but singalling ticketQueue instead of waitingQueue
            signal(ticketQueue);
            signal(ticketInfoRequest)
            wait(ticketInfoProvided)
            
            
            if(ticketsSold + numPassengers > 60)
            {
                if(nextBusTickets + numPassengers > 60)
                {
                    ticketType = SOLD_OUT;
                }
                else
                {
                    nextBusTickets = nextBusTickets + numPassengers;
                    ticketType = NEXT_BUS;
                }
            }
            else
            {
                ticketsSold = ticketsSold + numPassengers;
                ticketType = CURRENT_BUS;
            }
            
            if(ticketType != SOLD_OUT)
            {
                printTickets(tickets, numPassengers);
            }
            
            signal(ticketsReady);
            wait(ticketsReceived);
         }
    }
    else
    {
        // Rule* applies
        signal(ticketQueue);
        signal(ticketInfoRequest)
        wait(ticketInfoProvided)
        
        if(passengerType == FEMALE || (passengerType == GROUP && hasChildren))
        {
            if(ticketsSold + numPassengers > 60)
            {
                if(nextBusTickets + numPassengers > 60)
                {
                    ticketType = SOLD_OUT;
                }
                else
                {
                    nextBusTickets = nextBusTickets + numPassengers;
                    ticketType = NEXT_BUS;
                }
            }
            else
            {
                ticketsSold = ticketsSold + numPassengers;
                ticketType = CURRENT_BUS;
            }
        }
        else
        {
            numWaiting++;
            ticketType = WAIT_FOR_TICKET;
        }
        
        if(ticketType != SOLD_OUT && ticketType != WAIT_FOR_TICKET)
        {
            printTickets(tickets, numPassengers)
        }
        
        signal(ticketsReady);
        wait(ticketsReceived);
    }
    
    mutex.signal();
}
```

# Passenger

```
// if this is a group/family head or if this is a single passenger
// then this thread must buy tickets
if(isHead or l_numPassengers == 1)
{
    signal(ticketEvent);
    wait(ticketQueue);
    wait(ticketInfoRequested);
    
    numPassengers = l_numPassengers;
    hasChildren = l_hasChildren;
    passengerType = l_passengerType;
    
    signal(ticketInfoProvided);
    wait(ticketsReady)
    
    takeTicket(s_tickets, customerName);
    s_ticketType = ticketType;
    
    signal(ticketsReceived);
    
    if(s_ticketType == WAIT_FOR_TICKET)
    {
        wait(waitingQueue);
    }
}
else
{
    // wait for the head to come back with tickets
    wait(distributeTickets);  
    
    takeTicket(s_tickets, customerName);
}

if(s_ticketType == SOLD_OUT)
{
    signal(distributeTickets);
    balk(); // exit the process
}
else if(s_ticketType == NEXT_BUS)
{
    signal(distributeTickets);
    wait(nextBusQueue);
    
}
else
{
    signal(distributeTickets);
}

// board current bus
wait(busBoardable); // this is a turnstile controlled by the bus threads
signal(busBoardable);

wait(canBoard)
board();
boarded++;
signal(canBoard)

```

# Bus Thread

```
wait(gateEmpty)
load()

signal(busBoardable)

while(True)
{
    wait(mutex)
    if(20MinutesBefore(depatureTime))
    {
        // release mutex before continuing.
        signal(mutex);
        break;
    }
    signal(mutex);
}

signal(ticketEvent);

while(True)
{
    wait(mutex);
    if(isTimeToDepart(departureTime))
    {
        // this time keep mutex
        // need to prevent ticket agent
        // from modifying ticket variables until next bus
        break;
    }
    signal(mutex);
}

while(True)
{
    wait(canBoard)
    if(ticketsSold == boarded)
    {
        signal(canBoard);
        break;
    }
    signal(canBoard)
}

wait(busBoardable);

// let all those waiting for the next bus up to the gate
// busBoardable will prevent them from trying to board this bus
for(int i=0; i<nextBusTickets; i++)
{
    signal(nextBus); 
}

ticketsSold = nextBusTickets;
nextBusTickets = 0;
departureTime = setNextDepatureTime(departureTime);
signal(mutex);

signal(gateEmpty);

depart()

```
