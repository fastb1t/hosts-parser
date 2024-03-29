@echo off

if not exist "bin" (
    mkdir bin
)

if exist "src\main.cpp" (
    g++ src\main.cpp -o bin\hosts-parser.exe -static-libstdc++ -static-libgcc -s -Os
) else (
    echo [-] Error! Source code not found.
)
