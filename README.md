# dotnetAssemblyInjector
A dll to inject managed assemblies into a dotnet application. Made using the [official tutorial](https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting) on the dotnet website. 

# Build
```
mkdir objs
mkdir bin
g++ src/payload.cpp -c -o objs/payload.o -Wall -g
g++ src/log.cpp -c -o objs/log.o -Wall -g
g++ -shared objs/payload.o objs/log.o -Llib -lnethost -Lobjs -llog -o bin/payload.dll -g
```
# Use
