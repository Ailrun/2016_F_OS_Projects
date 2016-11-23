# OS SNU 16 - Proj3
> Team Member 강수람 / 김희연 / 장준영 (Junyoung Clare Jang)

## How to Build/Run

1. How to Compile Kernel

```c
$ cd <this kernel path>
$ export PATH="$PATH:<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/"
$ ./build.sh tizen_tm1 USR
$ sudo ls
$ export CROSS_COMPILE="<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/armv7l-tizen-linux-gnueabi-"
$ scripts/mkmodimg.sh
$ tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img
```
<br/>

2. How to Compile Test Program

```
$ cd <this kernel path>/test
```

다음의 경로에서 path를 변경한다.

```
$ vi Config.mk
TOOLBASEDIR=<extraction path>
```

Config.mk의 TOOLBASEDIR을 tizen 컴파일러들이 있는 opt의 부모 directory로 지정한다.<br/>
ex) tizen 컴파일러가 /opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin에 있다면<br/>
TOOLBASEDIR=<br/>
or<br/>
/home/user/download/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin에 있다면<br/>
TOOLBASEDIR=/home/user/download<br/>

```
$ make
```

3. How to Run Test Program

Tizen을 다운로드 모드로 변경하고 Linux PC와 연결한다.

```
$ sudo lthor IMAGE.tar

# The device will be rebooted
$ sdb devices

# Check your device as TM1
$ sdb push -NEED TO MODIFY-

$ sdb shell
$ /home/developer/ -NEED TO MODIFY-
```

## High-level Design and Implementation


## Lessons Learned


## Investigate

Demonstrate that your scheduler works with a test program that calculates the prime factorization of a number using the naive Trial Division method.<br/>
Track how long this program takes to execute with different weightings set and plot the result.<br/>
You should choose a number to factor that will take sufficiently long to calculate the prime factorization of, such that it demonstrates the effect of weighting has on its execution time.<br/>
You can measure the execution time either internally in the program's code or externally so long as it is sufficiently accurate to show the effect of weighting.<br/>

You should provide a complete set of results that show all your tests.<br/>
If there are any results that do not yield execution time proportional to weights, explain why.<br/>
Your results and any explanations should be put in the README.md file in the project branch of your team's repository.<br/>
Your plot should be named plot.pdf and should be put next to the README.md file.<br/>
