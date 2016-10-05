# OS SNU 16 - Proj1

### How to build/run

- How to Compile Kernel

```
$ cd <your kernel path>
$ export PATH="$PATH:<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/"
$ ./build.sh tizen_tm1 USR
$ sudo ls
$ export CROSS_COMPILE="<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/armv7l-tizen-linux-gnueabi-"
$ scripts/mkmodimg.sh
$ tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img
```
<br/>

- How to Compile Test Program

```
$ cd <your kernel path>/test
```

Makefile 두 번째 줄을 다음과 같이 변경한다.<br/>
GCCROOT=<\extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin
```
$ make
```
<br/>

- How to Run Test Program

Tizen을 다운로드 모드로 변경하고 Linux PC와 연결한다.
```
$ sudo lthor IMAGE.tar

# The device will be rebooted
$ sdb devices

# Check your device as TM1
$ sdb push test /home/developer/
$ sdb shell
$ su
$ ./test
```
<br/>

## High-level design and implementation

## Lessons learned
