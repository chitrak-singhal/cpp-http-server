This is an http server created using C++.
It has the follwing functionality:
1) Parse requests using custom parse functions and respond appropriately
2) Extract URL path, read headers and perform the desired action
3) Supports concurrent connections using multithreading
4) Fetch a file using GET
5) Post a file using POST
6) Supports Gzip Compression

To start, run:
```
./start_server.sh
```
To test fetching and posting files use the directory flag to specify the directory:
```
./start_server.sh --directory <directory_name>
```
