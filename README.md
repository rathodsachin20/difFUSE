## A File System in User Space

Implementation of a version of Unix file system in user space. The file system is created using [FUSE](https://github.com/libfuse/libfuse) library. This implementation provides operations such as create, open, read, write for files and directories. FUSE library traps the file system calls on a device mounted with new file system and calls appropriate handler in the user space implementation.

### Team:
Sachin Rathod (rathodsachin20@gmail.com)  
Sahaj Biyani (sahajbiyani@gmail.com)  


### Usage:
To Compile:
```
make clean
make
```

To Run: Assumes disk device attached as /dev/vdb. (currently hardcoded in src/layer3.c main function)
```
mkdir tmp
./bin/exec -s tmp
```


