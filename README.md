# OS SNU 16 - Proj3

> Team Member 강수람 / 김희연 / 장준영 (Junyoung Clare Jang)

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

- How to Compile Test Program

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

- How to Run Test Program

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

- `sched_wrr_entity`는 Task에 포함되어 있는 structure로 Task마다 weight가 다르고, tick마다 time_slice를 체크하며 run queue를 구성하기 위해 run_list를 포함한다.<br/>
- `wrr_rq`는 sched_wrr_entity의 entity를 run_queue에 넣기 위해 dummy head를 포함한다. cursor는 현재 running task를 추적하고, wrr_weight_total은 run queue 안에 있는 task weight의 합을 나타낸다. wrr_nr_running 현재 돌고 있는 task의 수를 나타내며 wrr_runtime_lock은 wrr_rq에 lock을 걸기 위한 용도이다.<br/>
- 기존 core.c의 policy는 rt가 아니면 fair로 설정하게 되어 있었다. Weighted Round Robin 스케줄링의 wrr 설정을 가능하게 만들기 위해 rt가 아니면 wrr policy를 선택하도록 한다.<br/>
  wrr을 initialize하는 init_wrr_rq 함수를 추가하고,<br/>
  policy가 wrr이면 weight과 time_slice를 설정하고 wrr의 run_list를 초기화한다.<br/>
  &rarr; `core.c`<br/>
- `sched_setweight`은 현재 task의 policy가 wrr일 때 wrr의 weight를 설정한다.<br/>
- `sched_getweight`은 현재 task의 weight를 return한다.<br/>
- `init_wrr_rq`는 wrr_rq의 모든 변수를 초기화한다.<br/>
- `enqueue_task_wrr`은 lock을 걸고 현재 task list tail에 cursor가 가리키는 task를 추가하며 추가되는 task의 weight과 수를 더한 후 lock을 푼다.<br/>
- `dequeue_task_wrr`은 lock을 걸고 running task list의 앞부분을 delete한다. 그리고 cursor의 head를 다음 cursor의 head로 변경한다. 이후 삭제한 task의 weight과 수를 뺀 후 lock을 푼다.<br/>
- `pick_next_task_wrr`은 현재 wrr의 cursor를 불러와 time_slice를 weight * WRR_TIMESLICE(defined)로 정의한다.<br/>
- `task_tick_wrr`은 lock을 걸고 매 tick마다 time_slice를 줄인 후 lock을 푼다. 이때 run_list.next와 run_list.prev가 같지 않으면 cursor를 재정의한다.<br/>
- `task_fork_wrr`은 wrr의 weight과 time_slice를 부모 task의 값으로 재설정하며,<br/>
- `switched_to_wrr`은 wrr의 weight과 time_slice를 기본값으로 설정한다.<br/>
- `get_rr_interval_wrr`은 wrr의 weight과 정의된 WRR_TIMESLICE의 곱을 return한다.<br/>
- `find_lowest_rq`는 cpu마다 돌면서 wrr_weight_total이 lowest_weight보다 작고 available cpu를 찾는다.<br/>
- `select_task_rq_wrr`은 현재 task의 cpu의 rq를 찾아 lowest rq를 찾아 target을 정하고 그 target을 return한다.<br/>
<br/>

- cpu를 고르게 할당하기 위해 load balancing을 한다. 현재 cpu의 rq의 wrr_weight_total이 min_weight보다 작으면 현재 rq를 min_rq로 assign하며 반대의 경우 max_rq로 assign한다. min_rq와 max_rq가 같으면 return 한다. 같지 않을 경우엔 min_rq와 max_rq에 lock을 걸고 migratable하고 min_weight+wrr_se->weight < max_weight-wrr_se->weight의 조건을 만족하면 현재 task를 move_p로 assign한다. 그 후 move_p가 load에 contribute하는지 체크하고 max_rq를 task queue에 dequeue한다. 그 후 min_rq의 cpu를 현재 cpu로 설정하며 마찬가지로 move_p가 load에 contribute하는지 체크하고 min_rq를 task queue에 enqueue한다. &rarr; `load_balance_wrr`<br/>

## Lessons Learned
- Linux Scheduling 방식을 조사해보는 기회가 되었다.
- 직접 구현해봄으로써 Linux Schedule의 구조와 작동 원리에 대해 깊게 공부하였다.

## Investigate

Demonstrate that your scheduler works with a test program that calculates the prime factorization of a number using the naive Trial Division method.<br/>
Track how long this program takes to execute with different weightings set and plot the result.<br/>
You should choose a number to factor that will take sufficiently long to calculate the prime factorization of, such that it demonstrates the effect of weighting has on its execution time.<br/>
You can measure the execution time either internally in the program's code or externally so long as it is sufficiently accurate to show the effect of weighting.<br/>

You should provide a complete set of results that show all your tests.<br/>
If there are any results that do not yield execution time proportional to weights, explain why.<br/>
Your results and any explanations should be put in the README.md file in the project branch of your team's repository.<br/>
Your plot should be named plot.pdf and should be put next to the README.md file.<br/>
