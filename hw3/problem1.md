# Question 1

## Syntax

For the CCRs I'm using the following syntax

```
region <region name> 
{
    S1;
    
    await(<boolean condition>);
    
    S2;
}
```

where `<region name>` is the name of the region (all regions with the same name are executed with mutual exclusion).
`S1` is a sequetional program fragment of 0 or more lines. `await` will block and release the CCR if the `<boolean condition>` evaluates to false. Each time a process leaves its CCR either through blocking or finishing the boolean condtions for each process stuck at an await statement will be evaluated in the order they were stuck. The first true boolean condition starts executing, if there are no true statements nothing is done. After the process makes it past the `await` statement it will execute the sequention program fragement `S2` which consists of 0 or more lines. 

Also as stated in the CCR definition if multiple processes are blocked waiting for the ME property they will be serviced
in FCFS order.

Notes: `await` is optional and without the region is just a CR. It still operates with ME, but will never block inside

The keyword `local` denotes that the variable is local to the thread it is declared in.

## Assumptions

I'm assuming that while the tickets must be sold on a FCFS basis, the actual order of 
bus boarding does not matter. So the race condition that occurs in the passenger threads between the
second TicketSales CCR and the Bus CCR does not matter. I'm assuming that passengers just care about
whether they get a seat and not how quickly they actually get on the bus.

I'm also assuming that the bus will not leave without all ticketed passengers. This is to prevent an
issue where a passenger receives a ticket and then leaves the CPU before boarding and then the bus
wants to depart. If the bus did not wait then the passenger would not have a valid ticket. And the passenger
can't get on the next bus because the next bus may be oversold (and it wouldn't be FCFS then anyways).

It is assumed that each passenger thread has a local variable `name MyName`.

`CB-Ticket(name Name)` returns a Ticket object with the specified name as the customer name for the current bus. Automaticlly specifies the depature time and seat number. `NB-Ticket(name Name)` returns a Ticket object with the specified name as the customer name for the next bus. Automatically specifies the departure time and seat number.
`SET-CB-DeptTime()` calculates and returns the current bus's departure time.
`SET-NB-DeptTime()` calculates and returns the next bus's departure time.

## Shared Variables

The ticket type is defined as
```
record Ticket
{
    name CustName;
    time DeptTime;
    int SeatNumber;
}
```

```
int TLineCnt = 0
bool TAReady = False
bool CReady = False
name CustNm = null
int CB-Avail-SCnt = 60
int NB-Avail-SCnt = 60
bool GateEmpty = True
bool busReady = False
int numBoarded = 0
Ticket IssuedTicket = null;
```

## Ticket Agent

```
local Ticket ticket = null;
local name Name = null;
while True
{

    region Bus
    {
        await(busReady);
    }
    
    region TicketSales
    {
        # only sell tickets when there are customers in line
        await(TLineCnt > 0);
        
        # sell ticket
        TAReady = True
    }
    
    region TicketSales
    {
        # wait until the customer is ready
        await(CReady);
        CReady = False;
        
        Name := CustNm;
    }
    
    region Bus
    {
        if(CB-Avail-SCnt != 0)
        {
            ticket := CB-Ticket(Name);
            CB-Avail-SCnt--;
        }
        else
        {
            ticket := NB-Ticket(Name);
            NB-Avail-SCnt--
        }
    }
    
    region TicketSales
    {
        IssuedTicket = ticket;
        ticketReady = True;
        TLineCnt--;
    }
}
```

## Passenger Threads

```
region TicketSales
{
    TLineCnt++
}

region TicketSales
{
    await(TAReady and not SaleInProgress);
    # only allow one passenger through
    TAReady = False;
    SaleInProgress = True;
    CustNm := MyName;
    
    # tell the ticket agent the customer is ready
    CReady = True;
}

region TicketSales
{
    await(ticketReady);
    ticketReady = False;
    SaleInProgress = False;
    
    MyTicket = IssuedTicket;
}

region Bus
{
    await(MyTicket.DeptTime == CB-DeptTime and busReady)
    board();
    numBoarded++;
}


```

## Bus Threads

```
local time dept_time;
region Bus
{
    # assuming all bus threads are running
    # this prevents two busses from pulling into the gate and accepting passengers
    await(GateEmpty)
    GateEmpty = False;
    
    ARRIVE;
    
    numBoarded = 0;
    CB-DeptTime = SET-CB-DeptTime();
    NB-DeptTime = SET-NB-DeptTime();
    CB-Avail-SCnt = 60 - NB-Avail-SCnt;
    NB-Avail-SCnt = 60;
    
    busReady = True;
    
    dept_time = CB-DeptTime;
}

sleepUntil(dept_time);

region Bus
{
    # tell the ticket agent the bus is not ready
    busReady = False
}
    
region TicketSales
{
    # wait until the ticket sales are finished
    await(not SaleInProgress);
}

region Bus
{
    
    # wait until everyone boards
    await(numBoarded == 60 - CB-Avail-SCnt)
    # let the next bus into the gate
    GateEmpty = True;
}
DEPART
```
