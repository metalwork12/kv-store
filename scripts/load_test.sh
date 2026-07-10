#!/bin/bash

echo "Starting load test...\n"

printf "AUTH password\nDEL counter\n" | nc -w2 localhost 6380

for i in $(seq 1 10); do
    (
        

        printf "AUTH password\nSET key$i value$i\n" | nc -w2 localhost 6380
        printf "AUTH password\nINCR counter\n" | nc -w2 localhost 6380

    ) &
done

wait

echo "All clients done. Checking counter..."
printf "AUTH password\nGET counter\n" | nc -w2 localhost 6380
