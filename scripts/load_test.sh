#!/bin/bash

echo "Starting load test...\n"
echo "DEL counter" | nc -q1 localhost 6380
for i in $(seq 1 10); do
    (
        

        echo "SET key$i value$i" | nc -q1 localhost 6380
        echo "INCR counter" | nc -q1 localhost 6380
    ) &
done

wait

echo "All clients done. Checking counter..."
echo "GET counter" | nc -w2 localhost 6380