#!/bin/bash

echo -e "\nSincronización con Mutex (2 productor-receptor) \n"

gcc mutex_2.c -o mutex_2 && ./mutex_2 #-lpthread -lm
