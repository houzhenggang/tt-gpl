#ifndef _LINUX_TRACEPOINT_H
#define _LINUX_TRACEPOINT_H

/*
 * Kernel Tracepoint API.
 *
 * See Documentation/tracepoint.txt.
 *
 * (C) Copyright 2008 Mathieu Desnoyers <mathieu.desnoyers@polymtl.ca>
 *
 * Heavily inspired from the Linux Kernel Markers.
 *
 * This file is released under the GPLv2.
 * See the file COPYING for more details.
 */

#include <linux/immediate.h>
#include <linux/types.h>
#include <linux/rcupdate.h>

struct module;
struct tracepoint;

struct tracepoint {
	const char *name;		/* Tracepoint name */
	DEFINE_IMV(char, state);	/* State. */
	void **funcs;
} __attribute__((aligned(8)));


#define TPPROTO(args...)	args
#define TPARGS(args...)		args

#ifdef CONFIG_TRACEPOINTS

#define TRACEPOINT_NOTIFY_PRIO	1

/*
 * it_func[0] is never NULL because there is at least one element in the array
 * when the array itself is non NULL.
 */
#define __DO_TRACE(tp, proto, args)					\
	do {								\
		void **it_func;						\
									\
		rcu_read_lock_sched_notrace();				\
		it_func = rcu_dereference((tp)->funcs);			\
		if (it_func) {						\
			do {						\
				((void(*)(proto))(*it_func))(args);	\
			} while (*(++it_func));				\
		}							\
		rcu_read_unlock_sched_notrace();			\
	} while (0)

#define __CHECK_TRACE(name, generic, proto, args)			\
	do {								\
		static const char __tpstrtab_##name[]			\
		__attribute__((section("__tracepoints_strings")))	\
		= #name;						\
		static struct tracepoint __tracepoint_##name		\
		__attribute__((section("__tracepoints"), aligned(8))) =	\
		{ __tpstrtab_##name, 0, NULL };				\
		if (!generic) {						\
			if (unlikely(imv_read(__tracepoint_##name.state))) \
				__DO_TRACE(&__tracepoint_##name,	\
					TPPROTO(proto), TPARGS(args));	\
		} else {						\
			if (unlikely(_imv_read(__tracepoint_##name.state))) \
				__DO_TRACE(&__tracepoint_##name,	\
					TPPROTO(proto), TPARGS(args));	\
		}							\
	} while (0)

/*
 * Make sure the alignment of the structure in the __tracepoints section will
 * not add unwanted padding between the beginning of the section and the
 * structure. Force alignment to the same alignment as the section start.
 *
 * The "generic" argument, passed to the declared __trace_##name inline function
 * controls which tracepoint enabling mechanism must be used.
 * If generic is true, a variable read is used.
 * If generic is false, immediate values are used.
 */
#define DEFINE_TRACE(name, proto, args)					\
	static inline void trace_##name(proto)				\
	{								\
		__CHECK_TRACE(name, 0, TPPROTO(proto), TPARGS(args));	\
	}								\
	static inline void _trace_##name(proto)				\
	{								\
		__CHECK_TRACE(name, 1, TPPROTO(proto), TPARGS(args));	\
	}								\
	static inline int register_trace_##name(void (*probe)(proto))	\
	{								\
		return tracepoint_probe_register(#name, (void *)probe);	\
	}								\
	static inline int unregister_trace_##name(void (*probe)(proto))	\
	{								\
		return tracepoint_probe_unregister(#name, (void *)probe);\
	}

extern void tracepoint_update_probe_range(struct tracepoint *begin,
	struct tracepoint *end);

#else /* !CONFIG_TRACEPOINTS */
#define DEFINE_TRACE(name, proto, args)			\
	static inline void trace_##name(proto)				\
	{ }								\
	static inline void _trace_##name(proto)				\
	{ }								\
	static inline int register_trace_##name(void (*probe)(proto))	\
	{								\
		return -ENOSYS;						\
	}								\
	static inline int unregister_trace_##name(void (*probe)(proto))	\
	{								\
		return -ENOSYS;						\
	}

static inline void tracepoint_update_probe_range(struct tracepoint *begin,
	struct tracepoint *end)
{ }
#endif /* CONFIG_TRACEPOINTS */

/*
 * Connect a probe to a tracepoint.
 * Internal API, should not be used directly.
 */
extern int tracepoint_probe_register(const char *name, void *probe);

/*
 * Disconnect a probe from a tracepoint.
 * Internal API, should not be used directly.
 */
extern int tracepoint_probe_unregister(const char *name, void *probe);

extern int tracepoint_probe_register_noupdate(const char *name, void *probe);
extern int tracepoint_probe_unregister_noupdate(const char *name, void *probe);
extern void tracepoint_probe_update_all(void);

struct tracepoint_iter {
	struct module *module;
	struct tracepoint *tracepoint;
};

extern void tracepoint_iter_start(struct tracepoint_iter *iter);
extern void tracepoint_iter_next(struct tracepoint_iter *iter);
extern void tracepoint_iter_stop(struct tracepoint_iter *iter);
extern void tracepoint_iter_reset(struct tracepoint_iter *iter);
extern int tracepoint_get_iter_range(struct tracepoint **tracepoint,
	struct tracepoint *begin, struct tracepoint *end);

/*
 * tracepoint_synchronize_unregister must be called between the last tracepoint
 * probe unregistration and the end of module exit to make sure there is no
 * caller executing a probe when it is freed.
 */
static inline void tracepoint_synchronize_unregister(void)
{
	synchronize_sched();
}

#endif
