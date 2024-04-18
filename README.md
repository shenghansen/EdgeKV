# EdgeKV:KV systems optimized for edge environments
## Compilation
Compilers
- g++ >= 5.3.0 
- cmake >= 3.22

make sure you have install `brpc` in your server
```shell
git submodule update --init --recursive
mkdir build&&cd build
cmake ..
make
```
## Runing code
run this two commands to start datacenter and edge server.
```shell
./datacenterKV &
./edgeKV &
```
to get more information,please read `doc/EdgeKV.pdf`