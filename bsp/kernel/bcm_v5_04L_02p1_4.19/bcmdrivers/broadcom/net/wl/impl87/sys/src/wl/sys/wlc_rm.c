/*
 * Radio Measurement functions
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
 * $Id: wlc_rm.c 785082 2020-03-12 12:46:01Z $
 */

/**
 * @file
 * @brief
 * CCX related feature
 */

/**
 * @file
 * @brief
 * XXX  Twiki: [WlDriverRMCQ]
 */

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmendian.h>
#include <bcmutils.h>
#include <siutils.h>
#include <wlioctl.h>
#include <bcmwpa.h>
#include <802.1d.h>
#include <802.11.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_pub.h>
#include <wlc_bsscfg.h>
#include <wlc.h>
#include <wlc_hw.h>
#include <wlc_pub.h>
#include <wl_export.h>
#include <wlc_rm.h>
#include <phy_utils_api.h>
#include <phy_misc_api.h>
#include <wlc_scan.h>
#include <wlc_bmac.h>
#include <wlc_assoc.h>
#include <wlc_utils.h>
#ifdef WLOLPC
#include <wlc_olpc_engine.h>
#endif /* WLOLPC */
#include <wlc_lq.h>
#include <wlc_pm.h>

#if defined(STA) && defined(WLRM)
/* RM Defs */
/* RPI power bin definitions (IEEE Std 802.11, sec 7.3.2.22.3) */
static const int8 wlc_rpi_bin_max[WL_NUM_RPI_BINS] = { -87, -82, -77, -72, -67, -62, -57, 0x7f };

/* RM Protos */
static void wlc_rm_validate(rm_info_t *rm_info);
static void wlc_rm_end(rm_info_t *rm_info);
static void wlc_rm_free(rm_info_t *rm_info);
static void wlc_rm_timer(void *arg);
static void wlc_rm_next_set(rm_info_t *rm_info);
static chanspec_t wlc_rm_chanspec(rm_info_t *rm_info);
static void wlc_rm_begin(rm_info_t *rm_info);
static void wlc_rm_cca_timer_cb(void *arg);
static void wlc_rm_cca_begin(rm_info_t  *rm_info, uint32 dur);
static void wlc_rm_rpi_begin(rm_info_t  *rm_info, uint32 dur);
static void wlc_rm_rpi_timer(void *arg);
static void wlc_rm_state_upd(rm_info_t  *rm_info, uint state);

static void wlc_rm_report_ioctl(rm_info_t  *rm_info, wlc_rm_req_t *req_block, int count);
static int wlc_rm_req_ioctl(rm_info_t  *rm_info, wl_rm_req_t *req_set, int len);
static int wlc_rm_doiovar(void *hdl, uint32 actionid,
        void *params, uint plen, void *arg, uint len, uint val_size, struct wlc_if *wlcif);

/* IOVar table */
/* Parameter IDs, for use only internally to wlc -- in the wlc_iovars
 * table and by the wlc_doiovar() function.  No ordering is imposed:
 * the table is keyed by name, and the function uses a switch.
 */
enum {
	IOV_RM_REQ = 1,
	IOV_RM_REP,
	IOV_LAST
};

static const bcm_iovar_t rm_iovars[] = {
	{"rm_req", IOV_RM_REQ, (IOVF_SET_UP|IOVF_OPEN_ALLOW), 0, IOVT_BUFFER, WL_RM_REQ_FIXED_LEN},
	{"rm_rep", IOV_RM_REP, (IOVF_OPEN_ALLOW), 0, IOVT_BUFFER, WL_RM_REP_FIXED_LEN},
	{NULL, 0, 0, 0, 0, 0}
};

#define WLC_RM_RPI_INTERVAL	20	/* ms, time between RPI/Noise samples */

/* This includes the auto generated ROM IOCTL/IOVAR patch handler C source file (if auto patching is
 * enabled). It must be included after the prototypes and declarations above (since the generated
 * source file may reference private constants, types, variables, and functions).
 */
#include <wlc_patch.h>

static int
wlc_rm_down(void  *ctx)
{
	rm_info_t *rm_info = (rm_info_t *)ctx;
	int callbacks = 0;
	/* abort any radio measures in progress */
	WL_EAP_TRC_RM(("%s: wlc_rm_down\n", __FUNCTION__));
	if (!wlc_rm_abort(rm_info->wlc))
		callbacks++;
	/* cancel the radio measure timers */
	if (!wl_del_timer(rm_info->wlc->wl, rm_info->rm_timer))
		callbacks++;
	if (!wl_del_timer(rm_info->wlc->wl, rm_info->rm_rpi_timer))
		callbacks++;
	return callbacks;
}

/* handle RM related iovars */
static int
wlc_rm_doiovar(void *hdl, uint32 actionid,
	void *params, uint plen, void *arg, uint len, uint val_size, struct wlc_if *wlcif)
{
	rm_info_t *rm_info = (rm_info_t *)hdl;
	int err = BCME_OK;

	switch (actionid) {
	case IOV_GVAL(IOV_RM_REP):
	{
		wl_rm_rep_t rep;
		rep.token = 0;
		rep.len = rm_info->rm_ioctl_rep_len;

		/* generic table already checked min len */
		bcopy(&rep, arg, WL_RM_REP_FIXED_LEN);
		if ((int)len < WL_RM_REP_FIXED_LEN + (int)rep.len) {
			err = BCME_BUFTOOSHORT;
			return err;
		}

		if (rep.len > 0)
			bcopy(rm_info->rm_ioctl_rep, (int8*)arg + WL_RM_REP_FIXED_LEN,
				rm_info->rm_ioctl_rep_len);
		break;
	}

	case IOV_SVAL(IOV_RM_REQ):
		err = wlc_rm_req_ioctl(rm_info, (wl_rm_req_t*)arg, len);
		break;
	default:
		err = BCME_UNSUPPORTED;
		break;
	}
	return err;
}

rm_info_t *
BCMATTACHFN(wlc_rm_attach)(wlc_info_t *wlc)
{
	rm_info_t *rm_info;
	if ((rm_info = (rm_info_t *)MALLOCZ(wlc->osh, sizeof(rm_info_t))) == NULL) {
		WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
			wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
		goto fail;
	}

	rm_info->wlc = wlc;
	if ((rm_info->rm_state = (wlc_rm_req_state_t*)MALLOCZ(wlc->osh,
		sizeof(wlc_rm_req_state_t))) == NULL) {
		WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
			wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
		goto fail;
	}

	if (!(rm_info->rm_timer = wl_init_timer(wlc->wl, wlc_rm_timer, rm_info, "rm"))) {
		WL_ERROR(("rm_timer init failed\n"));
		goto fail;
	}
	if (!(rm_info->rm_rpi_timer = wl_init_timer(wlc->wl, wlc_rm_rpi_timer, rm_info,
		"rm_rpi"))) {
		WL_ERROR(("rm_rpi_timer init failed\n"));
		goto fail;
	}
	if (wlc_module_register(wlc->pub, rm_iovars, "rm", rm_info, wlc_rm_doiovar,
	                        NULL, NULL, wlc_rm_down)) {
		WL_ERROR(("wlc_module_register() for rm  failed\n"));
		goto fail;
	}
	/*
	 * CCA measurement timer
	 */
	if (!(rm_info->rm_cca_timer = wl_init_timer(wlc->wl, wlc_rm_cca_timer_cb,
			rm_info, "rm_cca"))) {
		WL_ERROR(("rm_cca_timer init failed\n"));
		goto fail;
	}
	rm_info->rm_state->cca_active = FALSE;

	/* Enable RM module at attach */
	wlc->pub->_rm = TRUE;

	return rm_info;
fail:
	if (rm_info) {
		if (rm_info->rm_state) {
			MFREE(wlc->osh, rm_info->rm_state, sizeof(wlc_rm_req_state_t));
		}
		MFREE(wlc->osh, rm_info, sizeof(rm_info_t));
	}
	return NULL;
}

void
BCMATTACHFN(wlc_rm_detach)(rm_info_t *rm_info)
{
	wlc_info_t *wlc;

	if (!rm_info)
		return;

	wlc = rm_info->wlc;
	ASSERT(wlc);
	wlc_module_unregister(wlc->pub, "rm", rm_info);
	/* free radio measurement ioctl reports */
	if (rm_info->rm_ioctl_rep) {
		MFREE(wlc->osh, rm_info->rm_ioctl_rep, rm_info->rm_ioctl_rep_len);
		rm_info->rm_ioctl_rep_len = 0;
		rm_info->rm_ioctl_rep = NULL;
	}
	if (rm_info->rm_timer) {
		wl_free_timer(wlc->wl, rm_info->rm_timer);
	}
	if (rm_info->rm_rpi_timer) {
		wl_free_timer(wlc->wl, rm_info->rm_rpi_timer);
	}
	if (rm_info->rm_state) {
		MFREE(wlc->osh, rm_info->rm_state, sizeof(wlc_rm_req_state_t));
	}
	MFREE(wlc->osh, rm_info, sizeof(rm_info_t));
}

void
wlc_rm_start(wlc_info_t *wlc)
{
	rm_info_t *rm_info = wlc->rm_info;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
#ifdef BCMDBG
	wlc_rm_req_t *req;
	const char *name;
	int i;
#endif /* BCMDBG */
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	rm_state->cur_req = 0;
	wlc_rm_state_upd(rm_info, WLC_RM_IDLE);
#ifdef WLOLPC
	/* if on olpc chan notify - if needed, terminate active cal; go off channel */
	if (OLPC_ENAB(wlc)) {
		wlc_olpc_eng_hdl_chan_update(wlc->olpc_info);
	}
#endif /* WLOLPC */

#ifdef BCMDBG
	WL_INFORM(("wl%d: wlc_rm_start(): %d RM Requests, token 0x%x (%d)\n",
		rm_info->wlc->pub->unit, rm_state->req_count,
		rm_state->token, rm_state->token));

	for (i = 0; i < rm_state->req_count; i++) {
		req = &rm_state->req[i];
		switch (req->type) {
		case WLC_RM_TYPE_BASIC:
			name = " Basic";
			break;
		case WLC_RM_TYPE_CCA:
			name = " CCA";
			break;
		case WLC_RM_TYPE_RPI:
			name = " RPI";
			break;
		default:
			name = "";
			break;
		}

		WL_INFORM(("RM REQ token 0x%02x (%2d) type %2d%s chanspec %s tsf 0x%x:%08x dur %4d"
			" TUs\n",
			req->token, req->token, req->type, name,
			wf_chspec_ntoa_ex(req->chanspec, chanbuf), req->tsf_h, req->tsf_l,
			req->dur));
	}
#endif /* BCMDBG */
	wlc_rm_validate(rm_info);

	wlc_rm_next_set(rm_info);
}

static void
wlc_rm_validate(rm_info_t * rm_info)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	chanspec_t rm_chanspec;
	wlc_rm_req_t *req;
	int i;
	wlc_info_t *wlc = rm_info->wlc;
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	rm_chanspec = 0;

	for (i = 0; i < rm_state->req_count; i++) {
		req = &rm_state->req[i];
		if (!(req->flags & WLC_RM_FLAG_PARALLEL)) {
			/* new set of parallel measurements */
			rm_chanspec = 0;
		}

		/* skip Basic measurements */
		if (req->type == WLC_RM_TYPE_BASIC) {
			req->flags |= WLC_RM_FLAG_INCAPABLE;
			continue;
		}

		/* check for RM Noise with insufficient hardware */
		if (req->type == WLC_RM_TYPE_RPI && !WLC_RM_NOISE_SUPPORTED(wlc)) {
			req->flags |= WLC_RM_FLAG_INCAPABLE;
			continue;
		}

		/* check for an unsupported channel */
		if (!wlc_valid_chanspec_db(wlc->cmi, req->chanspec)) {
			req->flags |= WLC_RM_FLAG_INCAPABLE;
			continue;
		}

		/* refuse zero dur measurements */
		if (req->dur == 0) {
			req->flags |= WLC_RM_FLAG_REFUSED;
			continue;
		}

		/* pick up the channel for this parallel set */
		if (rm_chanspec == 0)
			rm_chanspec = req->chanspec;

		/* check for parallel measurements on different channels */
		if (req->chanspec && req->chanspec != rm_chanspec) {
			WL_INFORM(("wl%d: wlc_rm_validate: refusing parallel "
				   "measurement with different channel than "
				   "others, RM type %d token %d chanspec %s\n",
				   wlc->pub->unit, req->type, req->token,
				   wf_chspec_ntoa_ex(req->chanspec, chanbuf)));
			req->flags |= WLC_RM_FLAG_REFUSED;
			continue;
		}
	}
}

void
wlc_rm_stop(wlc_info_t *wlc)
{
	/* just abort for now, eventually, send reports then stop */
	wlc_rm_abort(wlc);
}

static void
wlc_rm_end(rm_info_t *rm_info)
{
	/* clean up state */
	wlc_rm_free(rm_info);
	if (rm_info->rm_state->cb) {
		cb_fn_t cb = (cb_fn_t)(rm_info->rm_state->cb);
		void *cb_arg = rm_info->rm_state->cb_arg;

		(cb)(cb_arg);
		rm_info->rm_state->cb = NULL;
		rm_info->rm_state->cb_arg = NULL;
	}

}
void
wlc_rm_terminate(rm_info_t *rm_info)
{
	wlc_info_t *wlc = rm_info->wlc;

	int idx;
	wlc_bsscfg_t *cfg;

	/* enable CFP & TSF update */
	wlc_mhf(wlc, MHF2, MHF2_SKIP_CFP_UPDATE, 0, WLC_BAND_ALL);
	wlc_skip_adjtsf(wlc, FALSE, NULL, WLC_SKIP_ADJTSF_RM, WLC_BAND_ALL);

	/* come out of PS mode if appropriate */
	FOREACH_BSS(wlc, idx, cfg) {
		if (!BSSCFG_STA(cfg))
			continue;
		/* un-block PSPoll operations and restore PS state */
		mboolclr(cfg->pm->PMblocked, WLC_PM_BLOCK_CHANSW);
		if (cfg->pm->PM != PM_MAX || cfg->pm->WME_PM_blocked) {
			WL_RTDC(wlc, "wlc_rm_meas_end: exit PS", 0, 0);
			wlc_set_pmstate(cfg, FALSE);
			wlc_pm2_sleep_ret_timer_start(cfg, 0);
		}
	}
	wlc_set_wake_ctrl(wlc);
#ifdef WLOLPC
	/* if on olpc chan notify - if needed, terminate active cal; go off channel */
	if (OLPC_ENAB(wlc)) {
		wlc_olpc_eng_hdl_chan_update(wlc->olpc_info);
	}
#endif /* WLOLPC */
}

int
wlc_rm_abort(wlc_info_t *wlc)
{
	rm_info_t *rm_info = wlc->rm_info;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	bool canceled = FALSE;

	if (rm_state->step == WLC_RM_IDLE) {
		return TRUE;
	}
	if (rm_state->step == WLC_RM_ABORT) {
		/* timer has been canceled, but not fired yet */
		return FALSE;
	}

	wlc_rm_state_upd(rm_info, WLC_RM_ABORT);

	/* cancel any timers and clear state */
	if (wl_del_timer(wlc->wl, rm_info->rm_timer)) {
		wlc_rm_state_upd(rm_info, WLC_RM_IDLE);
		canceled = TRUE;
	}

	/* Change the radio channel to the return channel */
	if ((rm_state->chanspec_return != 0) &&
	(phy_utils_get_chanspec(WLC_PI(wlc)) != rm_state->chanspec_return)) {
		wlc_suspend_mac_and_wait(wlc);
		wlc_set_chanspec(wlc, rm_state->chanspec_return, CHANSW_REASON(CHANSW_IOVAR));
		wlc_enable_mac(wlc);
	}

	wlc_rm_terminate(wlc->rm_info);

	/* un-suspend the data fifos in case they were suspended
	 * for off channel measurements
	 */
	wlc_tx_resume(wlc);

	wlc_rm_free(rm_info);

	return canceled;
}

static void
wlc_rm_free(rm_info_t *rm_info)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_info_t *wlc = rm_info->wlc;

	if (rm_state->req)
		MFREE(wlc->osh, rm_state->req, rm_state->req_count * sizeof(wlc_rm_req_t));

	WL_EAP_TRC_RM(("%s: freeing all rm_state\n", __FUNCTION__));
	wlc_phy_hold_upd(WLC_PI(wlc), PHY_HOLD_FOR_RM, FALSE);
	wl_del_timer(wlc->wl, rm_info->rm_timer);
	wlc_rm_state_upd(rm_info, WLC_RM_IDLE);
	/* update ps control */
	wlc_set_wake_ctrl(wlc);
	rm_state->chanspec_return = 0;
	rm_state->cca_active = FALSE;
	rm_state->rpi_active = FALSE;
	rm_state->cur_req = 0;
	rm_state->req_count = 0;
	rm_state->req = NULL;
	WL_EAP_TRC_RXTXUTIL_MSG(("%s[%d]: cca_active %d\n", __func__, __LINE__,
		rm_state->cca_active));
	return;
}

static uint32
wlc_rm_time_offset(rm_info_t *rm_info, wlc_rm_req_t *req)
{
	uint32 tsf_h, tsf_l;
	uint32 offset_h, offset_l;
	uint32 offset;
	wlc_info_t *wlc = rm_info->wlc;

	/* calculate how long until this measurement should start */
	if (req->tsf_h == 0 && req->tsf_l == 0) {
		offset = 0;
	} else {
		wlc_read_tsf(wlc, &tsf_l, &tsf_h);
		if (wlc_uint64_lt(req->tsf_h, req->tsf_l, tsf_h, tsf_l)) {
			offset = 0;
		} else {
			offset_h = req->tsf_h;
			offset_l = req->tsf_l;

			wlc_uint64_sub(&offset_h, &offset_l, tsf_h, tsf_l);

			/* if offset_h is non-zero, then the measurement time is more than 74 min
			 * in the future, so the request is messed up
			 */
			if (offset_h != 0) {
				offset = 0;
			} else {
				/* convert from micro to milli secs */
				offset = offset_l / 1000;
			}
		}
	}

	return offset;
}

static void
wlc_rm_timer(void *arg)
{
	rm_info_t *rm_info = (rm_info_t*)arg;
	wlc_info_t *wlc = rm_info->wlc;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *req;
	uint32 offset;
	int idx;
	wlc_bsscfg_t *cfg;
	WL_TRACE(("wl%d: wlc_rm_timer", wlc->pub->unit));
	if (!wlc->pub->up)
		return;
	if (DEVICEREMOVED(wlc)) {
		WL_ERROR(("wl%d: %s: dead chip\n", wlc->pub->unit, __FUNCTION__));
		wl_down(wlc->wl);
		return;
	}
	switch (rm_state->step) {
	       case WLC_RM_WAIT_PREP_CHANNEL:
		/* announce PS mode to the AP if we are not already in PS mode */
		rm_state->ps_pending = TRUE;
		FOREACH_AS_STA(wlc, idx, cfg) {
			/* block any PSPoll operations since we are just holding off AP traffic */
			mboolset(cfg->pm->PMblocked, WLC_PM_BLOCK_CHANSW);
			if (cfg->pm->PMenabled)
				continue;
			WL_INFORM(("wl%d.%d: wlc_rm_timer: PREP_CHANNEL, "
				"entering PS mode for off channel measurement\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(cfg)));
			wlc_set_pmstate(cfg, TRUE);
		}
		/* We are supposed to wait for PM0->PM1 transition to finish but in case
		 * * we failed to send PM indications or failed to receive ACKs fake a PM0->PM1
		 * * transition so that anything depending on the transition to finish can move
		 * * forward i.e. scan engine can continue.
		*/
		wlc_pm_pending_complete(wlc);
		if (!wlc->PMpending)
			rm_state->ps_pending = FALSE;
		wlc_set_wake_ctrl(wlc);

		/* suspend tx data fifos here for off channel measurements
		 * * if we are not announcing PS mode
		*/
		if (!rm_state->ps_pending)
			wlc_tx_suspend(wlc);
		/* calculate how long until this measurement should start */
		req = &rm_state->req[rm_state->cur_req];
		offset = wlc_rm_time_offset(rm_info, req);
		if (offset < WLC_RM_PREP_MARGIN)
			offset = 0;
		else
			offset -= WLC_RM_PREP_MARGIN;
		if (offset > 0) {
			wlc_rm_state_upd(rm_info, WLC_RM_WAIT_BEGIN_MEAS);
			wl_add_timer(wlc->wl, rm_info->rm_timer, offset, 0);
			WL_INFORM(("wl%d: wlc_rm_timer: PREP_OFF_CHANNEL, "
				"%d ms until measurement, waiting for measurement timer\n",
				wlc->pub->unit, offset));
		} else if (rm_state->ps_pending) {
			wlc_rm_state_upd(rm_info, WLC_RM_WAIT_PS_ANNOUNCE);
			/* wait for PS state to be communicated by
			 * waiting for the Null Data packet tx to
			 * complete, then start measurements
			 */
		WL_INFORM(("wl%d: wlc_rm_timer: PREP_OFF_CHANNEL, "
				"%d ms until measurement, waiting for PS announcement\n",
				wlc->pub->unit, offset));
		} else {
			wlc_rm_state_upd(rm_info, WLC_RM_WAIT_TX_SUSPEND);
			/* wait for the suspend interrupt to come back
			 * and start measurements
			 */
			WL_INFORM(("wl%d: wlc_rm_timer: PREP_OFF_CHANNEL, "
				"%d ms until measurement, waiting for TX fifo suspend\n",
				wlc->pub->unit, offset));
		}
		break;
	case WLC_RM_WAIT_PS_ANNOUNCE:
	case WLC_RM_WAIT_START_SET:
	case WLC_RM_WAIT_TX_SUSPEND:
	case WLC_RM_WAIT_BEGIN_MEAS:
		if (rm_state->step == WLC_RM_WAIT_START_SET)
			WL_INFORM(("wl%d: wlc_rm_timer: START_SET\n",
				wlc->pub->unit));
		else if (rm_state->step == WLC_RM_WAIT_TX_SUSPEND)
			WL_INFORM(("wl%d: wlc_rm_timer: TX_SUSPEND\n",
				wlc->pub->unit));
		else
			WL_INFORM(("wl%d: wlc_rm_timer: BEGIN_MEASUREMENTS\n",
				wlc->pub->unit));
		wlc_rm_begin(rm_info);
		break;
	case WLC_RM_WAIT_END_MEAS:
		WL_INFORM(("wl%d: wlc_rm_timer: END_MEASUREMENTS\n",
			wlc->pub->unit));
		wlc_rm_meas_end(wlc);
		break;
	case WLC_RM_ABORT:
		wlc_rm_state_upd(rm_info, WLC_RM_IDLE);
		break;
	case WLC_RM_IDLE:
		WL_ERROR(("wl%d: %s: error, in timer with state WLC_RM_IDLE\n",
			wlc->pub->unit, __FUNCTION__));
		break;
	default:
		WL_ERROR(("wl%d: %s: Invalid RM state:%d\n", WLCWLUNIT(wlc),
			__FUNCTION__, rm_state->step));
		break;
	}
	/* update PS control */
	wlc_set_wake_ctrl(wlc);
}

void
wlc_rm_pm_pending_complete(rm_info_t *rm_info)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_info_t *wlc = rm_info->wlc;
	if (!rm_state->ps_pending)
		return;

	rm_state->ps_pending = FALSE;
	/* if the RM state machine is waiting for the PS announcement,
	 * then schedule the timer
	 */
	if (rm_state->step == WLC_RM_WAIT_PS_ANNOUNCE)
		wl_add_timer(wlc->wl, rm_info->rm_timer, 0, 0);
}

/* start the radio measurement state machine working on
 * the next set of parallel measurements.
 */
static void
wlc_rm_next_set(rm_info_t *rm_info)
{
	uint32 offset = 0;
	chanspec_t chanspec;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_info_t *wlc = rm_info->wlc;

	if (rm_state->cur_req >= rm_state->req_count) {
		/* signal that all requests are done */
		wlc_rm_end(rm_info);
		return;
	}

	/* Start a timer for off-channel prep if measurements are off-channel,
	 * or for the measurements if on-channel
	 */
	if (wlc->pub->associated &&
		(chanspec = wlc_rm_chanspec(rm_info)) != 0 &&
		chanspec != wlc->home_chanspec) {
		if (rm_state->chanspec_return != 0)
			offset = WLC_RM_HOME_TIME;
		/* off-channel measurements, set a timer to prepare for the channel switch */
		wlc_rm_state_upd(rm_info, WLC_RM_WAIT_PREP_CHANNEL);
	} else {
		wlc_rm_req_t *req = &rm_state->req[rm_state->cur_req];
		offset = wlc_rm_time_offset(rm_info, req);
		/* either unassociated, so no channel prep even if we do change channels,
		 * or channel-less measurement set (all suppressed or internal state report),
		 * or associated and measurements on home channel,
		 * set the timer for WLC_RM_WAIT_START_SET
		 */

		wlc_rm_state_upd(rm_info, WLC_RM_WAIT_START_SET);
	}

	if (offset > 0)
		wl_add_timer(wlc->wl, rm_info->rm_timer, offset, 0);
	else
		wlc_rm_timer(rm_info);

	return;
}

static chanspec_t
wlc_rm_chanspec(rm_info_t* rm_info)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *req;
	int req_idx;
	chanspec_t chanspec = 0;

	for (req_idx = rm_state->cur_req; req_idx < rm_state->req_count; req_idx++) {
		req = &rm_state->req[req_idx];

		/* check for end of parallel measurements */
		if (req_idx != rm_state->cur_req &&
		    (req->flags & WLC_RM_FLAG_PARALLEL) == 0) {
			break;
		}

		/* check for request that have already been marked to skip */
		if (req->flags & (WLC_RM_FLAG_INCAPABLE | WLC_RM_FLAG_REFUSED))
			continue;

		if (req->dur > 0) {
			/* found the first non-zero dur request that will not be skipped */
			chanspec = req->chanspec;
			break;
		}
	}

	return chanspec;
}

static void
wlc_rm_begin(rm_info_t* rm_info)
{
	int dur_max;
	chanspec_t rm_chanspec;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *req;
	int req_idx;
	bool blocked = FALSE;
	wlc_info_t *wlc = rm_info->wlc;

	rm_chanspec = wlc_rm_chanspec(rm_info);

	if (rm_chanspec != 0)
		blocked = (wlc_mac_request_entry(wlc, NULL, WLC_ACTION_RM, 0) != BCME_OK);

	/* check for a channel change */
	if (!blocked &&
	    rm_chanspec != 0 &&
	    rm_chanspec != phy_utils_get_chanspec(WLC_PI(wlc))) {
		/* has the PS announcement completed yet? */
		if (rm_state->ps_pending) {
			/* wait for PS state to be communicated before switching channels */
			wlc_rm_state_upd(rm_info, WLC_RM_WAIT_PS_ANNOUNCE);
			return;
		}
		/* has the suspend completed yet? */
		if (wlc->pub->associated && !wlc_tx_suspended(wlc)) {
			/* suspend tx data fifos for off channel measurements */
			wlc_tx_suspend(wlc);
			/* wait for the suspend before switching channels */
			wlc_rm_state_upd(rm_info, WLC_RM_WAIT_TX_SUSPEND);
			return;
		}
		rm_state->chanspec_return = phy_utils_get_chanspec(WLC_PI(wlc));
		/* skip CFP & TSF update */
		wlc_mhf(wlc, MHF2, MHF2_SKIP_CFP_UPDATE, MHF2_SKIP_CFP_UPDATE, WLC_BAND_ALL);
		wlc_skip_adjtsf(wlc, TRUE, NULL, WLC_SKIP_ADJTSF_RM, WLC_BAND_ALL);

		wlc_suspend_mac_and_wait(wlc);
		wlc_set_chanspec(wlc, rm_chanspec, CHANSW_REASON(CHANSW_IOVAR));
		wlc_enable_mac(wlc);
	} else {
		rm_state->chanspec_return = 0;
		if (rm_chanspec != 0 && rm_chanspec != phy_utils_get_chanspec(WLC_PI(wlc)))
			WL_ERROR(("wl%d: %s: fail to set channel "
				"for rm due to rm request is blocked\n",
				wlc->pub->unit, __FUNCTION__));
	}

	/* record the actual start time of the measurements */
	wlc_read_tsf(wlc, &rm_state->actual_start_l, &rm_state->actual_start_h);

	dur_max = 0;

	for (req_idx = rm_state->cur_req; req_idx < rm_state->req_count; req_idx++) {
		req = &rm_state->req[req_idx];

		/* check for end of parallel measurements */
		if (req_idx != rm_state->cur_req &&
		    (req->flags & WLC_RM_FLAG_PARALLEL) == 0) {
			break;
		}

		/* check for request that have already been marked to skip */
		if (req->flags & (WLC_RM_FLAG_INCAPABLE | WLC_RM_FLAG_REFUSED))
			continue;

		/* mark all requests as refused if blocked from measurement */
		if (blocked) {
			req->flags |= WLC_RM_FLAG_REFUSED;
			continue;
		}

		if (req->dur > dur_max)
			dur_max = req->dur;

		/* record the actual start time of the measurements */
		req->tsf_h = rm_state->actual_start_h;
		req->tsf_l = rm_state->actual_start_l;

		switch (req->type) {
		case WLC_RM_TYPE_BASIC:
			WL_INFORM(("wl%d: wlc_rm_begin: starting Basic measurement\n",
				wlc->pub->unit));
			/* wlc_rm_begin_basic(wlc, req->dur) */
			break;
		case WLC_RM_TYPE_CCA:
			WL_INFORM(("wl%d: wlc_rm_begin: starting CCA measurement\n",
				wlc->pub->unit));
			wlc_rm_cca_begin(rm_info, req->dur);
			break;
		case WLC_RM_TYPE_RPI:
			WL_INFORM(("wl%d: wlc_rm_begin: starting RPI Histogram measurement\n",
				wlc->pub->unit));
			wlc_rm_rpi_begin(rm_info, req->dur);
			break;
		default:
			WL_ERROR(("wl%d: %s: unknown measurement "
				"request type %d in request token %d\n",
				wlc->pub->unit, __FUNCTION__, req->type, rm_state->token));
			break;
		}
	}

	wlc_rm_state_upd(rm_info, WLC_RM_WAIT_END_MEAS);

	/* check if we need to set WLC_RM_WAIT_END_CCA */
	WL_EAP_TRC_RXTXUTIL_MSG(("%s[%d]: cca_active %d\n",
		__func__, __LINE__, rm_state->cca_active));
	if (rm_state->cca_active &&
	    !rm_state->rpi_active) {
		/* need to flag CCA as the ending measurement since nothing else
		 * will be calling wlc_rm_meas_end
		 */
		rm_state->cca_active = FALSE;
		WL_EAP_TRC_RXTXUTIL_MSG(("%s[%d]: cca_active %d\n", __func__, __LINE__,
			rm_state->cca_active));
		wlc_rm_state_upd(rm_info, WLC_RM_WAIT_END_CCA);
	}

	if (dur_max == 0) {
		WL_INFORM(("wl%d: wlc_rm_begin: zero dur for measurements, "
			   "scheduling end timer now\n", wlc->pub->unit));
		wl_add_timer(wlc->wl, rm_info->rm_timer, 0, 0);
	}

	return;
}

void
wlc_rm_meas_end(wlc_info_t* wlc)
{
	rm_info_t* rm_info = wlc->rm_info;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *req;
	int req_idx;
	int req_count;
	/* check for a pending RPI measurement */
	if (rm_state->rpi_active) {
		/* we need to wait for the RPI completion */
		WL_INFORM(("wl%d: wlc_rm_meas_end: waiting for RPI, dur %d TUs\n",
			wlc->pub->unit, rm_state->rpi_dur));
		return;
	}

	/* check for a pending CCA measurement */
	if (rm_state->cca_active) {
		/* we need to wait for the CCA completion
		 * With WLC_RM_WAIT_END_CCA set, the CCA measurement will set
		 * the rm_timer to fire when done
		 */
		rm_state->cca_active = FALSE;
		WL_EAP_TRC_RXTXUTIL_MSG(("%s[%d]: cca_active %d\n", __func__, __LINE__,
			rm_state->cca_active));
		wlc_rm_state_upd(rm_info, WLC_RM_WAIT_END_CCA);
		WL_INFORM(("wl%d: all measurements ended before CCA, waiting for CCA interrupt,"
			" dur %d TUs\n", wlc->pub->unit, rm_state->cca_dur));
		return;
	}
	/* return to the home channel if needed */
	if (rm_state->chanspec_return != 0) {
		wlc_suspend_mac_and_wait(wlc);
		wlc_set_chanspec(wlc, rm_state->chanspec_return, CHANSW_REASON(CHANSW_IOVAR));
		wlc_enable_mac(wlc);
	}
	wlc_rm_terminate(rm_info);
	/* un-suspend the data fifos in case they were suspended
	 * * for off channel measurements
	 */
	if (!SCAN_IN_PROGRESS(wlc->scan) || ((wlc->scan->state & SCAN_STATE_WSUSPEND) == 0))
		wlc_tx_resume(wlc);
	/* count the requests for this set */
	for (req_idx = rm_state->cur_req; req_idx < rm_state->req_count; req_idx++) {
		req = &rm_state->req[req_idx];
		/* check for end of parallel measurements */
		if (req_idx != rm_state->cur_req && (req->flags & WLC_RM_FLAG_PARALLEL) == 0) {
			break;
		}
	}
	req_count = req_idx - rm_state->cur_req;
	req = &rm_state->req[rm_state->cur_req];
	switch (rm_state->report_class) {
		case WLC_RM_CLASS_IOCTL:
			wlc_rm_report_ioctl(rm_info, req, req_count);
			break;
#ifdef RM_11H_SUPPORT
		case WLC_RM_CLASS_11H:
			wlc_rm_report_11h(rm_info, req, req_count);
			break;
#endif /* RM_11H_SUPPORT */
	default:
		WL_ERROR(("wl%d: %s: Unsupported report type:%d\n", WLCWLUNIT(wlc),
			__FUNCTION__, rm_state->report_class));
		break;
	}
	/* done with the current set of measurements,
	 * advance the pointers and start the next set
	 */
	rm_state->cur_req += req_count;
	wlc_rm_next_set(rm_info);
}

static void
wlc_rm_cca_begin(rm_info_t *rm_info, uint32 dur)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;

	/* start measurement */
	if (wlc_rm_cca_start(rm_info->wlc, dur)) {
		WL_EAP_TRC_RXTXUTIL_MSG(("%s[%d]: cca_active %d: ",
			__FUNCTION__, __LINE__, rm_state->cca_active));
		rm_state->cca_idle = 0;
		rm_state->cca_dur = dur;
	}
	return;
}

void
wlc_rm_cca_end(wlc_info_t *wlc)
{
	rm_info_t *rm_info = wlc->rm_info;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	uint32 cca_dur_us;
	uint32 cca_end_h;
	cca_ucode_counts_t *delta = &wlc->util_delta_counts;
	uint32 rx_util_ucode = 0;
	uint32 tx_util_ucode = 0;
	uint32 chan_util_ucode = 0;
	uint32 cca_busy_us = 0;
	uint32 interference = 0;
	uint32 congest_ibss = 0;
	uint32 congest_obss = 0;
	uint8 frac = 0;
	uint8 utilization;
	uint8 chan_utilization = 0;

	if (!rm_state->cca_active) {
		return;
	}
	wlc->channel_util_seq++;

	/*
	 * TSF timestamp - Actual CCA meas end time
	 */
	wlc_bmac_read_tsf(wlc->hw, &rm_state->cca_end_l, &cca_end_h);

	wlc_bmac_cca_stats_read(wlc->hw, &wlc->util_counts);
	/*
	 * Calculate actual measurement duration.
	 */
	cca_dur_us = DELTA(rm_state->cca_end_l, rm_state->cca_start_l);

	/*
	* At the end of a CCA measurement interval, calculate the receive and transmit utilization
	* and save in wlc for retrieval by application
	*/
	delta->usecs = cca_dur_us;
	/*
	 * Detailed stats. Calculate delta and update previous util counts
	 */
	wlc->util_last_counts.usecs = wlc->util_counts.usecs;
	delta->ibss = DELTA(wlc->util_counts.ibss, wlc->util_last_counts.ibss);
	wlc->util_last_counts.ibss = wlc->util_counts.ibss;
	delta->obss = DELTA(wlc->util_counts.obss, wlc->util_last_counts.obss);
	wlc->util_last_counts.obss = wlc->util_counts.obss;
	delta->PM = DELTA(wlc->util_counts.PM, wlc->util_last_counts.PM);
	wlc->util_last_counts.PM = wlc->util_counts.PM;
	delta->noctg = DELTA(wlc->util_counts.noctg, wlc->util_last_counts.noctg);
	wlc->util_last_counts.noctg = wlc->util_counts.noctg;
	delta->nopkt = DELTA(wlc->util_counts.nopkt, wlc->util_last_counts.nopkt);
	wlc->util_last_counts.nopkt = wlc->util_counts.nopkt;
	delta->txdur = DELTA(wlc->util_counts.txdur, wlc->util_last_counts.txdur);
	wlc->util_last_counts.txdur = wlc->util_counts.txdur;
#if defined(ISID_STATS)
	delta->crsglitch = DELTA(wlc->util_counts.crsglitch, wlc->util_last_counts.crsglitch);
	wlc->util_last_counts.crsglitch = wlc->util_counts.crsglitch;
	delta->badplcp = DELTA(wlc->util_counts.badplcp, wlc->util_last_counts.badplcp);
	wlc->util_last_counts.badplcp = wlc->util_counts.badplcp;
	delta->bphy_badplcp =
		DELTA(wlc->util_counts.bphy_badplcp, wlc->util_last_counts.bphy_badplcp);
	wlc->util_last_counts.bphy_badplcp = wlc->util_counts.bphy_badplcp;
	delta->bphy_crsglitch = DELTA(wlc->util_counts.bphy_crsglitch,
		wlc->util_last_counts.bphy_crsglitch);
	wlc->util_last_counts.bphy_crsglitch = wlc->util_counts.bphy_crsglitch;
#endif

#if defined(WL_PROT_OBSS)
	delta->rxdrop20s = DELTA(wlc->util_counts.rxdrop20s, wlc->util_last_counts.rxdrop20s);
	wlc->util_last_counts.rxdrop20s = wlc->util_counts.rxdrop20s;
	delta->rx20s = DELTA(wlc->util_counts.rx20s, wlc->util_last_counts.rx20s);
	wlc->util_last_counts.rx20s = wlc->util_counts.rx20s;
	delta->rxcrs_pri = DELTA(wlc->util_counts.rxcrs_pri, wlc->util_last_counts.rxcrs_pri);
	wlc->util_last_counts.rxcrs_pri = wlc->util_counts.rxcrs_pri;
	delta->rxcrs_sec20 = DELTA(wlc->util_counts.rxcrs_sec20,
		wlc->util_last_counts.rxcrs_sec20);
	wlc->util_last_counts.rxcrs_sec20 = wlc->util_counts.rxcrs_sec20;
	delta->rxcrs_sec40 = DELTA(wlc->util_counts.rxcrs_sec40,
		wlc->util_last_counts.rxcrs_sec40);
	wlc->util_last_counts.rxcrs_sec40 = wlc->util_counts.rxcrs_sec40;
	delta->sec_rssi_hist_hi = DELTA(wlc->util_counts.sec_rssi_hist_hi,
		wlc->util_last_counts.sec_rssi_hist_hi);
	wlc->util_last_counts.sec_rssi_hist_hi = wlc->util_counts.sec_rssi_hist_hi;
	delta->sec_rssi_hist_med = DELTA(wlc->util_counts.sec_rssi_hist_med,
		wlc->util_last_counts.sec_rssi_hist_med);
	wlc->util_last_counts.sec_rssi_hist_med = wlc->util_counts.sec_rssi_hist_med;
	delta->sec_rssi_hist_low = DELTA(wlc->util_counts.sec_rssi_hist_low,
		wlc->util_last_counts.sec_rssi_hist_low);
	wlc->util_last_counts.sec_rssi_hist_low = wlc->util_counts.sec_rssi_hist_low;
#endif

	delta->wifi = DELTA(wlc->util_counts.wifi, wlc->util_last_counts.wifi);
	wlc->util_last_counts.wifi = wlc->util_counts.wifi;

	/*
	 * Calculate rx_util, tx_util from CRS counters and txdur.
	 */
	WL_EAP_TRC_RXTXUTIL_MSG(("%s: ********* Pri=%d BW=%d ***********\n",
			__FUNCTION__, ((wlc->chanspec & 0x0700) >> 8),
			((wlc->chanspec & 0x3800) >> 11)));
	frac = (uint8)CEIL((255 * (delta->ibss + delta->obss + delta->noctg)), delta->usecs);
	rx_util_ucode = (frac * 100)/255;
	wlc->rx_util_ucode = rx_util_ucode;

	frac = (uint8)CEIL((255 * delta->txdur), delta->usecs);
	tx_util_ucode = (frac * 100)/255;
	wlc->tx_util_ucode = tx_util_ucode;

	cca_busy_us = delta->ibss + delta->obss + delta->noctg + delta->PM + delta->txdur +
		delta->wifi;
	frac = (uint8)CEIL((255 * cca_busy_us), delta->usecs);
	chan_util_ucode = (frac * 100)/255;
	wlc->channel_util_ucode = chan_util_ucode;

	chan_utilization = (uint8)CEIL(100 * (delta->ibss + delta->obss +
		delta->noctg + delta->txdur + delta->nopkt + delta->wifi), delta->usecs);
	chan_utilization = MIN(100, chan_utilization);

	congest_ibss = delta->ibss + delta->txdur;
	congest_obss = delta->obss + delta->noctg;
	interference = delta->nopkt + delta->wifi;

	utilization = (uint8)CEIL(100 * (interference + congest_ibss + congest_obss), delta->usecs);
	utilization = MIN(100, utilization);

	WL_EAP_TRC_RXTXUTIL_MSG(("%s: Raw Stats: \n"
		" rxdrop20s \t %u (%u %%)\n"
		" rx20s \t %u (%u %%)\n"
		" rxcrs_pri \t %u (%u %%)\n"
		" rxcrs_sec20 \t %u (%u %%)\n"
		" rxcrs_sec40 \t %u (%u %%)\n"
		" sec_rssi_hist_hi \t %u\n"
		" sec_rssi_hist_med \t %u\n"
		" sec_rssi_hist_lo \t %u\n"
		" ibss \t\t %u (%u %%) \n"
		" obss \t\t %u (%u %%) \n"
		" noctg \t\t %u (%u %%) \n"
		" nopkt \t\t %u (%u %%) \n"
		" PM \t\t %u (%u %%)\n"
		" txdur \t\t %u (%u %%) \n"
		" Meas dur \t\t %u"
		" Meas sequence \t %u",
		__FUNCTION__,
		delta->rxdrop20s, (uint8)CEIL((100 * delta->rxdrop20s), delta->usecs),
		delta->rx20s, (uint8)CEIL((100 * delta->rx20s), delta->usecs),
		delta->rxcrs_pri, (uint8)CEIL((100 * delta->rxcrs_pri), delta->usecs),
		delta->rxcrs_sec20, (uint8)CEIL((100 * delta->rxcrs_sec20), delta->usecs),
		delta->rxcrs_sec40, (uint8)CEIL((100 * delta->rxcrs_sec40), delta->usecs),
		delta->sec_rssi_hist_hi, delta->sec_rssi_hist_med, delta->sec_rssi_hist_low,
		delta->ibss, (uint8)CEIL((100 * delta->ibss), delta->usecs),
		delta->obss, (uint8)CEIL((100 * delta->obss), delta->usecs),
		delta->noctg, (uint8)CEIL((100 * delta->noctg), delta->usecs),
		delta->nopkt, (uint8)CEIL((100 * delta->nopkt), delta->usecs),
		delta->PM, (uint8)CEIL((100 * delta->PM), delta->usecs),
		delta->txdur, (uint8)CEIL((100 * delta->txdur), delta->usecs),
		delta->usecs, wlc->channel_util_seq));

#if defined(ISID_STATS)
	WL_EAP_TRC_RXTXUTIL_MSG(("%s: CCA delta->crsglitch %u, delta->badplcp %u\n",
		__FUNCTION__, delta->crsglitch, delta->badplcp));
#endif
	/* check if we should call the timer */
	if (rm_state->step == WLC_RM_WAIT_END_CCA) {
		wlc_rm_state_upd(rm_info, WLC_RM_WAIT_END_MEAS);
		wl_add_timer(wlc->wl, rm_info->rm_timer, 0, 0);
	}
}

static void
wlc_rm_rpi_begin(rm_info_t *rm_info, uint32 dur)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;

	ASSERT(WLC_RM_NOISE_SUPPORTED(wlc));

	if (!WLC_RM_NOISE_SUPPORTED(wlc))
		return;

	rm_state->rssi_sample = 0;
	rm_state->rssi_sample_num = 0;
	rm_state->rpi_sample_num = 0;
	bzero(rm_state->rpi, sizeof(rm_state->rpi));
	rm_state->rpi_active = TRUE;
	rm_state->rpi_end = FALSE;
	rm_state->rpi_dur = dur;

	/* start the measurement */
	wlc_rm_rpi_timer(rm_info);

	return;
}

static void
wlc_rm_rpi_timer(void *arg)
{
	rm_info_t *rm_info = (rm_info_t*)arg;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	uint32 dummy_tsf_h, tsf_l;
	uint32 diff;
	uint32 dur_us;
	wlc_info_t *wlc = rm_info->wlc;
	int i;

	if (!wlc->pub->up)
		return;

	if (DEVICEREMOVED(wlc)) {
		WL_ERROR(("wl%d: %s: dead chip\n", wlc->pub->unit, __FUNCTION__));
		wl_down(wlc->wl);
		return;
	}

	if (!rm_state->rpi_active)
		return;

	if (rm_state->rpi_end) {
		/* done with the measurement */
		rm_state->rpi_active = FALSE;

		/* normalize histogram to 0-255 */
		WL_INFORM(("wl%d: wlc_rm_rpi_timer: RPI measurement done\n", wlc->pub->unit));

		for (i = 0; i < 8; i++) {
			if (rm_state->rpi[i] > 0)
				rm_state->rpi[i] = CEIL((255 * rm_state->rpi[i]),
					rm_state->rpi_sample_num);
			WL_INFORM(("wl%d: wlc_rm_rpi_timer: RPI bin[%d] = %3d/255\n",
				wlc->pub->unit, i, rm_state->rpi[i]));
		}

		/* see if we are done with all measures in the current set */
		wlc_rm_meas_end(wlc);
		return;
	}

	/* continue measuring */
	wlc_lq_noise_sample_request(wlc, WLC_NOISE_REQUEST_RM,
	                         CHSPEC_CHANNEL(phy_utils_get_chanspec(WLC_PI(wlc))));

	/* calc duration remaining */
	wlc_read_tsf(wlc, &tsf_l, &dummy_tsf_h);
	diff = tsf_l - rm_state->actual_start_l;
	dur_us = rm_state->rpi_dur * DOT11_TU_TO_US;

	if (dur_us > diff &&
	    dur_us - diff > WLC_RM_RPI_INTERVAL * 1000) {
		wl_add_timer(wlc->wl, rm_info->rm_rpi_timer, WLC_RM_RPI_INTERVAL, 0);
	} else {
		/* end the measurement as we collect the next sample */
		rm_state->rpi_end = TRUE;
	}
}

/* this is a callback and must defer calling noise_sample_request */
bool
wlc_rm_rpi_sample(rm_info_t *rm_info, int8 rssi)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	int i;
	wlc_info_t *wlc = rm_info->wlc;

	/* update the histogram */
	WL_NONE(("wl%d: wlc_rm_rpi_sample: adding rssi sample %d to histogram\n",
		wlc->pub->unit, rssi));

	for (i = 0; i < 8; i++) {
		if (rssi <= wlc_rpi_bin_max[i]) {
			rm_state->rpi[i] += 1;
			break;
		}
	}
	rm_state->rpi_sample_num++;

	if (rm_state->rpi_end)
		wl_add_timer(wlc->wl, rm_info->rm_rpi_timer, 0, 0);

	return FALSE;
}

static void
wlc_rm_report_ioctl(rm_info_t *rm_info, wlc_rm_req_t *req_block, int count)
{
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *req;
	uint8 *rep_block;
	uint8 *rep_ptr;
	uint8 *data;
	wl_rm_rep_elt_t rep;
	int rep_len;
	int data_len;
	wl_rm_rpi_rep_t *rpi_rep;
	int bin;
	int len;
	uint32 val;
	bool measured;
	int i;

	bzero(&rep, sizeof(rep));
	/* calculate the length of this report block */
	len = 0;
	for (i = 0; i < count; i++) {
		req = req_block + i;
		data_len = 0;
		switch (req->type) {
		case WLC_RM_TYPE_BASIC:
			len += WL_RM_REP_ELT_FIXED_LEN;
			data_len = sizeof(uint32);	/* uint32 basic report */;
			break;
		case WLC_RM_TYPE_CCA:
			len += WL_RM_REP_ELT_FIXED_LEN;
			data_len = sizeof(uint32);	/* uint32 CCA report */;
			break;
		case WLC_RM_TYPE_RPI:
			len += WL_RM_REP_ELT_FIXED_LEN;
			data_len = sizeof(wl_rm_rpi_rep_t);	/* RPI report */;
			break;
		case WLC_RM_TYPE_NONE:
			break;
		default:
			WL_ERROR(("wl%d: %s: Invalid wlc_rm_req type:%d\n",
				WLCWLUNIT(rm_info->wlc), __FUNCTION__, req->type));
			break;
		}
		if ((req->flags & (WLC_RM_FLAG_LATE |
			WLC_RM_FLAG_INCAPABLE |
			WLC_RM_FLAG_REFUSED))) {
			data_len = 0;
		}
		len += data_len;
	}

	if (len == 0) {
		WL_INFORM(("wl%d: wlc_rm_report_ioctl: empty ioctl RM report "
			   "after round of measurements.\n",
			   rm_info->wlc->pub->unit));
		return;
	}

	/* account for any pending report elts */
	len += rm_info->rm_ioctl_rep_len;
	rep_block = (uint8*)MALLOCZ(rm_info->wlc->osh, len);
	if (rep_block == NULL) {
		WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
			rm_info->wlc->pub->unit, __FUNCTION__, MALLOCED(rm_info->wlc->osh)));
		return;
	}

	rep_ptr = rep_block;

	/* merge any pending report elts */
	if (rm_info->rm_ioctl_rep_len > 0) {
		bcopy(rm_info->rm_ioctl_rep, rep_block, rm_info->rm_ioctl_rep_len);
		rep_ptr = rep_block + rm_info->rm_ioctl_rep_len;
		MFREE(rm_info->wlc->osh, rm_info->rm_ioctl_rep, rm_info->rm_ioctl_rep_len);
	}
	rm_info->rm_ioctl_rep_len = len;
	rm_info->rm_ioctl_rep = (wl_rm_rep_elt_t*)rep_block;

	/* format each report based on the request info and measured data */
	for (i = 0; i < count; i++) {
		req = req_block + i;

		rep_len = WL_RM_REP_ELT_FIXED_LEN;
		data_len = 0;
		data = rep_ptr + WL_RM_REP_ELT_FIXED_LEN;

		if ((req->flags & (WLC_RM_FLAG_LATE |
			WLC_RM_FLAG_INCAPABLE |
			WLC_RM_FLAG_REFUSED))) {
			measured = FALSE;
		} else {
			measured = TRUE;
		}

		switch (req->type) {
		case WLC_RM_TYPE_BASIC:
			rep.type = WL_RM_TYPE_BASIC;
			if (measured) {
				val = 0;
				data_len = sizeof(uint32);
				bcopy(&val, data, data_len);
			}
			break;
		case WLC_RM_TYPE_CCA:
			rep.type = WL_RM_TYPE_CCA;
			if (measured) {
				val = rm_state->cca_busy;
				data_len = sizeof(uint32);
				bcopy(&val, data, data_len);
			}
			break;
		case WLC_RM_TYPE_RPI:
			rep.type = WL_RM_TYPE_RPI;
			if (measured) {
				rpi_rep = (wl_rm_rpi_rep_t*)data;
				for (bin = 0; bin < WL_RPI_REP_BIN_NUM; bin++) {
					rpi_rep->rpi[bin] = (uint8)rm_state->rpi[bin];
					rpi_rep->rpi_max[bin] = wlc_rpi_bin_max[bin];
				}
				data_len = sizeof(wl_rm_rpi_rep_t);
			}
			break;
		default:
			rep_len = 0;
			break;
		}

		if (rep_len > 0) {
			rep.flags = 0;
			if (req->flags & WLC_RM_FLAG_PARALLEL)
				rep.flags |= WL_RM_FLAG_PARALLEL;
			if (req->flags & WLC_RM_FLAG_LATE)
				rep.flags |= WL_RM_FLAG_LATE;
			if (req->flags & WLC_RM_FLAG_INCAPABLE)
				rep.flags |= WL_RM_FLAG_INCAPABLE;
			if (req->flags & WLC_RM_FLAG_REFUSED)
				rep.flags |= WL_RM_FLAG_REFUSED;
			rep.chanspec = req->chanspec;
			rep.token = req->token;
			rep.tsf_h = req->tsf_h;
			rep.tsf_l = req->tsf_l;
			rep.dur = req->dur;
			rep.len = data_len;

			bcopy(&rep, rep_ptr, rep_len);
			rep_ptr = rep_ptr + (rep_len + data_len);
		}
	}

	ASSERT(rep_ptr - rep_block == len);
}

static bool
wlc_is_rm_abort_ioctl(rm_info_t *rm_info, wl_rm_req_t *req_set)
{
	bool is_rm_abort = FALSE;
	wlc_info_t *wlc = rm_info->wlc;

	/* if only 1 request and the type is ABORT, this is
	 * a request to ABORT any in-progress RM requests
	 */
	if ((req_set->count == 1) &&
	    (req_set->req[0].type == WL_RM_TYPE_ABORT)) {

		is_rm_abort = TRUE;

		/* abort radio measurement if enabled and active */
		if (WLRM_ENAB(wlc->pub) && WLC_RM_IN_PROGRESS(wlc)) {

			wlc_rm_abort(wlc);
		}
	}

	return is_rm_abort;
}

static int
wlc_rm_req_ioctl(rm_info_t *rm_info, wl_rm_req_t *req_set, int len)
{
	int i;
	int err = 0;
	int rm_req_size;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_rm_req_t *rm_req;
	wl_rm_req_t wl_req_set;
	wl_rm_req_elt_t wl_req;
	int8 type;
	wlc_info_t *wlc = rm_info->wlc;

	ASSERT(len >= (int)WL_RM_REQ_FIXED_LEN);
	bcopy(req_set, &wl_req_set, WL_RM_REQ_FIXED_LEN);
	len -= WL_RM_REQ_FIXED_LEN;

	if (wl_req_set.count == 0) {
		WL_ERROR(("wl%d: %s: invalid args, "
			  "zero count for Radio Measurement requests\n",
			  wlc->pub->unit, __FUNCTION__));
		return BCME_RANGE;
	}

	/* check for an RM ABORT and handle it. No need to proceed */
	if (wlc_is_rm_abort_ioctl(rm_info, req_set)) {
		return BCME_OK;
	}

	/* if not IDLE, then RM requests are already in progress */
	if (rm_state->step != WLC_RM_IDLE) {
		WL_ERROR(("wl%d: %s: Radio Measurement request "
			  "blocked since requests are in progress\n",
			  wlc->pub->unit, __FUNCTION__));
		return BCME_BUSY;
	}

	rm_req_size = wl_req_set.count * sizeof(wlc_rm_req_t);
	rm_req = (wlc_rm_req_t*)MALLOCZ(wlc->osh, rm_req_size);
	if (rm_req == NULL) {
		WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
			wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
		return BCME_NORESOURCE;
	}

	for (i = 0; i < (int)wl_req_set.count; i++) {
		if (len < (int)sizeof(wl_rm_req_elt_t)) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		bcopy(&req_set->req[i], &wl_req, sizeof(wl_rm_req_elt_t));
		len -= sizeof(wl_rm_req_elt_t);

		switch (wl_req.type) {
		case WL_RM_TYPE_BASIC:
			type = WLC_RM_TYPE_BASIC;
			break;
		case WL_RM_TYPE_CCA:
			type = WLC_RM_TYPE_CCA;
			break;
		case WL_RM_TYPE_RPI:
			type = WLC_RM_TYPE_RPI;
			break;
		default:
			type = WLC_RM_TYPE_NONE;
			err = BCME_RANGE;
			break;
		}
		if (err) break;

		rm_req[i].type	= type;
		if (wl_req.flags & WL_RM_FLAG_PARALLEL)
			rm_req[i].flags	|= WLC_RM_FLAG_PARALLEL;
		rm_req[i].token	= wl_req.token;
		rm_req[i].chanspec = wl_req.chanspec;
		rm_req[i].tsf_h	= wl_req.tsf_h;
		rm_req[i].tsf_l	= wl_req.tsf_l;
		rm_req[i].dur	= wl_req.dur;
	}

	if (err) {
		MFREE(wlc->osh, rm_req, rm_req_size);
		return err;
	}

	rm_state->report_class = WLC_RM_CLASS_IOCTL;
	rm_state->broadcast = FALSE;
	rm_state->token = wl_req_set.token;
	rm_state->req_count = wl_req_set.count;
	rm_state->req = rm_req;
	rm_state->cb = wl_req_set.cb;
	rm_state->cb_arg = wl_req_set.cb_arg;

	/* free radio measurement ioctl reports */
	if (rm_info->rm_ioctl_rep) {
		MFREE(wlc->osh, rm_info->rm_ioctl_rep, rm_info->rm_ioctl_rep_len);
		rm_info->rm_ioctl_rep_len = 0;
		rm_info->rm_ioctl_rep = NULL;
	}

	wlc_rm_start(wlc);

	return err;
}

void
wlc_rm_state_upd(rm_info_t *rm_info, uint state)
{
	bool was_in_progress;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	wlc_info_t *wlc = rm_info->wlc;

	if (rm_state->step == state)
		return;

	WL_INFORM(("wlc_rm_state_upd; change from %d to %d\n", rm_state->step, state));
	was_in_progress = WLC_RM_IN_PROGRESS(wlc);
	rm_state->step = state;
	if (WLC_RM_IN_PROGRESS(wlc) != was_in_progress)
		wlc_phy_hold_upd(WLC_PI(wlc), PHY_HOLD_FOR_RM, !was_in_progress);

	return;
}
bool
wlc_rm_cca_start(wlc_info_t *wlc, uint32 dur)
{
	rm_info_t *rm_info = (rm_info_t *)wlc->rm_info;
	wlc_rm_req_state_t* rm_state = rm_info->rm_state;
	uint32 cca_start_h;

	/*
	 * Scan takes priority over RM. Yield if scan is in progress
	 */

	/*
	 * TSF timestamp - Actual measurement start time
	 */
	wlc_bmac_read_tsf(wlc->hw, &rm_state->cca_start_l, &cca_start_h);

	/*
	 * Read previous stats
	 */
	wlc_bmac_cca_stats_read(wlc->hw, &wlc->util_last_counts);
	rm_state->cca_active = TRUE;
	WL_EAP_TRC_RXTXUTIL_MSG(("wl%d: %s: start %u: cca_active %d dur %u\n",
		wlc->pub->unit, __FUNCTION__, rm_state->cca_start_l, rm_state->cca_active, dur));
	/*
	 * Arm the timer
	 */
	wl_add_timer(wlc->wl, rm_info->rm_cca_timer, dur, 0);
	return TRUE;
}

static void
wlc_rm_cca_timer_cb(void *arg)
{
	rm_info_t *rm_info = (rm_info_t*)arg;
	wlc_info_t *wlc = rm_info->wlc;

	/*
	 * Reset PHY measure hold flag and invoke rm_cca_complete
	 */
	wlc_rm_cca_end(wlc);
}
#endif /* STA && WLRM */
