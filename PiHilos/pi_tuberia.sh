#!/bin/bash

echo -e "\nCálculo del número pi (Leibniz) con Hilos\n"

echo -e "\nCon x Hilos y un pipe compartido\n"
gcc pi_tuberia.c -o pi_tuberia && ./pi_tuberia
