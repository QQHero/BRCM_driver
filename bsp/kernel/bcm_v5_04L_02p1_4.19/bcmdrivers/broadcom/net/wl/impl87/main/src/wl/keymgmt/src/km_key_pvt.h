/*
 * Private header - wireless security key implementation
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
 * $Id: km_key_pvt.h 794050 2020-12-18 08:18:58Z $
 */

#ifndef _km_key_pvt_h_
#define _km_key_pvt_h_

#include "km.h"
#include "km_key.h"

/* convenience typedefs */
typedef wlc_keymgmt_t keymgmt_t;
/* key_t is taken - sys/types.h */
typedef wlc_key_info_t key_info_t;
typedef wlc_key_algo_t key_algo_t;
typedef wlc_key_id_t key_id_t;
typedef wlc_key_index_t key_index_t;
typedef wlc_key_seq_id_t key_seq_id_t;
typedef wlc_key_data_type_t key_data_type_t;
typedef km_key_dump_type_t key_dump_type_t;

/* algorithm specific initialiazation support */
typedef int (*key_algo_init_t)(wlc_key_t *key);

#define DECL_KEY_ALGO_INIT(algo)  int km_key_##algo##_init(wlc_key_t *key)

DECL_KEY_ALGO_INIT(none); /* no algorithm, null key */
DECL_KEY_ALGO_INIT(wep); /* 40 and 104 bit keys */

DECL_KEY_ALGO_INIT(tkip);
DECL_KEY_ALGO_INIT(tkip_linux);

DECL_KEY_ALGO_INIT(aes); /* 128 and 256 (future) bit keys, incl BIP */
DECL_KEY_ALGO_INIT(nalg);

#ifdef BCMWAPI_WPI
DECL_KEY_ALGO_INIT(wapi);
#endif /* BCMWAPI_WPI */

#undef DECL_KEY_ALGO_INIT

typedef unsigned long long km_uint64_t;
#define KM_SEQ_TO_U64(_seq) ((km_uint64_t)(uint32)ltoh32_ua(_seq) | \
	((km_uint64_t)(ltoh16_ua((const uint8 *)(_seq) + 4)) << 32))

/* a key algorithm is implemented via follwing callbacks */

/* destroy algorithm specific key */
typedef int (*key_algo_destroy_cb_t)(wlc_key_t *key);
/* get algorithm specific key info */
typedef int (*key_algo_get_cb_t)(wlc_key_t *key, uint8 *data,
	size_t data_size, size_t *data_len, key_data_type_t type,
	int instance, bool tx);
/* set algorithm specific key info. if data_len is 0, the key
 * material is cleared
 */
typedef int (*key_algo_set_cb_t)(wlc_key_t *key, const uint8 *data,
	size_t data_len, key_data_type_t type, int instance, bool tx);

/* packet handlers. packet data starts with 802.11 header for mpdu
 * callbacks and ethernet header for msdu callbacks.
 */

/* rx mpdu callback
 * 		decrypt mpdu if required - mgmt, data or qos data
 *		do replay checks
 *		update ivs/rx seq
 *		increment algo specific counters, both for h/w & s/w decr
 *		for h/w decr, only rx iv is updated.
 */

/* key pkt info flags */
enum {
	KEY_PKT_HWDEC			= 0x0001,	/* decrypted by hardware */
	KEY_PKT_PKTC_FIRST		= 0x0002,	/* first pkt in a chain */
	KEY_PKT_PKTC_LAST		= 0x0004,	/* last pkt in chain */
	KEY_PKT_QC_PRESENT		= 0x0008,	/* QC Present */
	KEY_PKT_HTC_PRESENT		= 0x0010	/* HTC Present */
};

typedef int16 key_pkt_info_flags_t;

struct key_pkt_info {
	int status;
	key_pkt_info_flags_t flags;
	uint16 fc;
	uint16 qc;
	int body_len;
};
typedef struct key_pkt_info key_pkt_info_t;

typedef int (*key_rx_cb_t)(wlc_key_t *key, void* pkt, struct dot11_header *hdr,
	uint8* body, int body_len, const key_pkt_info_t *pkt_info);

/* rx msdu callback
 *		do mic check over fully re-assembled 802.3 frame (after conversion from 802.11).
 */
typedef int (*key_rx_msdu_cb_t)(wlc_key_t *key, void* pkt, struct ether_header *hdr,
	uint8* body, int body_len, const key_pkt_info_t *pkt_info);

/* tx mpdu callback
 *	 check key flags for error generation etc. and skip iv update (state)
 *	 otherwise update iv (state)
 *	 set iv in the pkt
 *	 if sw encryption, encrypt frame and add mpdu MIC
 *	 check key flags for error generation etc. generate icv error
 *	 update txh (ac or pre-ac) with h/w algo, hw idx etc. for h/w enc
 */
typedef int (*key_tx_cb_t)(wlc_key_t *key, void *pkt, struct dot11_header *hdr,
	uint8* body, int body_len, d11txhdr_t *txd);

/* tx msdu callback
 *	process 802.3 msdu for mic, if applicable.
 */
typedef int (*key_tx_msdu_cb_t)(wlc_key_t *key, void *pkt, struct ether_header *hdr,
	uint8* body, int body_len, size_t frag_length, uint8 prio);

/* dump */
typedef int (*key_dump_cb_t)(const wlc_key_t *key, struct bcmstrbuf *b);

struct key_algo_callbacks {
	key_algo_destroy_cb_t			destroy;
	key_algo_get_cb_t			get_data;
	key_algo_set_cb_t			set_data;
	key_rx_cb_t				rx_mpdu;
	key_rx_msdu_cb_t			rx_msdu;
	key_tx_cb_t				tx_mpdu;
	key_tx_msdu_cb_t			tx_msdu;
	key_dump_cb_t				dump;
};
typedef struct key_algo_callbacks key_algo_callbacks_t;

#define KEY_ALGO_CB(_ret, _fn, _args) {\
	_ret = (((_fn) != NULL) ? ((*_fn) _args) : BCME_UNSUPPORTED); \
}

struct key_algo_impl {
	const key_algo_callbacks_t	*cb;
	void *ctx;
};
typedef struct key_algo_impl key_algo_impl_t;

/* key definition */
struct wlc_key {
	uint32					magic;
	wlc_info_t				*wlc;
	key_info_t				info;
	wlc_key_expiration_t			exp;
	wlc_key_hw_index_t			hw_idx;
	key_algo_impl_t				algo_impl;
};

#define KEY_MAGIC	0x004B4559
#define KEY_BAD_MAGIC	0xdeaddaed

/* helper macros */
#define KEY_VALID(k) ((k) != NULL &&\
	(k)->magic == KEY_MAGIC &&\
	(k)->wlc != NULL &&\
	(k)->wlc->keymgmt != NULL &&\
	(k)->algo_impl.cb != NULL)

#define KEY_WLC(_key) (_key)->wlc
#define KEY_KM(_key) KEY_WLC(_key)->keymgmt
#define KEY_WLUNIT(_key) WLCWLUNIT(KEY_WLC(_key))
#define KEY_OSH(_key) KEY_WLC(_key)->osh
#define KEY_PUB(_key) KEY_WLC(_key)->pub
#define KEY_CNT(_key) KEY_PUB(_key)->_cnt
#define KEY_MPDU_HDR(_key, _pkt) (struct dot11_header *)\
	PKTDATA(KEY_OSH(_key), (_pkt))
#define KEY_MSDU_HDR(_key, _pkt) (struct ether_header *)\
	PKTDATA(KEY_OSH(_key), (_pkt))
#define KEY_PKT_LEN(_key, _pkt) PKTLEN(KEY_OSH(_key), _pkt)

#define KEY_COREREV(_key) KEY_PUB(_key)->corerev
#define KEY_DEFAULT_BSSCFG(_key) KEY_WLC(_key)->primary_bsscfg

#define KEY_COREREV_LT40(_key) (D11REV_LT(KEY_COREREV(_key), 40))
#define KEY_COREREV_GE40(_key) (D11REV_GE(KEY_COREREV(_key), 40))
#define KEY_COREREV_LT128(_key) (D11REV_LT(KEY_COREREV(_key), 128))
#define KEY_COREREV_GE128(_key) (D11REV_GE(KEY_COREREV(_key), 128))
#define KEY_ERR(args) WL_ERROR(args)
#define KEY_LOG(args) KM_LOG(args)
#define KEY_PRINTF(args) KM_PRINTF(args)
#define KEY_LOG_DECL(stmt) KM_LOG_DECL(stmt)
#define KEY_LOG_DUMP(stmt) KM_LOG_DUMP(stmt)
#define KEY_LOG_DUMP_PKT(_msg, _key, _pkt) KM_LOG_DUMP_PKT(_msg, KEY_WLC(_key), _pkt)

#define KEY_MPDU_LEN(_key, _pkt) PKTLEN(KEY_OSH(_key), _pkt)

/* whether to use sw/enc, dec - see km_util.c:km_needs_hw_key() */
#define KEY_SW_ENC_DEC(_key, _pkt, _rxh) (\
	!((_key)->info.flags & WLC_KEY_FLAG_IN_HW) ||\
	!((D11RXHDR_ACCESS_VAL(_rxh, KEY_PUB(_key)->corerev, \
	RxStatus1)) & RXS_DECATMPT))

#define KEY_ID_BODY_OFFSET KM_PKT_KEY_ID_BODY_OFFSET

#define KEY_WLC_UP(_key) KEY_WLC(_key)->pub->up

/* whether or not tx/rx are enabled */
#define KEY_TX_ENABLED(_key) (KEY_WLC_UP(_key) && \
	WLC_KEY_TX_ALLOWED(&(_key)->info))

#define KEY_RX_ENABLED(_key) (KEY_WLC_UP(_key) && \
	WLC_KEY_RX_ALLOWED(&(_key)->info))

#define KEY_NUM_RX_SEQ(_key) KEY_PUB(_key)->tunables->num_rxivs

#define KEY_RESOLVE_SEQ(_key, _ins, _tx, _ins_begin, _ins_end) {\
	_ins_begin = _ins; \
	_ins_end = (_ins) + 1; \
	if ((_ins) == WLC_KEY_SEQ_ID_ALL) {\
		KM_DBG_ASSERT(!(_tx) && !WLC_KEY_IS_MGMT_GROUP(&(_key)->info)); \
		_ins_begin = 0; \
		_ins_end = (key_seq_id_t)KEY_NUM_RX_SEQ(_key); \
	}\
}

#define KM_KEY_HW_IDX_TO_SLOT(_key, _hw_idx) WLC_KM_HW_IDX_TO_SLOT((_key)->wlc, _hw_idx)

#define KM_KEY_TKIP_TSC_TTAK_GRP_SLOT	7
#define KM_KEY_TKIP_TSC_TTAK_IDX(_key, _hw_idx) \
		((_hw_idx < WLC_KEYMGMT_NUM_GROUP_KEYS) ? (KM_KEY_TKIP_TSC_TTAK_GRP_SLOT) : \
		(_hw_idx - WLC_KEYMGMT_NUM_GROUP_KEYS))
/* support for in-line body/body len computation */

/* Qos header length - data frames only */
#define KM_KEY_FC_TYPE_QOS_DATA(_fc) ((FC_TYPE(_fc) == FC_TYPE_DATA) && \
	FC_SUBTYPE_ANY_QOS(FC_SUBTYPE(_fc)))

#define KM_KEY_PKT_DOT11_QOS_LEN(_fc) (\
	KM_KEY_FC_TYPE_QOS_DATA(_fc) ?  DOT11_QOS_LEN : 0)

#define KM_KEY_FC_TYPE_HTC(_fc) (_fc & FC_ORDER)

/* incremental length if A4 is present - data frames only */
#define KM_KEY_PKT_DOT11_A4_LEN(_fc) ((((_fc) & (FC_TODS|FC_FROMDS)) == \
	(FC_TODS|FC_FROMDS) && \
	(FC_TYPE(_fc) == FC_TYPE_DATA)) ? ETHER_ADDR_LEN : 0);

#endif /* _km_key_pvt_h_ */
