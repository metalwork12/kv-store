# kv-store

A networked key-value store written in C. Clients are able to connect over TCP and can store and manipulate key-value pairs. Data is stored to the disk regularly and loaded on start up so that data persists over sessions. Concurrent clients are handled via a threadpool. Key-value pairs also have an expiry time value, that when met is cleaned up automatically.

## Overview

This project is a server program that stores data in memory and lets multiple clients connect to it over the network at the same time to read and write that data. It's built from scratch in C using raw sockets. The project stores key-value pairs as entries in a hash table, by hashing the key's value, and assigning it a bucket in the data store. If the result of two hashed keys are the same, chaining is used to overcome collisions.

The main goal of this project is to develop a greater understanding of networking and backend systems by developing a system from scratch.

- **Networking** — how data moves over TCP, handling multiple connections, dealing with partial reads/writes
- **Concurrency** — multiple threads safely accessing shared data without corrupting it
- **Systems design trade-offs** — locking strategies, persistence, durability vs performance

## Tech stack

- **Language:** C
- **Networking:** POSIX sockets (`socket()`, `bind()`, `listen()`, `accept()`)
- **Concurrency:** POSIX threads (`pthread`)
- **Persistence:** snapshotting to disk
- **Environment:** developed on WSL (Ubuntu), compiled with `gcc`

## Project structure
```
kv-store/
├── README.md
├── Makefile
├── .gitignore
├── src/
│   ├── main.c           (entry point — creates hash table, starts server)
│   ├── hashtable.c      (hash table with djb2 hashing and chaining)
│   ├── hashtable.h
│   ├── server.c         (TCP socket setup, accept loop, command parsing)
│   ├── server.h
│   ├── threadpool.c     (worker threads, job queue, sweeper, snapshot thread)
│   ├── threadpool.h
├── client/
│   └── client.c         (CLI client)
├── benchmarks/
│   └── benchmark.c      (throughput and latency benchmark)
├── scripts/
│   └── load_test.sh     (concurrent client load test)
└── data_test/
    └── data.txt         (snapshot file)
```

## Commands 

- **SET** command takes two arguments: key and value. An entry is created with these arguments which is then added to the in-memory store e.g. "SET name John".

- **GET** command takes one argument: key. This key is then used to search the data store, if an entry with a matching key is found its value is returned. If no matching entry is found an error is returned, e.g. "GET name".

- **DEL** command takes any number of argument: key. The delete command then searches for the entries matching the entered keys and safely deletes them, dealing with any linked list issues, e.g. "DEL name1 name2 name3".

- **EXISTS** command takes one argument: key. The data store is searched for the corresponding key, 1 is returned if the key exists and 0 is returned if it does not, e.g. "EXISTS name".

- **INCR** command takes one argument: key. If the value of the corresponding key-value pair is an integer, then the value is incremented by one. If the value is not a integer an error pointer is set. e.g. "INCR age"

- **EXPIRE** command takes two arguments: key and expiry. The entry corresponding to the key argument has its expiry value set to the expiry argument plus the current POSIX time, e.g. "EXPIRE age 1000". This command is supported by the sweeper helper function that loops through the entries of the hash table and removes any entries once their expiry time has elapsed.

- **AUTH** command takes one argument: server_password. This value is checked and if matches the set password the user is authenticated for that session and is able to run commands, if the password is wrong an error message is send to the user and they are unable to run commands.


## Build and Run

**Requirements:** gcc, make, WSL or Linux

**Build:**
```bash
make server
make client
```

**Run the server:**
```bash
./server
```

**Connect with the client:**
```bash
./kv-client
```

The server listens on port 6380 by default.

## Benchmarking

Environment: WSL2, Ubuntu, Windows 11

Testing on both the SET and GET commands was completed by sending 100000 requests of each command. Both commands performed similarly as shown below.

- **SET Benchmarking** 
Time: 11.8527 (seconds) 
Requests per second: 8436.866 
Average Latency: 0.1185 (milliseconds)

- **GET Benchmarking**
Time: 11.8520 (seconds)
Requests per second: 8437.3738
Average Latency: 0.1185 (milliseconds)

### Concurrent Load Test 

Testing was completed using a bash script where 10 clients connected at the same time each setting a key-value pair and incrementing the value. This test was ran multiple times and confirmed that under concurrent loads the system was able to handle requests without corruption.


## What's Next?

- **Replication**

- **Pub/Sub**

- **Fine-grained locking (per-bucket mutex instead of global lock)**

- **RESP protocol support**

- **TTL command (time feft until expiry)**

- **Persistence improvements (such as AOF (append-only file))**

- **CLI Improvements (command history, autocomplete)**

- **Refactoring code (server.c handel client function (replace with lookup table and function pointers?))**

## Author

Tom Walker — [GitHub](https://github.com/metalwork12) · [LinkedIn](https://www.linkedin.com/in/tom-walker-872699304/)
