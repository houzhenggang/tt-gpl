/*
 * TCP Westwood+
 *
 *	Angelo Dell'Aera:	TCP Westwood+ support
 */

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/tcp_diag.h>
#include <net/tcp.h>

/* TCP Westwood structure */
struct westwood {
	u32    bw_ns_est;        /* first bandwidth estimation..not too smoothed 8) */
	u32    bw_est;           /* bandwidth estimate */
	u32    rtt_win_sx;       /* here starts a new evaluation... */
	u32    bk;
	u32    snd_una;          /* used for evaluating the number of acked bytes */
	u32    cumul_ack;
	u32    accounted;
	u32    rtt;
	u32    rtt_min;          /* minimum observed RTT */
};


/* TCP Westwood functions and constants */
#define TCP_WESTWOOD_RTT_MIN   (HZ/20)	/* 50ms */
#define TCP_WESTWOOD_INIT_RTT  (20*HZ)	/* maybe too conservative?! */

/*
 * @tcp_westwood_create
 * This function initializes fields used in TCP Westwood+,
 * it is called after the initial SYN, so the sequence numbers
 * are correct but new passive connections we have no
 * information about RTTmin at this time so we simply set it to
 * TCP_WESTWOOD_INIT_RTT. This value was chosen to be too conservative
 * since in this way we're sure it will be updated in a consistent
 * way as soon as possible. It will reasonably happen within the first
 * RTT period of the connection lifetime.
 */
static void tcp_westwood_init(struct tcp_sock *tp)
{
	struct westwood *w = tcp_ca(tp);

	w->bk = 0;
        w->bw_ns_est = 0;
        w->bw_est = 0;
        w->accounted = 0;
        w->cumul_ack = 0;
	w->rtt_min = w->rtt = TCP_WESTWOOD_INIT_RTT;
	w->rtt_win_sx = tcp_time_stamp;
	w->snd_una = tp->snd_una;
}

/*
 * @westwood_do_filter
 * Low-pass filter. Implemented using constant coefficients.
 */
static inline u32 westwood_do_filter(u32 a, u32 b)
{
	return (((7 * a) + b) >> 3);
}

static inline void westwood_filter(struct westwood *w, u32 delta)
{
	w->bw_ns_est = westwood_do_filter(w->bw_ns_est, w->bk / delta);
	w->bw_est = westwood_do_filter(w->bw_est, w->bw_ns_est);
}

/*
 * @westwood_pkts_acked
 * Called after processing group of packets.
 * but all westwood needs is the last sample of srtt.
 */
static void tcp_westwood_pkts_acked(struct tcp_sock *tp, u32 cnt)
{
	struct westwood *w = tcp_ca(tp);
	if (cnt > 0)
		w->rtt = tp->srtt >> 3;
}

/*
 * @westwood_update_window
 * It updates RTT evaluation window if it is the right moment to do
 * it. If so it calls filter for evaluating bandwidth.
 */
static void westwood_update_window(struct tcp_sock *tp)
{
	struct westwood *w = tcp_ca(tp);
	s32 delta = tcp_time_stamp - w->rtt_win_sx;

	/*
	 * See if a RTT-window has passed.
	 * Be careful since if RTT is less than
	 * 50ms we don't filter but we continue 'building the sample'.
	 * This minimum limit was chosen since an estimation on small
	 * time intervals is better to avoid...
	 * Obviously on a LAN we reasonably will always have
	 * right_bound = left_bound + WESTWOOD_RTT_MIN
	 */
	if (w->rtt && delta > max_t(u32, w->rtt, TCP_WESTWOOD_RTT_MIN)) {
		westwood_filter(w, delta);

		w->bk = 0;
		w->rtt_win_sx = tcp_time_stamp;
	}
}

/*
 * @westwood_fast_bw
 * It is called when we are in fast path. In particular it is called when
 * header prediction is successful. In such case in fact update is
 * straight forward and doesn't need any particular care.
 */
static inline void westwood_fast_bw(struct tcp_sock *tp)
{
	struct westwood *w = tcp_ca(tp);

	westwood_update_window(tp);

	w->bk += tp->snd_una - w->snd_una;
	w->snd_una = tp->snd_una;
	w->rtt_min = min(w->rtt, w->rtt_min);
}

/*
 * @westwood_acked_count
 * This function evaluates cumul_ack for evaluating bk in case of
 * delayed or partial acks.
 */
static inline u32 westwood_acked_count(struct tcp_sock *tp)
{
	struct westwood *w = tcp_ca(tp);

	w->cumul_ack = tp->snd_una - w->snd_una;

        /* If cumul_ack is 0 this is a dupack since it's not moving
         * tp->snd_una.
         */
        if (!w->cumul_ack) {
		w->accounted += tp->mss_cache;
		w->cumul_ack = tp->mss_cache;
	}

        if (w->cumul_ack > tp->mss_cache) {
		/* Partial or delayed ack */
		if (w->accounted >= w->cumul_ack) {
			w->accounted -= w->cumul_ack;
			w->cumul_ack = tp->mss_cache;
		} else {
			w->cumul_ack -= w->accounted;
			w->accounted = 0;
		}
	}

	w->snd_una = tp->snd_una;

	return w->cumul_ack;
}

static inline u32 westwood_bw_rttmin(const struct tcp_sock *tp)
{
	struct westwood *w = tcp_ca(tp);
	return max_t(u32, (w->bw_est * w->rtt_min) / tp->mss_cache, 2);
}

/*
 * TCP Westwood
 * Here limit is evaluated as Bw estimation*RTTmin (for obtaining it
 * in packets we use mss_cache). Rttmin is guaranteed to be >= 2
 * so avoids ever returning 0.
 */
static u32 tcp_westwood_cwnd_min(struct tcp_sock *tp)
{
	return westwood_bw_rttmin(tp);
}

static void tcp_westwood_event(struct tcp_sock *tp, enum tcp_ca_event event)
{
	struct westwood *w = tcp_ca(tp);

	switch(event) {
	case CA_EVENT_FAST_ACK:
		westwood_fast_bw(tp);
		break;

	case CA_EVENT_COMPLETE_CWR:
		tp->snd_cwnd = tp->snd_ssthresh = westwood_bw_rttmin(tp);
		break;

	case CA_EVENT_FRTO:
		tp->snd_ssthresh = westwood_bw_rttmin(tp);
		break;

	case CA_EVENT_SLOW_ACK:
		westwood_update_window(tp);
		w->bk += westwood_acked_count(tp);
		w->rtt_min = min(w->rtt, w->rtt_min);
		break;

	default:
		/* don't care */
		break;
	}
}


/* Extract info for Tcp socket info provided via netlink. */
static void tcp_westwood_info(struct tcp_sock *tp, u32 ext,
			      struct sk_buff *skb)
{
	const struct westwood *ca = tcp_ca(tp);
	if (ext & (1<<(TCPDIAG_VEGASINFO-1))) {
		struct rtattr *rta;
		struct tcpvegas_info *info;

		rta = __RTA_PUT(skb, TCPDIAG_VEGASINFO, sizeof(*info));
		info = RTA_DATA(rta);
		info->tcpv_enabled = 1;
		info->tcpv_rttcnt = 0;
		info->tcpv_rtt = jiffies_to_usecs(ca->rtt);
		info->tcpv_minrtt = jiffies_to_usecs(ca->rtt_min);
	rtattr_failure:	;
	}
}


static struct tcp_congestion_ops tcp_westwood = {
	.init		= tcp_westwood_init,
	.ssthresh	= tcp_reno_ssthresh,
	.cong_avoid	= tcp_reno_cong_avoid,
	.min_cwnd	= tcp_westwood_cwnd_min,
	.cwnd_event	= tcp_westwood_event,
	.get_info	= tcp_westwood_info,
	.pkts_acked	= tcp_westwood_pkts_acked,

	.owner		= THIS_MODULE,
	.name		= "westwood"
};

static int __init tcp_westwood_register(void)
{
	BUG_ON(sizeof(struct westwood) > TCP_CA_PRIV_SIZE);
	return tcp_register_congestion_control(&tcp_westwood);
}

static void __exit tcp_westwood_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_westwood);
}

module_init(tcp_westwood_register);
module_exit(tcp_westwood_unregister);

MODULE_AUTHOR("Stephen Hemminger, Angelo Dell'Aera");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TCP Westwood+");
