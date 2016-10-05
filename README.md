# OS SNU 16 - Proj1
> <b>Team Member</b> 김희연 / 강수람 / 최윤영 / 장준영 (Junyoung Clare Jang)

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
$ /home/developer/test <Buffer Length>
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
- 첫번째 자식과 형제를 우선시하는 tree 기반의 DFS를 구상한다. → `walk_process_tree(int *)`
- 모든 성공 process의 정보를 기록을 하고 → `copy_to_prinfo_from_task(struct prinfo *, struct task_struct *)`
- 재귀적 방법을 사용하여 전위 순회 (preorder)로 정렬한다 → `copy_in_preorder(struct task_struct *, int *)`
- swapper process (pid = 0) 는 Linux 상에서 실제로 swapper process로 동작하지 않으며 일반적으로 idle process이기 때문에, 출력하지 않았다.

## Lessons Learned

- System call의 기본 구조에 대해 이해하였다
- Double linked list process tree 구조를 배웠다
- Swapper process에 대해 배웠다.

## Investigate the Tizen process tree

1. Run your test program several times. Which fields in the prinfo structure change? Which ones do not? Discuss why different fields might change with different frequency.

   Upon several testings with no applications run, `name` field, `parent_pid` field, `first_child_pid` field, and `uid` field didn't change, but the others changed. The reason why fields change is simply because processes keep changing. Some process needs to be died after its task, some needs to start, and some proceeds and gets into a new state. These transitions make the fields in the prinfo structure change.

2. Start the mobile camera (or any other apps are fine) in the emulator, and re-run your test program. How many processes are started? What is/are the parent process(es) of the new process(es)? Close the browser (press the "Home" button). How many processes were destroyed? Discuss your findings.

   Before run camera:
	```
	.
	.
	.
	launchpad-proce,1020,1,1,1033,1023,0
		launchpad-loade,1033,1,1020,0,1034,5000
		launchpad-loade,1034,1,1020,0,1035,5000
		launchpad-loade,1035,1,1020,0,0,5000
	.
	.
	.
	kworker/1:3,730,1,2,0,839,0
	kworker/3:3,839,1,2,0,1624,0
	kworker/u8:2,1624,1,2,0,1738,0
	kworker/0:0,1738,1,2,0,1789,0
	kworker/u8:1,1789,1,2,0,1867,0
	kworker/0:3,1867,1,2,0,2038,0
	kworker/u8:3,2038,1,2,0,0,0

	```
   After run camera:
	```
	.
	.
	.
	launchpad-proce,1020,1,1,1033,1023,0
		launchpad-loade,1033,1,1020,0,1034,5000
		launchpad-loade,1034,1,1020,0,1035,5000
		camera,1035,1,1020,0,1694,5000
		launchpad-loade,1694,1,1020,0,0,5000
	.
	.
	.
	kworker/1:3,730,1,2,0,839,0
	kworker/3:3,839,1,2,0,1624,0
	kworker/u8:2,1624,1,2,0,1634,0
	kworker/0:2,1634,1,2,0,1738,0
	kworker/0:0,1738,1,2,0,1789,0
	kworker/u8:1,1789,1,2,0,1866,0
	kworker/0:1,1866,1,2,0,1867,0
	kworker/0:3,1867,1,2,0,1868,0
	kworker/0:4,1868,1,2,0,1893,0
	dcam_flash_thre,1893,1,2,0,1894,0
	img_zoom_thread,1894,1,2,0,1929,0
	ipp_cmd_1,1929,1,2,0,1931,0
	kworker/u8:3,1931,1,2,0,0,0
	```
   After close camera:
	```
	launchpad-proce,1020,1,1,1034,1023,0
		launchpad-loade,1034,1,1020,0,1694,5000
		launchpad-loade,1694,1,1020,0,1742,5000
		launchpad-loade,1742,1,1020,0,0,5000
	.
	.
	.
	kworker/1:3,730,1,2,0,839,0
	kworker/3:3,839,1,2,0,1624,0
	kworker/u8:2,1624,1,2,0,1634,0
	kworker/0:2,1634,1,2,0,1738,0
	kworker/0:0,1738,1,2,0,1789,0
	kworker/u8:1,1789,1,2,0,1866,0
	kworker/0:1,1866,1,2,0,1867,0
	kworker/0:3,1867,0,2,0,1868,0
	kworker/0:4,1868,1,2,0,1931,0
	kworker/u8:3,1931,1,2,0,0,0
	```
    7 processes are started upon camera running. The two parent processes of these new started ones are: 'launchpad-proce' and 'kthreadd'. When we close the camera application, 4 processes are destroyed.

3. In 4.2, you may notice that there are launchpad and launchpad-loader. Investigate these processes.

  1. Focusing on pids of launchpad, launchpad-loader, and applications, try 4.2 again. Explain changes of the process names or pids.

     The names remained the same, but pid of the last launchpad-loader was changed. `camera` got the former pid of the last launchpad-loader. 
  2. Explain what launchpad and launchpad-loader do. And Discuss the reason Tizen use them.

     Launchpad is a parent process of all applications and handles launch request from amd (application management daemon). It also manages launchpad-loaders, which are pre-initialized processes for applications. Using launchpad and launchpad-loader makes lauching applications fast, because applications can use pre-initialized parts from pre-initialized launchpad-loader process. This is why Tizen uses them. (referred to: [http://www.slideshare.net/silverlee2/tizen-application-inside-out])
