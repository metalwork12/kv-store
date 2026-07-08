# kv-store

A networked key-value store written in C. Clients are able to connect over TCP and can store and manipulate key-value pairs. Data is stored to the disk regularly and loaded on start up so that data persists over sessions. Concurrent clients are handled via a threadpool. Key-value pairs also have an expiry time value, that when met is cleaned up automatically.

## Overview

This project is a server program that stores data in memory and lets multiple clients connect to it over the network at the same time to read and write that data. It's built from scratch in C using raw sockets.

The main goal of this project is to develop a greater understanding of networking and backend systems by developing an system from scratch.

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
│   ├── main.c          (entry point — starts the server)
│   ├── hashtable.c     (hash table implementation)
│   ├── hashtable.h     (hash table's public interface)
│   ├── server.c        (socket setup, accept loop)
│   ├── server.h
├── tests/
│   └── (test files, later)
└── client/
    └── client.c        (simple CLI client to talk to your server)
```

## Project plan

### Week 1 — Core engine + single-client networking

**Goal:** a working server that one client can talk to.

- In-memory hash table supporting `SET key value`, `GET key`, `DEL key` (DONE).
- Simple text-based protocol over TCP (plain-text commands, not full RESP)
- Single-threaded TCP server (`socket → bind → listen → accept → recv/send`) (DONE)
- A basic CLI client to send commands 
- **Milestone:** run the server, connect with the client (or `telnet`/`nc`), and successfully `SET`/`GET` values. (DONE)

### Week 2 — Concurrency (done)

**Goal:** multiple clients can use the server at the same time without breaking it.

- Multi-threaded handling — a thread pool serving connections (rather than one thread per connection) (DONE)
- Mutex protection around the hash table (DONE)(or sharded locks across buckets, for a performance/talking-point upgrade)(NOT DONE TO DO !!!!!!!!!)
- New commands: `INCR`, `EXISTS`, `EXPIRE` (TTL) (DONE)
- A background thread that sweeps and removes expired keys (DONE)
- **Milestone:** several clients hit the server simultaneously with no data corruption — demo with a small load-test script.

### Week 3 — Persistence + robustness

**Goal:** the server survives a restart, and handles bad input gracefully.

- Periodic snapshotting — dump the in-memory store to disk, reload on startup
- (Optional, stretch) append-only log (AOF-style) for durability between snapshots
- Robust handling of malformed commands, client disconnects, and partial `recv()`/`send()` calls
- **Milestone:** kill the server, restart it, and confirm the data is still there.

### Week 4 — Polish + one standout feature

**Goal:** the project looks and feels like a finished, professional piece of software.

Pick **one** feature to go deep on:
- **Benchmarking** — load tester measuring throughput/latency, with a results graph for the README
- **Replication** — a primary server and a read-only replica that mirrors writes
- **Pub/sub** — `SUBSCRIBE`/`PUBLISH` channel support

Plus:
- A clear, well-written README (this file) with an architecture diagram and key design decisions
- Clean, readable commit history on GitHub

## Status

- [ ] Week 1 — Core engine + single-client networking
- [ ] Week 2 — Concurrency
- [ ] Week 3 — Persistence + robustness
- [ ] Week 4 — Polish + standout feature

## Author

Tom Walker — [GitHub](https://github.com/) · [LinkedIn](https://linkedin.com/)
