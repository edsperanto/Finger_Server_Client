# Finger Server/Client
A simple client and server program that communicates via TCP
Created for CPSC 3500 by Edward Gao

### How to compile

```
make clean
make
```

### How to run
```
./fingerserver <port #>
./fingerclient username@hostname:port
```

### Strengths
- Allows multiple client connections
- Uses TCP, so it is more reliable

### Weaknesses
- Forks program to handle clients instead of threads, more expensive
- Limited functionality, only performs finger command