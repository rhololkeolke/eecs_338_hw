
# Question 2

## Shared Variables

```
int TLineCnt = 0
bool TAReady = False
bool CReady = False
name CustNm = null
int CB-Avail-SCnt = 60
int NB-Avail-SCnt = 60
bool GateEmpty = True
```

## Ticket Agent

```
while True
{
    region ticketSales
    {
        # only sell tickets when there are customers in line
        await(TLineCnt > 0);
        
        # sell ticket
        TAReady = True
    }
    
    region ticketSales
    {
        # wait until the customer is ready
        await(CReady);
        CReady = False;
    }
    
    region Bus
    {
        if(CB-Avail-SCnt != 0)
        {
            Name := CustNm;
            IssuedTicket := CB-Ticket(Name);
            CB-Avail-SCnt--;
        }
        else
        {
            Name := CustNm;
            IssuedTicket := NB-Ticket(Name);
            NB-Avail-SCnt--
        }
    }
    
    region ticketSales
    {
        SaleInProgress = False;
        ticketReady = True;
        TLineCnt--;
    }
}
```

## Passenger Threads

```
region ticketSales
{
    TLineCnt++
    await(TAReady and not SaleInProgress);
    # only allow one passenger through
    TAReady = False;
    SaleInProgress = True;
}

CustNm := MyName;

region ticketSales
{
    # tell the ticket agent the customer is ready
    CReady = True;
    await(ticketReady);
    ticketReady = False;
    
    MyTicket = IssuedTicket;
}

region Bus
{
    await(MyTicket.DeptTime == CB-DeptTime)
    board();
}


```

## Bus Threads

```
region Bus
{
    await(GateEmpty)
    GateEmpty = False;
    CB-DeptTime = SET-CB-DeptTime();
    NB-DeptTime = SET-NB-DeptTime();
    CB-Avail-SCnt = 60 - NB-Avail-SCnt;
    NB-Avail-SCnt = 60;
}
sleepUntil(CB-DeptTime);

region Bus
{
    GateEmpty = True;
}
DEPART
```
