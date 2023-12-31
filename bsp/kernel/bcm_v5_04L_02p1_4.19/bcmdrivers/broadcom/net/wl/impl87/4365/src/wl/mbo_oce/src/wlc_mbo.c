/*
 * MBO implementation for
 * Broadcom 802.11bang Networking Device Driver
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
 * $Id$
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 */

/**
 * @file
 * @brief
 * This file implements a part of WFA MBO features
 */
#include <wlc_cfg.h>
#include <typedefs.h>
#include <osl.h>
#include <wl_dbg.h>
#include <wlioctl.h>
#include <wlc_bsscfg.h>

#include <wlc_pub.h>
#include <wlc.h>
#include <wlc_types.h>
#include <wlc_ie_mgmt_types.h>
#include <wlc_ie_mgmt.h>
#include <wlc_ie_mgmt_vs.h>
#include <mbo.h>
#include <mbo_oce.h>
#include <wlc_mbo.h>
#include <wlc_wnm.h>
#include <wlc_ie_mgmt_ft.h>
#include "wlc_mbo_oce_priv.h"
#include <bcmendian.h>
#ifndef WLWNM
#error "WNM is required for MBO"
#endif /* WNM */

#include <wlc_scb.h>

/* macro to control alloc and dealloc for sta only entity */
#define MBO_BSSCFG_STA(cfg) (BSSCFG_INFRA_STA(cfg) && ((cfg)->type == BSSCFG_TYPE_GENERIC))

#define MBO_NP_CHAN_ATTR_OPCLASS_LEN		1 /* 1 byte */
#define MBO_NP_CHAN_ATTR_PREF_LEN		1 /* 1 byte */
#define MBO_NP_CHAN_ATTR_REASON_LEN		1 /* 1 byte */
#define MBO_NP_ATTR_FIXED_LEN	(MBO_NP_CHAN_ATTR_OPCLASS_LEN + \
						MBO_NP_CHAN_ATTR_PREF_LEN + \
						MBO_NP_CHAN_ATTR_REASON_LEN)

#define MBO_NP_SUBELEMENT_OPCLASS_OFFSET		5
#define MBO_OUI_LEN					3
#define MBO_OUI_TYPE_LEN				1
#define MBO_NP_SUBELEMENT_FIXED_LEN	 (MBO_NP_ATTR_FIXED_LEN + \
						MBO_OUI_LEN + MBO_OUI_TYPE_LEN)

#define MBO_NP_SUBELEMENT_CHAN_OUI_TYPE			2
#define MBO_NP_SUBELEMENT_CELL_OUI_TYPE			3

#define MBO_ATTRIBUTE_ID_OFFSET				0
#define MBO_ATTRIBUTE_LEN_OFFSET			1
#define MBO_ATTRIBUTE_OUI_OFFSET			2
#define MBO_WNM_SUBELEMENT_ID_AND_LEN			2
#define MBO_EMPTY_SUBELEMENT_LIST_LEN			4
#define MBO_NO_CHAN_LIST_SUBELEMENT_LEN			6
#define MBO_WNM_NOTIFICATION_MIN_SUBELEMENT_LEN		2
#define MBO_EMPTY_CELL_SUBELEMENT_LEN			0
#define MBO_WNM_CELL_SUBELEMENT_DATA_OFFSET		6

#define MBO_STA_MARKED_CHANNEL_NON_OPERABLE		0
#define MBO_STA_MARKED_CHANNEL_NON_PREFERABLE		1
#define MBO_STA_MARKED_CHANNEL_RANGE_RESERVED		2 /* 2- 254 */
#define MBO_STA_MARKED_CHANNEL_PREFERABLE		255

#define MBO_BSSCFG_ENABLE               0x01
#define MBO_BSSCFG_IS_ENABLED(mbc)      (mbc->flags & MBO_BSSCFG_ENABLE)
typedef struct np_chan_entry {
	uint8 chan;
	uint8 ref_cnt;
} np_chan_entry_t;

typedef struct mbo_np_chan_list mbo_np_chan_list_t;
struct mbo_np_chan_list {
	mbo_np_chan_list_t *next;
	uint8 opclass;
	uint8 pref;
	uint8 reason;
	uint8 list_len;
	uint8 list[16];	/* Max channels present in opclass eg: E-4 opclass - 81 */
};

typedef struct mbo_chan_pref_list mbo_chan_pref_list_t;
struct mbo_chan_pref_list {
	mbo_chan_pref_list_t *next;
	uint8	opclass;
	uint8	chan;
	uint8	pref;
	uint8	reason;
};

typedef struct wlc_mbo_data {
	/* configured cellular data capability of device */
	wlc_mbo_oce_ie_build_hndl_t build_ie_hndl;
	wlc_mbo_oce_ie_parse_hndl_t parse_ie_hndl;
} wlc_mbo_data_t;

struct wlc_mbo_info {
	wlc_info_t *wlc;
	/* shared data */
	wlc_mbo_data_t *mbo_data;
	int      cfgh;    /* mbo bsscfg cubby handle */
	int      scbh;    /* mbo scb cubby handle */
	uint8	 fwd_gas_rqst_to_app;
};

typedef struct wlc_mbo_bsscfg_cubby {
	uint8 mbo_assoc_disallowed;
	uint8 mbo_ap_attr;
	uint8 flags;
} wlc_mbo_bsscfg_cubby_t;

typedef struct wlc_mbo_scb_cubby {
	/* flags for associated bss capability etc. */
	uint8 flags[1];
	/* configured non pref chan list for this bss */
	mbo_np_chan_list_t *np_chan_list_head;
	/* bss transition reject reason */
	uint8 bsstrans_reject_reason;
} wlc_mbo_scb_cubby_t;

#define MBO_SCB_CUBBY_LOC(mbo, scb) ((wlc_mbo_scb_cubby_t **)SCB_CUBBY((scb), (mbo)->scbh))
#define MBO_SCB_CUBBY(mbo, scb) (*(MBO_SCB_CUBBY_LOC(mbo, scb)))

#define MBO_BSSCFG_CUBBY_LOC(mbo, cfg) ((wlc_mbo_bsscfg_cubby_t **)BSSCFG_CUBBY(cfg, (mbo)->cfgh))
#define MBO_BSSCFG_CUBBY(mbo, cfg) ((wlc_mbo_bsscfg_cubby_t*)BSSCFG_CUBBY(cfg, ((mbo)->cfgh)))
#define MBO_CUBBY_CFG_SIZE  sizeof(wlc_mbo_bsscfg_cubby_t)

#define SUBCMD_TBL_SZ(_cmd_tbl)  (sizeof(_cmd_tbl)/sizeof(*_cmd_tbl))

static const bcm_iovar_t mbo_iovars[] = {
	{"mbo", 0, 0, IOVT_BUFFER, 0},
	{NULL, 0, 0, 0, 0}
};

static void wlc_mbo_watchdog(void *ctx);
static int wlc_mbo_wlc_up(void *ctx);
static int wlc_mbo_wlc_down(void *ctx);

static int
wlc_mbo_doiovar(void *hdl, const bcm_iovar_t *vi, uint32 actionid, const char *name,
	void *params, uint p_len, void *arg, int len, int val_size, struct wlc_if *wlcif);

static int wlc_mbo_iov_get_mbo_ap_attr(void *hndl, const uint8 *ibuf, size_t ilen, uint8 *obuf,
	size_t *olen, wlc_bsscfg_t *bsscfg);

static int wlc_mbo_iov_get_ap_attr_assoc_disallowed(void *hndl, const uint8 *ibuf, size_t ilen,
	uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg);

static int wlc_mbo_iov_set_ap_attr_assoc_disallowed(void *hndl, const uint8 *ibuf, size_t ilen,
	uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg);

static int wlc_mbo_iov_set_fwd_gas_rqst_to_app(void *hndl, const uint8 *ibuf, size_t ilen,
	uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg);

static int wlc_mbo_iov_get_fwd_gas_rqst_to_app(void *hndl, const uint8 *ibuf, size_t ilen,
	uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg);

static int wlc_mbo_iov_get_mbo_ap(void *hndl, const uint8 *ibuf, size_t ilen, uint8 *obuf,
		size_t *olen, wlc_bsscfg_t *bsscfg);
static int wlc_mbo_iov_set_mbo_ap(void *hndl, const uint8 *ibuf, size_t ilen, uint8 *obuf,
		size_t *olen, wlc_bsscfg_t *bsscfg);

static uint wlc_mbo_calc_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_mbo_build_ie(void *ctx, wlc_iem_build_data_t *data);

static int wlc_mbo_parse_ie(void *ctx, wlc_iem_parse_data_t *data);

static int wlc_mbo_scb_init(void *ctx, struct scb *scb);
static void wlc_mbo_scb_deinit(void *ctx, struct scb *scb);

static int wlc_mbo_process_scb_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb,
	uint8* ibuf, bool attr);
static int wlc_mbo_update_scb_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb,
	uint8 *ibuf, bool attr,	uint8 chan_list_len);

static int wlc_mbo_free_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb);

static uint8 wlc_mbo_get_count_chan_from_non_pref_list(uint8* ibuf, bool attr);

static void wlc_mbo_bss_updn(void *ctx, bsscfg_up_down_event_data_t *evt);
#define MAX_SET_GET_DATA_CAP_SIZE  8

static const bcm_iov_cmd_info_t mbo_sub_cmds[] = {
	{WL_MBO_CMD_AP_ATTRIBUTE, BCM_IOV_CMD_FLAG_NONE,
	0, BCM_XTLV_OPTION_ALIGN32,
	NULL, wlc_mbo_iov_get_mbo_ap_attr,
	NULL, 0,
	MAX_SET_GET_DATA_CAP_SIZE, MAX_SET_GET_DATA_CAP_SIZE, 0, 0
	},
	{WL_MBO_CMD_AP_ASSOC_DISALLOWED, BCM_IOV_CMD_FLAG_NONE,
	0, BCM_XTLV_OPTION_ALIGN32,
	NULL, wlc_mbo_iov_get_ap_attr_assoc_disallowed,
	wlc_mbo_iov_set_ap_attr_assoc_disallowed, 0,
	MAX_SET_GET_DATA_CAP_SIZE, MAX_SET_GET_DATA_CAP_SIZE, 0, 0
	},
	{WL_MBO_CMD_AP_FWD_GAS_RQST_TO_APP, BCM_IOV_CMD_FLAG_NONE,
	0, BCM_XTLV_OPTION_ALIGN32,
	NULL, wlc_mbo_iov_get_fwd_gas_rqst_to_app,
	wlc_mbo_iov_set_fwd_gas_rqst_to_app, 0,
	MAX_SET_GET_DATA_CAP_SIZE, MAX_SET_GET_DATA_CAP_SIZE, 0, 0
	},
	{WL_MBO_CMD_AP_ENAB, BCM_IOV_CMD_FLAG_NONE,
	0, BCM_XTLV_OPTION_ALIGN32,
	NULL, wlc_mbo_iov_get_mbo_ap,
	wlc_mbo_iov_set_mbo_ap, 0,
	MAX_SET_GET_DATA_CAP_SIZE, MAX_SET_GET_DATA_CAP_SIZE, 0, 0
	},
};

static int
wlc_mbo_doiovar(void *hdl, const bcm_iovar_t *vi, uint32 actionid, const char *name,
	void *params, uint p_len, void *arg, int len, int val_size, struct wlc_if *wlcif)
{
	int err = BCME_OK;
	uint16 version;
	uint8 *p_buf = NULL;
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hdl;
	wlc_bsscfg_t *bsscfg;
	bcm_iov_buf_t *p_resp;
	bcm_iov_buf_t *p_cmd;
	const bcm_iov_cmd_info_t *cmd_arg = NULL;
	const uint8* cmd_data;
	uint16 i;
	uint16 cmd_len, min_len = 0, max_len = 0;
	void* result;
	int res_len;
	int avail;
	uint8 *res = NULL;
	bool cmd_id_match = FALSE;
	int index;
	uint16 cmd_id;

	ASSERT(params != NULL);
	ASSERT(mbo != NULL);
	ASSERT(arg != NULL);

	if (p_len < OFFSETOF(bcm_iov_buf_t, len)) {
		err = BCME_BUFTOOSHORT;
		goto done;
	}

	/*
	 * Get version
	 */
	memcpy(&version, params, sizeof(version));
	version = ltoh16_ua(&version);

	/* update bsscfg w/provided interface context */
	bsscfg = wlc_bsscfg_find_by_wlcif(mbo->wlc, wlcif);
	ASSERT(bsscfg != NULL);

	/* update wlcif pointer */
	if (wlcif == NULL)
		wlcif = bsscfg->wlcif;
	ASSERT(wlcif != NULL);
	/*
	 * Input buffer does not overlap with output. Just point
	 * p_buf to input buffer
	 */
	p_buf = params;
	p_cmd = (bcm_iov_buf_t *)p_buf;
	p_resp = (bcm_iov_buf_t *)arg;
	cmd_id = ltoh16(p_cmd->id);

	cmd_data = ((uint8 *)p_cmd + OFFSETOF(bcm_iov_buf_t, data));
	cmd_len = ltoh16(p_cmd->len);
	result = arg;
	avail = (int)len;
	/*
	 * Set up resp buffer and its max len and ensure space for resp hdr
	 */
	res = (uint8 *)result + OFFSETOF(bcm_iov_buf_t, data);
	if (avail < (res - (uint8 *)result)) {
		err = BCME_BUFTOOSHORT;
		goto done; /* note: no space to return command status */
	}
	res_len = avail - (int)(res - (uint8 *)result);

	for (i = 0; i < (size_t)SUBCMD_TBL_SZ(mbo_sub_cmds); i++) {
		if (cmd_id == mbo_sub_cmds[i].cmd) {
			index = i;
			cmd_id_match = TRUE;
			cmd_arg = &mbo_sub_cmds[index];
			break;
		}
	}

	if (!cmd_id_match) {
		err = BCME_NOTFOUND;
		goto done;
	}
	/*
	 * Validate lengths.
	 */
	if (IOV_ISSET(actionid)) {
		min_len = cmd_arg->min_len_set;
		max_len = cmd_arg->max_len_set;
	} else {
		min_len = cmd_arg->min_len_get;
		max_len = cmd_arg->max_len_get;
	}

	if (cmd_len < min_len) {
		err = BCME_BADLEN;
		goto have_result;
	}

	/* process only supported len and allow future extension */
	cmd_len = MIN(cmd_len, max_len);
	/*
	 * Dispatch get/set
	 */
	if (IOV_ISSET(actionid)) {
		if (cmd_arg->set_h) {
			err = (cmd_arg->set_h)(mbo, cmd_data, (size_t)cmd_len,
				res, (size_t *)&res_len, bsscfg);
		} else {
			err = BCME_UNSUPPORTED;
		}
	} else {
		if (cmd_arg->get_h) {
			err = (cmd_arg->get_h)(mbo, cmd_data, (size_t)cmd_len,
				res, (size_t *)&res_len, bsscfg);
		} else {
			err = BCME_UNSUPPORTED;
		}
	}

	p_resp->version = htol16(version);

have_result:
	/* upon error, return only status - return values/buffer not deterministic */
	if (err != BCME_OK) {
		res_len = 0;
	}
	/*
	 * Done with non-batched command. handlers fill the data
	 * framework fills the version, id and len. Non batched
	 * commands get return status in bcm_iovar_t
	 */
	bcm_iov_buf_t *p_subcmd = (bcm_iov_buf_t *)result;
	p_subcmd->version = htol16((uint16)version);
	p_subcmd->id = htol16(cmd_id);
	if (cmd_arg->flags & BCM_IOV_CMD_FLAG_HDR_IN_LEN) {
		res_len += OFFSETOF(bcm_iov_buf_t, data);
	}
	p_subcmd->len = htol16((uint16)res_len);

done:
	return err;
}

wlc_mbo_info_t *
BCMATTACHFN(wlc_mbo_attach)(wlc_info_t *wlc)
{
	wlc_mbo_info_t *mbo = NULL;
	int ret = BCME_OK;

	uint16 ie_parse_fstbmp = FT2BMP(FC_ASSOC_REQ) |	FT2BMP(FC_REASSOC_REQ);
	uint16 ie_build_fstbmp = FT2BMP(FC_BEACON) | FT2BMP(FC_PROBE_RESP)
				| FT2BMP(FC_ASSOC_RESP) | FT2BMP(FC_REASSOC_RESP);

	mbo = (wlc_mbo_info_t *)MALLOCZ(wlc->osh, sizeof(*mbo));
	if (mbo == NULL) {
		WL_ERROR(("wl%d: %s:out of mem. alloced %u bytes\n",
			wlc->pub->unit, __FUNCTION__,  MALLOCED(wlc->osh)));
		goto fail;
	}
	mbo->wlc = wlc;

	/* register module */
	ret = wlc_module_register(wlc->pub, mbo_iovars, "MBO", mbo, wlc_mbo_doiovar,
		wlc_mbo_watchdog, wlc_mbo_wlc_up, wlc_mbo_wlc_down);
	if (ret != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_module_register() failed\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	mbo->cfgh = wlc_bsscfg_cubby_reserve_ext(wlc, sizeof(wlc_mbo_bsscfg_cubby_t *), NULL,
			NULL, NULL, MBO_CUBBY_CFG_SIZE,	NULL, NULL, ((void*)mbo));
	if (mbo->cfgh < 0) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_cubby_reserve() failed\n",
			wlc->pub->unit, __FUNCTION__));
			goto fail;
	}

	/* reserve cubby in the scb container for per-scb private data */
	if ((mbo->scbh = wlc_scb_cubby_reserve(wlc, sizeof(wlc_mbo_scb_cubby_t *),
	                wlc_mbo_scb_init, wlc_mbo_scb_deinit, NULL /* for dump routine */,
	                (void *)mbo)) < 0) {
		WL_ERROR(("wl%d: %s: wlc_scb_cubby_reserve() failed\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	/* bcn/prbrsp */
	if (wlc_iem_vs_add_build_fn_mft(wlc->iemi, ie_build_fstbmp, WLC_IEM_VS_IE_PRIO_MBO_OCE,
	      wlc_mbo_calc_ie_len, wlc_mbo_build_ie, mbo) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_add_build_fn failed, mbo in bcn\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	ret = wlc_iem_vs_add_parse_fn_mft(wlc->iemi, ie_parse_fstbmp,
		WLC_IEM_VS_IE_PRIO_MBO_OCE, wlc_mbo_parse_ie, mbo);
	if (ret != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_vs_add_parse_fn failed\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;

	}

	/* bsscfg up/down callback */
	if (wlc_bsscfg_updown_register(wlc, wlc_mbo_bss_updn, mbo) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_updown_register() failed\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	wlc->pub->cmn->_mbo = TRUE;
	return mbo;
fail:

	wlc_mbo_detach(mbo);
	return NULL;
}

void
BCMATTACHFN(wlc_mbo_detach)(wlc_mbo_info_t* mbo)
{
	ASSERT(mbo != NULL);
	/* unregister for bss up/down */
	wlc_bsscfg_updown_unregister(mbo->wlc, wlc_mbo_bss_updn, (void *)mbo);
	mbo->wlc->pub->cmn->_mbo = FALSE;
	wlc_module_unregister(mbo->wlc->pub, "mbo", mbo);
	MFREE(mbo->wlc->osh, mbo, sizeof(*mbo));
	mbo = NULL;
}

static void
wlc_mbo_watchdog(void *ctx)
{

}

static int
wlc_mbo_wlc_up(void *ctx)
{
	return BCME_OK;
}

static int
wlc_mbo_wlc_down(void *ctx)
{
	return BCME_OK;
}

/* "wl mbo cell_data_cap" handler */
static int
wlc_mbo_iov_get_mbo_ap_attr(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint16 xtlv_size = 0;
	uint16 nbytes = 0;
	wlc_mbo_bsscfg_cubby_t *mbc;

	uint16 buflen = 0;
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	if (!mbo || !MBO_ENAB(mbo->wlc->pub)) {
		return BCME_UNSUPPORTED;
	}
	mbc = MBO_BSSCFG_CUBBY(mbo, bsscfg);
	ASSERT(mbc);
	nbytes = sizeof(mbc->mbo_ap_attr);

	xtlv_size = bcm_xtlv_size_for_data(sizeof(uint8), BCM_XTLV_OPTION_ALIGN32);
	if (xtlv_size > *olen) {
		WL_MBO_ERR(("wl%d: %s: short buffer length %d expected %u\n",
			mbo->wlc->pub->unit, __FUNCTION__, (int)(*olen), xtlv_size));
		*olen = 0;
		ret = BCME_BUFTOOSHORT;
		goto fail;
	}
	buflen = *olen;

	ret = bcm_pack_xtlv_entry(&obuf, &buflen, WL_MBO_XTLV_AP_ATTR,
			nbytes, &mbc->mbo_ap_attr, BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: packing xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	*olen = *olen - buflen;

fail:
	return ret;
}

/* "wl mbo cell_data_cap" handler */
static int
wlc_mbo_iov_get_ap_attr_assoc_disallowed(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint16 xtlv_size = 0;
	wlc_mbo_bsscfg_cubby_t *mbc;

	uint16 buflen = 0;
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	if (!mbo || !MBO_ENAB(mbo->wlc->pub)) {
		return BCME_UNSUPPORTED;
	}
	mbc = MBO_BSSCFG_CUBBY(mbo, bsscfg);
	ASSERT(mbc);

	xtlv_size = bcm_xtlv_size_for_data(sizeof(mbc->mbo_assoc_disallowed),
		BCM_XTLV_OPTION_ALIGN32);
	if (xtlv_size > *olen) {
		WL_MBO_ERR(("wl%d: %s: short buffer length %d expected %u\n",
			mbo->wlc->pub->unit, __FUNCTION__, (int)(*olen), xtlv_size));
		*olen = 0;
		ret = BCME_BUFTOOSHORT;
		goto fail;
	}
	buflen = *olen;

	ret = bcm_pack_xtlv_entry(&obuf, &buflen, WL_MBO_XTLV_AP_ASSOC_DISALLOWED,
			sizeof(mbc->mbo_assoc_disallowed), &mbc->mbo_assoc_disallowed,
			BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: packing xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	*olen = *olen - buflen;

fail:
	return ret;
}

/* "wl mbo cell_data_cap <>" handler */
static int
wlc_mbo_iov_set_ap_attr_assoc_disallowed(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint8 data = 0;
	wlc_mbo_bsscfg_cubby_t *mbc;
	uint16 nbytes = 0;

	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;
	if (!mbo || !MBO_ENAB(mbo->wlc->pub)) {
		return BCME_UNSUPPORTED;
	}
	mbc = MBO_BSSCFG_CUBBY(mbo, bsscfg);
	ASSERT(mbc);
	nbytes = sizeof(mbc->mbo_assoc_disallowed);

	ret = bcm_unpack_xtlv_entry((uint8 **)&ibuf, WL_MBO_XTLV_AP_ASSOC_DISALLOWED, nbytes,
		&data, BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: unpacking xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	wlc_mbo_update_attr_assoc_disallowed(bsscfg, data);
fail:
	return ret;
}

static int
wlc_mbo_iov_get_fwd_gas_rqst_to_app(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint16 xtlv_size = 0;

	uint16 buflen = 0;
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	if (!mbo || !MBO_ENAB(mbo->wlc->pub)) {
		return BCME_UNSUPPORTED;
	}
	xtlv_size = bcm_xtlv_size_for_data(sizeof(mbo->fwd_gas_rqst_to_app),
		BCM_XTLV_OPTION_ALIGN32);
	if (xtlv_size > *olen) {
		WL_MBO_ERR(("wl%d: %s: short buffer length %d expected %u\n",
			mbo->wlc->pub->unit, __FUNCTION__, (int)(*olen), xtlv_size));
		*olen = 0;
		ret = BCME_BUFTOOSHORT;
		goto fail;
	}
	buflen = *olen;

	ret = bcm_pack_xtlv_entry(&obuf, &buflen, WL_MBO_XTLV_AP_FWD_GAS_RQST_TO_APP,
			sizeof(mbo->fwd_gas_rqst_to_app), &mbo->fwd_gas_rqst_to_app,
			BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: packing xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	*olen = *olen - buflen;

fail:
	return ret;
}

static int
wlc_mbo_iov_set_fwd_gas_rqst_to_app(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint8 data = 0;
	uint16 nbytes = 0;

	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	nbytes = sizeof(mbo->fwd_gas_rqst_to_app);

	ret = bcm_unpack_xtlv_entry((uint8 **)&ibuf, WL_MBO_XTLV_AP_FWD_GAS_RQST_TO_APP, nbytes,
		&data, BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: unpacking xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	mbo->fwd_gas_rqst_to_app = data;
fail:
	return ret;
}

/* wl mbo ap fetaure enab/disable */
static int
wlc_mbo_iov_get_mbo_ap(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	uint16 xtlv_size = 0;

	uint16 buflen = 0;
	uint8 enab = FALSE;
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	if ((mbo->wlc) && (mbo->wlc->pub)) {
		enab = mbo->wlc->pub->cmn->_mbo;
	}
	xtlv_size = bcm_xtlv_size_for_data(sizeof(enab),
		BCM_XTLV_OPTION_ALIGN32);
	if (xtlv_size > *olen) {
		WL_MBO_ERR(("wl%d: %s: short buffer length %d expected %u\n",
			mbo->wlc->pub->unit, __FUNCTION__, (int)(*olen), xtlv_size));
		*olen = 0;
		ret = BCME_BUFTOOSHORT;
		goto fail;
	}
	buflen = *olen;

	ret = bcm_pack_xtlv_entry(&obuf, &buflen, WL_MBO_XTLV_AP_ENAB,
			sizeof(enab), &enab,
			BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: packing xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	*olen = *olen - buflen;

fail:
	return ret;
}

/* "wl mbo cell_data_cap <>" handler */
static int
wlc_mbo_iov_set_mbo_ap(void *hndl, const uint8 *ibuf,
	size_t ilen, uint8 *obuf, size_t *olen, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	int32 data = -1;
	uint16 nbytes = 0;
	uint8 *pibuf = NULL;
	uint8 *ptr = NULL;

	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)hndl;

	ASSERT(mbo);
	nbytes = sizeof(data);
	pibuf = (uint8*)MALLOCZ(mbo->wlc->osh, ilen);
	if (pibuf == NULL) {
		WL_ERROR(("wl%d: %s:out of mem. alloced %lu bytes\n",
			mbo->wlc->pub->unit, __FUNCTION__, (unsigned long)ilen));
		goto fail;
	}
	ptr = pibuf;
	memcpy(pibuf, ibuf, ilen);
	ret = bcm_unpack_xtlv_entry((uint8 **)&pibuf, WL_MBO_XTLV_AP_ENAB, nbytes,
		(uint8*)&data, BCM_XTLV_OPTION_ALIGN32);
	if (ret != BCME_OK) {
		WL_MBO_ERR(("wl%d: %s: unpacking xtlv failed\n",
			mbo->wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	mbo->wlc->pub->cmn->_mbo = (uint8)data ? TRUE: FALSE;
fail:
	if (ptr) {
		MFREE(mbo->wlc->osh, ptr, ilen);
	}
	return ret;
}
/* Build MBO IE with MBO AP attribute and Association Disallowed
 * attribute, This IE present in Beacon, Probe Response and
 * (Re)Association response frames
 */
static int
wlc_mbo_build_ie(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t *)ctx;
	wlc_mbo_bsscfg_cubby_t *mbc;
	uint8 *cp = NULL;
	wifi_mbo_oce_ie_t *ie_hdr = NULL;
	uint total_len = 0; /* len to be put in IE header */
	int len = 0;
	wifi_mbo_ap_cap_ind_attr_t *ap_attr = NULL;
	wifi_mbo_assoc_disallowed_attr_t *ap_assoc_attr = NULL;

	ASSERT(mbo != NULL);

	if (!MBO_ENAB(mbo->wlc->pub)) {
		return BCME_OK;
	}
	cp = data->buf;
	ie_hdr = (wifi_mbo_oce_ie_t *)cp;

	mbc = MBO_BSSCFG_CUBBY(mbo, data->cfg);
	if (!mbc) {
		return BCME_OK;
	}

	/* add interworking element for beacon and probe response */
	if ((WL11U_ENAB(mbo->wlc)) && ((data->ft == FC_BEACON) || (data->ft == FC_PROBE_RESP))) {
		wlc_bsscfg_set_ext_cap(data->cfg, DOT11_EXT_CAP_IW, TRUE);
	}
	/* fill in MBO-OCE IE header */
	ie_hdr->id = MBO_OCE_IE_ID;
	memcpy(ie_hdr->oui, MBO_OCE_OUI, WFA_OUI_LEN);
	ie_hdr->oui_type = MBO_OCE_OUI_TYPE;
	len = MBO_OCE_IE_HDR_SIZE;
	cp += len;
	total_len = MBO_OCE_IE_NO_ATTR_LEN;

	/* fill in MBO AP attribute */
	ap_attr = (wifi_mbo_ap_cap_ind_attr_t *)cp;
	ap_attr->id = MBO_ATTR_MBO_AP_CAPABILITY;
	ap_attr->len = sizeof(*ap_attr) - MBO_ATTR_HDR_LEN;
	ap_attr->cap_ind = mbc->mbo_ap_attr;

	cp += sizeof(*ap_attr);
	total_len += sizeof(*ap_attr);
	/* MBO standard possible values:
	 * 1: Unspecified reason
	 * 2: Max number of sta association reahed
	 * 3: Air interface is overloaded
	 * 4: Authentication server overloaded
	 * 5: Insufficient RSSI
	 * 6 - 255: Reserved
	 */
	if ((mbc->mbo_assoc_disallowed >= MBO_ASSOC_DISALLOWED_REASON_UNSPECIFIED) &&
		(mbc->mbo_assoc_disallowed <= MBO_ASSOC_DISALLOWED_REASON_INSUFFICIENT_RSSI)) {
		/* fill in MBO AP association disallowed */
		ap_assoc_attr = (wifi_mbo_assoc_disallowed_attr_t *)cp;
		ap_assoc_attr->id = MBO_ATTR_ASSOC_DISALLOWED;
		ap_assoc_attr->len = sizeof(*ap_assoc_attr) - MBO_ATTR_HDR_LEN;
		ap_assoc_attr->reason_code = mbc->mbo_assoc_disallowed;

		cp += sizeof(*ap_assoc_attr);
		total_len += sizeof(*ap_assoc_attr);
	}

	ie_hdr->len = total_len;
	return BCME_OK;
}

static uint
wlc_mbo_calc_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t *)ctx;
	wlc_mbo_bsscfg_cubby_t *mbc;
	uint total_len = 0;
	ASSERT(mbo);

	mbc = MBO_BSSCFG_CUBBY(mbo, data->cfg);
	if (!mbc) {
		return 0;
	}
	total_len = MBO_OCE_IE_HDR_SIZE;
	if (!MBO_ENAB(mbo->wlc->pub)) {
		return 0;
	} else {
		/* add for MBO ap attribute indicate IE */
		total_len += sizeof(wifi_mbo_ap_cap_ind_attr_t);
		if ((mbc->mbo_assoc_disallowed >= MBO_ASSOC_DISALLOWED_REASON_UNSPECIFIED) &&
			(mbc->mbo_assoc_disallowed <=
				MBO_ASSOC_DISALLOWED_REASON_INSUFFICIENT_RSSI)) {
			/* add for MBO ap assoc disallowed attribute */
			total_len += sizeof(wifi_mbo_assoc_disallowed_attr_t);
		}
	}
	return total_len;
}

/* Parse MBO IE in ASSOC/Reassoc/Probe request frame for Non preferred chan and cellular
 * capability MBO attributes. There can be more than one Non preferred chan attribute in
 * MBO IE in frame.
 * MBO IE with Non preferred chan attribute frees earlier scb's non pref chan list
 * and allocate new one with the contents provided in Non preferred chan subelement.
 * For Cellular data capability attribute update scb's cell capability with new
 * value provided in Cellular data subelement
 */
static int
wlc_mbo_parse_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t *)ctx;
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	uint8 *cp = NULL;
	uint8 *ptr = NULL;
	uint8 *local = NULL;
	wlc_mbo_scb_cubby_t *mbo_scb = NULL;
	struct scb *scb =  ftpparm->assocreq.scb;
	int ret = BCME_OK;

	if (!(data->ie)) {
		return BCME_OK;
	}

	/* validate minimum IE length */
	if (data->ie_len <= MBO_OCE_IE_HDR_SIZE) {
		return BCME_OK;
	}
	ASSERT(data->ie);

	ASSERT(scb);

	mbo_scb = MBO_SCB_CUBBY(mbo, scb);

	cp = data->ie + MBO_OCE_IE_HDR_SIZE;
	local = cp;

	switch (data->ft) {
		case FC_ASSOC_REQ:
		case FC_REASSOC_REQ: {
			/* WFA MBO standard(3.2) Tech spec
			 * On (re)association an MBO STA shall indicate non-preferred channels by
			 * including zero (0) or more Non-preferred Channel Report Attributes
			 * in the (Re)Association Request frame. Every time an  MBO STA informs
			 * an MBO AP of its channel and band preferences, either via the inclusion
			 * of at least one Non-preferred Channel Report Attribute in a
			 * (Re)Association frame or the inclusion of at least one Non-preferred
			 * Channel Report Subelement in a WNM-Notification Request frame, the MBO
			 * AP shall replace all (if any) previously stored information
			 * (irrespective of Operating Class) with the most current information
			 * as indicated by the MBO STA.
			 */
			wlc_mbo_free_np_chan_list(mbo, mbo_scb);

			/* check for Non preferred channel report attribute */
			while ((ptr = (uint8*)bcm_parse_tlvs(local,
					(data->ie_len - MBO_OCE_IE_HDR_SIZE),
					MBO_ATTR_NON_PREF_CHAN_REPORT)) != NULL) {

				bcm_tlv_t *elt = (bcm_tlv_t*)ptr;
				ret = wlc_mbo_process_scb_np_chan_list(mbo, mbo_scb, ptr, TRUE);
				if (ret != BCME_OK) {
					return ret;
				}
				local = local + (elt->len + TLV_HDR_LEN);
			}
			break;
		}
	}

	/* Update SCB3 flag for MBO capable Station */
	scb->flags3 |= SCB3_MBO;

	return BCME_OK;
}

/* Process Non preferred chan attribute or subelemnt from (Re)Association request or WNM
 * notification request frame from STA.
 */
static int
wlc_mbo_process_scb_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb, uint8* ibuf,
	bool attr)
{
	uint8 chan_list_len = 0;
	int ret = BCME_OK;

	chan_list_len = wlc_mbo_get_count_chan_from_non_pref_list(ibuf, attr);
	if (!chan_list_len) {
		/* It is possible to receive Attribute or subelement
		 * having zero chan list, skip further processing
		 */
		return BCME_OK;
	}
#ifdef BCMDBG
	/* Include ID and len of element for debug */
	prhex(" MBO Non preferred element data  ==>", ibuf, (ibuf[MBO_ATTRIBUTE_LEN_OFFSET] + 2));
#endif	/* BCMDBG */

	ret = wlc_mbo_update_scb_np_chan_list(mbo, mbo_scb, ibuf, attr, chan_list_len);
	return ret;
}

/* Returns the number of channels in list
 * Argument - attr:
 *	True for Non preferred chan report attribute
 *	False for Non preferred chan subelement in WNM Notification request frame
 */
static uint8
wlc_mbo_get_count_chan_from_non_pref_list(uint8* ibuf, bool attr)
{
	uint8 *ptr = ibuf;
	uint8 len = 0;
	uint8 empty_list_len = 0;

	ptr++;
	len = *ptr;

	if (attr) {
		/* if len == 0, No chan list if present in this Attribute
		 * return
		 */
		if (len != 0) {
			return (len - MBO_NP_ATTR_FIXED_LEN);
		}
	} else {
		/* possible values in length byte:
		 * 0x04 or variable
		 * 0x04 - No chan list provided, empty element return with 0 len
		 */
		if (ibuf[MBO_ATTRIBUTE_LEN_OFFSET] > MBO_EMPTY_SUBELEMENT_LIST_LEN) {
			return (len - MBO_NP_SUBELEMENT_FIXED_LEN);
		}
	}
	return empty_list_len;
}

static int
wlc_mbo_free_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb)
{
	mbo_np_chan_list_t *cur = NULL;
	mbo_np_chan_list_t *list = NULL;

	if (mbo_scb != NULL) {
		list = mbo_scb->np_chan_list_head;
		cur = list;
		while (cur != NULL) {
			list = cur->next;
			cur->next = NULL;
			MFREE(mbo->wlc->osh, cur, sizeof(*cur));
			cur = list;
		}
		mbo_scb->np_chan_list_head = NULL;
	}
	return BCME_OK;
}

static int
wlc_mbo_update_scb_np_chan_list(wlc_mbo_info_t* mbo, wlc_mbo_scb_cubby_t *mbo_scb, uint8 *ibuf,
	bool attr, uint8 chan_list_len)
{
	mbo_np_chan_list_t *cur, *list;
	uint8 *ptr = ibuf;
	uint8 i;

	list = (mbo_np_chan_list_t*)MALLOCZ(mbo->wlc->osh, sizeof(mbo_np_chan_list_t));
	if (!list) {
		WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
			mbo->wlc->pub->unit, __FUNCTION__, MALLOCED(mbo->wlc->osh)));
		return BCME_NOMEM;
	}

	cur = mbo_scb->np_chan_list_head;

	if (!cur) {
		mbo_scb->np_chan_list_head = list;
	} else {
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = list;
	}

	/*  Non preferred chan attribute report information
	 *  --------------------------------------------------------------------------------
	 *  attribute_id | length | operating class | channel list | preference | reason code
	 *   1 byte        1 byte    1 byte              variable     1 byte       1 byte
	 *  --------------------------------------------------------------------------------
	 */
	/*  Non preferred subelement information (*number of bytes)
	 *  --------------------------------------------------------------------------------
	 *    id(1) | length(1) | OUI(3)  | OUI TYPE(1) | operating class(1) | channel list(VAR)
	 *	| preference(1) | reason code(1)
	 *  --------------------------------------------------------------------------------
	 */
	ptr++;
	if (attr) {
		/* Update Opclass from Non preferred chan Attribute */
		ptr++;
		list->opclass = *ptr;
	} else {
		/* update Opclass from Non preferred chan Subelement */
		ptr = ptr + MBO_NP_SUBELEMENT_OPCLASS_OFFSET;
		list->opclass = *ptr;
	}

	ptr++;
	if (chan_list_len >= 0) {
		/* Load chan list for the corresponding opclass */
		for (i = 0; i < chan_list_len; i++) {
			list->list[i] = *ptr++;
		}
	}

	list->pref = *ptr++;
	list->reason = *ptr;

	return BCME_OK;
}

static int
wlc_mbo_scb_init(void *ctx, struct scb *scb)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t *)ctx;
	wlc_info_t *wlc = mbo->wlc;
	wlc_mbo_scb_cubby_t **pmbo_scb = MBO_SCB_CUBBY_LOC(mbo, scb);
	wlc_mbo_scb_cubby_t *mbo_scb;

	if ((mbo_scb = MALLOCZ(wlc->osh, sizeof(wlc_mbo_scb_cubby_t))) == NULL) {
		WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
			wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
		return BCME_NOMEM;
	}

	*pmbo_scb = mbo_scb;

	return BCME_OK;
}

static void
wlc_mbo_scb_deinit(void *ctx, struct scb *scb)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t *)ctx;
	wlc_info_t *wlc = mbo->wlc;
	wlc_mbo_scb_cubby_t **pmbo_scb = MBO_SCB_CUBBY_LOC(mbo, scb);
	wlc_mbo_scb_cubby_t *mbo_scb = *pmbo_scb;

	if (mbo_scb != NULL) {
		wlc_mbo_free_np_chan_list(mbo, mbo_scb);
		MFREE(wlc->osh, mbo_scb, sizeof(wlc_mbo_scb_cubby_t));
	}
	*pmbo_scb = NULL;
}

/* Parse Non preferred chan subelement or cellular data capability subelement in
 * WNM notification request frame from STA. For every WNM notification request
 * Frame with Non preferred chan subelement free earlier scb's non pref chan list
 * and allocate new one with the contents provided in Non preferred chan subelement.
 * For Cellular data capability subelement update scb's cell capability with new
 * value provided in Cellular data subelement
 */
int
wlc_mbo_process_wnm_notif(wlc_info_t* wlc, struct scb *scb, uint8 *body, int body_len)
{
	dot11_wnm_notif_req_t *wnm_notif;
	wlc_mbo_info_t *mbo = NULL;
	wlc_mbo_scb_cubby_t *mbo_scb = NULL;
	uint8* ptr = NULL;
	uint8 bytes_rd = 0, nbytes = 0, len = 0;
	bool do_scb_chan_list_cleanup = TRUE;
	uint8 oui_type_offset = 0;
	int ret = BCME_OK;

	mbo = wlc->mbo;
	ASSERT(mbo);
	mbo_scb = MBO_SCB_CUBBY(mbo, scb);

	ASSERT(mbo_scb);

	if (body_len < DOT11_WNM_NOTIF_REQ_LEN) {
		WL_ERROR(("WNM notification request frame with invalid length\n"));
		return BCME_BADARG;
	}

	wnm_notif = (dot11_wnm_notif_req_t*)body;
	ptr = &(wnm_notif->data[MBO_ATTRIBUTE_ID_OFFSET]);
	oui_type_offset = MBO_ATTRIBUTE_OUI_OFFSET + WFA_OUI_LEN;
	/* body_len is whole notification request frame len, update body_len to
	 * have only MBO specific number of bytes
	 */
	body_len = body_len - DOT11_WNM_NOTIF_REQ_LEN;

	/* parse number of subelements if present */
	while ((body_len - bytes_rd) > MBO_WNM_NOTIFICATION_MIN_SUBELEMENT_LEN) {
		/* Confirm WFA OUI tag 0X50,0X6F,0X9A */
		if (memcmp(&ptr[MBO_ATTRIBUTE_OUI_OFFSET], WFA_OUI, WFA_OUI_LEN) != 0) {
			return BCME_IE_NOTFOUND;
		}
		if (ptr[oui_type_offset] == MBO_NP_SUBELEMENT_CHAN_OUI_TYPE) {
			/* WFA MBO standard(3.2) Tech spec
			 * Every time an MBO STA informs an MBO AP of its channel and
			 * band preferences, either via the inclusion of at least one
			 * Non-preferred Channel Report Attribute in a (Re)Association
			 * frame or the inclusion of at least one Non-preferred Channel
			 * Report Subelement in a WNM-Notification Request frame, the MBO
			 * AP shall replace all (if any) previously stored information
			 * (irrespective of Operating Class) with the most current
			 * information as indicated by the MBO STA
			 */
			if (do_scb_chan_list_cleanup) {
				wlc_mbo_free_np_chan_list(mbo, mbo_scb);
				do_scb_chan_list_cleanup = FALSE;
			}
			ret = wlc_mbo_process_scb_np_chan_list(mbo, mbo_scb, ptr, FALSE);
			if (ret != BCME_OK) {
				return ret;
			}
		}
		/* continue to look out for more subelements if any in WNM notification frame */
		len = ptr[MBO_ATTRIBUTE_LEN_OFFSET];
		nbytes = (len + MBO_WNM_SUBELEMENT_ID_AND_LEN);
		ptr += nbytes;
		bytes_rd += nbytes;
	}
	return BCME_OK;
}
/* Update 2G, 5G band capability of scb */
void
wlc_mbo_update_scb_band_cap(wlc_info_t* wlc, struct scb* scb, uint8* data)
{
	wlc_mbo_info_t* mbo = NULL;
	wlc_mbo_scb_cubby_t *mbo_scb = NULL;

	mbo = wlc->mbo;
	ASSERT(mbo);

	mbo_scb = MBO_SCB_CUBBY(mbo, scb);
	mbo_scb->flags[0] = data[0];
}

int
wlc_mbo_process_bsstrans_resp(wlc_info_t* wlc, struct scb* scb, uint8* body, int body_len)
{
	dot11_bsstrans_resp_t *rsp = NULL;
	uint16 data_len = 0;
	uint8 mbotype = WFA_OUI_TYPE_MBO;
	wifi_mbo_ie_t* mbo_ie;
	wlc_mbo_info_t *mbo = NULL;
	wifi_mbo_trans_reason_code_attr_t* reject_attr = NULL;
	wlc_mbo_scb_cubby_t *mbo_scb = NULL;
	uint8 attr_len = 0;

	mbo = wlc->mbo;
	ASSERT(mbo);
	mbo_scb = MBO_SCB_CUBBY(mbo, scb);

	ASSERT(mbo_scb);
	/* Parse and store MBO related data */
	rsp = (dot11_bsstrans_resp_t *)body;
	data_len = body_len - DOT11_BSSTRANS_RESP_LEN;
	mbo_ie = (wifi_mbo_ie_t *) bcm_find_vendor_ie(rsp->data, data_len,
		MBO_OUI, &mbotype, 1);

	if (!mbo_ie) {
		return BCME_NOTFOUND;
	}
	if (mbo_ie && (mbo_ie->len > (MBO_OUI_LEN + sizeof(mbo_ie->oui_type)))) {
		attr_len = mbo_ie->len - (MBO_OUI_LEN + sizeof(mbo_ie->oui_type));

		/* Transition rejection reason Attribute */
		reject_attr = (wifi_mbo_trans_reason_code_attr_t *)
			bcm_parse_tlvs(mbo_ie->attr, attr_len,
			MBO_ATTR_TRANS_REJ_REASON_CODE);

		if (reject_attr) {
			mbo_scb->bsstrans_reject_reason = reject_attr->trans_reason_code;
		}
	}
	return BCME_OK;
}

int
wlc_mbo_calc_len_mbo_ie_bsstrans_req(uint8 reqmode, bool* assoc_retry_attr)
{
	uint8 len = 0;

	len += MBO_OCE_IE_HDR_SIZE;
	/* add transition reassoc code attr */
	len += sizeof(wifi_mbo_trans_reason_code_attr_t);

	if (reqmode & DOT11_BSSTRANS_REQMODE_DISASSOC_IMMINENT) {
		*assoc_retry_attr = TRUE;
		len += sizeof(wifi_mbo_assoc_retry_delay_attr_t);
	}
	return len;
}

void
wlc_mbo_add_mbo_ie_bsstrans_req(wlc_info_t* wlc, uint8* data, bool assoc_retry_attr,
	uint8 retry_delay, uint8 transition_reason)
{
	uint8 *cp = NULL;
	wifi_mbo_oce_ie_t *ie_hdr = NULL;
	uint total_len = 0; /* len to be put in IE header */
	int len = 0;
	wifi_mbo_trans_reason_code_attr_t* trans_rc_attr = NULL;
	wifi_mbo_assoc_retry_delay_attr_t* retry_delay_attr = NULL;

	cp = data;
	ie_hdr = (wifi_mbo_oce_ie_t *)cp;

	/* fill in MBO-OCE IE header */
	ie_hdr->id = MBO_OCE_IE_ID;
	memcpy(ie_hdr->oui, MBO_OCE_OUI, WFA_OUI_LEN);
	ie_hdr->oui_type = MBO_OCE_OUI_TYPE;
	len = MBO_OCE_IE_HDR_SIZE;
	cp += len;
	total_len = MBO_OCE_IE_NO_ATTR_LEN;

	/* fill transition reason code */
	trans_rc_attr = (wifi_mbo_trans_reason_code_attr_t *)cp;
	trans_rc_attr->id = MBO_ATTR_TRANS_REASON_CODE;
	trans_rc_attr->len = sizeof(*trans_rc_attr) - MBO_ATTR_HDR_LEN;
	trans_rc_attr->trans_reason_code = transition_reason;

	cp += sizeof(*trans_rc_attr);
	total_len += sizeof(*trans_rc_attr);

	if (assoc_retry_attr) {
		/* fill Association retry delay attribute */
		retry_delay_attr = (wifi_mbo_assoc_retry_delay_attr_t *)cp;
		retry_delay_attr->id = MBO_ATTR_ASSOC_RETRY_DELAY;
		retry_delay_attr->len = sizeof(*retry_delay_attr) - MBO_ATTR_HDR_LEN;
		retry_delay_attr->reassoc_delay = retry_delay;

		cp += sizeof(*retry_delay_attr);
		total_len += sizeof(*retry_delay_attr);
	}

	ie_hdr->len = total_len;
}

bool
wlc_mbo_reject_assoc_req(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
	wlc_mbo_info_t *mbo = NULL;
	wlc_mbo_bsscfg_cubby_t *mbo_bsscfg = NULL;

	mbo = wlc->mbo;
	ASSERT(mbo);

	mbo_bsscfg = MBO_BSSCFG_CUBBY(mbo, bsscfg);
	ASSERT(mbo_bsscfg);

	if ((mbo_bsscfg->mbo_assoc_disallowed >= MBO_ASSOC_DISALLOWED_REASON_UNSPECIFIED) &&
		(mbo_bsscfg->mbo_assoc_disallowed <=
			MBO_ASSOC_DISALLOWED_REASON_INSUFFICIENT_RSSI)) {

		return TRUE;
	}
	return FALSE;
}

bool
wlc_mbo_is_channel_non_preferred(wlc_info_t* wlc, struct scb* scb, uint8 channel, uint8 opclass)
{
	wlc_mbo_info_t *mbo = NULL;
	wlc_mbo_scb_cubby_t *mbo_scb = NULL;
	mbo_np_chan_list_t *cur;
	uint8 list_cnt;
	bool ret = FALSE;

	mbo = wlc->mbo;
	ASSERT(mbo);
	if (!scb) {
		/* No need to check */
		return ret;
	}
	mbo_scb = MBO_SCB_CUBBY(mbo, scb);

	if (!(mbo_scb->np_chan_list_head)) {
		return ret;
	}

	cur = mbo_scb->np_chan_list_head;
	while (cur) {
		if (cur->opclass != opclass) {
			cur = cur->next;
			continue;
		}
		for (list_cnt = 0; list_cnt < sizeof(cur->list); list_cnt++) {
			if (cur->list[list_cnt] == channel) {
				/* possible preference:
				 * 0: Non operable
				 * 1: Non preferable
				 * 2-254: Reserved
				 * 255: preferable
				 */
				if (cur->pref == MBO_STA_MARKED_CHANNEL_PREFERABLE) {
					/* Mark this as preferred */
					return FALSE;
				} else {
					return TRUE;
				}
			}
		}
		cur = cur->next;
	}
	return ret;
}

int32
wlc_mbo_get_gas_support(wlc_info_t* wlc)
{
	wlc_mbo_info_t *mbo = NULL;

	mbo = wlc->mbo;
	ASSERT(mbo);
	return (int32)(mbo->fwd_gas_rqst_to_app);
}

void
wlc_mbo_bss_updn(void *ctx, bsscfg_up_down_event_data_t *evt)
{
	wlc_mbo_info_t *mbo = (wlc_mbo_info_t*)ctx;
	wlc_bsscfg_t *cfg = evt->bsscfg;
	wlc_mbo_bsscfg_cubby_t *mbc = NULL;

	wlc_bsscfg_update_rclass(mbo->wlc, cfg);

	mbc = MBO_BSSCFG_CUBBY(mbo, cfg);

	if (!mbc) {
		return;
	}

	if (evt->up == FALSE) {
		mbc->mbo_assoc_disallowed = 0;
	}
}

void
wlc_mbo_update_attr_assoc_disallowed(wlc_bsscfg_t *cfg, uint8 assoc_disallowed)
{
	uint8 prev_val = 0;
	wlc_info_t *wlc;
	wlc_mbo_info_t *mbo;
	wlc_mbo_bsscfg_cubby_t *mbc;
	wlc_mbo_bss_status_t mbo_bss_status;

	wlc = cfg->wlc;
	if (!wlc) {
		return;
	}

	mbo = wlc->mbo;
	if (!mbo) {
		return;
	}

	mbc = MBO_BSSCFG_CUBBY(mbo, cfg);
	ASSERT(mbc);

	prev_val = mbc->mbo_assoc_disallowed;
	mbc->mbo_assoc_disallowed = assoc_disallowed;

	if (prev_val != mbc->mbo_assoc_disallowed) {
		/* update AP or IBSS beacons */
		wlc_bss_update_beacon(wlc, cfg);
		/* update AP or IBSS probe responses */
		wlc_bss_update_probe_resp(wlc, cfg, TRUE);
		/* TODO:
		 * Discuss dynamic option to update per bss  mbo capability in usersapce.
		 * Remove event forward from IOVAR
		 */
		memset(&mbo_bss_status, 0, sizeof(mbo_bss_status));
		mbo_bss_status.version = WLC_E_MBO_BSS_STATUS_VERSION;
		mbo_bss_status.length = sizeof(mbo_bss_status);
		mbo_bss_status.assoc_allowance_status  = mbc->mbo_assoc_disallowed;
		mbo_bss_status.ap_attr  = mbc->mbo_ap_attr;
		mbo_bss_status.mbo_bss_enable  = mbc->flags & MBO_BSSCFG_ENABLE;

		wlc_bss_mac_event(mbo->wlc, cfg, WLC_E_MBO_CAPABILITY_STATUS,
				NULL, 0, 0, 0, &mbo_bss_status, sizeof(mbo_bss_status));
	}
}

uint8
wlc_mbo_get_attr_assoc_disallowed(wlc_bsscfg_t *cfg)
{
	uint8 prev_val = 0;
	wlc_info_t *wlc;
	wlc_mbo_info_t *mbo;
	wlc_mbo_bsscfg_cubby_t *mbc;

	wlc = cfg->wlc;
	if (!wlc) {
		return 0;
	}

	mbo = wlc->mbo;
	if (!mbo) {
		return 0;
	}

	mbc = MBO_BSSCFG_CUBBY(mbo, cfg);
	ASSERT(mbc);

	prev_val = mbc->mbo_assoc_disallowed;
	return prev_val;
}

bool
wlc_is_mbo_enabled(wlc_mbo_info_t *mbo, wlc_bsscfg_t *bsscfg)
{
	wlc_mbo_bsscfg_cubby_t *mbc = NULL;
	bool ret = FALSE;

	mbc = MBO_BSSCFG_CUBBY(mbo, bsscfg);
	if (mbc && MBO_BSSCFG_IS_ENABLED(mbc)) {
		ret = TRUE;
	}
	return ret;

}
