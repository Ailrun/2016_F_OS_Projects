# Operating System Project 4 - Team 16

## Team Members

- 강수람 ([Github](https://github.com/sooram))
- 김희연 ([Github](https://github.com/s-01))
- 장준영 ([Github](https://github.com/ailrun))

## How to Build/Run

- How to Compile Kernel

```
$ cd <this kernel path>
$ export PATH="$PATH:<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/"
$ ./build.sh tizen_tm1 USR
$ sudo ls
$ export CROSS_COMPILE="<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/armv7l-tizen-linux-gnueabi-"
$ scripts/mkmodimg.sh
$ tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img
```

- How to Run Program

Tizen을 다운로드 모드로 변경하고 Linux PC와 연결한다.

```
$ sudo lthor IMAGE.tar
```

- How to Build Test Program

`test` directory 로 이동하여 `Config.mk` 의 `TOOLBASEDIR` 을 올바르게 설정한 뒤 `make` 명령을 실행한다.

Tizen toolchain 의 경로에 따른 올바른 `TOOLBASEDIR` 다음과 같다.

1. Tizen toolchain 이 `/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin`에 있을 때
    - `TOOLBASEDIR=` 로 설정한다.
2. Tizen toolchain 이 `/home/myuser/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin`에 있을 때
    - `TOOLBASEDIR=/home/myuser` 로 설정한다.

- How to Use Test Program

다음 명령을 순서대로 실행한다.

```
mylinux$ sdb push proj4.fs /home/developer
mylinux$ sdb push gpsupdate /home/developer
mylinux$ sdb push file_loc /home/developer
mylinux$ sdb shell
sh-4.1$ cd ~
sh-4.1$ su
sh-4.1# mkdir proj4
sh-4.1# mount -o loop -t ext2 /home/developer/proj4.fs /home/developer/proj4
sh-4.1# ./gpsupdate
sh-4.1# echo "a" > proj4/file1.txt
sh-4.1# ./file_loc proj4/file1.txt
```

그러면 다음과 같은 결과를 얻을 수 있다.

```
sh-4.1# ./file_loc proj4/file1.txt 
GPS coordinates: 37.449933, 126.950296
Accuracy: 25.000000m
Google map link: https://www.google.com/maps/@37.449933,126.950296
```

생성한 시기에 따라서 위치가 바뀌는 것 또한 확인할 수 있다.

## High-level Design and Implementation

- `set_gps_location`은 write lock을 걸고 location을 user의 location으로 assign한다. user의 location이 NULL이면 -EINVAL error를, user로부터 정보를 제대로 받아오지 못하면 -EFAULT error를 낸다.
- `get_gps_location`은 path.dentry의 inode에서 GPS location을 받아와 user에게 결과를 넘겨준다. 이 과정에서 오류가 나면 -EFAULT를 반환하고 user의 정보가 NULL이면 -EINVAL을 반환한다. get_gps_location이 제대로 수행되면 0을, 제대로 수행되지 않으면 -1을 반환한다.
- `file_loc.c`는 넘겨받을 정보의 수가 일치하는지 확인하고, `get_gps_location`을 이용해 GPS 정보를 받아와 latitude, longitude, accuracy 그리고 Google Map Link를 프린트한다. malloc으로 할당한 값들을 free 시키고 0을 반환한다.

## Lessons Learned

- System Call에 대해 다시 한 번 생각해볼 수 있었다
- GPS 시스템의 전체 흐름을 생각해볼 수 있었다
