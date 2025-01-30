# PyCppFifoBridge

PyCppFifoBridge is a minimal example of inter-process communication between C++ and Python using Linux named pipes (FIFOs). The C++ server listens on two FIFOs (in and out), and the Python client sends commands and reads responses.

## Problem and solution

PyCppFifoBridge is useful when you need lightweight, low-latency communication between C++ and Python processes on the same machine. Unlike an HTTP server running on localhost, FIFOs avoid the overhead of networking stacks, reducing latency and resource usage.

## Project Structure

File PipeServer.hpp implements the C++ PipeServer class. Main function in example.cpp presents example usage of that class. In the pipe_client.py is a PipeClient class. Example client-side code is in example.py.

## Building and Running

```bash
g++ example.cpp PipeServer.hpp -o server
./server
python example.py
```

## Notes

- Make sure the UUID in PipeClient (e.g., "12345") matches the one passed to PipeServer.
- The server automatically removes the FIFOs on stop.
