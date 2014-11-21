This project aims at creating a monitor for a home front door capable of reporting to a mobile phone activity through such door. In particular, for a double door entrance, it should report wether someone entered, exited the house or let someone in from insiside.

The monitor should have the following caractheristics:

* Wireless, including power supply (battery operated)
* Working on double doors entrances
* Report type of transit (exit, enter)
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

Delivering a message to mobile
=========================

Message delivery can be achieved in several ways. One, that would be a good candidate to gain some experience in Android development, is to use Google Cloud Messaging (GCM) and then build a service application listening for GCM messages. Since I had already explored this area in other projects I decided to try something different. So I came across "Pushover" which offers an Android client ($4.99), HTTP API and free messages up to what amounts to a very large cap for a personal application like this, that is 7500 messages per month. As I live in a private home and not a railway station those should do.

The API is a REST HTTP API and uses OAuth for authentication. The issue here is that, for clearly valid reasons, the API is offered only through HTTPS, and there is no way I am going to implement SSL in an Arduino sketch. This problem would have presented itself anyway also if I had decided to implement a GCM based solution obviously. The fix was to use an intermediate entity, in this case a RaspberryPI that was already sitting in my home network for other purposes. So I made the DoorMonitor to make a plain HTTP request just notifying the door event to a PHP script that eventually then used CURL to create a pushover request.

This might sound a bit articulated and, in fact, if the RaspberryPI had to be introduced on purpose it would completely void the all idea of having a cheap WiFi module. On the other hand there is not much to do around it as any service available at the moment to push messages to a mobile phone makes use of some sort of encrypted connection, and we don't argue with that. As a bonus now requests pass through a script I own, so I can there log the information for instance in a database, thing that wouldn't have been possible by sending the message request directly to Pushover.

Since my WiFi is encrypted and the RaspberryPI was running only on the inside network this solution is sound enough and works for me. Should one want to host the PHP script on a public server he would open though a hole that would allow anyone with a browser to flood his phone with messages. This could be solved by authenticating messages with an HMAC and a rolling code (or other form of nonce). Messages could be still be eavesdropped but they couldn't be faked. I will not include this in this release but I plan to have it as an example and possibly to use it in other projects of mine. HMAC is easily computable also on a limited resoueces platform as the ones we are considering here.





