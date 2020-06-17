# Lazy Pirate
Same as client-server except we add client side reliability

It's a brute force solution where the client waits for a reply for X seconds.
The client retries multiple times before giving up
After success or failure, the socket is destroyed and a new request can be made
