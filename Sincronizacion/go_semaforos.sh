#!/bin/bash

echo -e "\nSincronización con semáforos\n"

gcc semaforos.c -o semaforos && ./semaforos #-lpthread -lm
