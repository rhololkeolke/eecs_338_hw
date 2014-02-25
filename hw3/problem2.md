## Shared Variables

My Monitors use assumption one from the slides.
That is when signal is called on a condition variable, if
there is a process blocked on that condition variable the caller blocks
until the callee either finishes the procedure or calls wait agian. This assumption
is applied recursively in the event of multiple signals being called before a wait or end of a procedure.

```
Monitor GreyHoundStop
{
    TicketLine()
    {
        int TLineCnt = 0;
        condition TLine;
        
        condition TAReady;
        
        name CustNm = null;
        condition TicketReady;
        
        Ticket IssuedTicket;
        
        int CBAvailSCnt = 60;
        int NBAvailSCnt = 60;
        time CBDeptTime = 12 AM;
        time NBDeptTime = 3 AM;
        int numBoarded = 0;
        
        condition nextBusBoard;
        
        bool GateEmpty = True;
        condition Gate;
        
        bool BusReady = False;
        condition cBusReady;

        condition AllAboard;
    }
    
    procedure waitForCustomer()
    {
        if(TLineCnt == 0)
            TLine.wait();
    }
    
    procedure requestTicket()
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
        if(MyTicket.dept_time != CBDeptTime)
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
