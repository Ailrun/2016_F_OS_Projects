#include <linux/syscalls.h>

#include "sched.h"

SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight)
{
	return 0;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid)
{
	return 0;
}

const struct sched_class wrr_sched_class = {
	.next			= NULL,
	.enqueue_task		= NULL,
	.dequeue_task		= NULL,
	.yield_task		= NULL,

	.check_preempt_curr	= NULL,

	.pick_next_task		= NULL,
	.put_prev_task		= NULL,

#ifdef CONFIG_SMP
	.select_task_rq		= NULL,
	.migrate_task_rq	= NULL,

	.pre_schedule		= NULL,
	.post_schedule		= NULL,
	.task_waking		= NULL,
	.task_woken		= NULL,

	.set_cpus_allowed       = NULL,
	.rq_online              = NULL,
	.rq_offline             = NULL,
#endif

	.set_curr_task          = NULL,
	.task_tick		= NULL,
	.task_fork		= NULL,

	.switched_from		= NULL,
	.switched_to		= NULL,
	.prio_changed		= NULL,

	.get_rr_interval	= NULL,

#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group	= NULL,
#endif
};
