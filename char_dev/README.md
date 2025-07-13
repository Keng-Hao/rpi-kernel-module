# Raspberry Pi Char Device Driver

This is a simple Linux character device driver module for educational purposes.
Tested on Raspberry Pi (kernel 6.x).

## Features

- Register char device with dynamic major number
- Support open, read, write, release
- User-space communication via /dev


## Build & Test

```bash
make
sudo insmod mycharedv.ko
dmesg | tail 

# Create device node (example major=240)
sudo mknod /dev/mychardev c 240 0
sudo chmod 666 /dev/mychardev

#Test 
cat /dev/mychardev
echo "hello kernel" > /dev/mychardev
cat /dev/mychardev

#Unload
sudo rmmod mychardev
```
