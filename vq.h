#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/ktime.h>
#include <net/ip.h>
#include <net/inet_ecn.h>
#include <net/tcp.h>
#include <net/dst.h>
#include <linux/hash.h>
#include <linux/crc16.h>
#include <linux/completion.h>
#include <linux/hrtimer.h>
#include <linux/random.h>
#include <asm/atomic.h>
#include <linux/spinlock.h>

#include "params.h"

struct iso_vq_stats {
	u64 bytes_queued;
	u64 network_marked;
};

struct iso_vq {
	u8 enabled;
	u8 active;
	u8 is_static;

	u64 rate;
	u64 total_bytes_queued;
	u64 backlog;

	u64 weight;

	ktime_t last_update_time;

	struct iso_vq_stats __percpu *percpu_stats;
	spinlock_t spinlock;

	struct list_head list;
};

#define for_each_vq(vq) list_for_each_entry_rcu(vq, &vq_list, list)
#define ISO_VQ_DEFAULT_RATE_MBPS (100) /* This parameter shouldn't matter */

void iso_vqs_init(void);
void iso_vq_enqueue(struct iso_vq *, struct sk_buff *, u32);
inline int iso_vq_active(struct iso_vq *);
void iso_vq_tick(u64);
void iso_vq_drain(struct iso_vq *, u64);

/* Local Variables: */
/* indent-tabs-mode:t */
/* End: */