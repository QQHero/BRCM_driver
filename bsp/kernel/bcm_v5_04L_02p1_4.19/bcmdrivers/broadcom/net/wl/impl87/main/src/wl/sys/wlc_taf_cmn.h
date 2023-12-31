/*
 * wlc_taf_cmn.h
 *
 * This module contains the common definitions for the taf transmit module.
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
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wlc_taf_cmn.h 803989 2021-10-14 19:10:00Z $
 *
 */

#if !defined(__wlc_taf_cmn_h__)
#define __wlc_taf_cmn_h__

#ifdef PKTQ_LOG
//#define TAF_PKTQ_LOG
#endif

#ifdef DONGLEBUILD
/* DONGLE */
#if defined(BCMHWA3AB) || defined(BCMHWA3A)
#define HWA_TXPOST_BUILD
#endif
#if defined(WLSQS) && defined(HWA_TXPOST_BUILD)
#define TAF_VALID_CONFIG        1
#endif /* WLSQS */

#else /* !DONGLEBUILD, ie NIC */
/* NIC */
#ifdef WLAMPDU_PRECEDENCE
#error TAF does not support WLAMPDU_PRECEDENCE
#endif /* !WLAMPDU_PRECEDENCE */

#define TAF_VALID_CONFIG        1

#endif /* DONGLEBUILD */

#ifndef TAF_VALID_CONFIG
/* eg non SQS DONGLEBUILD */
#define TAF_VALID_CONFIG        0
#endif

#if TAF_VALID_CONFIG
/* define scheduler methods support */
#define WLTAF_IAS
#endif

#ifndef WLCNTSCB
#error TAF requires WLCNTSCB
#endif

/* define source data support */
#define TAF_ENABLE_NAR          TAF_VALID_CONFIG

#define TAF_ENABLE_AMPDU        TAF_VALID_CONFIG

#ifdef WL_MU_TX
#define TAF_ENABLE_MU_TX        TAF_VALID_CONFIG
#else
#define TAF_ENABLE_MU_TX        0
#endif

#if TAF_ENABLE_MU_TX
#define TAF_MAX_MU_MIMO          8
#define TAF_MAX_MU_OFDMA         8
#if TAF_MAX_MU_OFDMA > TAF_MAX_MU_MIMO
#define TAF_MAX_MU               TAF_MAX_MU_OFDMA
#else
#define TAF_MAX_MU               TAF_MAX_MU_MIMO
#endif /* TAF_MAX_MU_OFDMA > TAF_MAX_MU_MIMO */
#endif /* TAF_ENABLE_MU_TX */

#define TAF_ENABLE_MU_BOOST     TAF_ENABLE_MU_TX

#ifdef WLSQS
#define TAF_ENABLE_SQS_PULL     TAF_VALID_CONFIG
#define TAF_SQS_DELAY_WARN      50000
#else
#define TAF_ENABLE_SQS_PULL     0
#endif

#define TAF_ENABLE_PSQ_PULL     TAF_VALID_CONFIG

#ifdef WL_ULMU
#define TAF_ENABLE_UL     TAF_ENABLE_MU_TX
#else
#define TAF_ENABLE_UL     0
#endif

#if defined(BCMDBG)
#define TAF_DBG
#define TAF_DEBUG_VERBOSE
#define TAF_LOGL1       TAF_VALID_CONFIG
#define TAF_LOGL2       TAF_VALID_CONFIG
#define TAF_LOGL3       0
#define TAF_LOGL4       0
#else
#define TAF_LOGL1       TAF_VALID_CONFIG
#define TAF_LOGL2       0
#define TAF_LOGL3       0
#define TAF_LOGL4       0
#endif

#define TAF_DEFAULT_UNIFIED_TID ALLPRIO

#define TAF_ENABLE_TIMER        TAF_VALID_CONFIG

#ifndef TAF_ENABLE_TIMER
#define TAF_ENABLE_TIMER        0
#endif

#define TAF_MAXPRIO             NUMPRIO
#define TAF_NUMPRIO(s)          NUMPRIO

#include <wlc.h>
#include <wlc_scb_ratesel.h>
#include <wlc_airtime.h>
#include <wlc_scb.h>
#include <wlc_bmac.h>
#include <wlc_types.h>

#include <wlc_taf.h>
#include <wlc_ampdu.h>
#include <wlc_amsdu.h>

#if TAF_ENABLE_NAR
#include <wlc_nar.h>
#endif /* TAF_ENABLE_NAR */

#if TAF_ENABLE_SQS_PULL
#include <wlc_sqs.h>
#endif /* TAF_ENABLE_SQS_PULL */

#ifdef TAF_PKTQ_LOG
#include <wlioctl.h>
#endif /* TAF_PKTQ_LOG */

#if TAF_ENABLE_MU_TX
#include <wlc_mutx.h>
#endif /* TAF_ENABLE_MU_TX */

#if TAF_ENABLE_UL
#include <wlc_ulmu.h>
#endif /* TAF_ENABLE_UL */

#if TAF_ENABLE_PSQ_PULL
#include <wlc_apps.h>
#endif /* TAF_ENABLE_PSQ_PULL */

typedef struct taf_scb_cubby    taf_scb_cubby_t;
typedef uint8                   taf_traffic_map_t;

#define TAF_SCORE_MAX           0xFFFFFFFF
#define TAF_SCORE_MIN           0

#ifdef TAF_DBG
#define TAF_WD_STALL_RESET_LIMIT  60  /* 60 times WD 'samples' no pkts in flow, do hard reset */
#else
#define TAF_WD_STALL_RESET_LIMIT  5  /* 5 times WD 'samples' no pkts in flow, do hard reset */
#endif

/* order of scheduler priority */
typedef enum {
#ifdef WLTAF_IAS
#if TAF_ENABLE_SQS_PULL
	TAF_VIRTUAL_MARKUP = -1,
	FIRST_IAS_SCHEDULER = TAF_VIRTUAL_MARKUP,
	TAF_SCHED_FIRST_INDEX = FIRST_IAS_SCHEDULER,
#endif /* TAF_ENABLE_SQS_PULL */
	TAF_EBOS,
	TAF_SCHEDULER_START = TAF_EBOS,
#if !TAF_ENABLE_SQS_PULL
	FIRST_IAS_SCHEDULER = TAF_EBOS,
	TAF_SCHED_FIRST_INDEX = FIRST_IAS_SCHEDULER,
#endif /* !TAF_ENABLE_SQS_PULL */
	TAF_PSEUDO_RR,
	TAF_ATOS,
	TAF_ATOS2,
	LAST_IAS_SCHEDULER = TAF_ATOS2,
#ifndef WLTAF_SCHEDULER_AVAILABLE
#define WLTAF_SCHEDULER_AVAILABLE
#endif
#endif /* WLTAF_IAS */
#ifndef WLTAF_SCHEDULER_AVAILABLE
	TAF_SCHED_FIRST_INDEX,
	TAF_SCHED_LAST_INDEX = TAF_SCHED_FIRST_INDEX,
#else
	TAF_SCHED_LAST_INDEX,
#endif
	NUM_TAF_SCHEDULERS = TAF_SCHED_LAST_INDEX - TAF_SCHED_FIRST_INDEX,
	TAF_UNDEFINED = NUM_TAF_SCHEDULERS,
#ifndef WLTAF_SCHEDULER_AVAILABLE
	TAF_SCHEDULER_START = TAF_UNDEFINED,
	TAF_DEFAULT_SCHEDULER = TAF_UNDEFINED
#endif
#if defined(WLTAF_IAS)
	TAF_DEFAULT_SCHEDULER = TAF_ATOS
#endif
} taf_scheduler_kind;

typedef enum {
#ifdef WLTAF_IAS
	TAF_SCHEDULER_IAS_METHOD,
#endif
	NUM_TAF_SCHEDULER_METHOD_GROUPS,
	TAF_GROUP_UNDEFINED
} taf_scheduler_method_group;

typedef enum {
	TAF_SINGLE_PUSH_SCHEDULING,
	TAF_ORDERED_PULL_SCHEDULING,
	TAF_SCHEME_UNDEFINED
} taf_scheduling_scheme;

typedef enum {
	TAF_LIST_DO_NOT_SCORE,
	TAF_LIST_SCORE_MINIMUM,
	TAF_LIST_SCORE_MAXIMUM,
	TAF_LIST_SCORE_FIXED_INIT_MINIMUM,
	TAF_LIST_SCORE_FIXED_INIT_MAXIMUM
} taf_list_scoring_order_t;

typedef enum {
	TAF_FIRST_REAL_SOURCE,

	/* the order of sources here implicitly defines the ordering priority of that source */

#if TAF_ENABLE_UL
#ifdef TAF_SOURCE_FIRST
#error check me
#else
	TAF_SOURCE_UL = TAF_FIRST_REAL_SOURCE,
#define TAF_SOURCE_FIRST
#endif /* TAF_SOURCE_FIRST */
#endif /* TAF_ENABLE_UL */

#if TAF_ENABLE_PSQ_PULL
#ifdef TAF_SOURCE_FIRST
	/* TAF_SOURCE_PSQ are real data packets */
	TAF_SOURCE_PSQ,
#else
	TAF_SOURCE_PSQ = TAF_FIRST_REAL_SOURCE,
#define TAF_SOURCE_FIRST
#endif /* TAF_SOURCE_FIRST */
#endif /* TAF_ENABLE_PSQ_PULL */

#if TAF_ENABLE_NAR
#ifdef TAF_SOURCE_FIRST
	/* TAF_SOURCE_NAR are real data packets */
	TAF_SOURCE_NAR,
#else
	TAF_SOURCE_NAR = TAF_FIRST_REAL_SOURCE,
#define TAF_SOURCE_FIRST
#endif /* TAF_SOURCE_FIRST */
#endif /* TAF_ENABLE_NAR */

#if TAF_ENABLE_AMPDU
#ifdef TAF_SOURCE_FIRST
	/* TAF_SOURCE_AMPDU are real data packets */
	TAF_SOURCE_AMPDU,
#else
	TAF_SOURCE_AMPDU = TAF_FIRST_REAL_SOURCE,
#define TAF_SOURCE_FIRST
#endif /* TAF_SOURCE_FIRST */
#endif /* TAF_ENABLE_AMPDU */
#ifdef TAF_SOURCE_FIRST
	TAF_NUM_REAL_SOURCES,
#else
	TAF_NUM_REAL_SOURCES = TAF_FIRST_REAL_SOURCE,
#define TAF_SOURCE_FIRST
#endif
#if TAF_ENABLE_SQS_PULL
	/* TAF_SOURCE_HOST_SQS are virtual data packets */
	TAF_SOURCE_HOST_SQS = TAF_NUM_REAL_SOURCES,
	TAF_NUM_SCHED_SOURCES,
#else
	TAF_NUM_SCHED_SOURCES = TAF_NUM_REAL_SOURCES,
#endif

	TAF_SOURCE_UNDEFINED = TAF_NUM_SCHED_SOURCES
} taf_source_type_t;

#define TAF_SCHED_INDEX(sched)       ((int)(sched) - (int)TAF_SCHED_FIRST_INDEX)

#define TAF_SOURCE_IS_REAL(s_idx)    (!TAF_SOURCE_IS_VIRTUAL(s_idx))

#define TAF_SOURCE_IS_VIRTUAL(s_idx) TAF_SOURCE_IS_SQS(s_idx)

#if TAF_ENABLE_SQS_PULL
#define TAF_SOURCE_IS_PSQ(s_idx)     ((s_idx) == TAF_SOURCE_PSQ)
#else
#define TAF_SOURCE_IS_PSQ(s_idx)     FALSE
#endif

#if TAF_ENABLE_NAR
#define TAF_SOURCE_IS_NAR(s_idx)     ((s_idx) == TAF_SOURCE_NAR)
#else
#define TAF_SOURCE_IS_NAR(s_idx)     FALSE
#endif

#if TAF_ENABLE_AMPDU
#define TAF_SOURCE_IS_AMPDU(s_idx)   ((s_idx) == TAF_SOURCE_AMPDU)
#else
#define TAF_SOURCE_IS_AMPDU(s_idx)   FALSE
#endif

#if TAF_ENABLE_SQS_PULL
#define TAF_SOURCE_IS_SQS(s_idx)     ((s_idx) == TAF_SOURCE_HOST_SQS)
#else
#define TAF_SOURCE_IS_SQS(s_idx)     FALSE
#endif

#if TAF_ENABLE_UL
#define TAF_SOURCE_IS_UL(s_idx)     ((s_idx) == TAF_SOURCE_UL)
#else
#define TAF_SOURCE_IS_UL(s_idx)     FALSE
#endif

#define TAF_SOURCE_IS_DL(s_idx)     (!TAF_SOURCE_IS_UL(s_idx))

typedef enum {
	TAF_TYPE_DL,
#if TAF_ENABLE_UL
	TAF_TYPE_UL,
#endif
	TAF_NUM_LIST_TYPES,
	TAF_TYPE_ALL = TAF_NUM_LIST_TYPES
} taf_list_type_t;

extern const char*                taf_type_name[TAF_NUM_LIST_TYPES];
#define TAF_TYPE(type)            taf_type_name[(type)]

typedef enum {
	TAF_ORDER_TID_SCB,
	TAF_ORDER_NUM_OPTIONS
} taf_schedule_order_t;

typedef struct taf_list {
	struct taf_list *next;
	struct taf_list *prev;
	uint8 type; /* UL or DL */
	taf_traffic_map_t used;
	taf_traffic_map_t bridged;
	union {
		taf_scb_cubby_t *scb_taf;
	};
} taf_list_t;

typedef enum {
	TAF_RSPEC_SU_DL,
	TAF_RSPEC_SU_DL_INSTANT,
#if TAF_ENABLE_MU_BOOST
	TAF_RSPEC_MU_DL_BOOST,
#endif /* TAF_ENABLE_MU_BOOST */
#if TAF_ENABLE_MU_TX
	TAF_RSPEC_MU_DL_INSTANT,
#endif /* TAF_ENABLE_MU_TX */
#if TAF_ENABLE_UL
	TAF_RSPEC_UL,
#endif /* TAF_ENABLE_UL */
	NUM_TAF_RATES
} taf_rspec_index_t;

typedef struct {
	void *        (*attach_fn) (wlc_taf_info_t *, taf_scheduler_kind);
	int           (*detach_fn) (void *);
	void          (*up_fn)     (void *);
	int           (*down_fn)   (void *);
	taf_list_t ** (*list_fn)   (void *);
} taf_scheduler_def_t;

extern const taf_scheduler_def_t * taf_scheduler_definitions[NUM_TAF_SCHEDULERS];

#ifdef WLTAF_IAS
extern const taf_scheduler_def_t taf_ias_scheduler;
#endif

extern const struct ether_addr      taf_undef_ea;

#if TAF_ENABLE_MU_BOOST
#define TAF_RIDX_IS_BOOST(ridx)     ((ridx) == TAF_RSPEC_MU_DL_BOOST)
#else
#define TAF_RIDX_IS_BOOST(ridx)     FALSE
#endif

#if TAF_ENABLE_MU_TX
#define TAF_RIDX_IS_MUMIMO(ridx)    (((ridx) == TAF_RSPEC_MU_DL_INSTANT) || TAF_RIDX_IS_BOOST(ridx))
#else
#define TAF_RIDX_IS_MUMIMO(ridx)    FALSE
#endif

#if TAF_ENABLE_UL
#define TAF_RIDX_IS_UL(ridx)        ((ridx) == TAF_RSPEC_UL)
#else
#define TAF_RIDX_IS_UL(ridx)        FALSE
#endif

#ifdef DONGLEBUILD
#define TAF_STATIC_CONTEXT          TAF_VALID_CONFIG
#else
#define TAF_STATIC_CONTEXT          0
#endif

#if TAF_ENABLE_UL
#define TAF_STATIC_UL_CONTEXT       84
#ifdef TAF_DBG
#define TAF_STATIC_UL_DBG_CONTEXT   4
#endif
#endif /* TAF_ENABLE_UL */

#ifndef TAF_STATIC_UL_CONTEXT
#define TAF_STATIC_UL_CONTEXT       0
#endif

#ifndef TAF_STATIC_UL_DBG_CONTEXT
#define TAF_STATIC_UL_DBG_CONTEXT   0
#endif

#ifndef TAF_STATIC_DBG_CONTEXT
#define TAF_STATIC_DBG_CONTEXT      (0 + TAF_STATIC_UL_DBG_CONTEXT)
#endif

#if TAF_STATIC_CONTEXT
#define TAF_STATIC_CONTEXT_SIZE     (80 + TAF_STATIC_DBG_CONTEXT + TAF_STATIC_UL_CONTEXT)
#endif

#define TAF_COMPILE_ASSERT(type, arg) extern char __taf_compile_assert_##type[(arg) ? 1 : -1];

typedef struct {
	ratespec_t rspec;
	uint32     rate;
} taf_rate;
typedef struct {
#ifdef WLTAF_IAS
	struct  {
		struct {
			uint32     dcaytimestamp[TAF_NUM_LIST_TYPES];
			uint32     byte_rate[NUM_TAF_RATES];
			uint32     pkt_rate[NUM_TAF_RATES];
			uint16     relative_score[TAF_NUM_LIST_TYPES];
			uint16     overhead_with_rts;
			uint16     overhead_without_rts;
			uint16     overhead_rts;
			bool       use[TAF_NUM_SCHED_SOURCES];
			uint8      ridx_used[TAF_NUM_LIST_TYPES];
#if TAF_ENABLE_NAR
			uint32     nar_pkt_rate;
			uint32     nar_byte_rate;
#endif
		} data;
#ifdef TAF_DBG
		struct {
			uint32  ps_enter;
			uint32  ps_enter_index;
		} debug;
#endif /* TAF_DBG */
	} ias;
#endif /* WLTAF_IAS */
	struct {
		struct {
			taf_rate   rate[NUM_TAF_RATES];
			uint8      ratespec_mask;
			uint8      max_nss;
			uint8      bw_idx;
#ifdef TAF_DBG
			uint8      encode;
			uint8      mcs;
#endif /* TAF_DBG */
#if TAF_ENABLE_MU_TX
			struct {
				uint8      nss;
#ifdef TAF_DBG
				uint8      encode;
				uint8      mcs;
#endif /* TAF_DBG */

#if TAF_ENABLE_MU_BOOST
				uint8      clients_count;
#endif /* TAF_ENABLE_MU_BOOST */
			} mu;
#endif /* TAF_ENABLE_MU_TX */
		} rdata;
	} global;
} taf_scheduler_scb_stats_t;

typedef void* taf_scheduler_tid_info_t;

typedef enum {
#if TAF_ENABLE_MU_TX
	TAF_TECH_DL_HEMUMIMO,
	TAF_TECH_DL_VHMUMIMO,
	TAF_TECH_DL_OFDMA,
#if TAF_ENABLE_UL
	TAF_TECH_UL_OFDMA,
#endif /* TAF_ENABLE_UL */
#endif /* TAF_ENABLE_MU_TX */
	TAF_NUM_MU_TECH_TYPES,
	TAF_TECH_DL_SU = TAF_NUM_MU_TECH_TYPES,
	TAF_NUM_TECH_TYPES,
	TAF_TECH_DONT_ASSIGN = TAF_NUM_TECH_TYPES,
	TAF_TECH_INVALID,
	TAF_TECH_UNASSIGNED = TAF_TECH_INVALID,
	TAF_NUM_TECH_TYPE_NAMES
} taf_tech_type_t;

typedef enum {
	TAF_MUBOOST_OFF = 0,
	TAF_MUBOOST_FACTOR = 1,
	TAF_MUBOOST_RATE = 2,
	TAF_MUBOOST_RATE_FACTOR = 3,
	TAF_MUBOOST_SU_NSS_FAIR = 4,
	TAF_NUM_MUBOOST_CONFIGS
} taf_muboost_mode_t;

#define TAF_TECH_DL_HEMUMIMO_MASK   (1 << TAF_TECH_DL_HEMUMIMO)
#define TAF_TECH_DL_VHMUMIMO_MASK   (1 << TAF_TECH_DL_VHMUMIMO)
#define TAF_TECH_DL_OFDMA_MASK      (1 << TAF_TECH_DL_OFDMA)
#if TAF_ENABLE_UL
#define TAF_TECH_UL_OFDMA_MASK      (1 << TAF_TECH_UL_OFDMA)
#define TAF_TECH_MASK_IS_UL(tech)   (((tech) & TAF_TECH_UL_OFDMA_MASK) != 0)
#else
#define TAF_TECH_UL_OFDMA_MASK      (0)
#define TAF_TECH_MASK_IS_UL(tech)   FALSE
#endif /* TAF_ENABLE_UL */
#define TAF_TECH_DL_SU_MASK         (1 << TAF_TECH_DL_SU)

#define TAF_TECH_DL_MIMO_MASK       (TAF_TECH_DL_HEMUMIMO_MASK | TAF_TECH_DL_VHMUMIMO_MASK)
#define TAF_TECH_DL_MU_MASK         (TAF_TECH_DL_MIMO_MASK | TAF_TECH_DL_OFDMA_MASK)

#if TAF_ENABLE_MU_TX
#define TAF_TECH_MASK_IS_MUMIMO(tech)  (((tech) & TAF_TECH_DL_MIMO_MASK) != 0)
#define TAF_TECH_MASK_IS_OFDMA(tech)   (((tech) & (TAF_TECH_DL_OFDMA_MASK | \
							TAF_TECH_UL_OFDMA_MASK)) != 0)
#define TAF_TECH_MASK_IS_SU_ONLY(tech) (((tech) & (TAF_TECH_DL_MIMO_MASK | \
							TAF_TECH_DL_OFDMA_MASK)) == 0)

typedef struct {
	taf_traffic_map_t         enable;
} taf_mu_tech_t;

#else

#define TAF_TECH_MASK_IS_MUMIMO(tech)    FALSE
#define TAF_TECH_MASK_IS_OFDMA(tech)     FALSE
#define TAF_TECH_MASK_IS_SU_ONLY(tech)   TRUE
#endif /* TAF_ENABLE_MU_TX */

typedef struct {
	taf_list_t                list[TAF_NUM_LIST_TYPES];
	uint8                     linkstate[TAF_NUM_SCHED_SOURCES][TAF_MAXPRIO];
	taf_scheduler_tid_info_t  tid_info[TAF_MAXPRIO];
	struct {
		taf_traffic_map_t     map[TAF_NUM_SCHED_SOURCES]; /* bitmask for TID per src */
		                      /* global bitmask for all srcs/TID */
		taf_traffic_map_t     available[TAF_NUM_LIST_TYPES];
		uint16                count[TAF_NUM_SCHED_SOURCES][TAF_MAXPRIO];
		uint32                est_units[TAF_NUM_LIST_TYPES][TAF_MAXPRIO];
	} traffic;
	taf_traffic_map_t         tid_enabled;
	taf_traffic_map_t         force;
	uint8                     max_pdu;
	bool                      ps_mode;
	uint8                     tech_enable_mask;
	int8                      tech_type[TAF_NUM_LIST_TYPES][TAF_MAXPRIO];
	uint8                     suspend[TAF_NUM_LIST_TYPES][TAF_MAXPRIO];
#if TAF_ENABLE_MU_TX
	taf_traffic_map_t         mu_tech_en[TAF_NUM_MU_TECH_TYPES];
#endif
#if TAF_ENABLE_SQS_PULL
	taf_traffic_map_t         pkt_pull_map;
	uint16                    pkt_pull_dequeue;
	uint16                    pkt_pull_request[TAF_MAXPRIO];
#endif /* TAF_ENABLE_SQS_PULL */
	taf_scheduler_scb_stats_t scb_stats;
#if TAF_STATIC_CONTEXT
	uint8                     static_context[TAF_MAXPRIO][TAF_STATIC_CONTEXT_SIZE];
#endif
} taf_scheduler_info_t;

typedef struct {
	void *           scbh;
	void *           tidh;
} taf_sched_handle_t;

typedef enum {
	TAF_SCHEDULE_REAL_PACKETS = 0,
	TAF_SCHEDULE_VIRTUAL_PACKETS = TAF_SQS_TRIGGER_TID,
	TAF_MARKUP_REAL_PACKETS = TAF_SQS_V2R_COMPLETE_TID
} taf_schedule_state_t;

typedef enum {
	TAF_CYCLE_INCOMPLETE,
	TAF_CYCLE_COMPLETE,
	TAF_CYCLE_FAILURE,
	TAF_CYCLE_LEGACY,
	NUM_TAF_CYCLE_TYPES
} taf_cycle_completion_t;

typedef struct  {
	taf_schedule_state_t     op_state;
	taf_scheduler_public_t   public;
	int                      tid;
	struct scb*              scb;
	uint32                   now_time;
	uint32                   actual_release;
	uint32                   actual_release_n;
	uint32                   virtual_release;
	uint32                   pending_release;
	uint32                   state[NUM_TAF_SCHEDULERS];
	taf_cycle_completion_t   status;
} taf_release_context_t;

typedef struct {
	bool  (*scheduler_fn)     (wlc_taf_info_t *, taf_release_context_t *, void *);
	int   (*watchdog_fn)      (void *);
	int   (*dump_fn)          (void *, struct bcmstrbuf *);
	bool  (*linkstate_fn)     (void *, struct scb *, int, taf_source_type_t, taf_link_state_t);
	int   (*scbstate_fn)      (void *, struct scb *, taf_source_type_t, void *,
		taf_scb_state_t);
	bool  (*bssstate_fn)      (void *, wlc_bsscfg_t *, void *, taf_bss_state_t);
	bool  (*rateoverride_fn)  (void *, ratespec_t, wlcband_t *);
	int   (*iovar_fn)         (void *, taf_scb_cubby_t *, const char *, wl_taf_define_t *,
		struct bcmstrbuf *);
	bool  (*txstat_fn)        (void *, taf_scb_cubby_t *, int, void *, taf_txpkt_state_t);
	bool  (*rxstat_fn)        (void *, taf_scb_cubby_t *, int, int, void *, taf_rxpkt_state_t);
	void  (*schedstate_fn)    (void *, taf_scb_cubby_t *, int, int, taf_source_type_t,
		taf_sched_state_t);
	int   (*traffic_stats_fn) (void *, taf_scb_cubby_t *, int, taf_traffic_stats_t *);
#ifdef TAF_PKTQ_LOG
	uint32 (*dpstats_log_fn)  (void *, taf_scb_cubby_t *, mac_log_counters_v06_t*, bool,
		uint32, uint32, uint32);
#endif
} taf_scheduler_fn_t;

typedef struct taf_method_info {
	taf_scheduler_kind         type;
	const char*                name;
#ifdef TAF_DBG
	const char*                dump_name;
#endif
	taf_scheduling_scheme      scheme;
	taf_list_scoring_order_t   ordering;
	taf_scheduler_method_group group;
	taf_scheduler_fn_t         funcs;
	struct wlc_taf_info*       taf_info;
	taf_list_t*                active_list;
	taf_list_t*                list;
	taf_traffic_map_t*         ready_to_schedule;
	uint32*                    now_time_p;
	uint32                     counter;
	uint32                     score_init;
} taf_method_info_t;

/*
 * Per SCB data, malloced, to which a pointer is stored in the SCB cubby.
 *
 * This struct is malloc'ed when initializing the scb, but it is not free with the scb!
 * It will be kept in the "pool" list to save the preferences assigned to the STA, and
 * reused if the STA associates again to the AP.
 */
struct taf_scb_cubby {
	struct taf_scb_cubby* next;        /* Next cubby for TAF list. Must be first */
	struct scb *          scb;        /* Back pointer */
	uint32                score[TAF_NUM_LIST_TYPES]; /* UL/DL score for scheduler ordering:
	                                   * constant for EBOS and dynamic otherwise
					   */
#if TAF_ENABLE_SQS_PULL
	uint32                pscore; /* pending score update (not final) */
#endif
	uint32                timestamp[TAF_NUM_LIST_TYPES];  /* release timestamp */
	taf_method_info_t*    method;     /* Which scheduling method owns the SCB */
	taf_scheduler_info_t  info;       /* keep in last place */
};

#if TAF_ENABLE_UL
#define TAF_TYPE_SCORE(c, t)	((c)->score[t])
#else
#define TAF_TYPE_SCORE(c, t)	((c)->score[TAF_TYPE_DL])
#endif

#if TAF_ENABLE_SQS_PULL
#define TAF_SCORE(c, t)		(((t) == TAF_TYPE_DL) ? (TAF_TYPE_SCORE(c, t) + (c)->pscore) : \
				TAF_TYPE_SCORE(c, t))
#else
#define TAF_SCORE(c, t)		TAF_TYPE_SCORE(c, t)
#endif

#define TAF_LIST_SCORE(l)	TAF_SCORE((l)->scb_taf, (l)->type)

typedef struct {
	void*  (*scb_h_fn)       (void *, struct scb *);
	void*  (*tid_h_fn)       (void *, int);
	bool   (*update_fn)      (void *, void *, void *, uint32);
	uint16 (*pktqlen_fn)     (void *, void *, void *, uint32);
	bool   (*release_fn)     (void *, void *, void *, bool, taf_scheduler_public_t *);
	bool   (*open_close_fn)  (void *, int, bool);
} taf_support_funcs_t;

enum {
	TAF_DEFER_RESET_NONE,
	TAF_DEFER_RESET_SOFT,
	TAF_DEFER_RESET_HARD
};

typedef struct taf_trace {
	char   *buf;
	uint32 len;
	uint32 output_len;
	int    index;
	uint32 prev_dump_time;
	uint16 filewr;
#ifndef DONGLEBUILD
	char   *print_buf;
#endif
} taf_trace_t;

/* TAF per interface context */
struct wlc_taf_info {
	wlc_info_t           *wlc;       /* Back link to wlc */
	bool                 enabled;    /* On/Off switch */
	uint8                deferred_reset; /* reset is deferred */
	bool                 bypass;     /* taf bypass (soft taf disable) */
	bool                 bypass_pending;
	uint8                super;
	bool                 super_active;
	bool                 bulk_commit;
	int8                 scheduler_nest;
	uint8                scheduler_inhibit;
	bool                 scheduler_defer;
#ifdef TAF_DEBUG_VERBOSE
	taf_trace_t          trace;
#endif
	uint8                mpdu_dens[TAF_NUM_LIST_TYPES];
	int                  scb_handle; /* Offset for scb cubby */
	taf_scheduler_kind   default_scheduler;
	uint32               default_score;
	taf_scb_cubby_t      *head;      /* Ordered list of associated STAs */
	uint32               scheduler_index;
	void*                scheduler_context[NUM_TAF_SCHEDULERS];
	void*                group_context[NUM_TAF_SCHEDULER_METHOD_GROUPS];
	uint8                group_use_count[NUM_TAF_SCHEDULER_METHOD_GROUPS];
	void**               source_handle_p[TAF_NUM_SCHED_SOURCES];
	uint32               use_sampled_rate_sel;
#if TAF_ENABLE_MU_TX
	uint32               mu;
	taf_traffic_map_t    mu_g_enable_mask[TAF_NUM_MU_TECH_TYPES];
	uint16               mu_admit_count[TAF_NUM_MU_TECH_TYPES];
#if TAF_ENABLE_MU_BOOST
	uint8                mu_boost_factor[TAF_NUM_MU_TECH_TYPES];
	uint8                mu_boost;
#endif /* TAF_ENABLE_MU_BOOST */
	uint8                dlofdma_maxn[D11_REV128_BW_SZ];
#if TAF_ENABLE_UL
	uint8                ulofdma_maxn[D11_REV128_BW_SZ];
	bool                 ul_enabled;  /* UL enable/disable */
#endif /* TAF_ENABLE_UL */
#endif /* TAF_ENABLE_MU_TX */
	taf_support_funcs_t  funcs[TAF_NUM_SCHED_SOURCES];
	taf_traffic_map_t    opened[TAF_NUM_SCHED_SOURCES];
	uint32               force_time;
	uint32               release_count;
	uint16               watchdog_data_stall;
	uint16               watchdog_data_stall_limit;
	taf_schedule_state_t op_state;
	uint32               last_scheduler_run;
#if TAF_ENABLE_SQS_PULL
	uint16               eops_rqst;
	uint16               total_pull_requests;
	uint16               sqs_state_fault;
#ifdef TAF_DBG
	uint32               sqs_state_fault_prev;
	uint32               virtual_complete_time;
	uint32               virtual_complete_time_total;
#endif /* TAF_DBG */
#endif /* TAF_ENABLE_SQS_PULL */
#ifdef TAF_DBG
	uint32               dbgflags;
#endif /* TAF_DBG */
};

#ifdef TAF_DBG
#define TAF_DBGF(t, f)       ((t)->dbgflags & (1 << TAF_DBGF_##f) ? TRUE : FALSE)
enum {
	TAF_NUM_DBGF
};
#else
#define TAF_DBGF(t, f)       (FALSE)
#endif /* TAF_DBG */

#define TAF_METHOD_GROUP_INFO(_m)  (((_m)->taf_info)->group_context[(_m)->group])
#define TAF_CUBBY_TIDINFO(_s, _t)  ((_s)->info.tid_info[(_t)])

#ifdef WLTAF_IAS
#define TAF_TYPE_IS_IAS(type)  (type >= FIRST_IAS_SCHEDULER && type <= LAST_IAS_SCHEDULER)
#else
#define TAF_TYPE_IS_IAS(type)  (FALSE)
#endif

#define SCB_TAF_CUBBY_PTR(info, scb)    ((taf_scb_cubby_t **)(SCB_CUBBY((scb), (info)->scb_handle)))
#define SCB_TAF_CUBBY(taf_info, scb)    (*SCB_TAF_CUBBY_PTR(taf_info, scb))

#define TAF_WLCT(_T)                    ((_T)->wlc)
#define TAF_WLCM(_M)                    TAF_WLCT((_M)->taf_info)
#define TAF_WLCC(_C)                    TAF_WLCM((_C)->method)

#define TAF_ETHERS(s)                   ETHER_TO_MACF((s)->ea)
#define TAF_ETHERC(cub)                 TAF_ETHERS((cub)->scb)

#define TAF_SAFE_ETHERS(s)              CONST_ETHERP_TO_MACF((s) ? &((s)->ea) : &taf_undef_ea)
#define TAF_SAFE_ETHERC(cub)            TAF_SAFE_ETHERS(((cub) ? ((cub)->scb) : NULL))

#if TAF_ENABLE_UL
#define TAF_SRC_TO_TYPE(s_idx)  (TAF_SOURCE_IS_UL(s_idx) ? TAF_TYPE_UL : TAF_TYPE_DL)
#else
#define TAF_SRC_TO_TYPE(s_idx)  (TAF_TYPE_DL)
#endif

#if TAF_ENABLE_UL
#define TAF_DLPRIO(prio)        ((prio) << 1)
#define TAF_ULPRIO(prio)        (TAF_DLPRIO(prio) + 1)
#define TAF_PRIO(prio)          ((prio) >> 1)
#else
#define TAF_DLPRIO(prio)        (prio)
#define TAF_PRIO(prio)          (prio)
#endif /* TAF_ENABLE_UL */

#define TAF_LIST(cubby, type)         &((cubby)->info.list[type])
#define TAF_LISTUL(cubby)             TAF_LIST(cubby, TAF_TYPE_UL)
#define TAF_LISTDL(cubby)             TAF_LIST(cubby, TAF_TYPE_DL)

#define TAF_MPDU_DENS_TO_UNITS(dens)    (TAF_MICROSEC_TO_UNITS(1 << ((dens) - 1)) / 4)

#define WL_TAFM(_M, format, ...)	WL_TAFF(TAF_WLCM(_M), "(%s) "format, \
						TAF_SCHED_NAME(_M), ##__VA_ARGS__)

#define WL_TAFT(_T, ...)		WL_TAFF(TAF_WLCT(_T), ##__VA_ARGS__)
#define WL_TAFW(_W, ...)		WL_TAFF((_W), ##__VA_ARGS__)

#define TAF_MAX_SUSPEND			16
#define TAF_PRIO_SUSPENDED(cub, tid, type)	((cub)->info.suspend[type][tid] > 0)

#if TAF_LOGL1
#define WL_TAFM1(...)           WL_TAFM(__VA_ARGS__)
#define WL_TAFT1(...)           WL_TAFT(__VA_ARGS__)
#else
#define WL_TAFM1(...)           do {} while (0)
#define WL_TAFT1(...)           do {} while (0)
#endif

#if TAF_LOGL2
#define WL_TAFM2(...)           WL_TAFM(__VA_ARGS__)
#define WL_TAFT2(...)           WL_TAFT(__VA_ARGS__)
#else
#define WL_TAFM2(...)           do {} while (0)
#define WL_TAFT2(...)           do {} while (0)
#endif

#if TAF_LOGL3
#define WL_TAFM3(...)           WL_TAFM(__VA_ARGS__)
#define WL_TAFT3(...)           WL_TAFT(__VA_ARGS__)
#else
#define WL_TAFM3(...)           do {} while (0)
#define WL_TAFT3(...)           do {} while (0)
#endif

#if TAF_LOGL4
#define WL_TAFM4(...)           WL_TAFM(__VA_ARGS__)
#define WL_TAFT4(...)           WL_TAFT(__VA_ARGS__)
#else
#define WL_TAFM4(...)           do {} while (0)
#define WL_TAFT4(...)           do {} while (0)
#endif

#define TAF_TIME_FORCE_DEFAULT  0

#define TAF_IOVAR_OK_SET        (BCME_OK + 1)
#define TAF_IOVAR_OK_GET        (BCME_OK + 2)

extern int
wlc_taf_param(const char** cmd, uint32* param, uint32 min, uint32 max, struct bcmstrbuf* b);

extern const char taf_undefined_string[];
extern const char* taf_tx_sources[TAF_NUM_SCHED_SOURCES + 1];

#ifdef TAF_DBG
extern const char* taf_rel_complete_text[NUM_TAF_REL_COMPLETE_TYPES];
extern const char* taf_link_states_text[NUM_TAF_LINKSTATE_TYPES];
extern const char* taf_scb_states_text[NUM_TAF_SCBSTATE_TYPES];
extern const char* taf_txpkt_status_text[TAF_NUM_TXPKT_STATUS_TYPES];
extern const char* taf_rxpkt_status_text[TAF_NUM_RXPKT_STATUS_TYPES];
void wlc_taf_mem_log(wlc_info_t* wlc, const char* func, const char* format, ...);

#define TAF_REL_COMPLETE_TXT(idx)          taf_rel_complete_text[idx]
#define TAF_LINK_STATE_TXT(idx)            taf_link_states_text[idx]
#define TAF_SCB_STATE_TXT(idx)             taf_scb_states_text[idx]
#define TAF_TXPKT_STAT_TEXT(idx)           taf_txpkt_status_text[idx]
#define TAF_RXPKT_STAT_TEXT(idx)           taf_rxpkt_status_text[idx]
#else
extern char * taf_state_idx_debug(int idx);
#define TAF_REL_COMPLETE_TXT(idx)          taf_state_idx_debug(idx)
#define TAF_LINK_STATE_TXT(idx)            taf_state_idx_debug(idx)
#define TAF_SCB_STATE_TXT(idx)             taf_state_idx_debug(idx)
#define TAF_TXPKT_STAT_TEXT(idx)           taf_state_idx_debug(idx)
#define TAF_RXPKT_STAT_TEXT(idx)           taf_state_idx_debug(idx)
#endif /* TAF_DBG */

#if TAF_ENABLE_MU_TX
extern const char* taf_mutech_text[TAF_NUM_TECH_TYPE_NAMES];
#define TAF_TECH_NAME(a)   taf_mutech_text[a]
#else
#define TAF_TECH_NAME(a)   "SU"
#endif /* TAF_ENABLE_MU_TX */

extern void BCMFASTPATH
wlc_taf_sort_list(taf_list_t ** source_head, taf_list_scoring_order_t ordering, uint32 now_time);

extern void wlc_taf_handle_ul_transition(taf_scb_cubby_t* scb_taf, bool ul_enable);
extern taf_tech_type_t taf_mutype_to_tech(int idx);
extern int8 taf_tech_to_mutype(taf_tech_type_t tech);

void BCMFASTPATH wlc_taf_list_append(taf_list_t** head, taf_list_t* item);
void BCMFASTPATH wlc_taf_list_remove(taf_list_t** head, taf_list_t* item);
taf_list_t* BCMFASTPATH wlc_taf_merge_list(taf_list_t** headA, taf_list_t** headB,
	taf_list_scoring_order_t ordering, uint32 now_time);
void wlc_taf_list_delete(taf_scb_cubby_t *scb_taf, taf_list_type_t type);
void BCMFASTPATH wlc_taf_list_insert(taf_list_t* parent, taf_list_t* item);
void BCMFASTPATH taf_close_all_sources(wlc_taf_info_t* taf_info, int tid);
bool BCMFASTPATH taf_flush_source(wlc_taf_info_t* taf_info, taf_source_type_t s_idx, int tid,
	bool reopen);
void BCMFASTPATH taf_open_all_sources(wlc_taf_info_t* taf_info, int tid);

taf_source_type_t wlc_taf_get_source_index(taf_source_index_public_t source);

bool wlc_taf_marked_up(wlc_taf_info_t *taf_info);

#if TAF_ENABLE_SQS_PULL
void wlc_taf_reset_sqs(wlc_taf_info_t *taf_info);
#endif

#define TAF_SCHED_NAME(method)    taf_get_sched_name(method)
#define TAF_DUMP_NAME(method)     taf_get_dump_name(method)
#define TAF_SOURCE_NAME(s_index)  ((s_index >= 0 && s_index < TAF_NUM_SCHED_SOURCES) ? \
					(const char*)taf_tx_sources[s_index] : \
					(const char*)taf_tx_sources[TAF_SOURCE_UNDEFINED])

#ifndef WLTAF_PUSH_SCHED_AVAILABLE
#define taf_push_schedule(_t, _f, _c)   FALSE
#endif

static INLINE BCMFASTPATH uint32 taf_timestamp(wlc_info_t* wlc)
{
	if (wlc->pub->up) {
		uint32 _t_timestamp = wlc_bmac_read_usec_timer(wlc->hw);
		/* '0 time' is used some places to init state logic, small error here to avoid
		 * rare issue
		 */
		return _t_timestamp ? _t_timestamp : 1;
	}
	return 0;
}

static INLINE BCMFASTPATH uint32 taf_nowtime(wlc_info_t* wlc, uint32 * now_time)
{
	if (*now_time == 0) {
		*now_time = taf_timestamp(wlc);
	}
	return *now_time;
}

static INLINE BCMFASTPATH
taf_method_info_t* taf_get_method_info(wlc_taf_info_t* taf_info, taf_scheduler_kind sched)
{
	if ((sched >= TAF_SCHED_FIRST_INDEX) && (sched < TAF_SCHED_LAST_INDEX)) {
		return (taf_method_info_t*)(taf_info->scheduler_context[TAF_SCHED_INDEX(sched)]);
	}
	return NULL;
}

static INLINE BCMFASTPATH const char* taf_get_sched_name(taf_method_info_t* method)
{
	if (method && method->name) {
		return method->name;
	}
	return taf_undefined_string;
}

static INLINE const char* taf_get_dump_name(taf_method_info_t* method)
{
#ifdef TAF_DBG
	if (method && method->dump_name) {
		return method->dump_name;
	}
#endif /* TAF_DBG */
	return taf_undefined_string;
}

static INLINE bool BCMFASTPATH taf_is_bypass(wlc_taf_info_t *taf_info)
{
	bool is_bypass = TRUE;
#if TAF_VALID_CONFIG
	if (taf_info && !taf_info->bypass) {
		return FALSE;
	}
#endif
	return is_bypass;
}

static INLINE bool BCMFASTPATH taf_enabled(wlc_taf_info_t* taf_info)
{
#if TAF_VALID_CONFIG
	bool result = (taf_info != NULL) ? taf_info->enabled : FALSE;
	return result;
#else
	return FALSE;
#endif
}

static INLINE bool BCMFASTPATH taf_in_use(wlc_taf_info_t* taf_info)
{
#if TAF_VALID_CONFIG
	bool result = taf_enabled(taf_info) && !taf_is_bypass(taf_info);
	return result;
#else
	return FALSE;
#endif
}

static INLINE bool BCMFASTPATH taf_scheduler_running(wlc_taf_info_t* taf_info)
{
#if TAF_VALID_CONFIG
	return taf_info->scheduler_nest != 0;
#else
	return FALSE;
#endif
}

static INLINE bool BCMFASTPATH taf_ul_enabled(wlc_taf_info_t* taf_info)
{
#if TAF_VALID_CONFIG && TAF_ENABLE_UL
	bool result = (taf_info != NULL) ? (taf_info->enabled && taf_info->ul_enabled) : FALSE;
	return result;
#else
	return FALSE;
#endif
}

static INLINE taf_list_t** taf_get_list_head_ptr(taf_method_info_t* method)
{
	int index = TAF_SCHED_INDEX(method->type);

	if (taf_scheduler_definitions[index]->list_fn) {
		return taf_scheduler_definitions[index]->list_fn(method);
	}
	return &method->list;
}

static INLINE taf_list_t* taf_get_list_head(taf_method_info_t* method)
{
	taf_list_t** head = taf_get_list_head_ptr(method);

	if (head) {
		return *head;
	}
	return NULL;
}

static INLINE taf_list_t* taf_list_ea_find(taf_list_t* iter, const struct ether_addr*  ea)
{
	taf_scb_cubby_t* scb_taf;

	while (iter) {
		scb_taf = iter->scb_taf;
		if (eacmp(ea, (const char*)&(scb_taf->scb->ea)) == 0) {
			break;
		}
		iter = iter->next;
	}
	return iter ? TAF_LISTDL(iter->scb_taf) : NULL;
}

static INLINE taf_list_t* taf_list_find(taf_scb_cubby_t *scb_taf, taf_list_type_t type)
{
	if (scb_taf) {
		taf_list_t* item = TAF_LIST(scb_taf, type);

		if (item->scb_taf) {
			return item;
		}
	}
	return NULL;
}
static INLINE uint8 BCMFASTPATH taf_nss(taf_scb_cubby_t *scb_taf)
{
	uint32 nss = scb_taf->info.scb_stats.global.rdata.max_nss;
	TAF_ASSERT(nss > 0);
	return nss;
}

#if TAF_ENABLE_MU_TX
static INLINE uint8 BCMFASTPATH taf_nss_mu(taf_scb_cubby_t *scb_taf)
{
	uint32 nss = scb_taf->info.scb_stats.global.rdata.mu.nss;
	if (nss == 0) {
		return taf_nss(scb_taf);
	}
	return nss;
}

static INLINE uint8 BCMFASTPATH taf_mcs_mu(taf_scb_cubby_t *scb_taf)
{
#ifdef TAF_DBG
	return scb_taf->info.scb_stats.global.rdata.mu.mcs;
#else
	return 0;
#endif
}

static INLINE uint8 BCMFASTPATH taf_encode_mu(taf_scb_cubby_t *scb_taf)
{
#ifdef TAF_DBG
	return scb_taf->info.scb_stats.global.rdata.mu.encode;
#else
	return 0;
#endif
}
#endif /* TAF_ENABLE_MU_TX */

#if TAF_ENABLE_MU_BOOST
static INLINE bool BCMFASTPATH taf_mu_boost(taf_scb_cubby_t *scb_taf)
{
	return (bool)(scb_taf->info.scb_stats.global.rdata.ratespec_mask &
		(1 << TAF_RSPEC_MU_DL_BOOST));
}
#else
#define taf_mu_boost(_X)     FALSE
#endif /* TAF_ENABLE_MU_BOOST */

static INLINE uint32 BCMFASTPATH taf_mcs(taf_scb_cubby_t *scb_taf)
{
#ifdef TAF_DBG
	return scb_taf->info.scb_stats.global.rdata.mcs;
#else
	return 0;
#endif
}

static INLINE uint32 BCMFASTPATH taf_encode(taf_scb_cubby_t *scb_taf)
{
#ifdef TAF_DBG
	return scb_taf->info.scb_stats.global.rdata.encode;
#else
	return 0;
#endif
}

static INLINE uint32 BCMFASTPATH taf_bw_idx(taf_scb_cubby_t *scb_taf)
{
	uint32 bw_idx = scb_taf->info.scb_stats.global.rdata.bw_idx;

	TAF_ASSERT(bw_idx < D11_REV128_BW_SZ);
	return bw_idx;
}

static INLINE uint32 BCMFASTPATH taf_bw(taf_scb_cubby_t *scb_taf)
{
	return 20 << taf_bw_idx(scb_taf);
}

static INLINE bool BCMFASTPATH taf_src_type_match(taf_source_type_t s_idx, taf_list_type_t type)
{
#if TAF_ENABLE_UL
	if (TAF_SOURCE_IS_UL(s_idx) && (type != TAF_TYPE_UL)) {
		return FALSE;
	}
	if (TAF_SOURCE_IS_DL(s_idx) && (type != TAF_TYPE_DL)) {
		return FALSE;
	}
#endif /* TAF_ENABLE_UL */
	return TRUE;
}

static INLINE uint8 BCMFASTPATH taf_aggsf(taf_scb_cubby_t *scb_taf, int tid)
{
	wlc_info_t* wlc = TAF_WLCM(scb_taf->method);

	if (CFP_ENAB(wlc->pub) && WLC_AMSDU_IN_AMPDU(wlc, scb_taf->scb, tid)) {
		return wlc_amsdu_scb_aggsf(wlc->ami, scb_taf->scb, tid);
	}
	if (SCB_AMSDU(scb_taf->scb) && AMSDU_TX_AC_ENAB(wlc->ami, tid)) {
		return wlc_amsdu_scb_max_sframes(wlc->ami, scb_taf->scb);
	}
	return 1;
}

static INLINE uint32 BCMFASTPATH taf_mpdus(taf_scb_cubby_t *scb_taf, int tid, uint32 len)
{
	uint8 aggsf = taf_aggsf(scb_taf, tid);

	if (aggsf > 0) {
		return len / aggsf;
	}
	return 0;
}

#ifdef TAF_DEBUG_VERBOSE
#define taf_div64(a, b)     taf_fndiv64((a), (b), __FUNCTION__)

static INLINE uint32 BCMFASTPATH
taf_fndiv64(uint64 numerator, uint32 denominator, const char* fn)
{
	uint64 orig_num = numerator;
	uint32 orig_denom = denominator;
#else
static INLINE uint32 BCMFASTPATH taf_div64(uint64 numerator, uint32 denominator)
{
#endif
	uint32 sc;

	while (numerator > (uint64)UINT32_MAX) {
		numerator = (numerator + 1) >> 1;
		denominator = (denominator + 1) >> 1;
	}

	sc = (uint32)numerator;

	if (sc == 0) {
		return 0;
	}
	if (denominator > 0) {
		return (sc + (denominator >> 1)) / denominator;
	}
#ifdef TAF_DEBUG_VERBOSE
	printf("%s (%s) divide by 0 (0x%08x%08x, 0x%08x, 0x%08x, 0x%08x)\n", __FUNCTION__, fn,
	       (uint32)(orig_num >> 32), (uint32)(orig_num & UINT32_MAX), orig_denom,
	       (uint32)(numerator & UINT32_MAX), denominator);
#endif
	TAF_ASSERT(0);
	return 0;
}

static INLINE int8 taf_bitpos(taf_traffic_map_t value)
{
	uint32 count = 8 * sizeof(value);
	do {
		--count;
		if (value & (1 << count)) {
			return count;
		}
	} while (count);
	return -1;
}

static INLINE taf_method_info_t* taf_get_scb_method(wlc_taf_info_t* taf_info, struct scb * scb)
{
	taf_scb_cubby_t* scb_taf;

	if ((scb_taf = *SCB_TAF_CUBBY_PTR(taf_info, scb))) {
		return scb_taf->method;
	} else {
		taf_scheduler_kind type = TAF_UNDEFINED;
		taf_method_info_t* method = NULL;

#ifdef WLTAF_IAS
		if (scb->flags3 & SCB3_TS_EBOS) {
			type = TAF_EBOS;
		} else if (scb->flags3 & SCB3_TS_ATOS) {
			type = TAF_ATOS;
		} else if (scb->flags3 & SCB3_TS_ATOS2) {
			type = TAF_ATOS2;
		}
#endif
		if (type != TAF_UNDEFINED) {
			method = taf_get_method_info(taf_info, type);
		}
		WL_TAFT1(taf_info, MACF" no cubby -> %s\n", TAF_ETHERS(scb),
			method ? TAF_SCHED_NAME(method) : "unrecoverable");
		return method;
	}
}
#endif /* __wlc_taf_cmn_h__ */
