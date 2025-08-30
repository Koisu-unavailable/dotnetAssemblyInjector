# dotnetAssemblyInjector
A dll to inject managed assemblies into a dotnet application.

# Build
```
g++ main.cpp -c 
g++ -shared main.o -L./ -lnethost -o main.dll 
```
