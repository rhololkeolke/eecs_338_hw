# Question 2

## Syntax

My Monitor syntax is as follows

```
Monitor <Monitor Type Name>
{
    // list of monitor variables and their types
    type var1;
    type var2;
    ...
    type var3;

    Init
    {
        // initializes monitor variables
    }
    
    procedure <return type> <name>(<arguments>)
    {
        // code
    }
}
```

Where `<Monitor Type Name>` is the name of this monitor type. `type var` specifies that this monitor contains a variable with name `var` and type `type`. `Init{}` is a block that defines what the monitor variables are initialized to at the creation of the monitor. `procedure` is a keyword indicating that the following block is a method of the monitor that is executed with mutual exclusion. `<return type>` is optional. If present it specifies that somewhere in the procedure body the keywords `return <var>` will be used which will return `<var>` as a local copy. This means if a shared variable is returned a local copy is kept by the process that it is returned to. `<name>` specifies the procedure name. `<arguments>` are optional and are a list of `type var` separated by commas. The argument list specifies variable names and types.

Monitors are declared like so

```
<Monitor Type Name> <name>
```

Where `<Monitor Type Name>` is the monitor's type which matches a monitor definition (as described above) and `<name>` is the name of this instantiation of the monitor definition. Methods are called on the monitor via the dot-notation like a class method in Java.

Condition variables are defined as

```
condition <name>
```

Where `<name>` specifies the condition variables name. condition variables can be called with `<name>.signal()` which signals the condition variable and `<name>.wait()` which blocks on the condition variable.

The keyword `local` denotes that the variable is local to the thread it is declared in.

## Assumptions

My Monitors use assumption one from the slides.
That is when signal is called on a condition variable, if
there is a process blocked on that condition variable the caller blocks
until the callee either finishes the procedure or calls wait agian.

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
Monitor GreyHoundStop
{
    int TLineCnt;
    condition TLine;
    condition TAReady;
    name CustNm = null;
    condition TicketReady;
    Ticket IssuedTicket;
    int CBAvailSCnt;
    int NBAvailSCnt;
    time CBDeptTime;
    time NBDeptTime;
    int numBoarded;
    condition nextBusBoard;
    bool GateEmpty;
    condition Gate;
    bool BusReady;
    condition cBusReady;
    condition AllAboard;

    Init
    {
        TLineCnt = 0;
        name CustNm = null;
        Ticket IssuedTicket = null;
        int CBAvailSCnt = 60;
        int NBAvailSCnt = 60;
        time CBDeptTime = 12 AM;
        time NBDeptTime = 3 AM;
        int numBoarded = 0;
        bool GateEmpty = True;
        bool BusReady = False;
    }
    
    procedure waitForCustomer()
    {
        if(TLineCnt == 0)
            TLine.wait();
    }
    
    procedure Ticket requestTicket()
    {
        TLineCnt++;
        TLine.signal();
        TAReady.wait();

        CustNm = MyName;

        TicketReady.wait();
        
        Ticket MyTicket = IssuedTicket;
        TLineCnt--;
        return MyTicket;
    }
    
    procedure printTicket()
    {
        TAReady.signal();
            
        if(not BusReady)
        {
            cBusReady.wait();
        }
        
        if(CBAvailSCnt != 0)
        {
            IssuedTicket := CBTicket(Name);
            CBAvailSCnt--;
        }
        else
        {
            IssuedTicket := NBTicket(Name);
            NBAvailSCnt--
        }
        
        TicketReady.signal();
    }
    
    procedure boardBus(Ticket MyTicket)
    {
        if(MyTicket.DeptTime != CBDeptTime)
            nextBusBoard.wait();
            
        board();
        
        numBoarded++;
        AllAboard.signal();
    }
    
    procedure waitForGate()
    {
        if(not GateEmpty)
            Gate.wait();
        GateEmpty = False;
    }
    
    procedure pullIntoGate()
    {
    
        numBoarded = 0;
        CBDeptTime = SETCBDeptTime();
        NBDeptTime = SETNBDeptTime();
    
        // let all of the people waiting board
        for(i = 0; i< 60 - NBAvailSCnt; i++)
        {
            nextBusBoard.signal();
        }

        CBAvailSCnt = 60 - NBAvailSCnt;
        NBAvailSCnt = 60;
    
        BusReady = True;
        cBusReady.signal();
        
        return CBDeptTime;
    }
    
    procedure leaveGate()
    {
        BusReady = False;
        
        while(numBoarded != 60 - CBAvailSCnt)
        {
            AllAboard.wait();
        }
        
        GateEmpty = True;
        Gate.signal();
    }
}

```

```
GreyHoundStop NYC-Route;
```

## Ticket Agent

```
while True
{
    NYC-Route.waitForCustomer()
    NYC-Route.printTicket();
}
```

## Passenger

```
local Ticket MyTicket = NYC-Route.requestTicket();
NYC-Route.boardBus(MyTicket);
```

## Bus

```
NYC-Route.waitForGate();
ARRIVE();
Time dept_time = NYC-Route.pullIntoGate();

sleepUntil(dept_time);

NYC-Route.leaveGate();

DEPART();

```
