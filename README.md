# The Linux - based TCP Socket Client/Server toolkit with Bloom filter implementation.

## Information.

This toolkit has been writen by C/C++ programing.

* The Bloom filter core has been writen by C++ STL (bitset)
* The wrapper is TCP Socket architecture, which has been writen by C standard Network lib.

Toolkit has 2 component:
* Server side: The program with Socket Server role. Listen incoming connections from socket clients.
* Client side: The program with Socket Client role. Send request command to Socket Server to impact with the Bloom filter at Server side.

Runtime:
* Server side will run forever.
* Client side will run only one time per request.

User can impact to Bloom filter (at Server side) with commands of Client side.

## Build.

#### Client side
```bash
$ g++ bf_client.cpp -std=c++14 -o bf_client
```

#### Server side
```bash
$ g++ bf_server.cpp -std=c++14 -o bf_server
```

Note: ```sudo``` may be necessary.

## Run.

#### Server side
```bash
$ ./bf_server
```

or run as a daemon
```bash
$ ./bf_server &
```

#### Client side
```bash
$ ./bf_client <command> <param>
```