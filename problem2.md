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
local bool allAboard = False

gateEmpty.wait()
load()

busBoardable.signal()

while True:
    ticketModeMutex.wait()
    if(20MinutesBefore(departureTime)):
        break
    ticketModeMutex.signal()

ticketModeMutex.signal()

ticketEvent.signal()

while True:
    ticketModeMutex.wait()
    if(timeIs(departureTime)):
        break
    ticketModeMutex.signal()
    
ticketModeMutex.signal()

ticketsForSale.wait() # TODO: not currently used in this solution

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
ticketsForSale.signal()

depart()
```
