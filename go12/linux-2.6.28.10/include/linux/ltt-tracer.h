/*
 * Copyright (C) 2005,2006,2008 Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * This contains the definitions for the Linux Trace Toolkit tracer.
 */

#ifndef _LTT_TRACER_H
#define _LTT_TRACER_H

#include <stdarg.h>
#include <linux/types.h>
#include <linux/limits.h>
#include <linux/list.h>
#include <linux/cache.h>
#include <linux/kernel.h>
#include <linux/timex.h>
#include <linux/wait.h>
#include <linux/ltt-relay.h>
#include <linux/ltt-core.h>
#include <linux/marker.h>
#include <linux/trace-clock.h>
#include <asm/atomic.h>
#include <asm/local.h>

/* Number of bytes to log with a read/write event */
#define LTT_LOG_RW_SIZE			32L

/* Interval (in jiffies) at which the LTT per-CPU timer fires */
#define LTT_PERCPU_TIMER_INTERVAL	1

#ifndef LTT_ARCH_TYPE
#define LTT_ARCH_TYPE			LTT_ARCH_TYPE_UNDEFINED
#endif

#ifndef LTT_ARCH_VARIANT
#define LTT_ARCH_VARIANT		LTT_ARCH_VARIANT_NONE
#endif

struct ltt_active_marker;

/* Maximum number of callbacks per marker */
#define LTT_NR_CALLBACKS	10

struct ltt_serialize_closure;
struct ltt_probe_private_data;

/* Serialization callback '%k' */
typedef size_t (*ltt_serialize_cb)(struct rchan_buf *buf, size_t buf_offset,
			struct ltt_serialize_closure *closure,
			void *serialize_private, int *largest_align,
			const char *fmt, va_list *args);

struct ltt_serialize_closure {
	ltt_serialize_cb *callbacks;
	long cb_args[LTT_NR_CALLBACKS];
	unsigned int cb_idx;
};

size_t ltt_serialize_data(struct rchan_buf *buf, size_t buf_offset,
			struct ltt_serialize_closure *closure,
			void *serialize_private,
			int *largest_align, const char *fmt, va_list *args);

struct ltt_available_probe {
	const char *name;		/* probe name */
	const char *format;
	marker_probe_func *probe_func;
	ltt_serialize_cb callbacks[LTT_NR_CALLBACKS];
	struct list_head node;		/* registered probes list */
};

struct ltt_probe_private_data {
	struct ltt_trace_struct *trace;	/*
					 * Target trace, for metadata
					 * or statedump.
					 */
	ltt_serialize_cb serializer;	/*
					 * Serialization function override.
					 */
	void *serialize_private;	/*
					 * Private data for serialization
					 * functions.
					 */
};

struct ltt_active_marker {
	struct list_head node;		/* active markers list */
	const char *name;
	const char *format;
	struct ltt_available_probe *probe;
	uint16_t id;
	uint16_t channel;
};

extern void ltt_vtrace(void *probe_data, void *call_data,
	const char *fmt, va_list *args);
extern void ltt_trace(void *probe_data, void *call_data,
	const char *fmt, ...);

/*
 * Unique ID assigned to each registered probe.
 */
enum marker_id {
	MARKER_ID_SET_MARKER_ID = 0,	/* Static IDs available (range 0-7) */
	MARKER_ID_SET_MARKER_FORMAT,
	MARKER_ID_COMPACT,		/* Compact IDs (range: 8-127)	    */
	MARKER_ID_DYNAMIC,		/* Dynamic IDs (range: 128-65535)   */
};

/* static ids 0-1 reserved for internal use. */
#define MARKER_CORE_IDS		2
static inline enum marker_id marker_id_type(uint16_t id)
{
	if (id < MARKER_CORE_IDS)
		return (enum marker_id)id;
	else
		return MARKER_ID_DYNAMIC;
}

#ifdef CONFIG_LTT

struct ltt_trace_struct;

struct ltt_channel_struct {
	/* First 32 bytes cache-hot cacheline */
	struct ltt_trace_struct	*trace;
	void *buf;
	void *trans_channel_data;
	int overwrite;
	int compact;
	/* End of first 32 bytes cacheline */

	/*
	 * buffer_begin - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the new sub-buffer
	 */
	void (*buffer_begin) (struct rchan_buf *buf,
			u64 tsc, unsigned int subbuf_idx);
	/*
	 * buffer_end - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the previous sub-buffer
	 */
	void (*buffer_end) (struct rchan_buf *buf,
			u64 tsc, unsigned int offset, unsigned int subbuf_idx);
	struct kref kref;
	unsigned int n_subbufs_order;
	char channel_name[PATH_MAX];
} ____cacheline_aligned;

struct user_dbg_data {
	unsigned long avail_size;
	unsigned long write;
	unsigned long read;
};

struct ltt_trace_ops {
	/* First 32 bytes cache-hot cacheline */
	int (*reserve_slot) (struct ltt_trace_struct *trace,
				struct ltt_channel_struct *channel,
				void **transport_data, size_t data_size,
				size_t *slot_size, long *buf_offset, u64 *tsc,
				unsigned int *rflags,
				int largest_align,
				int cpu);
	void (*commit_slot) (struct ltt_channel_struct *channel,
				void **transport_data, long buf_offset,
				size_t slot_size);
	void (*wakeup_channel) (struct ltt_channel_struct *ltt_channel);
	int (*user_blocking) (struct ltt_trace_struct *trace,
				unsigned int index, size_t data_size,
				struct user_dbg_data *dbg);
	/* End of first 32 bytes cacheline */
	int (*create_dirs) (struct ltt_trace_struct *new_trace);
	void (*remove_dirs) (struct ltt_trace_struct *new_trace);
	int (*create_channel) (const char *trace_name,
				struct ltt_trace_struct *trace,
				struct dentry *dir, char *channel_name,
				struct ltt_channel_struct **ltt_chan,
				unsigned int subbuf_size,
				unsigned int n_subbufs, int overwrite);
	void (*finish_channel) (struct ltt_channel_struct *channel);
	void (*remove_channel) (struct ltt_channel_struct *channel);
	void (*user_errors) (struct ltt_trace_struct *trace,
				unsigned int index, size_t data_size,
				struct user_dbg_data *dbg, int cpu);
#ifdef CONFIG_HOTPLUG_CPU
	int (*handle_cpuhp) (struct notifier_block *nb,
				unsigned long action, void *hcpu,
				struct ltt_trace_struct *trace);
#endif
} ____cacheline_aligned;

struct ltt_transport {
	char *name;
	struct module *owner;
	struct list_head node;
	struct ltt_trace_ops ops;
};


enum trace_mode { LTT_TRACE_NORMAL, LTT_TRACE_FLIGHT, LTT_TRACE_HYBRID };

/* Per-trace information - each trace/flight recorder represented by one */
struct ltt_trace_struct {
	/* First 32 bytes cache-hot cacheline */
	struct list_head list;
	struct ltt_trace_ops *ops;
	int active;
	/* Second 32 bytes cache-hot cacheline */
	struct {
		/*
		 * Changing the order requires to change ltt_channel_index*() */
		struct ltt_channel_struct	*cpu;
		struct ltt_channel_struct	*processes;
		struct ltt_channel_struct	*interrupts;
		struct ltt_channel_struct	*network;
		/* End of second 32 bytes cache-hot cacheline */
		struct ltt_channel_struct	*modules;
		struct ltt_channel_struct	*metadata;
	} channel;
	u32 freq_scale;
	u64 start_freq;
	u64 start_tsc;
	unsigned long long start_monotonic;
	struct timeval		start_time;
	struct {
		struct dentry			*trace_root;
		struct dentry			*control_root;
	} dentry;
	struct rchan_callbacks callbacks;
	enum trace_mode mode;
	struct kref kref; /* Each channel has a kref of the trace struct */
	struct ltt_transport *transport;
	struct kref ltt_transport_kref;
	wait_queue_head_t kref_wq; /* Place for ltt_trace_destroy to sleep */
	char trace_name[NAME_MAX];
} ____cacheline_aligned;

/*
 * First and last channels in ltt_trace_struct.
 */
#define ltt_channel_index_size()	sizeof(struct ltt_channel_struct *)
#define ltt_channel_index_begin()	GET_CHANNEL_INDEX(cpu)
#define ltt_channel_index_end()	\
	(GET_CHANNEL_INDEX(metadata) + ltt_channel_index_size())

enum ltt_channels {
	LTT_CHANNEL_CPU,
	LTT_CHANNEL_PROCESSES,
	LTT_CHANNEL_INTERRUPTS,
	LTT_CHANNEL_NETWORK,
	LTT_CHANNEL_MODULES,
	LTT_CHANNEL_METADATA,
};

/* Hardcoded event headers
 *
 * event header for a trace with active heartbeat : 27 bits timestamps
 *
 * headers are 32-bits aligned. In order to insure such alignment, a dynamic per
 * trace alignment value must be done.
 *
 * Remember that the C compiler does align each member on the boundary
 * equivalent to their own size.
 *
 * As relay subbuffers are aligned on pages, we are sure that they are 4 and 8
 * bytes aligned, so the buffer header and trace header are aligned.
 *
 * Event headers are aligned depending on the trace alignment option.
 *
 * Note using C structure bitfields for cross-endianness and portability
 * concerns.
 */

#define LTT_RESERVED_EVENTS	3
#define LTT_EVENT_BITS		5
#define LTT_FREE_EVENTS		((1 << LTT_EVENT_BITS) - LTT_RESERVED_EVENTS)
#define LTT_TSC_BITS		27
#define LTT_TSC_MASK		((1 << LTT_TSC_BITS) - 1)

struct ltt_event_header {
	u32 id_time;		/* 5 bits event id (MSB); 27 bits time (LSB) */
};

/* Reservation flags */
#define	LTT_RFLAG_ID			(1 << 0)
#define	LTT_RFLAG_ID_SIZE		(1 << 1)
#define	LTT_RFLAG_ID_SIZE_TSC		(1 << 2)

/*
 * We use asm/timex.h : cpu_khz/HZ variable in here : we might have to deal
 * specifically with CPU frequency scaling someday, so using an interpolation
 * between the start and end of buffer values is not flexible enough. Using an
 * immediate frequency value permits to calculate directly the times for parts
 * of a buffer that would be before a frequency change.
 *
 * Keep the natural field alignment for _each field_ within this structure if
 * you ever add/remove a field from this header. Packed attribute is not used
 * because gcc generates poor code on at least powerpc and mips. Don't ever
 * let gcc add padding between the structure elements.
 */
struct ltt_subbuffer_header {
	uint64_t cycle_count_begin;	/* Cycle count at subbuffer start */
	uint64_t cycle_count_end;	/* Cycle count at subbuffer end */
	uint32_t magic_number;		/*
					 * Trace magic number.
					 * contains endianness information.
					 */
	uint8_t major_version;
	uint8_t minor_version;
	uint8_t arch_size;		/* Architecture pointer size */
	uint8_t alignment;		/* LTT data alignment */
	uint64_t start_time_sec;	/* NTP-corrected start time */
	uint64_t start_time_usec;
	uint64_t start_freq;		/*
					 * Frequency at trace start,
					 * used all along the trace.
					 */
	uint32_t freq_scale;		/* Frequency scaling */
	uint32_t lost_size;		/* Size unused at end of subbuffer */
	uint32_t buf_size;		/* Size of this subbuffer */
	uint32_t events_lost;		/*
					 * Events lost in this subbuffer since
					 * the beginning of the trace.
					 * (may overflow)
					 */
	uint32_t subbuf_corrupt;	/*
					 * Corrupted (lost) subbuffers since
					 * the begginig of the trace.
					 * (may overflow)
					 */
	uint8_t header_end[0];		/* End of header */
};

#ifdef CONFIG_LTT_ALIGNMENT

/*
 * Calculate the offset needed to align the type.
 * size_of_type must be non-zero.
 */
static inline unsigned int ltt_align(size_t align_drift, size_t size_of_type)
{
	size_t alignment = min(sizeof(void *), size_of_type);
	return (alignment - align_drift) & (alignment - 1);
}
/* Default arch alignment */
#define LTT_ALIGN

static inline int ltt_get_alignment(void)
{
	return sizeof(void *);
}

#else

static inline unsigned int ltt_align(size_t align_drift,
		 size_t size_of_type)
{
	return 0;
}

#define LTT_ALIGN __attribute__((packed))

static inline int ltt_get_alignment(void)
{
	return 0;
}
#endif /* CONFIG_LTT_ALIGNMENT */

/**
 * ltt_subbuffer_header_size - called on buffer-switch to a new sub-buffer
 *
 * Return header size without padding after the structure. Don't use packed
 * structure because gcc generates inefficient code on some architectures
 * (powerpc, mips..)
 */
static inline size_t ltt_subbuffer_header_size(void)
{
	return offsetof(struct ltt_subbuffer_header, header_end);
}

/* Get the offset of the channel in the ltt_trace_struct */
#define GET_CHANNEL_INDEX(chan)	\
	offsetof(struct ltt_trace_struct, channel.chan)

static inline struct ltt_channel_struct *ltt_get_channel_from_index(
		struct ltt_trace_struct *trace, unsigned int index)
{
	return *(struct ltt_channel_struct **)((void *)trace+index);
}

/*
 * ltt_get_header_size
 *
 * Calculate alignment offset to 32-bits. This is the alignment offset of the
 * event header.
 *
 * Important note :
 * The event header must be 32-bits. The total offset calculated here :
 *
 * Alignment of header struct on 32 bits (min arch size, header size)
 * + sizeof(header struct)  (32-bits)
 * + (opt) u16 (ext. event id)
 * + (opt) u16 (event_size) (if event_size == 0xFFFFUL, has ext. event size)
 * + (opt) u32 (ext. event size)
 * + (opt) u64 full TSC (aligned on min(64-bits, arch size))
 *
 * The payload must itself determine its own alignment from the biggest type it
 * contains.
 * */
static inline unsigned char ltt_get_header_size(
		struct ltt_channel_struct *channel,
		size_t offset,
		size_t data_size,
		size_t *before_hdr_pad,
		unsigned int rflags)
{
	size_t orig_offset = offset;
	size_t padding;

	BUILD_BUG_ON(sizeof(struct ltt_event_header) != sizeof(u32));

	padding = ltt_align(offset, sizeof(struct ltt_event_header));
	offset += padding;
	offset += sizeof(struct ltt_event_header);

	switch (rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		offset += sizeof(u16) + sizeof(u16);
		if (data_size >= 0xFFFFU)
			offset += sizeof(u32);
		offset += ltt_align(offset, sizeof(u64));
		offset += sizeof(u64);
		break;
	case LTT_RFLAG_ID_SIZE:
		offset += sizeof(u16) + sizeof(u16);
		if (data_size >= 0xFFFFU)
			offset += sizeof(u32);
		break;
	case LTT_RFLAG_ID:
		offset += sizeof(u16);
		break;
	}

	*before_hdr_pad = padding;
	return offset - orig_offset;
}

/*
 * ltt_write_event_header
 *
 * Writes the event header to the offset (already aligned on 32-bits).
 *
 * @trace : trace to write to.
 * @channel : pointer to the channel structure..
 * @buf : buffer to write to.
 * @buf_offset : buffer offset to write to (aligned on 32 bits).
 * @eID : event ID
 * @event_size : size of the event, excluding the event header.
 * @tsc : time stamp counter.
 * @rflags : reservation flags.
 *
 * returns : offset where the event data must be written.
 */
static inline size_t ltt_write_event_header(struct ltt_trace_struct *trace,
		struct ltt_channel_struct *channel,
		struct rchan_buf *buf, long buf_offset,
		u16 eID, size_t event_size,
		u64 tsc, unsigned int rflags)
{
	struct ltt_event_header header;
	size_t small_size;

	switch (rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		header.id_time = 29 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID_SIZE:
		header.id_time = 30 << LTT_TSC_BITS;
		break;
	case LTT_RFLAG_ID:
		header.id_time = 31 << LTT_TSC_BITS;
		break;
	default:
		header.id_time = eID << LTT_TSC_BITS;
		break;
	}
	header.id_time |= (u32)tsc & LTT_TSC_MASK;
	ltt_relay_write(buf, buf_offset, &header, sizeof(header));
	buf_offset += sizeof(header);

	switch (rflags) {
	case LTT_RFLAG_ID_SIZE_TSC:
		small_size = min_t(size_t, event_size, 0xFFFFU);
		ltt_relay_write(buf, buf_offset,
			(u16[]){ (u16)eID }, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_write(buf, buf_offset,
			(u16[]){ (u16)small_size }, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == 0xFFFFU) {
			ltt_relay_write(buf, buf_offset,
				(u32[]){ (u32)event_size }, sizeof(u32));
			buf_offset += sizeof(u32);
		}
		buf_offset += ltt_align(buf_offset, sizeof(u64));
		ltt_relay_write(buf, buf_offset,
			(u64[]){ (u64)tsc }, sizeof(u64));
		buf_offset += sizeof(u64);
		break;
	case LTT_RFLAG_ID_SIZE:
		small_size = min_t(size_t, event_size, 0xFFFFU);
		ltt_relay_write(buf, buf_offset,
			(u16[]){ (u16)eID }, sizeof(u16));
		buf_offset += sizeof(u16);
		ltt_relay_write(buf, buf_offset,
			(u16[]){ (u16)small_size }, sizeof(u16));
		buf_offset += sizeof(u16);
		if (small_size == 0xFFFFU) {
			ltt_relay_write(buf, buf_offset,
				(u32[]){ (u32)event_size }, sizeof(u32));
			buf_offset += sizeof(u32);
		}
		break;
	case LTT_RFLAG_ID:
		ltt_relay_write(buf, buf_offset,
			(u16[]){ (u16)eID }, sizeof(u16));
		buf_offset += sizeof(u16);
		break;
	default:
		break;
	}

	return buf_offset;
}

/* Lockless LTTng */

/* Buffer offset macros */

/*
 * BUFFER_TRUNC zeroes the subbuffer offset and the subbuffer number parts of
 * the offset, which leaves only the buffer number.
 */
#define BUFFER_TRUNC(offset, chan) \
	((offset) & (~((chan)->alloc_size-1)))
#define BUFFER_OFFSET(offset, chan) ((offset) & ((chan)->alloc_size - 1))
#define SUBBUF_OFFSET(offset, chan) ((offset) & ((chan)->subbuf_size - 1))
#define SUBBUF_ALIGN(offset, chan) \
	(((offset) + (chan)->subbuf_size) & (~((chan)->subbuf_size - 1)))
#define SUBBUF_TRUNC(offset, chan) \
	((offset) & (~((chan)->subbuf_size - 1)))
#define SUBBUF_INDEX(offset, chan) \
	(BUFFER_OFFSET((offset), chan) >> (chan)->subbuf_size_order)

/*
 * ltt_reserve_slot
 *
 * Atomic slot reservation in a LTTng buffer. It will take care of
 * sub-buffer switching.
 *
 * Parameters:
 *
 * @trace : the trace structure to log to.
 * @channel : the chanel to reserve space into.
 * @transport_data : specific transport data.
 * @data_size : size of the variable length data to log.
 * @slot_size : pointer to total size of the slot (out)
 * @buf_offset : pointer to reserve offset (out)
 * @tsc : pointer to the tsc at the slot reservation (out)
 * @rflags : reservation flags (header specificity)
 * @cpu : cpu id
 *
 * Return : -ENOSPC if not enough space, else 0.
 */
static inline int ltt_reserve_slot(
		struct ltt_trace_struct *trace,
		struct ltt_channel_struct *channel,
		void **transport_data,
		size_t data_size,
		size_t *slot_size,
		long *buf_offset,
		u64 *tsc,
		unsigned int *rflags,
		int largest_align,
		int cpu)
{
	return trace->ops->reserve_slot(trace, channel, transport_data,
			data_size, slot_size, buf_offset, tsc, rflags,
			largest_align, cpu);
}


/*
 * ltt_commit_slot
 *
 * Atomic unordered slot commit. Increments the commit count in the
 * specified sub-buffer, and delivers it if necessary.
 *
 * Parameters:
 *
 * @channel : the chanel to reserve space into.
 * @transport_data : specific transport data.
 * @buf_offset : offset of beginning of reserved slot
 * @slot_size : size of the reserved slot.
 */
static inline void ltt_commit_slot(
		struct ltt_channel_struct *channel,
		void **transport_data,
		long buf_offset,
		size_t slot_size)
{
	struct ltt_trace_struct *trace = channel->trace;

	trace->ops->commit_slot(channel, transport_data, buf_offset, slot_size);
}

/*
 * Control channels :
 * control/metadata
 * control/interrupts
 * control/...
 *
 * cpu channel :
 * cpu
 */
#define LTT_RELAY_ROOT		"ltt"
#define LTT_RELAY_LOCKED_ROOT	"ltt-locked"
#define LTT_CONTROL_ROOT	"control"
#define LTT_METADATA_CHANNEL	"metadata"
#define LTT_INTERRUPTS_CHANNEL	"interrupts"
#define LTT_PROCESSES_CHANNEL	"processes"
#define LTT_MODULES_CHANNEL	"modules"
#define LTT_NETWORK_CHANNEL	"network"
#define LTT_CPU_CHANNEL		"cpu"
#define LTT_FLIGHT_PREFIX	"flight-"

/* Tracer properties */
#define LTT_DEFAULT_SUBBUF_SIZE_LOW	65536
#define LTT_DEFAULT_N_SUBBUFS_LOW	2
#define LTT_DEFAULT_SUBBUF_SIZE_MED	262144
#define LTT_DEFAULT_N_SUBBUFS_MED	2
#define LTT_DEFAULT_SUBBUF_SIZE_HIGH	1048576
#define LTT_DEFAULT_N_SUBBUFS_HIGH	2
#define LTT_TRACER_MAGIC_NUMBER		0x00D6B7ED
#define LTT_TRACER_VERSION_MAJOR	2
#define LTT_TRACER_VERSION_MINOR	2

/*
 * Size reserved for high priority events (interrupts, NMI, BH) at the end of a
 * nearly full buffer. User space won't use this last amount of space when in
 * blocking mode. This space also includes the event header that would be
 * written by this user space event.
 */
#define LTT_RESERVE_CRITICAL		4096

/* Register and unregister function pointers */

enum ltt_module_function {
	LTT_FUNCTION_RUN_FILTER,
	LTT_FUNCTION_FILTER_CONTROL,
	LTT_FUNCTION_STATEDUMP
};

extern int ltt_module_register(enum ltt_module_function name, void *function,
		struct module *owner);
extern void ltt_module_unregister(enum ltt_module_function name);

void ltt_transport_register(struct ltt_transport *transport);
void ltt_transport_unregister(struct ltt_transport *transport);

/* Exported control function */

enum ltt_control_msg {
	LTT_CONTROL_START,
	LTT_CONTROL_STOP,
	LTT_CONTROL_CREATE_TRACE,
	LTT_CONTROL_DESTROY_TRACE
};

union ltt_control_args {
	struct {
		enum trace_mode mode;
		unsigned subbuf_size_low;
		unsigned n_subbufs_low;
		unsigned subbuf_size_med;
		unsigned n_subbufs_med;
		unsigned subbuf_size_high;
		unsigned n_subbufs_high;
	} new_trace;
};

extern int ltt_control(enum ltt_control_msg msg, const char *trace_name,
		const char *trace_type, union ltt_control_args args);

enum ltt_filter_control_msg {
	LTT_FILTER_DEFAULT_ACCEPT,
	LTT_FILTER_DEFAULT_REJECT
};

extern int ltt_filter_control(enum ltt_filter_control_msg msg,
		const char *trace_name);

void ltt_write_trace_header(struct ltt_trace_struct *trace,
		struct ltt_subbuffer_header *header);
extern void ltt_buffer_destroy(struct ltt_channel_struct *ltt_chan);

void ltt_core_register(int (*function)(u8, void *));

void ltt_core_unregister(void);

void ltt_release_trace(struct kref *kref);
void ltt_release_transport(struct kref *kref);

extern int ltt_probe_register(struct ltt_available_probe *pdata);
extern int ltt_probe_unregister(struct ltt_available_probe *pdata);
extern int ltt_marker_connect(const char *mname, const char *pname,
		enum marker_id id, uint16_t channel, int user);
extern int ltt_marker_disconnect(const char *mname, const char *pname,
	int user);
extern void ltt_dump_marker_state(struct ltt_trace_struct *trace);
extern void probe_id_defrag(void);

void ltt_lock_traces(void);
void ltt_unlock_traces(void);

extern void ltt_dump_softirq_vec(void *call_data);

#ifdef CONFIG_HAVE_LTT_DUMP_TABLES
extern void ltt_dump_sys_call_table(void *call_data);
extern void ltt_dump_idt_table(void *call_data);
#else
static inline void ltt_dump_sys_call_table(void *call_data)
{
}

static inline void ltt_dump_idt_table(void *call_data)
{
}
#endif

/* Relay IOCTL */

/* Get the next sub buffer that can be read. */
#define RELAY_GET_SUBBUF		_IOR(0xF5, 0x00, __u32)
/* Release the oldest reserved (by "get") sub buffer. */
#define RELAY_PUT_SUBBUF		_IOW(0xF5, 0x01, __u32)
/* returns the number of sub buffers in the per cpu channel. */
#define RELAY_GET_N_SUBBUFS		_IOR(0xF5, 0x02, __u32)
/* returns the size of the sub buffers. */
#define RELAY_GET_SUBBUF_SIZE		_IOR(0xF5, 0x03, __u32)

#endif /* CONFIG_LTT */

#endif /* _LTT_TRACER_H */
