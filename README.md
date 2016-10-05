# OS SNU 16 - Proj1
> <b>Team Member</b> 김희연 / 강수람 / 최윤영 / 장준영(Junyoung Clare Jang)

## How to Build/Run

<b>1. How to Compile Kernel</b>

```
$ cd <your kernel path>
$ export PATH="$PATH:<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/"
$ ./build.sh tizen_tm1 USR
$ sudo ls
$ export CROSS_COMPILE="<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin/armv7l-tizen-linux-gnueabi-"
$ scripts/mkmodimg.sh
$ tar cf IMAGE.tar -C arch/arm/boot dzImage -C ../../../usr/tmp-mod modules.img
```


<b>2. How to Compile Test Program</b>

```
$ cd <your kernel path>/test
$ vi Makefile
```

Makefile 두 번째 줄을 다음과 같이 변경한다.

```
GCCROOT=<extraction path>/opt/tizen-toolchain-4.9~git-i686_armv7l-tizen-linux-gnueabi-20151113/bin
```
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
$ sdb push test /home/developer/
$ sdb shell
$ /home/developer/test
```


<b>4. Test Result</b>

테스트 프로그램을 실행시키면 다음과 같은 결과를 얻을 수 있다.

```
systemd,1,1,0,155,2,0
	systemd-journal,155,1,1,0,187,0
	systemd-udevd,187,1,1,0,480,0
	syslogd,480,1,1,0,495,0
	Xorg,495,1,1,0,556,0
	enlightenment,556,1,1,0,561,5000
.
.
.
	cbhm,1155,1,1,0,1157,5000
	contacts-servic,1157,1,1,0,1159,200
	callmgrd,1159,1,1,0,1165,200
	pushd,1165,1,1,0,1217,200
	popup-launcher,1217,1,1,0,0,200
kthreadd,2,1,0,3,0,0
	ksoftirqd/0,3,1,2,0,4,0
	kworker/0:0,4,1,2,0,5,0
	kworker/0:0H,5,1,2,0,6,0
	kworker/u8:0,6,1,2,0,7,0
	migration/0,7,1,2,0,8,0
.
.
.
```


## High-level Design and Implementation

- 입력값 buf와 nr이 NULL일 경우와 유저와 커널 사이 교환이 제대로 되지 않을 경우 각각 -EINVAL, -EFAULT 에러를 발생시키도록 한다.
- 동적할당에 실패할 경우 -ENOMEM 에러를 발생시킨다.
- 첫번째 자식과 형제를 우선시하는 트리 기반의 DFS를 구상한다. → `walk_process_tree(int *)`
- 모든 성공 프로세스의 정보를 기록을 하고 → `copy_to_prinfo_from_task(struct prinfo *, struct task_struct *)`
- 재귀적 방법을 사용하여 전위 순회(preorder)로 정렬한다 → `copy_in_preorder(struct task_struct *, int *)`


## Lessons Learned
- 시스템콜의 기본 구조에 대해 이해하였다
- 더블링크드리스트 프로세스 트리 구조를 배웠다


