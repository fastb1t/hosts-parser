#!/bin/bash

if ! [ -e "bin" ]
then
{
    mkdir bin
}
fi

if [ -f "src/main.cpp" ]
then
{
    g++ src/main.cpp -o bin/hosts-parser -static-libstdc++ -static-libgcc -s -Os
}
else
{
    echo "[-] Error! Source code not found."
}
fi
