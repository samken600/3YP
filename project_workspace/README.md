# RIOT Setup

* Install and set up [git](https://help.github.com/articles/set-up-git/)
* Install the build-essential packet (make, gcc etc.). This varies based on the operating system in use.
* Install [Native dependencies](https://github.com/RIOT-OS/RIOT/wiki/Family:-native#dependencies)
* Install [OpenOCD](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD)
* Install [GCC Arm Embedded Toolchain](https://launchpad.net/gcc-arm-embedded)
* On OS X: install [Tuntap for OS X](http://tuntaposx.sourceforge.net/)
* [additional tweaks](https://github.com/RIOT-OS/RIOT/wiki/Board:-Samr21-xpro) necessary to work with the targeted hardware (ATSAMR21/30)
* Install `netcat` with IPv6 support (if necessary)

  ```bash
  sudo apt-get install netcat-openbsd
  ```

## Installing the Arm toolchain

The steps found [Here](http://watr.li/samr21-dev-setup-ubuntu.html) can be useful to help installing the toolchain. Alternatively, the following script can be run to install the toolchain.

```
ARM_URLBASE=https://developer.arm.com/-/media/Files/downloads/gnu-rm
ARM_URL=${ARM_URLBASE}/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
ARM_MD5=fe0029de4f4ec43cf7008944e34ff8cc
ARM_FOLDER=gcc-arm-none-eabi-9-2019-q4-major

echo 'Installing arm-none-eabi toolchain from arm.com' >&2 && \
mkdir -p /opt && \
curl -L -o /opt/gcc-arm-none-eabi.tar.bz2 ${ARM_URL} && \
echo "${ARM_MD5} /opt/gcc-arm-none-eabi.tar.bz2" | md5sum -c && \
tar -C /opt -jxf /opt/gcc-arm-none-eabi.tar.bz2 && \
rm -f /opt/gcc-arm-none-eabi.tar.bz2 && \
echo 'Removing documentation' >&2 && \
rm -rf /opt/gcc-arm-none-eabi-*/share/doc
# No need to dedup, the ARM toolchain is already using hard links for the duplicated files

export PATH:/opt/${ARM_FOLDER}/bin
```

## Installing OpenOCD

Navigate to the [OpenOCD](../openocd) folder. 
Process is something like: 

```bash 
./bootstrap 
./configure -- enable-ftdi 
make all 
sudo make install 
``` 

See the README within the directory for more detailed information.
