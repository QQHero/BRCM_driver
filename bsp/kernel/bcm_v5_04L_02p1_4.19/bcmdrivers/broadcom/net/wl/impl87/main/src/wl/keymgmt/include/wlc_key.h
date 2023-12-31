/*
 * Public interface to wireless security key operations
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
 * $Id: wlc_key.h 803587 2021-09-30 07:59:23Z $
 */

#ifndef _wlc_key_h_
#define _wlc_key_h_

#include <typedefs.h>
#include <bcmutils.h>
#include <ethernet.h>
#include <wlc_types.h>

union d11rxhdr;
union d11txhdr;
struct d11linkmem_entry;

/* indcies assigned to a key by the keymgmt module */
typedef uint16 wlc_key_index_t;
typedef uint16 wlc_key_hw_index_t;

#define WLC_KEY_INDEX_INVALID 0xffff

/* ID for the key - defined by 802.11, relative to a AP/STA. Used to refer
 * to a specific Group key, for example. Valid values are
 *      0..3 - default WEP keys, Group keys for default BSS
 *      1..2 - IBSS peer Group keys (per SCB), mapped to 0..1 internally
 *      1..2 - STA group keys - mapped to 0..1 for non-default BSS
 *      4..5 - IGTK keys for a BSS
 *      rest are invalid
 * When a Group key is used for TX, a Group key is designated for TX.
 */
enum {
	WLC_KEY_ID_PAIRWISE = 0,
	WLC_KEY_ID_GTK_1 	= 1,
	WLC_KEY_ID_GTK_2 	= 2,
	WLC_KEY_ID_IGTK_1 	= 4,
	WLC_KEY_ID_IGTK_2 	= 5,
	WLC_KEY_ID_INVALID	= 0xff
};
typedef uint8 wlc_key_id_t;

#define WLC_KEY_ID_8021X_WEP 3

/* Sequence (IV) ID - 0 for TX, 0..WLC_KEY_NUM_RX_SEQ-1 for RX */
typedef uint8 wlc_key_seq_id_t;

/* Number of RX (replay) counters and RX counter for MFP */
#if defined(WOWL)
#define WLC_KEY_BASE_RX_SEQ 4
#else
#define WLC_KEY_BASE_RX_SEQ 16
#endif /* WOWL */

#if defined(MFP)
#define WLC_KEY_EXT_RX_SEQ 1
#else
#define WLC_KEY_EXT_RX_SEQ 0
#endif /* MFP */

#define WLC_KEY_NUM_RX_SEQ (WLC_KEY_BASE_RX_SEQ + WLC_KEY_EXT_RX_SEQ)
#define WLC_KEY_MFP_RX_SEQ (WLC_KEY_NUM_RX_SEQ - 1)

/* special key ids */
#define WLC_KEY_SEQ_ID_INVALID 			0xff
#define WLC_KEY_SEQ_ID_ALL 				0x7f

/* Key algorithm - CRYPTO_ALGO_* see wlioctl.h.  */
typedef uint8 wlc_key_algo_t;

/* Hardware Key algorithm - WSEC_ALGO_*, see d11.h.  */
typedef uint8 wlc_key_hw_algo_t;

/* Key flags - Encompasses key usage and state. The former is set during
 * creation and the latter may also be set during use
 */
enum {
	WLC_KEY_FLAG_NONE  			= 0x00000000,
	WLC_KEY_FLAG_VALID			= 0x00000001,
	WLC_KEY_FLAG_TX				= 0x00000002,
	WLC_KEY_FLAG_RX				= 0x00000004,
	WLC_KEY_FLAG_IN_HW			= 0x00000008,

	WLC_KEY_FLAG_DEFAULT_BSS		= 0x00000010, /* belongs to wlc->primary_bsscfg */
	WLC_KEY_FLAG_GROUP			= 0x00000020,
	WLC_KEY_FLAG_MGMT_GROUP			= 0x00000040,	/* IGTK */
	WLC_KEY_FLAG_IBSS			= 0x00000080,	/* key belongs to IBSS */

	WLC_KEY_FLAG_IBSS_PEER_GROUP		= 0x00000100,	/* Group TX key of IBSS peer */
	WLC_KEY_FLAG_HW_MIC			= 0x00000200,	/* hardware mic check */
	WLC_KEY_FLAG_MULTI_BAND			= 0x00000400,	/* shared across bands */
	WLC_KEY_FLAG_AP				= 0x00000800,	/* key belongs to an AP bsscfg */

	WLC_KEY_FLAG_NO_REPLAY_CHECK		= 0x00001000,
	WLC_KEY_FLAG_NO_HW_UPDATE		= 0x00002000,
	WLC_KEY_FLAG_ARM_TX_ENABLED		= 0x00004000,
	WLC_KEY_FLAG_USE_IVTW			= 0x00008000,
	/* 4 bits 0x000F0000 available */

	/* bits 0x00f00000 are designated for algo specific use. */
	WLC_KEY_FLAG_ALGO_1			= 0x00100000,
	WLC_KEY_FLAG_ALGO_2			= 0x00200000,
	WLC_KEY_FLAG_ALGO_3			= 0x00400000,
	WLC_KEY_FLAG_ALGO_4			= 0x00800000,

	/* bits 0x3f000000 for debug - error generation */
	WLC_KEY_FLAG_GEN_MIC_ERR		= 0x01000000,	/* msdu mic error */
	WLC_KEY_FLAG_GEN_REPLAY			= 0x02000000,	/* replay */
	WLC_KEY_FLAG_GEN_ICV_ERR		= 0x04000000,	/* icv error */
	WLC_KEY_FLAG_GEN_MFP_ACT_ERR		= 0x08000000,	/* mfp action error */
	WLC_KEY_FLAG_GEN_MFP_DISASSOC_ERR	= 0x10000000,	/* mfp disassoc error */
	WLC_KEY_FLAG_GEN_MFP_DEAUTH_ERR		= 0x20000000,	/* mfp deauth error */

	WLC_KEY_FLAG_GTK_RESET			= 0x40000000,	/* replay (war) support */
	WLC_KEY_FLAG_DESTROYING			= 0x80000000
};

/* begin algorithm specific flags */

/* WEP specific */
#define WLC_KEY_FLAG_CKIP_KP WLC_KEY_FLAG_ALGO_1
#define WLC_KEY_FLAG_CKIP_MMH WLC_KEY_FLAG_ALGO_2
#define WLC_KEY_FLAG_CKIP_MIC WLC_KEY_FLAG_ALGO_3

/* TKIP specific */
#define	WLC_KEY_FLAG_LINUX_CRYPTO		WLC_KEY_FLAG_ALGO_1

/* AES specific */
#define WLC_KEY_FLAG_AES_MODE_LEGACY WLC_KEY_FLAG_ALGO_1

/* WAPI specific */
#define WLC_KEY_FLAG_WAPI_HAS_PREV_KEY WLC_KEY_FLAG_ALGO_1
#define WLC_KEY_FLAG_WAPI_PREV_KEY WLC_KEY_FLAG_ALGO_2
/* end algorithm specific flags */

typedef uint32 wlc_key_flags_t;

/* Default keys are synonymous with Group keys */
#define WLC_KEY_FLAG_DEFAULT WLC_KEY_FLAG_GROUP

/* Primary key is synonymous with the TX key */
#define WLC_KEY_FLAG_PRIMARY WLC_KEY_FLAG_TX

/* externally settable flags */
#if defined(BCMDBG) || defined(MFP_TEST)
#define WLC_KEY_DBG_SETTABLE_FLAGS (\
	WLC_KEY_FLAG_GEN_MIC_ERR|\
	WLC_KEY_FLAG_GEN_REPLAY|\
	WLC_KEY_FLAG_GEN_ICV_ERR|\
	WLC_KEY_FLAG_GEN_MFP_ACT_ERR|\
	WLC_KEY_FLAG_GEN_MFP_DISASSOC_ERR|\
	WLC_KEY_FLAG_GEN_MFP_DEAUTH_ERR)
#else
#define WLC_KEY_DBG_SETTABLE_FLAGS 0
#endif /* BCMDBG || MFP_TEST */

#define WLC_KEY_SETTABLE_FLAGS WLC_KEY_DBG_SETTABLE_FLAGS

/* Key expiration - absolute time in seconds when key expires. Time reference
 * is the same as wlc->pub->now
 */
typedef int32 wlc_key_expiration_t;

/* Key information used for query, creation and update  - key data, sequence,
 * and expiration are treated separately.  Lengths are specified in bytes.
 * IV and ICV lengths are per PDU (WAPI/TKIP MIC dealt with separately i.e.
 * IV length for TKIP is same as that of WEP). Key length may include
 * additional data - such as MIC keys for TKIP, and interface accepts
 * hex encoded keys, but always returns binary keys
 */
struct wlc_key_info {
	wlc_key_index_t		key_idx;	/* Key index assigned by keymgmt */
	struct ether_addr 	addr;		/* Peer addr if pairwise, {0} otherwise */
	wlc_key_flags_t		flags;
	wlc_key_algo_t  	algo;
	wlc_key_id_t		key_id;
	wlc_key_hw_algo_t	hw_algo;	/* hw algo - see d11.h */
	uint8 				key_len;	/* algorithm key (data) length */
	uint8 				iv_len;
	uint8 				icv_len;
};

/* Convenience macros */
#define WLC_KEY_ID_IS_DEFAULT(_id) (((wlc_key_id_t)(_id)) < \
	WLC_KEYMGMT_NUM_GROUP_KEYS)

#define WLC_KEY_ID_IS_STA_GROUP(_id) ((_id) == WLC_KEY_ID_GTK_1 || \
	(_id) == WLC_KEY_ID_GTK_2)
#define WLC_KEY_ID_OTHER_STA_GROUP(_id) ((_id) == WLC_KEY_ID_GTK_1 ?\
	WLC_KEY_ID_GTK_2 : WLC_KEY_ID_GTK_1)

#define WLC_KEY_IN_HW(ki) (((ki)->flags & WLC_KEY_FLAG_IN_HW) != 0)
#define WLC_KEY_MIC_IN_HW(ki) (((ki)->flags & WLC_KEY_FLAG_HW_MIC) != 0)
#define WLC_KEY_SW_ONLY(ki) (((ki)->flags & WLC_KEY_FLAG_IN_HW) == 0)

#define WLC_KEY_IS_PRIMARY(ki) (((ki)->flags & WLC_KEY_FLAG_PRIMARY) != 0)
#define WLC_KEY_IS_PAIRWISE(ki) ((((ki)->flags & \
	(WLC_KEY_FLAG_GROUP | WLC_KEY_FLAG_MGMT_GROUP)) == 0) && \
	!(WLC_KEY_IS_IBSS_PEER_GROUP(ki)))

#define WLC_KEY_IS_LINUX_CRYPTO(ki) ((((ki)->flags & WLC_KEY_FLAG_LINUX_CRYPTO) != 0) && \
	((ki)->algo == CRYPTO_ALGO_TKIP))
#define WLC_KEY_IS_MGMT_GROUP(ki) (((ki)->flags & WLC_KEY_FLAG_MGMT_GROUP) != 0)
#define WLC_KEY_IS_IGTK(ki) WLC_KEY_IS_MGMT_GROUP(ki)
#define WLC_KEY_IS_GROUP(ki) (((ki)->flags & WLC_KEY_FLAG_GROUP) != 0)
#define WLC_KEY_IS_AP(ki) (((ki)->flags & WLC_KEY_FLAG_AP) != 0)
#define WLC_KEY_CHECK_REPLAY(ki) (((ki)->flags & WLC_KEY_FLAG_NO_REPLAY_CHECK) == 0)

#define WLC_KEY_ID_IS_IGTK(_id) (\
	((wlc_key_id_t)(_id)) == WLC_KEY_ID_IGTK_1 || \
	((wlc_key_id_t)(_id)) == WLC_KEY_ID_IGTK_2)

#define WLC_KEY_IS_IBSS(ki) (((ki)->flags & WLC_KEY_FLAG_IBSS) != 0)
#define WLC_KEY_IS_DEFAULT_BSS(ki) (((ki)->flags & WLC_KEY_FLAG_DEFAULT_BSS) != 0)

#define WLC_KEY_IS_IBSS_GROUP(ki) (\
	((ki)->flags & (WLC_KEY_FLAG_GROUP | WLC_KEY_FLAG_IBSS)) == \
	(WLC_KEY_FLAG_GROUP | WLC_KEY_FLAG_IBSS))

#define WLC_KEY_IS_IBSS_PEER_GROUP(ki) (\
	((ki)->flags & (WLC_KEY_FLAG_IBSS_PEER_GROUP | WLC_KEY_FLAG_IBSS)) == \
	(WLC_KEY_FLAG_IBSS_PEER_GROUP | WLC_KEY_FLAG_IBSS))

/* Is this a SMS4 key in s/w only? */
#define WLC_KEY_SMS4_SW_ONLY(ki) ((ki)->algo == CRYPTO_ALGO_SMS4 && \
	WLC_KEY_SW_ONLY(ki))

#ifdef BCMWAPI_WPI
#define WLC_KEY_SMS4_HAS_PREV_KEY(ki) ((ki)->algo == CRYPTO_ALGO_SMS4 && \
	((ki)->flags & WLC_KEY_FLAG_WAPI_HAS_PREV_KEY) != 0)

#define WLC_KEY_SMS4_PREV_KEY(ki) ((ki)->algo == CRYPTO_ALGO_SMS4 && \
	((ki)->flags & WLC_KEY_FLAG_WAPI_PREV_KEY) != 0)
#else
#define WLC_KEY_SMS4_HAS_PREV_KEY(ki) FALSE
#define WLC_KEY_SMS4_PREV_KEY(ki) FALSE
#endif /* BCMWAPI_WPI */

#define WLC_KEY_ID_IS_VALID(_id) WLC_KEY_ID_IS_DEFAULT(_id) || \
	WLC_KEY_ID_IS_IGTK(_id)
#define WLC_KEY_ID_IS_PAIRWISE(id) ((id) == WLC_KEY_ID_PAIRWISE)

#define WLC_KEY_RX_ALLOWED(_ki) (((_ki)->flags & WLC_KEY_FLAG_RX) != 0)
#define WLC_KEY_TX_ALLOWED(_ki) (((_ki)->flags & WLC_KEY_FLAG_TX) != 0)

/* tkip mic applies for TKIP keys that have hw mic configured provided they
 * are not fragmented or management frames (ckip), otherwise they apply to
 * last two fragments - either or both of which may contain the mic.
 */
#define WLC_KEY_FRAG_HAS_TKIP_MIC(_pkt, _ki, _frag, _nfrags) (\
	(_ki)->algo == CRYPTO_ALGO_TKIP && (\
		(WLC_KEY_MIC_IN_HW(_ki) && ((_nfrags) == 1) &&\
		!WLPKTFLAG_MFP(WLPKTTAG(_pkt))) || (((_frag) + 1) >= (_nfrags))))

#define WLC_KEY_ALGO_IS_BIPXX(_algo) (\
	(_algo) == CRYPTO_ALGO_BIP ||\
	(_algo) == CRYPTO_ALGO_BIP_CMAC256 ||\
	(_algo) == CRYPTO_ALGO_BIP_GMAC ||\
	(_algo) == CRYPTO_ALGO_BIP_GMAC256)

#define WLC_KEY_MMIC_IE_LEN(_ki) (WLC_KEY_ALGO_IS_BIPXX((_ki)->algo) ? \
	(OFFSETOF(mmic_ie_t, mic) + (_ki)->icv_len) : 0)

#define WLC_KEY_IS_AES_LEGACY(_ki) (((_ki)->algo == CRYPTO_ALGO_AES_CCM) &&\
	((_ki)->flags & WLC_KEY_FLAG_AES_MODE_LEGACY))

#define WLC_KEY_ALLOWS_AMPDU(_ki) ((_ki) == NULL || (_ki)->algo  == CRYPTO_ALGO_OFF || \
	 (_ki)->algo  == CRYPTO_ALGO_AES_CCM || (_ki)->algo  == CRYPTO_ALGO_AES_CCM256 || \
	(_ki)->algo  == CRYPTO_ALGO_AES_GCM || (_ki)->algo  == CRYPTO_ALGO_AES_GCM256 || \
	(_ki)->algo  == CRYPTO_ALGO_SMS4)

#define WLC_KEY_ALGO_IS_AES_CCMXX(_ki) (((_ki)->algo == CRYPTO_ALGO_AES_CCM) || \
	((_ki)->algo == CRYPTO_ALGO_AES_CCM256))
#define WLC_KEY_ALGO_IS_AES_GCMXX(_ki) (((_ki)->algo == CRYPTO_ALGO_AES_GCM) || \
	((_ki)->algo == CRYPTO_ALGO_AES_GCM256))

#define WLC_KEY_ALLOWS_PKTC(_ki, _bsscfg) (\
	((_ki)->algo == CRYPTO_ALGO_OFF && (_bsscfg)->WPA_auth == WPA_AUTH_DISABLED) || \
	(WLC_KEY_IN_HW(_ki) && (WLC_KEY_ALGO_IS_AES_CCMXX(_ki) || \
		WLC_KEY_ALGO_IS_AES_GCMXX(_ki))))

#define WLC_KEY_ARM_TX_ENABLED(_ki) (((_ki)->flags & WLC_KEY_FLAG_ARM_TX_ENABLED) != 0)
#define WLC_KEY_NO_HW_UPDATE(_ki) (((_ki)->flags & WLC_KEY_FLAG_NO_HW_UPDATE) != 0)

#define WLC_KEY_ALGO_HAS_MIC(_algo) ((_algo) == CRYPTO_ALGO_TKIP || (_algo) == CRYPTO_ALGO_SMS4)

#define WLC_KEY_USE_IVTW(_ki) (((_ki)->flags & WLC_KEY_FLAG_USE_IVTW) != 0)

/* Data type selector for a key. This is to accommodate additional data
 * that may be get/set for some algorithms.
 */
enum {
	WLC_KEY_DATA_TYPE_KEY 				= 1,
	WLC_KEY_DATA_TYPE_SEQ 				= 2,
	WLC_KEY_DATA_TYPE_TKHASH_P1			= 3,	/* TKIP Phase 1 Key */
	WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS	= 4,
	WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS		= 5,
	WLC_KEY_DATA_TYPE_FLAGS				= 6,
	WLC_KEY_DATA_TYPE_MIC_KEY			= 7,	/* incl. WAPI integrity key */
	WLC_KEY_DATA_TYPE_TKIP_TK			= 8
};
typedef int wlc_key_data_type_t;

#define WLC_KEY_DATA_TYPE_MIC_KEY_AUTH_TX WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS
#define WLC_KEY_DATA_TYPE_MIC_KEY_AUTH_RX WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS
#define WLC_KEY_DATA_TYPE_MIC_KEY_SUP_TX WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS
#define WLC_KEY_DATA_TYPE_MIC_KEY_SUP_RX WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS
#ifdef WLWSEC
/* Interface definition */

/* Key creation and destruction is done automatically and is internal
 * to Key Management module.
 */

/* Get information about a key. Key data and TX/RX sequence is
 * handled separately. If key is NULL info is cleared - which
 * corresponds to CRYPTO_ALGO_OFF
 */
void wlc_key_get_info(const wlc_key_t *key, wlc_key_info_t *info);

/* Prepare the MPDU for TX. On entry, packet data points to 802.11 header and
 * packet length denotes the end-of-body to be protected.  On exit
 *	- Frame control WEP/Privacy bit it set
 *	- IV is inserted after header
 *	- For data frames and unicast manamgement frames
 *		ICV is inserted after body
 * 		Packet length now includes the ICV
 *		If msdu mic applies, mic is added to final frame(s)
 *	- For multicast mgmt frames
 *		body includes space for mgmt mic ie
 *		ie immediately preceding the body is updated
 */
int wlc_key_prep_tx_mpdu(wlc_key_t *key, void *pkt, union d11txhdr *txd);

/* Fill keymgmt related parts of (rev128) Link entry. */
int wlc_key_fill_link_entry(wlc_key_t *key, scb_t *scb, struct d11linkmem_entry *link_entry);

/* Prepare the MSDU for TX for adding MIC, if required. On entry, packet
 * data points to 802.3 header and packet length denotes the end-of-body
 * to be protected.  The packet may be the first of a chain. Fragment length,
 * if non-zero and pkt priority are used for SDU processing - e.g. MIC computattion
 * On exit
 *   - key state is modified, so MIC could be inserted into one or more MPDUs.
 */
int wlc_key_prep_tx_msdu(wlc_key_t *key, void *pkt, size_t frag_length, uint8 prio);

/* Receive a MPDU. On entry, packet data points to 802.11 header. On exit
 *   - ICV is stripped off the packet if decrypt is attempted (s/w or h/w)
 *   - IV is not removed.
 *   - FC_WEP is not cleared
 */
int wlc_key_rx_mpdu(wlc_key_t *key, void *pkt, union d11rxhdr *rxh);

/* Check MSDU MIC, if required. On entry, packet data points to 802.3
 * header after reassembly, and packet length * denotes the end-of-body including
 * the MIC. On exit
 *   - MIC is removed, if MIC check is successful
 *   - Packet length now excludes the MIC
 */
int wlc_key_rx_msdu(wlc_key_t *key, void *pkt, union d11rxhdr *rxh);

/* Get TX/RX sequence for the key in little-endian (LE) order.
 * ltohXX_ua macros can be used to convert the buffer as needed.
 * seq_id represents the sequence (replay)  counter ID
 *    - For TX only 0 is valid. Current implementation may ignore this
 *      parameter for TX.
 *    - For RX, 0..NUMRXIVS-1 (non-WEP, Pairwise), 0 for IGTK and GTK.
 * Returns
 *    - length copied if there is sufficient space
 *    - '-length', where length is the length needed, but seq is copied
 *      truncated to specified buffer size.
 */
int wlc_key_get_seq(wlc_key_t *key, uint8 *buf, size_t buf_len,
	wlc_key_seq_id_t seq_id, bool tx);

/* Set sequence/IV for RX/TX using input in LE order. If the sequence
 * length is invalid for the key an error is returned. NULL for
 * sequence with length 0 can be used to reset the sequence.
 * seq_id is reserved, and rx seq update updates all the ivs.
 */
int wlc_key_set_seq(wlc_key_t *key, const uint8 *seq, size_t seq_len,
	wlc_key_seq_id_t seq_id, bool tx);

/* advance key seq if seq is greater */
int wlc_key_advance_seq(wlc_key_t *key, const uint8 *seq, size_t seq_len,
	wlc_key_seq_id_t seq_id, bool tx);

/* check if key seq is less than seq */
bool
wlc_key_seq_less(wlc_key_t *key, const uint8* seq, size_t seq_len,
	wlc_key_seq_id_t seq_id, bool tx);

/* Set key data. For TKIP, MIC keys are part of the key data
 * <pairwise 0..15><from-DS MIC key 16..23><to-DS MIC key 24..31>
 * Similarly for WAPI <MIC Key  16..31>. If data_len is non-zero
 * and the operation is successful, the key, if it is group or management
 * group key, becomes the tx (primary) key for the BSS. The key may also be
 * disabled by setting data_len to 0 which is equilvalent to switching the
 * algorithm to CRYPTO_ALGO_OFF. when key data changes, all the
 * tx/rx seq are also reset.
 */
int wlc_key_set_data(wlc_key_t *key, wlc_key_algo_t algo,
	const uint8 *data, size_t data_len);

/* Reset data and any other state for the key. Algorithm is preserved. */
int wlc_key_reset(wlc_key_t *key);

/* Set key flags (for debugging). Not all flags are settable this way.
 * all settable flags are replaced i.e. the caller is responsible for
 * handling clearing the individual bits.
 */
int wlc_key_set_flags(wlc_key_t *key, wlc_key_flags_t flags);

/* Get key data - if BCME_BUFTOOSHORT is returned, the required length is
 * returned in data_len (if not NULL)
 */
int wlc_key_get_data(wlc_key_t *key, uint8* data, size_t data_size,
	size_t *data_len);

/* Extended access to key data - to support tkip etc. When setting sequence
 * the counter/IV ID (wlc_key_seq_id_t) is specified in the instance
 */
int wlc_key_get_data_ex(wlc_key_t *key, uint8* data, size_t data_size,
	size_t *data_len, wlc_key_data_type_t type, int instance, bool tx);

/* Set key data using the type of data; extensible interface, but
 * more complex to use.
 */
int wlc_key_set_data_ex(wlc_key_t *key, uint8* data, size_t data_len,
	wlc_key_data_type_t type, int instance, bool tx);

/* Make a key sequence from packet number, returns sequence length. Used
 * to support typical 6 byte sequence numbers in 802.11
 */
size_t wlc_key_pn_to_seq(uint8 *seq, size_t seq_size, uint16 lo, uint32 hi);

/* Get expiration time in seconds for a key (absolute, using the same
 * reference as wlc->pub->now). A Value of 0 indicates no expiration.
 */
wlc_key_expiration_t wlc_key_get_expiration(wlc_key_t *key);

/* Set expiration time for a key. Returns old expiration time */
wlc_key_expiration_t wlc_key_set_expiration(wlc_key_t *key,
	wlc_key_expiration_t exp);

#if defined(BCMDBG) || defined(WLMSG_WSEC)
/* get names for debug */
const char *wlc_key_get_data_type_name(wlc_key_data_type_t data_type);
#endif

wlc_key_hw_index_t wlc_key_get_hw_idx(wlc_key_t *key);
#else
#define wlc_key_get_info(key, info) do {} while (0)
#define wlc_key_prep_tx_msdu(key, pkt, frag_length, prio) NULL
#define wlc_key_prep_tx_mpdu(key, pkt, txd) NULL
#define wlc_key_rx_mpdu(key, pkt, rxh) NULL
#define wlc_key_rx_msdu(key, pkt, rxh) NULL
#define wlc_key_get_data(key, data, data_size, data_len) NULL
#define wlc_key_set_data(key, algo, data, data_len) NULL
#define wlc_key_rx_msdu(key, pkt, rxh) NULL
#endif /* WLWSEC */

typedef uint32 wlc_key_algo_mask_t;

#endif /* _wlc_key_h_ */
