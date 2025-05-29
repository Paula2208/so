#!/bin/bash
RED="red_operativos"
PASS="SISoperativos2025*"

echo -e "\nConectando a Hamachi...\n"
sudo hamachi login

echo "\nComprobando si ya estás en la red $RED...\n"
if sudo hamachi list | grep -q "$RED"; then
    echo "Ya eres miembro (o dueño) de la red '$RED'. No se requiere join."
else
    echo "Uniéndose a la red $RED..."
    sudo hamachi join $RED $PASS
fi

echo "\nMiembros actuales de la red:\n"
sudo hamachi list
