from pipe_client import PipeClient

if __name__ == "__main__":
    # 12345 is an example UUID. If you want to run multiple Python clients
    # with their paired C++ servers inside your OS, each connection
    # must use a unique UUID to prevent conflicts between different instances.
    client = PipeClient("12345")
    client.open()

    response = client.run("hello")
    print("Response 1:", response)

    response = client.run("calculate")
    print("Response 2:", response)

    response = client.run("bye")
    print("Response 3:", response)

    # When the Python program exits, the FIFO connection will automatically close.
    # This means that if we want to start the Python client again, we must also restart the C++ server.
    # However, we can explicitly close the connection using `client.close()`
    # if we want to disconnect without exiting the Python program.
    client.close()
