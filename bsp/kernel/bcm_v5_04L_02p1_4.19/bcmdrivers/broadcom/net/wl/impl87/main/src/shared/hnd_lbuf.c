/*
 * HND packet buffer routines.
 *
 * No caching,
 * Just a thin packet buffering data structure layer atop malloc/free .
 *
 * Copyright 2022 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 * $Id: hnd_lbuf.c 794017 2020-12-17 06:11:31Z $
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>

#include <hnd_lbuf.h>
#include <hnd_pt.h>

#if defined(BCMDBG_MEMFAIL)
#include <rte_heap.h>
#endif

#ifdef BCMHWA
#include <hwa_export.h>
#endif /* BCMHWA */

#if defined(BCMDBG) || defined(BCMDBG_MEMFAIL)
#define LBUF_MSG(x) printf x
#else
#define LBUF_MSG(x)
#endif

#define LBUF_BIN_SIZE	6

#if defined(BCMLFRAG) && !defined(BCMLFRAG_DISABLED)
bool _bcmlfrag = TRUE;
#else
bool _bcmlfrag = FALSE;
#endif

typedef struct {
	lbuf_free_cb_t cb;
	void* arg;
} lbuf_freeinfo_t;

static lbuf_freeinfo_t lbuf_info;

static lbuf_freeinfo_t* lbuf_info_get(void);

static lbuf_freeinfo_t*
BCMRAMFN(lbuf_info_get)(void)
{
	return (&lbuf_info);
}

void lbuf_free_register(lbuf_free_cb_t cb, void* arg)
{
	lbuf_freeinfo_t *lbuf_infoptr;

	lbuf_infoptr = lbuf_info_get();

	lbuf_infoptr->cb = cb;
	lbuf_infoptr->arg = arg;
}

static const uint lbsize[LBUF_BIN_SIZE] = {
	MAXPKTBUFSZ >> 4,
	MAXPKTBUFSZ >> 3,
	MAXPKTBUFSZ >> 2,
	MAXPKTBUFSZ >> 1,
	MAXPKTBUFSZ,
	4096 + LBUFSZ		/* ctrl queries on bus can be 4K */
};

#if !defined(BCMPKTPOOL) || defined(BCMDBG)
static uint lbuf_hist[LBUF_BIN_SIZE] = {0};
#endif
static uint lbuf_allocfail = 0;

#if defined(BCMPCIEDEV) && !defined(BME_PKTFETCH)
static lbuf_free_global_cb_t lbuf_free_cb = NULL;
#endif /* BCMPCIEDEV && !BME_PKTFETCH */

#ifdef BCM_DHDHDR
void
lb_set_buf(struct lbuf *lb, void *buf, uint size)
{
	lb->head = lb->data = (uchar *)buf;
	lb->end = lb->head + size;
	lb->len = size;
}
#endif /* BCM_DHDHDR */

/* Set flags and size according to pkt type */
static void
set_lbuf_params(enum lbuf_type lbuf_type, uint32* flags, uint* lbufsz)
{
	switch (lbuf_type) {
	case lbuf_basic:
		/* legacy pkt */
		*flags = 0;
		*lbufsz = LBUFSZ;
		break;
	case lbuf_frag:
		/* tx frag */
		*flags = LBF_TX_FRAG;
		*lbufsz = LBUFFRAGSZ;
		break;
	case lbuf_rxfrag:
		/* rx frag */
		*flags = 0;
		*lbufsz = LBUFFRAGSZ;
		break;
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	case lbuf_mgmt_tx:
		/* legacy mgmt pkt */
		*flags = (LBF_HWA_PKT | LBF_MGMT_TX_PKT);
		*lbufsz = HWA_TXPOST_PKT_BYTES + LBUFSZ;
		break;
#endif
	default:
		break;
	}
}

struct lbuf *
lb_init(struct lbuf *lb, enum lbuf_type lb_type, uint lb_sz)
{
#if defined(BCMHWA) && defined(HWA_TXPOST_BUILD)
	uchar * head;
	uint lbufsz = 0;
	uint end_off;
	uint32 flags = 0;

	ASSERT(ISALIGNED(lb, sizeof(int)));

	if (hnd_pktid_free_cnt() == 0) {
		LBUF_MSG(("lb_init: hnd_pktid_free_cnt 0\n"));
		hnd_pktid_inc_fail_cnt();
		goto error;
	}

	/* get lbuf params based on type */
	set_lbuf_params(lb_type, &flags, &lbufsz);
	ASSERT(lb_sz >= lbufsz);

	bzero(lb, lbufsz);
	head = (uchar*)((uchar*)lb + lbufsz);
	end_off = (lb_sz - lbufsz);
	lb->head = head;
	lb->data = head;
	lb->end = head + end_off;
	lb->len = end_off;
	lb->mem.pktid = hnd_pktid_allocate(lb);
	ASSERT(lb->mem.pktid != PKT_INVALID_ID);
	lb->mem.refcnt = 1;
	lb->flags = flags;

	return (lb);

error:
	return NULL;

#else

	ASSERT(sizeof(struct lbuf) == LBUFSZ);
	BCM_REFERENCE(lb);
	BCM_REFERENCE(lb_type);
	BCM_REFERENCE(lb_sz);
	return NULL;
#endif /* BCMHWA && HWA_TXPOST_BUILD */
}

struct lbuf *
#if defined(BCMDBG_MEMFAIL)
lb_alloc_header(void *data, uint size, enum lbuf_type lbuf_type, void* call_site)
#else
lb_alloc_header(void *data, uint size, enum lbuf_type lbuf_type)
#endif
{
	uchar * head;
	struct lbuf *lb;
	uint lbufsz, end_off;
	uint32 flags;

	BCM_REFERENCE(end_off);

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	printf("%s: FIXME, HWA doesn't handle it!\n", __FUNCTION__);
	ASSERT(0);
	goto error;
#endif

	if (hnd_pktid_free_cnt() == 0) {
		LBUF_MSG(("lb_alloc: hnd_pktid_free_cnt 0\n"));
		goto error;
	}

	flags = 0;
	lbufsz = 0;

	/* set lbuf params based on type */
	set_lbuf_params(lbuf_type, &flags, &lbufsz);

#if defined(BCMDBG_MEMFAIL)
	if ((lb = (struct lbuf*)hnd_malloc_align(lbufsz, 0, call_site)) == NULL) {
		printf("lb_alloc: size (%u); alloc failed; called from 0x%p\n", lbufsz, call_site);
		goto error;
	}

#else
	if ((lb = (struct lbuf*)hnd_malloc_align(lbufsz, 0)) == NULL) {
		LBUF_MSG(("lb_alloc: size (%u); alloc failed;\n", lbufsz));
		goto error;
	}
#endif

	ASSERT(ISALIGNED(lb, sizeof(int)));

	bzero(lb, lbufsz);

	head = (uchar*) data;
	end_off = ROUNDUP(size, sizeof(int));

	lb->data = head;
	lb->head = data;
	lb->end = lb->head + ROUNDUP(size, sizeof(int));
	lb->len = size;
	lb->mem.pktid = hnd_pktid_allocate(lb); /* BCMPKTIDMAP: pktid != 0 */
	ASSERT(lb->mem.pktid != PKT_INVALID_ID);
	lb->mem.refcnt = 1;
	lb->flags = flags;

	return (lb);

error:
	lbuf_allocfail++;
	return NULL;

}

struct lbuf *
#if defined(BCMDBG_MEMFAIL)
lb_alloc(uint size, enum lbuf_type lbuf_type, void *call_site)
#else
/**
 * @param size        size of packet buffer to allocate in dongle memory in [bytes]
 * @param lbuf_type   e.g. 'lbuf_frag'
 */
lb_alloc(uint size, enum lbuf_type lbuf_type)
#endif
{
	uchar * head;
	struct lbuf *lb;
	uint tot;     /**< [bytes] of dongle memory to allocate for lbuf+pktbuffer */
	uint lbufsz;  /**< sizeof(struct lbuf_frag) or sizeof(struct lbuf) */
	uint end_off;
	uint32 flags;

	if (hnd_pktid_free_cnt() == 0) {
		LBUF_MSG(("lb_alloc: hnd_pktid_free_cnt 0\n"));
		hnd_pktid_inc_fail_cnt();
		goto error;
	}

	tot = 0;
	flags = 0;
	lbufsz = 0; /* assumes e.g. LBUFFRAGSZ */

	/* get lbuf params based on type */
	set_lbuf_params(lbuf_type, &flags, &lbufsz);

#ifdef HND_PT_GIANT
	if (size > MAXPKTBUFSZ + lbufsz) {	/* giant rx pkt, get from fixed block partition */
		tot = lbufsz + ROUNDUP(size, sizeof(int));
		/* ASSERT(tot <= MEM_PT_BLKSIZE); */
		if ((lb = (struct lbuf*)hnd_malloc_pt(tot)) != NULL) {
			flags |= LBF_PTBLK;
			goto success;
		}

		LBUF_MSG(("lb_alloc: size (%u); alloc failed;\n", tot));
		goto error;
	}
#endif

#ifdef BCMPKTPOOL
	/* Don't roundup size if PKTPOOL enabled */
	tot = lbufsz + ROUNDUP(size, sizeof(int));
	if (tot > lbsize[ARRAYSIZE(lbsize)-1]) {
		LBUF_MSG(("lb_alloc: size too big (%u); alloc failed;\n",
		       (lbufsz + size)));
		goto error;
	}
#else
	{
		int i;
		for (i = 0; i < ARRAYSIZE(lbsize); i++) {
			if ((lbufsz + ROUNDUP(size, sizeof(int))) <= lbsize[i]) {
				tot = lbsize[i];
				lbuf_hist[i]++;
				break;
			}
		}
	}
#endif /* BCMPKTPOOL */

	if (tot == 0) {
		LBUF_MSG(("lb_alloc: size too big (%u); alloc failed;\n",
		       (lbufsz + size)));
		goto error;
	}
#if defined(BCMDBG_MEMFAIL)
	if ((lb = MALLOC_ALIGN_CALLSITE(OSH_NULL, tot, 0, call_site)) == NULL) {
		LBUF_MSG(("lb_alloc: size (%u); alloc failed; called from 0x%p\n", (lbufsz + size),
			call_site));
		goto error;
	}
#else
	if ((lb = MALLOC_ALIGN(OSH_NULL, tot, 0)) == NULL) {
		LBUF_MSG(("lb_alloc: size (%u); alloc failed;\n", (lbufsz + size)));
		goto error;
	}
#endif

#ifdef HND_PT_GIANT
	success:
#endif

	bzero(lb, lbufsz);

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	if (lbuf_type == lbuf_mgmt_tx) {
		// Shift HWA 3a SWPKT header offset
		lb = (struct lbuf *)HWAPKT2LFRAG(lb);
		lbufsz -= HWA_TXPOST_PKT_BYTES;
		tot -= HWA_TXPOST_PKT_BYTES;
	}
#endif
	ASSERT(ISALIGNED(lb, sizeof(int)));

	head = (uchar*)((uchar*)lb + lbufsz);
	end_off = (tot - lbufsz);

	lb->data = (head + end_off) - ROUNDUP(size, sizeof(int));
	lb->head = head;
	lb->end = head + end_off;
	lb->len = size;
	lb->mem.pktid = hnd_pktid_allocate(lb); /* BCMPKTIDMAP: pktid != 0 */
	ASSERT(lb->mem.pktid != PKT_INVALID_ID);
	lb->mem.refcnt = 1;
	lb->flags = flags;

	return (lb);

error:
	lbuf_allocfail++;
	return NULL;
} /* lb_alloc */

#if defined(BCMDBG_MEMFAIL)
#define LB_FREE(osh, p, sz)		BCM_REFERENCE(sz); hnd_free_callsite(p, call_site)
#else
#define LB_FREE(osh, p, sz)		MFREE(osh, p, sz)
#endif

static void
#if defined(BCMDBG_MEMFAIL)
lb_free_one(struct lbuf *lb, void *call_site)
#else
lb_free_one(struct lbuf *lb)
#endif
{
	ASSERT(lb_sane(lb));
	ASSERT(hnd_pktid_sane(lb));

	if (!lb_isclone(lb)) {
		ASSERT(lb->mem.refcnt > 0);
		if (--lb->mem.refcnt == 0) {
			if (lb_pool(lb)) {
				pktpool_t *pool = lb_getpool(lb);

				ASSERT(pool);
				ASSERT(lb->mem.poolid == POOLID(pool));

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
				/* Free head pointer to buffer pool if not freed */
				if (PKTISBUFALLOC(OSH_NULL, lb) && BCMLFRAG_ENAB()) {
					if (PKTHASHEAPBUF(OSH_NULL, lb)) {
						LB_FREE(OSH_NULL,
							PKTHEAD(OSH_NULL, lb), MAXPKTDATABUFSZ);
						PKTRESETHEAPBUF(OSH_NULL, lb);
					} else {
						lfbufpool_free(PKTHEAD(OSH_NULL, lb));
					}
					PKTRESETBUFALLOC(OSH_NULL, lb);

					/* reset head/data/end pointers */
					PKTSETBUF(OSH_NULL, lb, ((uchar *)lb + LBUFFRAGSZ),
						pool->max_pkt_bytes);
					ASSERT(lb_sane(lb));
				}
#endif /* BCM_DHDHDR &&  DONGLEBUILD */

#if defined(BCMHWA) && defined(HWA_RXFILL_BUILD)
				/* HWA RXPOST pkt free */
				if (PKTISHWAPKT(OSH_NULL, lb) && !PKTISMGMTTXPKT(OSH_NULL, lb)) {
					/* has_rph means PAIRED, means dropped free,
					 * means still having RXFRAG
					 */
					hwa_rxfill_rxbuffer_free(hwa_dev, 0,
						(hwa_rxbuffer_t *)LBHWARXPKT(lb),
						PKTISRXFRAG(OSH_NULL, lb));
				}
#endif /* BCMHWA && HWA_RXFILL_BUILD */

				lb_resetpool(lb, pool->max_pkt_bytes);
				pktpool_free(pool, lb); /* put back to pool */
			} else	if (!lb_isptblk(lb)) {
#if defined(BCMHWA) && defined(HWA_TXPOST_BUILD)
				/* HWA RX packet is lb_pool::lbuf_rxfrag + data buffer from RXBM
				 * HWA TX packet can be lbuf_mgmt_tx packet or
				 * packet from TXBM (3a header + extra pkttag size)
				 */
				if (PKTISHWAPKT(OSH_NULL, lb)) {
					if (PKTISMGMTTXPKT(OSH_NULL, lb)) {
						lb->data = (uchar*) 0xdeadbeef;
						hnd_pktid_release(lb, lb->mem.pktid);
						LB_FREE(OSH_NULL, LFRAG2HWAPKT(lb),
							LB_END(lb) - LB_HEAD(lb) +
							HWA_TXPOST_PKT_BYTES);
					} else {
						/* Free data buffer (D11_BUFFER or HEAPBUF) */
						if (PKTISBUFALLOC(OSH_NULL, lb)) {
							if (PKTHASHEAPBUF(OSH_NULL, lb)) {
								LB_FREE(OSH_NULL,
									PKTHEAD(OSH_NULL, lb),
									MAXPKTDATABUFSZ);
								PKTRESETHEAPBUF(OSH_NULL, lb);
							} else {
								lfbufpool_free(
									PKTHEAD(OSH_NULL, lb));
							}
							PKTRESETBUFALLOC(OSH_NULL, lb);
						}

						lb->mem.refcnt = 1;
						lb->data = lb_end(lb);
						lb->len = 0;
						lb->flags = LBF_TX_FRAG;

						/* Clear memory after reset field */
						bzero(&lb->reset, LBUFSZ -
							OFFSETOF(struct lbuf, reset));
#ifndef HWA_TXPOST_BZERO_PKTTAG
						bzero(lb->pkttag, sizeof(lb->pkttag));
#endif
#ifdef WL_REUSE_KEY_SEQ
						bzero(PKTFRAGFCTLV(OSH_NULL, lb), 8);
#endif
						/* Free HWA tx buffer */
						hwa_txpost_txbuffer_free(hwa_dev,
							(void *)LFRAG2HWAPKT(lb));
					}
				} else
#endif /* BCMHWA && HWA_TXPOST_BUILD */
				{
					lb->data = (uchar*) 0xdeadbeef;
					hnd_pktid_release(lb, lb->mem.pktid); /* BCMPKTIDMAP */
					LB_FREE(OSH_NULL, lb, LB_END(lb) - LB_HEAD(lb));
				}
			} else {
#ifdef HND_PT_GIANT
				hnd_pktid_release(lb, lb->mem.pktid); /* BCMPKTIDMAP */
				hnd_free_pt(lb);
#else
				ASSERT(0);
#endif
			}
		}
	} else {
		struct lbuf *orig = ((struct lbuf_clone*)lb)->orig;
		void *p = lb;
		uint len = sizeof(struct lbuf_clone);

		ASSERT(!lb_pool(lb)); /* clone is not from pool */

#if defined(BCMHWA) && defined(HWA_TXPOST_BUILD)
		if (PKTISHWAPKT(OSH_NULL, lb) && PKTISMGMTTXPKT(OSH_NULL, lb)) {
			p = LFRAG2HWAPKT(lb);
			len += HWA_TXPOST_PKT_BYTES;
		}
#endif

		/* clones do not get refs, just originals */
		ASSERT(lb->mem.refcnt == 0);
		lb->data = (uchar*) 0xdeadbeef;
		hnd_pktid_release(lb, lb->mem.pktid); /* BCMPKTIDMAP */
		LB_FREE(OSH_NULL, p, len);

#if defined(BCMDBG_MEMFAIL)
		lb_free_one(orig, call_site);
#else
		lb_free_one(orig);
#endif
	}
}

void
lb_free(struct lbuf *lb)
{
	struct lbuf *next;
	bool snarf;

	while (lb) {
		ASSERT(PKTLINK(lb) == NULL);

		next = PKTNEXT(OSH_NULL, lb);

		if (BCMLFRAG_ENAB()) {

			lbuf_freeinfo_t *lbuf_infoptr;

			lbuf_infoptr = lbuf_info_get();
			/* if a tx frag or rx frag , go to callback functions before free up */
			/* pciedev_lbuf_callback */
			if (lbuf_infoptr->cb) {
				PKTSETNEXT(OSH_NULL, lb, NULL);
				snarf = lbuf_infoptr->cb(lbuf_infoptr->arg, lb);
				if (snarf) {
					lb = next;
					continue;
				}
			}
		}

#if defined(BCMPCIEDEV) && !defined(BME_PKTFETCH)
		if (BCMPCIEDEV_ENAB()) {
			/* rte_pktfetch.c::hnd_lbuf_free_cb() */
			if (lbuf_free_cb != NULL)
				(lbuf_free_cb)(lb);
		}
#endif /* BCMPCIEDEV && !BME_PKTFETCH */

#if defined(BCMDBG_MEMFAIL)
		lb_free_one(lb, CALL_SITE);
#else
		lb_free_one(lb);
#endif
		lb = next;
	}
}

struct lbuf *
#if defined(BCMDBG_MEMFAIL)
lb_dup(struct lbuf *lb, void *call_site)
#else
lb_dup(struct lbuf *lb)
#endif
{
	struct lbuf *lb_new;

#if defined(BCMDBG_MEMFAIL)
	if (!(lb_new = lb_alloc(lb->len, lbuf_basic, call_site)))
#else
	if (!(lb_new = lb_alloc(lb->len, lbuf_basic)))
#endif
		return (NULL);

	bcopy(lb->data, lb_new->data, lb->len);

	return (lb_new);
}

struct lbuf *
#if defined(BCMDBG_MEMFAIL)
lb_clone(struct lbuf *lb, int offset, int len, void *call_site)
#else
lb_clone(struct lbuf *lb, int offset, int len)
#endif
{
	struct lbuf *clone;
	struct lbuf *orig;
	uint tot = sizeof(struct lbuf_clone);

	ASSERT(offset >= 0 && len >= 0);

	if (hnd_pktid_free_cnt() == 0) {
		LBUF_MSG(("lb_clone: hnd_pktid_free_cnt 0\n"));
		hnd_pktid_inc_fail_cnt();
		return (NULL);
	}

	if (offset < 0 || len < 0)
	        return (NULL);

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	tot += HWA_TXPOST_PKT_BYTES;
#endif

#if defined(BCMDBG_MEMFAIL)
	if ((clone = MALLOC_ALIGN_CALLSITE(OSH_NULL, tot, 0, call_site)) == NULL)
#else
	if ((clone = MALLOC_ALIGN(OSH_NULL, tot, 0)) == NULL)
#endif
	{
		LBUF_MSG(("lb_clone: size (%u); alloc failed; called from 0x%p\n", LBUFSZ,
			CALL_SITE));
		return (NULL);
	}

	/* find the actual non-clone original lbuf */
	if (lb_isclone(lb))
		orig = ((struct lbuf_clone*)lb)->orig;
	else
		orig = lb;

	ASSERT(orig->mem.refcnt < 255);
	orig->mem.refcnt++;

	bzero(clone, tot);
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	/* Shift HWA 3a SWPKT header offset */
	clone = (struct lbuf *)HWAPKT2LFRAG(clone);
#endif
	ASSERT(ISALIGNED(clone, sizeof(int)));

	/* clone's data extent is a subset of the current data of lb */
	ASSERT(lb->data + offset + len <= (lb_end(lb)));

	clone->data = lb->data + offset;
	clone->head = clone->data;
	clone->end = clone->head + len;
	clone->len = len;
	clone->flags |= LBF_CLONE;
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
	clone->flags |= (LBF_HWA_PKT | LBF_MGMT_TX_PKT);
#endif
	clone->mem.pktid = hnd_pktid_allocate(clone); /* BCMPKTIDMAP: id!=0 */
	ASSERT(clone->mem.pktid != PKT_INVALID_ID);

	((struct lbuf_clone*)clone)->orig = orig;

	return (clone);
}

/*
 * lb_sane() is invoked in various lb_xyz() functions. In the implementation of
 * lb_sane(), please do not include any calls to lb_xyz() to avoid stack
 * overflow from recursive calls.
 */
bool NO_INSTR_FUNC
lb_sane(struct lbuf *lb)
{
	int insane = 0;

#if defined(BCMPKTIDMAP)
	if ((lb->mem.pktid == PKT_NULL_ID) || (lb->mem.pktid > PKT_MAXIMUM_ID)) {
		insane |= 1;
	}
#endif /*   BCMPKTIDMAP */

	if (lb->mem.poolid != PKTPOOL_INVALID_ID) { /* From a pktpool */
		if (lb->mem.poolid > PKTPOOL_MAXIMUM_ID)
			insane |= 1;
		else
			insane |= (PKTPOOL_ID2PTR(lb->mem.poolid)->id != lb->mem.poolid);
	}

	insane |= (lb->data < LB_HEAD(lb));
	insane |= (lb->data + lb->len > LB_END(lb));

	if (insane) {
		LBUF_MSG(("lb_sane:\n"
		          "lbuf %p data %p head %p end %p len %d flags %d "
		          "pktid %u poolid %u\n",
		          lb, lb->data, LB_HEAD(lb), LB_END(lb), lb->len, lb->flags,
		          lb->mem.pktid, lb->mem.poolid));
	}

	return (!insane);
}

void
lb_audit(struct lbuf * lb)
{
	ASSERT(lb != NULL);

#if defined(BCMHWA) && defined(HWA_PKT_MACRO)
	if (PKTISHWAPKT(OSH_NULL, lb))
		return;
#endif

	if (lb_is_frag(lb)) {
		do {
			uint16 fragix, flen = 0;

			fragix = LBFP(lb)->flist.finfo[LB_FRAG_CTX].ctx.fnum;
			ASSERT((fragix >= 1) && (fragix <= LB_FRAG_MAX));
			while (fragix > 0) {
				ASSERT(LBFP(lb)->flist.finfo[fragix].frag.data_lo != 0);
				ASSERT(LBFP(lb)->flist.flen[fragix] > 0);
				flen += LBFP(lb)->flist.flen[fragix];
				fragix--;
			}
			ASSERT(flen == LBFP(lb)->flist.flen[LB_FRAG_CTX]);
			lb = PKTNEXT(OSH_NULL, lb);
			if (lb)
				ASSERT((lb->data == NULL) && (lb->len == 0));	/* disallow? */
		} while (lb != NULL);
	} else 	{
		do {
			ASSERT(lb->data != NULL);
			ASSERT(lb_sane(lb) == 0);
			lb = PKTNEXT(OSH_NULL, lb);
		} while (lb != NULL);
	}
}

/*
 * reset lbuf before recycling to pool
 *      PRESERVE                        FIX-UP
 *      mem.pktid, mem.poolid           mem.refcnt, mem.poolstate
 *      offsets | head, end
 *                                      data
 *                                      len, flags
 *                                      reset all other fields
 */
void
lb_resetpool(struct lbuf *lb, uint16 len)
{
#ifdef BCMPCIEDEV
	uint16 rxcpl_id = 0;
	if (BCMPCIEDEV_ENAB()) {
		rxcpl_id = lb->rxcpl_id;
	}
#endif

	ASSERT(lb_sane(lb));
	ASSERT(lb_pool(lb));

	lb->mem.refcnt = 1;
#ifdef BCMDBG_POOL
	lb_setpoolstate(lb, POOL_IDLE); /* mem.poolstate */
#endif

	lb->data = lb_end(lb) - ROUNDUP(len, sizeof(int));
	lb->len = len;
	if (lb->flags & LBF_TX_FRAG) {
		lb->flags = lb->flags & (LBF_TX_FRAG | LBF_PTBLK | LBF_RX_FRAG | LBF_HEAPBUF);
#ifdef WL_REUSE_KEY_SEQ
		bzero(PKTFRAGFCTLV(OSH_NULL, lb), 8);
#endif
	} else {
		lb->flags = lb->flags & (LBF_TX_FRAG | LBF_PTBLK | LBF_RX_FRAG);
	}

	bzero(&lb->reset, LBUFSZ - OFFSETOF(struct lbuf, reset));

#if defined(BCMHWA) && defined(HWA_PKT_MACRO)
	/* Because we move the pkttag to the first place of struct lbuf to get the enhancement
	 * from HWA_TX_TXPOST_CFG1_BZERO_PKTTAG_SUPPORT, but that is for TX packet in TXBM.
	 * In this function we need to bzero manually for lbuf from pool, like RX packet.
	 */
	bzero(lb->pkttag, sizeof(lb->pkttag));
#endif

#ifdef BCMPCIEDEV
	if (BCMPCIEDEV_ENAB()) {
		lb->rxcpl_id = rxcpl_id;
	}
#endif
#ifdef WLCFP
	/* Reset CFP Flowid
	 * Both 0 and CFP_FLOWID_INVALID are considered invalid for CFP flows
	 */
	lb->cfp_flowid = 0;
#endif
}

void
lb_clear_pkttag(struct lbuf *lb)
{
	bzero(lb->pkttag, sizeof(lb->pkttag));
}

void
lbuf_free_cb_set(lbuf_free_global_cb_t cb)
{
#if defined(BCMPCIEDEV) && !defined(BME_PKTFETCH)
	if (BCMPCIEDEV_ENAB()) {
		lbuf_free_cb = cb;
	}
#endif /* BCMPCIEDEV && !BME_PKTFETCH */
}

#ifdef BCMDBG
void
lb_dump(void)
{
	uint i;
	LBUF_MSG(("allocfail %d\n", lbuf_allocfail));
	for (i = 0; i < LBUF_BIN_SIZE; i++) {
		LBUF_MSG(("bin[%d] %d ", i, lbuf_hist[i]));
	}
	LBUF_MSG(("\n"));
}
#endif	/* BCMDBG */
