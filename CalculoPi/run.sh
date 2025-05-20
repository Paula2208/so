#!/bin/bash

echo -e "\nCálculo del número pi (Leibniz)\n"

echo -e "\nCon pipe()\n"
mkdir -p ex && gcc h1.c -o ex/h1
/usr/bin/time -f "Tiempo: %E" ./ex/h1

echo -e "\nCon memoria compartida\n"
mkdir -p ex && gcc h2.c -o ex/h2
/usr/bin/time -f "Tiempo: %E" ./ex/h2

echo -e "\nSecuencial\n"
mkdir -p ex && gcc h3.c -o ex/h3
/usr/bin/time -f "Tiempo: %E" ./ex/h3
