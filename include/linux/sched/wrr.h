#ifndef _SCHED_WRR_H
#define _SCHED_WRR_H

#define MAX_WRR_WEIGHT		20
#define MIN_WRR_WEIGHT		1

#define DEFAULT_WRR_WEIGHT	10

/*
 * default timeslice is 10 msecs (used only for SCHED_WRR tasks).
 */
#define WRR_TIMESLICE		(10 * HZ / 1000)

#endif /* _SCHED_WRR_H */
