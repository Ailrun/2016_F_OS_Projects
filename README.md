# Operating System Project 4 - Team 16

## Team Members

- 강수람 ([Github](https://github.com/sooram))
- 김희연 ([Github](https://github.com/s-01))
- 장준영 ([Github](https://github.com/ailrun))

## How to Build/Run

- How to Compile Kernel

- How to Compile Test Program

- How to Run Test Program

## High-level Design and Implementation

- `set_gps_location`은 write lock을 걸고 location을 user의 location으로 assign한다. user의 location이 NULL이면 -EINVAL error를, user로부터 정보를 제대로 받아오지 못하면 -EFAULT error를 낸다.
- `get_gps_location`은 path.dentry의 inode에서 GPS location을 받아와 user에게 결과를 넘겨준다. 이 과정에서 오류가 나면 -EFAULT를 반환하고 user의 정보가 NULL이면 -EINVAL을 반환한다. get_gps_location이 제대로 수행되면 0을, 제대로 수행되지 않으면 -1을 반환한다.
- `file_loc.c`는 넘겨받을 정보의 수가 일치하는지 확인하고, `get_gps_location`을 이용해 GPS 정보를 받아와 latitude, longitude, accuracy 그리고 Google Map Link를 프린트한다. malloc으로 할당한 값들을 free 시키고 0을 반환한다.

## Lessons Learned

- System Call에 대해 다시 한 번 생각해볼 수 있었다
- GPS 시스템의 전체 흐름을 생각해볼 수 있었다
