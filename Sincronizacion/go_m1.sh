#!/bin/bash

echo -e "\nSincronización con Mutex (1) \n"

gcc mutex_1.c -o mutex_1 && ./mutex_1 #-lpthread -lm
