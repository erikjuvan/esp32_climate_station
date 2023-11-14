import socket

# Define the server IP address and port
server_ip = '0.0.0.0'  # Listen on all available network interfaces
port = 8888

# Create a socket object
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the server IP and port
server_socket.bind((server_ip, port))

# Listen for incoming connections
server_socket.listen(5)  # Allow up to 5 queued connections

print("Server is listening on port", port)

while True:
    # Accept a connection from a client
    client_socket, client_address = server_socket.accept()
    print("Accepted connection from", client_address)

    while True:
        # Receive data from the client
        received_data = client_socket.recv(1024)
        if not received_data:
            break  # No more data from the client, exit the inner loop
        print("Received data from client:", received_data.decode())

    # Close the connection to the client
    client_socket.close()
