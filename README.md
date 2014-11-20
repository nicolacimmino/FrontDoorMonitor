This project aims at creating a monitor for a home front door capable of reporting to a mobile phone activity through such door. In particular, for a double door entrance, it should report wether someone entered, exited the house or let someone in from insiside.

The monitor should have the following caractheristics:

* Wireless, including power supply (battery operated)
* Possibility to work on single or double doors entrances
* Report type of transit (exit, enter, let guest in), only for double door
* Report to mobile phone, over internet not locally


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

