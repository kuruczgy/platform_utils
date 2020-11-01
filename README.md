# minipc - mini IPC library
The primary use case is for creating small Unix domain socket interfaces.
Currently, the interface can consist of commands without parameters, or
variable length data. See the `send` python script for a client example.

Also includes an event loop implementation, for convenience.
