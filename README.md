# libwebsockets-example

A simple libwebsockets example.

## stand subprotocol
< https://www.iana.org/assignments/websocket/websocket.xml#subprotocol-name>

## create pem file
``` shell
openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
```

## Build
```bash
mkdir build
cd build
cmake ..
make
```

## Run
In one terminal run:
```bash
./server
```

In another terminal run:
```bash
./client
```

In web browse
<http://127.0.0.1:8000>
