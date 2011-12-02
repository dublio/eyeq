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

enum iso_verdict {
	ISO_VERDICT_SUCCESS,
	ISO_VERDICT_DROP
};

struct iso_rl_queue {
	struct sk_buff *queue[ISO_MAX_QUEUE_LEN_PKT];
	int head;
	int tail;
	int length;

	u64 bytes_enqueued;
	u64 tokens;
	spinlock_t spinlock;
	struct tasklet_struct xmit_timeout;
	struct hrtimer timer;
	struct iso_rl *rl;
};

struct iso_rl {
	u64 rate;
	u64 total_tokens;
	ktime_t last_update_time;
	spinlock_t spinlock;

	// DEFINE_PER_CPU(struct iso_rl_queue, queue);
	struct iso_rl_queue __percpu *queue;
};

void iso_rl_init(struct iso_rl *);
void iso_rl_destroy(struct iso_rl *);
static int iso_rl_should_refill(struct iso_rl *);
inline u64 iso_rl_cap_tokens(u64);
void iso_rl_clock(struct iso_rl *);
enum iso_verdict iso_rl_enqueue(struct iso_rl *, struct sk_buff *);
void iso_rl_dequeue(unsigned long);
enum hrtimer_restart iso_rl_timeout(struct hrtimer *);
int iso_rl_borrow_tokens(struct iso_rl *, struct iso_rl_queue *);
inline ktime_t iso_rl_gettimeout(void);
inline u64 iso_rl_singleq_burst(struct iso_rl *);

void skb_xmit(struct sk_buff *);
void skb_drop(struct sk_buff *);
void skb_consume(struct sk_buff *);

static inline int skb_size(struct sk_buff *skb) {
	return skb->len;
}

/* Local Variables: */
/* indent-tabs-mode:t */
/* End: */