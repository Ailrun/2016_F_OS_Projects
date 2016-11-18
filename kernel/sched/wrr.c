#include <linux/syscalls.h>

#include "sched.h"

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
}

static void yield_task_wrr(struct rq *rq)
{
}

static bool
yield_to_task_wrr(struct rq *rq, struct task_struct *p, bool preempt)
{
	return false;
}

static void
 check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	return NULL;
}

static void put_prev_task_wrr (struct rq *rq, struct task_struct *p)
{
}

#ifdef CONFIG_SMP

static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)
{
	return 0;
}

static void migrate_task_rq_wrr(struct task_struct *p, int next_cpu)
{
}

static void pre_schedule_wrr(struct rq *this_rq, struct task_struct *task)
{
}

static void post_schedule_wrr(struct rq *this_rq)
{
}

static void task_waking_wrr(struct task_struct *task)
{
}

static void task_woken_wrr(struct rq *this_rq, struct task_struct *task)
{
}

static void
set_cpus_allowed_wrr(struct task_struct *p, const struct cpumask *newmask)
{
}

static void rq_online_wrr(struct rq *rq)
{
}

static void rq_offline_wrr(struct rq *rq)
{
}

#endif /* CONFIG_SMP */

static void set_curr_task_wrr(struct rq *rq)
{
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
}

static void task_fork_wrr(struct task_struct *p)
{
}

static void switched_from_wrr(struct rq *this_rq, struct task_struct *task)
{
}

static void switched_to_wrr(struct rq *this_rq, struct task_struct *task)
{
}

static void
prio_changed_wrr(struct rq *this_rq, struct task_struct *task, int oldprio)
{
}

static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
	return 0;
}

#ifdef CONFIG_FAIR_GROUP_SCHED

static void task_move_group_wrr(struct task_struct *p, int on_rq)
{
}

#endif /* CONFIG_FAIR_GROUP_SCHED */


const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,
	.yield_to_task		= yield_to_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,
	.migrate_task_rq	= migrate_task_rq_wrr,

	.pre_schedule		= pre_schedule_wrr,
	.post_schedule		= post_schedule_wrr,
	.task_waking		= task_waking_wrr,
	.task_woken		= task_woken_wrr,

	.set_cpus_allowed       = set_cpus_allowed_wrr,
	.rq_online              = rq_online_wrr,
	.rq_offline             = rq_offline_wrr,
#endif /* CONFIG_SMP */

	.set_curr_task          = set_curr_task_wrr,
	.task_tick		= task_tick_wrr,
	.task_fork		= task_fork_wrr,

	.switched_from		= switched_from_wrr,
	.switched_to		= switched_to_wrr,
	.prio_changed		= prio_changed_wrr,

	.get_rr_interval	= get_rr_interval_wrr,

#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group	= task_move_group_wrr,
#endif /* CONFIG_FAIR_GROUP_SCHED */
};
