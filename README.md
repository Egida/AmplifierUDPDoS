# Attention!

THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL USE ONLY! IF YOU ENGAGE IN ANY ILLEGAL ACTIVITY THE AUTHOR DOES NOT TAKE ANY RESPONSIBILITY FOR IT. BY USING THIS SOFTWARE YOU AGREE WITH THESE TERMS.

# Overview

Universal stress testing (DoS) tool with UDP amplification.

For example:
- DNS amplification DoS attack;
- memcached amplification DoS attack;
- NTP amplification DoS attack;

# Features

- Calculating amplification coefficient;
- server list support;
- amplification factor setting;
- setting binary data to send via file.

You can specify an amplification factor and the program will scan all the servers from the list and calculate the amplification factor for each server. Only servers that satisfy your amplification factor will be used for the attack.

# Usage

program [target-IP] [target-port] [file with server list] [binary file with packet] [minimum coefficient amplification or 0 if you want to use all servers] [sleep ms]

## Usage example

program 192.168.1.25 80 ./servers.txt ./packet.bin 50 0

# LICENSE

This software is distributed under [MIT](https://opensource.org/licenses/MIT) license.
