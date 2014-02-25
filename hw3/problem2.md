## Shared Variables

```
Monitor GreyHoundStop
{
    TicketLine()
    {
        int TLineCnt = 0;
        condition TLine;
        bool TAReady = False;
        condition cTAReady;
        
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

        ticketReady.wait();
        
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
        
        if(CB-Avail-SCnt != 0)
        {
            IssuedTicket := CB-Ticket(Name);
            CB-Avail-SCnt--;
        }
        else
        {
            IssuedTicket := NB-Ticket(Name);
            NB-Avail-SCnt--
        }
        
        ticketReady.signal();
    }
    
    procedure boardBus(Ticket MyTicket)
    {
        if(MyTicket.dept_time != CB-DeptTime)
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
        CB-DeptTime = SET-CB-DeptTime();
        NB-DeptTime = SET-NB-DeptTime();
    
        // let all of the people waiting board
        for(i = 0; i< 60 - NB-Avail-SCnt; i++)
        {
            nextBusBoard.signal();
        }

        CB-Avail-SCnt = 60 - NB-Avail-SCnt;
        NB-Avail-SCnt = 60;
    
        BusReady = True;
        cBusReady.signal();
        
        return CB-DeptTime;
    }
    
    procedure leaveGate()
    {
        BusReady = False;
        
        while(numBoarded != 60 - CB-Avail-SCnt)
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
