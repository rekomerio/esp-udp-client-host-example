# ESP8266 UDP Example
Example to show how to create simple UDP network between two ESP8266 modules.

Every client must have unique id starting from 0 and incrementing always by one.

For example, if you want to have 3 clients, adjust NETWORK_ID in the client file in the following way:

* Client 1: NETWORK_ID 0
* Client 2: NETWORK_ID 1
* Client 3: NETWORK_ID 2

Also remember to change MAX_CLIENTS to 3 (default: 1) in the host file. 
