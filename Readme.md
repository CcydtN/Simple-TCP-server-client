# Simple TCP server and client

# How to run
If you have `just`,
```bash
# For debug build
just debug

# For release build
just release

# To test both server and client
python test.py

# To run server (accept income connection from 127.0.0.1:8080)
./buildir/debug/server

# To run client (connect to 127.0.0.1:8080, with client id 1)
./buildir/debug/client 127.0.0.1 8080 1
```

For those who don't have `just`,
```bash
# Build the project
mkdir build
cd build
cmake ..
cmake --build .

# To test both server and client
python test.py

# To run server (accept income connection from 127.0.0.1:8080)
./buildir/debug/server

# To run client (connect to 127.0.0.1:8080, with client id 1)
./buildir/debug/client 127.0.0.1 8080 1
```

## Todo list
- [X] project folder structure
- [X] basic build system
- [X] tcp server
  - [X] listen to incoming connection
- [X] tcp client
  - [X] connect to a server
  - [X] send message periodically (e.g. every second)
  - [X] Handle large response (larger than buffer size)
    - [X] Make send message and recivie response non-blocking
  - [X] read command line options


## Idea for improvement
- [ ] ~~write a class to handle all the async task for client~~
- [X] Wrap file descriptor to handle `close` when out of scope
- [ ] Use `getopt` for reading command line option


## Consideration
- Use <experimental/net> header?
  - Pros: safer to use, avoid direct use of POSIX API
  - Cons: lack of documentation
  - Conclusion: No, missing documentation will make me spending more time to read the code than actually doing work.

- Use `getopt`?
  - Why using `getopt`?
    - A more standard and safe way to parse the command line option.
  - Why not using `getopt`?
    - Is it necessary? The main goal should be TCP connection.
