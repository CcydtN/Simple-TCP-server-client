# Simple TCP server and client

# How to run
> WIP

## Todo list
- [X] project folder structure
- [X] basic build system
- [X] a working tcp server
- [X] a working tcp client
  - [X] send message periodically (e.g. every second)
  - [ ] Handle large response (larger than buffer size)
    - [ ] Make send message and recivie response non-blocking

## Consideration
- Use <experimental/net> header?
  - Pros: safer to use, avoid direct use of POSIX API
  - Cons: lack of documentation, the api might change in the future
  - Conclusion: No, missing documentation will make me spending more time to read the code than actually doing work.
