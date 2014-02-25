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
        
        TAReady = True;
        cTAReady.signal();
    }
    
    // called by customer
    procedure requestTicket()
    {
        if(TAReady == False or SaleInProgress)
        {
            cTAReady.wait()
        }
        
        TAReady = False;
        SaleInProgress = True;
        CustNm = MyName;
        
        CReady = True;
        cCReady.signal();
        
        if(IssuedTicket == null)
        {
            TicketReady.wait();
        }
        
        MyTicket = IssuedTicket;
        IssuedTicket = null;
        SaleInProgress = False;
    }
    
    // called by TA
    procedure printTicket()
    {
        if(not CReady)
        {
            cCReady.wait();
        }
        
        CReady = False;
        
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

```
