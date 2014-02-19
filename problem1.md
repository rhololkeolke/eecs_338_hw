## Assumptions

- each customer process has a local variable called customerName of type `string`

## Datatypes

To declare the semaphores and their initial values I'm using the following syntax

```
BinarySemaphore <name>(<initial value>)
```
to declare a binary semaphore with name of `<name>` and an initial value of `<inital value>`

Similarly a non-binary semaphore is declared with the syntax

```
NonBinarySemaphore <name>(<initial value>)
```
where the new semaphore has a name of `<name>` and an initial value of `<initial value>`.

E.g.

```
BinarySemaphore example(0);
```
This declares a binary semaphore called example with an initial value of 0


For problem1 I've defined two ticket types via an enumerated value data type

```
enumerated TicketType {NEXT_BUS, CURRENT_BUS}
```

In the problem a ticket is specified as having a departure time, a seat number and the customer's name.
The datatype of departureTime doesn't really matter for this algorithm, just assume it is capable of representing
time to at least the resolution of an hour.

```
record Ticket
  Time departureTime
  int seatNumber
  string customerName
end
```

## Functions

```
Ticket printTicket(TicketType type)
```

This function takes in a ticket type argument and returns a constructed Ticket object.
The departureTime field is taken from the shared variable departureTime if the type is CURRENT_BUS. Otherwise
3 hours is added to the shared departureTime variable and stored in the field.
The seatNumber field is determined by an unspecified algorithm, but guarantees that
the seatNumbers assigned for a bus are unique.
The customerName is left unspecified. It is assumed that the `takeTicket` method fills this in.
Every call of this function should have a following `takeTicket` call.
This function is only called by the TicketAgent thread

```
Ticket takeTicket()
```

This function copies the value of the shared ticket variable to the local Ticket object. While doing
so it uses the calling processes customerName to fill in the customerName field of the Ticket object it
is creating. Every call of this should be preceded by a `printTicket` call

```
bool isTimeToDepart(Time departureTime)
```

This function returns true if the time is at or has passed the specified depatureTime.

## Shared Variables
