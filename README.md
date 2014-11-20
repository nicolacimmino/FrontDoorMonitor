This project aims at creating a monitor for a home front door capable of reporting to a mobile phone activity through such door. In particular, for a double door entrance, it should report wether someone entered, exited the house or let someone in from insiside.

The monitor should have the following caractheristics:

* Wireless, including power supply (battery operated)
* Working on double doors entrances
* Report type of transit (exit, enter, let guest in)
* Report to mobile phone, over internet


Direction of transit detection
=======

In case of double doors it's possible, by looking at the order in which doors open and close, to determine the direction in which the person has been moving. For instance if the external door opens first and the internal door after it means someone is coming in. The simplest logic, built on monitoring the opening oder would allow to recognize only two cases someone entering and someone leaving. We can though take things a step further if we factor in also the order in which doors close. In that case we can detect the following cases:

|Event 1|Event 2|Event 3|Event 4|Case
| --- | --- | --- | --- | --- |
|Ext Open|Int Open|Ext Close|Int Close|Coming in|
|Int Open|Ext Open|Int Close|Ext Close|Going out|
|Int Open|Ext Open|Ext Close|Int Close|Letting someone in|
|Int Open|Int Close| | |Checking door lock|
|Ext Open|Int Open|Int Close|Ext Close|Forgot to take something|

From a first guess we could assume we need to track two variables (internal and external door) and their status. Since we have maximum 4 transitions before the doors go back to being both closed we could fit in one byte a value that represents that all transaction. All we need to do is to store a 2-bits snapshot of the doors conditions at every change and process the result when we come back to both doors closed. If we define the higher bit of the pair to be the external door then the following binary sequences would apply:

|Binary Sequence|Decimal|Case
| --- | --- | --- | --- | --- |
|10110100|180|Coming in|
|01111000|120|Going out|
|01110100|116|Letting someone in|
|01000000|64|Checking door lock|
|10111000|184|Forgot to take something|
