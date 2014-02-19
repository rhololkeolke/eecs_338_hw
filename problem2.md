## Assumptions

- same assumptions as in problem unless otherwise noted
- There is no difference between a group ticket, a family ticket, a childs ticket, a man's ticket or a female's ticket once purchased. The differences are determined at purchase time. This means the ticket datatype can remain the same as from problem 1.
    - The ticket agent when printing the ticket will assign seats in first and second class based on the type of passenger purchasing. After that point the seat number implicitly determines what class the ticket is in.
- The only difference between groups and families is that families may have children which give them special ticket purchasing priority
- Each passenger thread has certain local variables that are properties of that thread. These properties are set by an unknown process when it begins

The local variables in each process are assumed to be

```
bool isHead
PassengerType passengerType
bool hasChildren
int numPassengers
```

`isHead` is True when the passenger thread is a family or a group head. `passengerType` is `MALE` when a single male passenger, `FEMALE` when a single female passenger, `GROUP` when a group or family. `hasChildren` can only be true in family threads and is True when the family has children associated with it. `numPassengers` specifies how many passengers are in this group/family. For single passengers `numPassengers = 1`.


- Each passenger thread has a set of shared semaphores and variables that are shared only within that family/group. For a single passenger shared variables and semaphores are also used in order to make the solution the same for all passenger thread types. However, in the single passenger no one else can access the shared variables so for all regular purposes they can be considered local.
 
```
NonBinarySemaphore distributeTickets(0);
Ticket p_tickets[numPassengers];
TicketType p_ticketType = CURRENT_BUS;
```

`distributeTickets` serializes access to the ticket array. The semaphore is non-binary because when the family head gets updated tickets after having to wait it needs to be guaranteed access to the ticket array and not starved. It does this by joining the non-binary semaphore queue. Additionally the ticket array can only be updated after every family member has read the information and reacted in order to prevent bad states.

`p_tickets` is an array of type Ticket of size `numPassengers`. `p_ticketType` is the current type of the tickets in the `p_tickets` array. Only when `p_ticketType` is `CURRENT_BUS` or `NEXT_BUS` is the information in the `p_tickets` array safe.

- The ticket agent only ever interacts with family heads, group heads and single passengers. Family heads and group heads distribute their tickets among the processes within that family/group.

## Datatypes

Unless otherwise stated assume the same datatypes from problem1.

`TicketType` has been updated to have the following definition

```
enumerated TicketType {CURRENT_BUS, NEXT_BUS, WAIT_FOR_TICKET, SOLD_OUT}
```

`CURRENT_BUS` and `NEXT_BUS` retain the same meaning as in problem1. `WAIT_FOR_TICKET` tells the customers that they need to step aside and wait to purchase a ticket due to Rule*. `SOLD_OUT` tells the customer that both the current bus and the next bus are completely booked.

## Functions

Assume the same functions from problem1 (although not all functions from problem 1 are used in this problem)

```
printTickets(Ticket[] tickets, int numTickets)
```
Same as in problem1 except now instead of returning a single ticket it creates `numTickets` Ticket objects and stores them in the first `numTickets` indices in the specified tickets array. It assumes that the specified array is big enough to hold that many tickets

```
takeTicket(Ticket[] tickets)
```
This function looks up the ticket index in the array based on the customersName and how many other people are in the family group (for single passengers there are 0 other people). It then fills in its own name in the ticket. The function used to get the index from the name is guaranteed to return a number between [0, numTickets) and be a unique index within the array (i.e. no two customerNames within a family/group will try to access the same index). How this function achieves the mapping is not specified as it is a sequential programming problem.

```
bool 20MinutesBefore(Time departureTime)
```
This function returns True when the specified departureTime is 20 minutes or less away.

# Shared Variables

```
semaphore ticketEvent(0)

semaphore ticketModeMutex(1)
int ticketLine = 0
int waitingLine = 0
time departureTime = 12 AM
int ticketsSold = 0
int nextBusTickets = 0

semaphore ticketInfoRequested(0)
# these get set by customer in middle of purchase
int numTickets = 0
bool haveChildren = False
enumerated sex:{male, female, group} = male
semaphore ticketInfoProvided(0)

semaphore ticketsReady(0)
# these get set by ticket agent during purchase
enumerated ticketType:{current, next, soldOut, wait} = current
bool busSoldOut = False
Ticket[] tickets = [60] # only the first numTickets have valid data after this is set
semaphore ticketsReceived(0)

non-binary semaphore ticketLineOpen(0)
non-binary semaphore ticketWaitingQueue(0)
semaphore nextBusQueue(0)

```

I'm assuming each family and group has its own shared semaphores. So family A has a set of semaphores and variables and family B has a set of semaphores and shared variables. Family A's set and family B's set are not the same set. Each family has the following

```
semaphore distributeTickets(0)
```

# Ticket Agent
```
while True:
    ticketEvent.wait()
    
    ticketMutex.wait()
    if(within20Minutes(departureTime)):
        # if there are customers in the ticket waiting queue
        # then this is the first time this section has been hit
        # for this bus
        if(waitingLine > 0):
            while(waitingLine > 0):
                # sell without Rule*
                ticketWaitingQueue.signal()
                ticketInfoRequest.signal()
                ticketInfoProvided.wait()
                
                if(ticketsSold + numTickets > 60):
                    if(nextBusTickets + numTickets > 60):
                        ticketType = SOLD_OUT
                    else:
                        nextBusTickets = nextBusTickets + numTickets
                        ticketType = NEXT_BUS
                else:
                    ticketsSold = ticketsSold + numTickets
                    ticketType = CURRENT_BUS
                    
                if(ticketType != SOLD_OUT):
                    # populate the ticket array with the specified number of tickets
                    printTickets(numTickets)
                    
                ticketsReady.signal()
                ticketsReceived.wait()
                
                waitingLine--
        # if ticking waiting queue was drained in previous iteration
        # it should not fill until next bus arrives. Therefore, every 
        # time but the first this else code will be run
        else:
            # sell ticket without Rule*
            ticketQueue.signal()
            ticketInfoRequest.signal()
            ticketInfoProvided.wait()
            
            if(ticketsSold + numTickets > 60):
                if(nextBusTickets + numTickets > 60):
                    ticketType = SOLD_OUT
                else:
                    nextBusTickets = nextBusTickets + numTickets
                    ticketType = NEXT_BUS
            else:
                ticketsSold = ticketsSold + numTickets
                ticketType = CURRENT_BUS
            
            if(ticketType != SOLD_OUT):
                # populate the ticket array with the specified number of tickets
                printTickets(numTickets)
                
            ticketsReady.signal()
            ticketsReceived.wait()
    else:
        # sell ticket with Rule*
        ticketQueue.signal()
        ticketInfoRequest.signal()
        ticketInfoProvided.wait()
        if(sex == female or (sex == group and haveChildren)):
            if(ticketsSold + numTickets > 60):
                if(nextBusTickets + numTickets > 60):
                    ticketType = SOLD_OUT
                else:
                    nextBusTickets = nextBusTickets + numTickets
                    ticketType = NEXT_BUS
            else:
                ticketsSold = ticketsSold + numTickets
                ticketType = CURRENT_BUS
        else:
            ticketType = WAIT_FOR_TICKET
            
        if(ticketType != SOLD_OUT and ticketType != WAIT_FOR_TICKET):
            # populate the ticket array with the specified number of tickets
            printTickets(numTickets)
            
    ticketMutex.signal()
```

# Passenger

```
# if this is a group/Family head or this is a single passenger
# initiate the ticket buying process
if(isHead or l_numTickets == 1):
    ticketEvent.signal()
    ticketLineOpen.wait()
    ticketInfoRequested.wait()

    # copies local variables by value into shared variables read by ticket agent
    provideTicketInfo(l_numTickets, l_haveChildren, group)

    ticketInfoProvided.signal()
    ticketsReady.wait()

    # copies by value shared variables into the specified family/group shared variables
    receiveTickets(s_ticketType, s_tickets)

    ticketsReceived.signal()
else:
    # otherwise wait for the tickets to be distributed
    distributeTickets.wait()

# follow ticket directions
if(s_ticketType == soldOut):
    distributeTickets.signal()
    balk() # exit the process
    
# follow ticket directions
if(s_ticketType == soldOut):
    distributeTickets.signal()
    balk() # exit the process
    
elif(s_ticketType == wait):
    distributeTickets.signal()
    # wait to get ticket
    ticketWaitingQueue.wait()
    distributeTickets.wait()
    
if(s_ticketType == next):
    distributeTickets.signal()
    # wait in next bus line
    nextBusQueue.wait()
    
distributeTickets.signal()

# board current bus
busBoardable.wait()
busBoardable.signal()

canBoard.wait()
board()
boardedMutex.wait()
boarded++
boardedMutex.signal()
canBoard.signal()

```

# Bus Thread

```
gateEmpty.wait()
load()

busBoardable.signal()

while True:
    ticketMutex.wait()
    if(20MinutesBefore(departureTime)):
        break
    ticketMutex.signal()

ticketMutex.signal()

ticketEvent.signal()

while True:
    ticketMutex.wait()
    if(timeIs(departureTime)):
        break
    ticketMutex.signal()
    
while True:
    boardedMutex.wait()
    if(ticketsSold == boarded)
        break
    boardedMutex.signal()
boardedMutex.signal()

busBoardable.wait()

ticketModeMutex.wait()
for(int i=0; i<nextBusTickets; i++)
{
    nextBus.signal()
}

ticketsSold = nextBusTickets
nextBusTickets = 0
setNextDepartureTime(departureTime)
ticketModeMutex.signal()

gateEmpty.signal()
ticketMutex.signal()

depart()
```
