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
    # sleep
    ticketEvent.wait()
    
    ticketModeMutex.wait()
    # if within 20 minutes of departure
    if(within20Minutes(departureTime)):
        
        ticket
        # while customers in waiting line
        while(waitingLine > 0):
            ticketWaitingQueue.signal()
            // sell tickets to everyone in the waiting line
            ticketInfoRequest.signal() # tell customer thread to set shared variables
            ticketInfoProvided.wait() # wait for customer thread to finish
            
            if(ticketsSold + numTickets > 60):
                if(nextBusTickets + numTickets > 60):
                    ticketType = soldOut
                else:
                    nextBusTickets = nextBusTickets + numTickets
                    ticketType = next
            else:
                ticketsSold = ticketsSold + numTickets
                ticketType = current
            
            if(ticketType != soldOut):
                # populate the ticket array with the specified number of tickets 
                printTickets(numTickets)
                
            ticketsReady.signal()
            ticketsReceived.wait()
            
            waitingLine--
        
        # if any customers in ticket line
        if(ticketLine > 0):
        # (May be an issue if it takes more than 20 minutes for waiting line customers to be service)
            ticketLineOpen.signal()
            # complete one ticket sale without Rule* 
            ticketInfoRequest.signal() # tell customer thread to set shared variables
            ticketInfoProvided.wait() # wait for customer thread to finish
            
            if(ticketsSold + numTickets > 60):
                if(nextBusTickets + numTickets > 60):
                    ticketType = soldOut
                else:
                    nextBusTickets = nextBusTickets + numTickets
                    ticketType = next
            else:
                ticketsSold = ticketsSold + numTickets
                ticketType = current
            
            if(ticketType != soldOut):
                # populate the ticket array with the specified number of tickets 
                printTickets(numTickets)
                
            ticketsReady.signal()
            ticketsReceived.wait()
            
            ticketLine--
        
        ticketModeMutex.signal()
        
        # while True
        while True:
        
            # sleep
            ticketEvent.wait()
            
            ticketModeMutex.wait()
            # if still within 20 minutes
            if(within20Minutes(departureTime)):
                ticketLineOpen.signal()
                
                # complete one ticket sale without Rule*
                ticketInfoRequest.signal() # tell customer thread to set shared variables
                ticketInfoProvided.wait() # wait for customer thread to finish
            
                if(ticketsSold + numTickets > 60):
                    if(nextBusTickets + numTickets > 60):
                        ticketType = soldOut
                    else:
                        nextBusTickets = nextBusTickets + numTickets
                        ticketType = next
                else:
                    ticketsSold = ticketsSold + numTickets
                    ticketType = current
            
                if(ticketType != soldOut):
                    # populate the ticket array with the specified number of tickets 
                    printTickets(numTickets)
                
                ticketsReady.signal()
                ticketsReceived.wait()
                
            else:
            
                # exit inner loop
                break
                
        # if any customers in ticket line
        if(ticketLine > 0):
            ticketLineOpen.signal()
            # TODO: complete one ticket sale with Rule*
            ticketInfoRequest.signal() # tell customer thread to set shared variables
            ticketInfoProvided.wait() # wait for customer thread to finish
            
            # these are the classes that can be sold tickets when Rule* is active
            if(sex == female or (sex == group and haveChidren)):
            
                # make sure there is space and if so sell them a ticket
                if(ticketsSold == 60 or ticketsSold + numTickets > 60):
                    if(nextBusTickets == 60 or nextBusTickets + numTickets > 60):
                        ticketType = soldOut
                    else:
                        nextBusTickets = nextBusTickets + numTickets
                        ticketType = next
                else:
                    ticketsSold = ticketsSold + numTickets
                    ticketType = current
            else:
                ticketType = wait
            
            if(ticketType != soldOut and ticketType != wait):
                # populate the ticket array with the specified number of tickets 
                printTickets(numTickets)
                
            ticketsReady.signal()
            ticketsReceived.wait()
            
            ticketLine--
            
        ticketModeMutex.signal()
            
    else:
        
        ticketLineOpen.signal()
        # complete one ticket sale with Rule*
        
        ticketInfoRequest.signal() # tell customer thread to set shared variables
        ticketInfoProvided.wait() # wait for customer thread to finish
        # these are the classes that can be sold tickets when Rule* is active
        if(sex == female or (sex == group and haveChidren)):
        
            # make sure there is space and if so sell them a ticket
            if(ticketsSold == 60 or ticketsSold + numTickets > 60):
                if(nextBusTickets == 60 or nextBusTickets + numTickets > 60):
                    ticketType = soldOut
                else:
                    nextBusTickets = nextBusTickets + numTickets
                    ticketType = next
            else:
                ticketsSold = ticketsSold + numTickets
                ticketType = current
        else:
            ticketType = wait
        
        if(ticketType != soldOut and ticketType != wait):
            # populate the ticket array with the specified number of tickets 
            printTickets(numTickets)
                
        ticketLine--
        ticketModeMutex.signal()
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
