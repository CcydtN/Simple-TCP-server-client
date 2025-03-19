# Simple TCP server and client

# How to run
> WIP

## Todo list
- [X] project folder structure
- [X] basic build system
- [X] tcp server
  - [X] listen to incoming connection
- [ ] tcp client
  - [X] connect to a server
  - [X] send message periodically (e.g. every second)
  - [X] Handle large response (larger than buffer size)
    - [X] Make send message and recivie response non-blocking
  - [X] read command line options


## Idea for improvement
- [ ] write a class to handle all the async task for client
- [ ] Wrap file descriptor to handle `close` when out of scope

## Consideration
- Use <experimental/net> header?
  - Pros: safer to use, avoid direct use of POSIX API
  - Cons: lack of documentation, the api might change in the future
  - Conclusion: No, missing documentation will make me spending more time to read the code than actually doing work.
