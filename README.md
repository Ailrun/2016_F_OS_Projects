# OS SNU 16 - Proj2
> <b>Team Member</b> 김희연 / 강수람 / 장준영(Junyoung Clare Jang)

## How to Build/Run

<b>1. How to Compile Kernel</b>

```
$ cd <this kernel path>
$ export PATH="$PATH:<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/"
$ ./build.sh tizen_tm1 USR
$ sudo ls
$ export CROSS_COMPILE="<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/armv7l-tizen-linux-gnueabi-"
$ scripts/mkmodimg.sh
$ tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img
```


<b>2. How to Compile Test Program</b>

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


<b>3. How to Run Test Program</b>

Tizen을 다운로드 모드로 변경하고 Linux PC와 연결한다.

```
$ sudo lthor IMAGE.tar

# The device will be rebooted
$ sdb devices

# Check your device as TM1
$ sdb push ./rotd/rotd /home/developer
$ sdb push ./selector/selector /home/developer
$ sdb push ./trial/trial /home/developer

$ sdb shell
$ /home/developer/selector <Buffer Length>
$ /home/developer/trial <Buffer Length>
```


<b>4. Test Result</b>

테스트 프로그램을 실행시키면 다음과 같은 결과를 얻을 수 있다.

```
$ ./rotd           
$ ./selector 1 &   
1214
$ ./trial &
1215
$ ./trial &
1216
...
selector: 56553
trial: 2^2*11^1*12853^1
selector: 56554
trial: 2^1*3^4*3491^1
selector: 56555
trial: 2^4*13^1*2719^1
selector: 56556
trial: 2^1*83^1*3407^1
selector: 56557
trial: 2^2*3^1*7^1*6733^1
selector: 56558
trial: 2^1*37^1*7643^1
selector: 56559
trial: 2^3*19^1*61^2
selector: 56560
trial: 2^1*3^1*107^1*881^1
trial: 2^1*3^1*107^1*881^1
trial: 2^1*3^1*107^1*881^1
trial: 2^1*3^1*107^1*881^1
selector: 56561
trial: 2^2*141403^1
selector: 56562
trial: 2^1*43^1*6577^1
selector: 56563
trial: 2^7*3^2*491^1
selector: 56564
trial: 2^1*7^1*11^1*3673^1
trial: 2^1*7^1*11^1*3673^1
selector: 56565
trial: 2^2*141413^1
selector: 56566
trial: 2^1*3^1*23^1*4099^1
selector: 56567
trial: 2^3*70709^1
selector: 56568
trial: 2^1*13^1*21757^1
selector: 56569
trial: 2^2*3^1*17^1*47^1*59^1
selector: 56570
trial: 2^1*282851^1
```



## High-level Design and Implementation
- 유저와 커널 사이 교환이 제대로 되지 않을 경우 -1을 Return 한다. 이때 lock을 걸고 작업해야 한다.
- read와 write의 reference 변수, 그리고 write를 기다리는 작업의 수와 큐를 포함한 rotareat_t라는 struct의 Pointer Array를 구현한다.
- Array에 degree를 포함하는 작업이 있고 read/write lock이 잡혀 있지 않다면 기다리고 있는 작업을 깨운다. &rarr; `set_rotation`
- Lock의 범위를 low와 high 변수가 체크하고 이를 벗어나면 -1을 return한다.
- Lock의 범위 안에 작업이 있을 경우 waiting 큐에 넣고, 없을 경우엔 array를 초기화 한다.
- Lock의 범위 안에서 Lock을 걸 수 있는 상태이면 read/write의 reference를 증가시키고 lock을 건다.
- write의 경우 lock을 걸 때 write를 기다리는 작업의 수를 하나 감소시킨다.
- Unlock의 경우 read/write reference를 감소시키며 degree를 포함하는 다음 작업이 있다면 이를 깨운다.
- 기다리는 큐도 없고 lock도 잡혀 있지 않다면 array를 초기화 한다.
&rarr; `rotlock_read`, `rotlock_write`, `rotunlock_read`, `rotunlock_write`

## Lessons Learned
- Reader/Writer Lock의 개념을 이해하였다.
- Queue를 이용한 Lock을 고려해볼 수 있었다.
