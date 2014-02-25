## Shared Variables

```
Monitor GreyHoundStop
{
    TicketLine()
    {
        // initialize here
        
        bool SaleInProgress = False
        
        int TLineCnt = 0;
        condition TLine;
        
        condition cTAReady;
        bool TAReady = False;
        
        condition cCReady;
        bool CReady = False;
    }
    
    // called by TA
    procedure waitForCustomer()
    {
        if(TLineCnt <= 0)
        {
            TLine.wait();
        }
        NextCustomer.signal();
    }
    
    // called by customer
    procedure requestTicket()
    {
        TLineCnt++;
        TLine.signal();
        
        if(SaleInProgress)
        {
            NextCustomer.wait();
        }
        
        SaleInProgress = True;
        
        CustNm = MyName;
        
        if(IssuedTicket == null)
        {
            TicketReady.wait();
        }
        
        MyTicket = IssuedTicket;
        IssuedTicket = null;
        SaleInProgress = False;
        TLineCnt--
    }
    
    // called by TA
    procedure printTicket()
    {

        if(not busReady)
        {
            cBusReady.wait()
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
        
        TicketReady.signal();
    }
    
    // called by passengers
    procedure boardBus(Ticket MyTicket)
    {
        if(MyTicket.dept_time != CB-DeptTime)
        {
            NextBusBoarding.wait()
        }
        board()
        boarded++;
        AnotherBoarded.signal();
    }
    
    // called by bus
    procedure arrive()
    {
        if(not GateEmpty)
        {
            NextBusArriving.wait()
        }
        
        GateEmpty = False;
        
        ARRIVE;
        
        # let all of the waiting passengers board
        CB-Avail-SCnt = 0;
        while(CB-Avail-SCnt != 60 - NB-Avail-Scnt)
        {
            NextBusBoarding.signal();
        }
        
        numBoarded = 0;
        CB-DeptTime = SET-CB-DeptTime();
        NB-DeptTime = SET-NB-DeptTime();
        NB-Avail-Scnt = 60;
        
        busReady = True;
        
    }
    
    // called by bus
    procedure depart()
    {
        while(numBoarded != 60 - CB-Avail-SCnt)
        {
            AnotherBoarded.wait();
        }
        
        GateEmpty = True;
        NextBusArriving.signal();
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
    NYC-Route.printTicket()
}
```

## Passenger

```
MyTicket = NYC-Route.requestTicket()
NYC-Route.boardBus(MyTicket)
```

## Bus

```
time departure_time = NYC-Route.arrive();
sleepUntil(departure_time);
NYC-Route.depart();
```
