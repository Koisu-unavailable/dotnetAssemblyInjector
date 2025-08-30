# dotnetAssemblyInjector
A dll to inject managed assemblies into a dotnet application. 

# Build
```
g++ payload.cpp -c 
g++ -shared main.o -Llib -lnethost -o payload.dll 
```
# Use
