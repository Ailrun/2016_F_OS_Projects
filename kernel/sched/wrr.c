#include "sched.h"
#include <linux/slab.h>
#include <linux/rcupdate.h>

const struct sched_class wrr_sched_class;

static inline bool is_wrr_rq_empty(struct wrr_rq *wrr_rq)
{
	return list_empty(&wrr_rq->run_queue);
}

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	return container_of(wrr_se, struct task_struct, wrr);
}
static inline
struct sched_wrr_entity *sched_wrr_entity_of(struct list_head *run_list)
{
	return container_of(run_list, struct sched_wrr_entity, run_list);
}

void init_wrr_rq(struct wrr_rq *wrr_rq)
{
	INIT_LIST_HEAD(&wrr_rq->run_queue);

	wrr_rq->wrr_weight_total = 0;
	wrr_rq->wrr_nr_running = 0;
	wrr_rq->cursor = NULL;

	raw_spin_lock_init(&wrr_rq->wrr_runtime_lock);
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_se = &p->wrr;

	raw_spin_lock(&wrr_rq->wrr_runtime_lock);

	if (wrr_rq->cursor)
		list_add_tail(&wrr_se->run_list, &wrr_rq->cursor->wrr.run_list);
	else {
		wrr_rq->cursor = p;
		list_add_tail(&wrr_se->run_list, &wrr_rq->run_queue);
	}

	wrr_rq->wrr_weight_total += wrr_se->weight;
	wrr_rq->wrr_nr_running++;

	inc_nr_running(rq);
	p->on_rq = 1;

	raw_spin_unlock(&wrr_rq->wrr_runtime_lock);
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *next_cursor_head;

	raw_spin_lock(&wrr_rq->wrr_runtime_lock);

	next_cursor_head = wrr_se->run_list.next;

	list_del_init(&wrr_se->run_list);

	if (is_wrr_rq_empty(wrr_rq))
		wrr_rq->cursor = NULL;
	else if (p == wrr_rq->cursor) {
		if (next_cursor_head == &wrr_rq->run_queue)
			next_cursor_head = next_cursor_head->next;
		wrr_rq->cursor =
			wrr_task_of(sched_wrr_entity_of(next_cursor_head));
	}

	wrr_rq->wrr_weight_total -= wrr_se->weight;
	wrr_rq->wrr_nr_running--;

	dec_nr_running(rq);
	p->on_rq = 0;

	raw_spin_unlock(&wrr_rq->wrr_runtime_lock);
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct task_struct *cursor = rq->wrr.cursor;

	if (cursor)
		cursor->wrr.time_slice = cursor->wrr.weight * WRR_TIMESLICE;

	return cursor;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct task_struct *cursor;
	struct sched_wrr_entity *wrr_se;
	struct list_head *next_cursor_head;

	raw_spin_lock(&wrr_rq->wrr_runtime_lock);

	if (!rq->wrr.cursor)
		goto skip;

	cursor = wrr_rq->cursor;
	wrr_se = &cursor->wrr;

	if (--wrr_se->time_slice)
		goto skip;

	if (wrr_se->run_list.next != wrr_se->run_list.prev) {
		next_cursor_head = wrr_se->run_list.next;
		if (next_cursor_head == &wrr_rq->run_queue)
			next_cursor_head = next_cursor_head->next;

		wrr_rq->cursor =
			wrr_task_of(sched_wrr_entity_of(next_cursor_head));
		set_tsk_need_resched(cursor);
	} else
		wrr_se->time_slice = wrr_se->weight * WRR_TIMESLICE;

skip:
	raw_spin_unlock(&wrr_rq->wrr_runtime_lock);
}

static void task_fork_wrr(struct task_struct *p)
{
	p->wrr.weight = p->real_parent->wrr.weight;
	p->wrr.time_slice = p->wrr.weight * WRR_TIMESLICE;
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	p->wrr.weight = DEFAULT_WRR_WEIGHT;
	p->wrr.time_slice = p->wrr.weight * WRR_TIMESLICE;
}

static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
	return task->wrr.weight * WRR_TIMESLICE;
}

static void yield_task_wrr(struct rq *rq)
{
}

static
bool yield_to_task_wrr(struct rq *rq, struct task_struct *p, bool preempt)
{
	return true;
}

static
void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	return;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{
}

static void set_curr_task_wrr(struct rq *rq)
{
}

static void switched_from_wrr(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
}

#ifdef CONFIG_SMP

static int find_lowest_rq(struct task_struct *p)
{
	struct wrr_rq *wrr_rq;
	int lowest_cpu = -1;
	unsigned long lowest_weight;

	int cpu;

	for_each_online_cpu(cpu) {
		wrr_rq = &cpu_rq(cpu)->wrr;
		if ((lowest_cpu == -1 ||
		     wrr_rq->wrr_weight_total < lowest_weight) &&
		    cpumask_test_cpu(cpu, tsk_cpus_allowed(p))) {
			lowest_cpu = cpu;
			lowest_weight = wrr_rq->wrr_weight_total;
		}
	}
	return lowest_cpu;
}

static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags)
{
	struct rq *rq;
	int cpu;
	int target;

	cpu = task_cpu(p);
	if (p->nr_cpus_allowed == 1)
		return cpu;

	rq = cpu_rq(cpu);

	rcu_read_lock();

	target = find_lowest_rq(p);
	if (target != -1)
		cpu = target;
	rcu_read_unlock();

	return cpu;
}

static
void set_cpus_allowed_wrr(struct task_struct *p, const struct cpumask *newmask)
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

static void rq_online_wrr(struct rq *rq)
{
}

static void rq_offline_wrr(struct rq *rq)
{
}

#endif /* CONFIG_SMP */

const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,
	.yield_to_task		= yield_to_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

	.set_curr_task		= set_curr_task_wrr,
	.task_tick		= task_tick_wrr,
	.task_fork		= task_fork_wrr,

	.switched_to		= switched_to_wrr,
	.switched_from		= switched_from_wrr,
	.prio_changed		= prio_changed_wrr,
	.get_rr_interval	= get_rr_interval_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,
	.set_cpus_allowed	= set_cpus_allowed_wrr,
	.pre_schedule		= pre_schedule_wrr,
	.post_schedule		= post_schedule_wrr,
	.task_waking		= task_waking_wrr,
	.task_woken		= task_woken_wrr,
	.rq_online		= rq_online_wrr,
	.rq_offline		= rq_offline_wrr,
#endif
};

#ifdef CONFIG_SCHED_DEBUG
extern void print_wrr_rq(struct seq_file *m, int cpu, struct wrr_rq *wrr_rq);

void print_wrr_stats(struct seq_file *m, int cpu)
{
	struct wrr_rq *wrr_rq;

	rcu_read_lock();

	wrr_rq = &cpu_rq(cpu)->wrr;
	print_wrr_rq(m, cpu, wrr_rq);
	rcu_read_unlock();
}
#endif /* CONFIG_SCHED_DEBUG */
