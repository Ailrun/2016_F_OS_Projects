#include "sched.h"

#include <linux/syscalls.h>

int sched_wrr_timeslice = WRR_TIMESLICE;

#define rt_entity_is_task(rt_se) (!(rt_se)->my_q)

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
#ifdef CONFIG_SCHED_DEBUG
	WARN_ON_ONCE(!wrr_entity_is_task(wrr_se));
#endif
	return container_of(wrr_se, struct task_struct, wrr);
}

void init_wrr_rq(struct wrr_rq *wrr_rq)
{
	INIT_LIST_HEAD(&wrr_rq->queue);
	raw_spin_lock_init(&wrr_rq->wrr_runtime_lock);
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct wrr_rq *wrr_rq = &rq->wrr;
	
	if(!wrr_rq) {
		return;
	}
	raw_spin_lock(&wrr_rq->wrr_runtime_lock);
	list_add_tail(&(p->wrr.run_list),&(wrr_rq->rq->queue));
	rq->nr_running++;
	raw_spin_unlock(&wrr_rq->wrr_runtime_lock);
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct wrr_rq *wrr_rq = &rq->wrr;
	
	if(!wrr_rq)
		return;
	raw_spin_lock(&wrr_rq->wrr_runtime_lock);
	list_del_init(&(wrr_rq->rq->queue));
	rq->nr_running--;
	raw_spin_unlock(&wrr_rq->wrr_runtime_lock);
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

static struct sched_wrr_entity *pick_next_task_wrr_entity(struct rq *rq,
								struct wrr_rq *wrr_rq)
{
	struct sched_wrr_entity *next = NULL;
	struct list_head *queue = wrr->rq->queue;

	next = list_entry(queue->next, struct sched_wrr_entity, run_list);

	return next;
}

static struct task_struct *_pick_next_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *wrr_se;
	struct task_struct *p;
	struct wrr_wq *wrr_rq;
	
	wrr_rq = &rq->wrr;

	if (!wrr_rq->wrr_nr_running)
		return NULL;

	do {
		wrr_se = pick_next_wrr_entity(rq, wrr_rq);
		BUG_ON(!wrr_se);
		wrr_rq = wrr_se->my_q;
	} while (wrr_rq);

	p = wrr_task_of(wrr_se);
	p->se.exec_start = rq->clock_task;

	return p;
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct task_struct *p = _pick_next_task_wrr(rq);

#ifdef CONFIG_SMP

//	rq->post_schedule = 0;
#endif

	return p;
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
