#!/bin/bash

echo "Compilando syntax-commons"
cd syntax-commons/Debug
make
cd ..
cd ..
echo "Compilacion finalizada"

echo "Compilando planificador"
cd Planificador
gcc -Wall -L"../syntax-commons/Debug" -o "Planificador" *.c -lpthread -lcommons -lsyntax-commons
cd ..
echo "Compilacion finalizada"

echo "Compilando coordinador"
cd Coordinador
gcc -Wall -L"../syntax-commons/Debug" -o "Coordinador" *.c -lpthread -lcommons -lsyntax-commons
cd ..
echo "Compilacion finalizada"

echo "Compilando esi"
cd ESI
gcc -Wall -L"../syntax-commons/Debug" -o "ESI" *.c -lpthread -lcommons -lsyntax-commons -lparsi
cd ..
echo "Compilacion finalizada"

echo "Compilando instancia"
cd Instancia
gcc -Wall -L"../syntax-commons/Debug" -o "Instancia" *.c -lpthread -lcommons -lsyntax-commons -lparsi
cd ..
echo "Compilacion finalizada"