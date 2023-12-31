/*
 * BSS Configuration routines for
 * Broadcom 802.11abg Networking Device Driver
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
 * $Id: wlc_bsscfg.c 810655 2022-04-12 06:08:44Z $
 */

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <bcmtlv.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_pub.h>
#include <wlc_bsscfg.h>
#include <wlc.h>
#include <wlc_dbg.h>
#include <wlc_scb.h>
#include <wlc_mbss.h>
#include <wl_export.h>
#include <wlc_ap.h>
#include <bcm_notif_pub.h>
#include <phy_utils_api.h>
#include <wlc_cubby.h>
#include <wlc_event_utils.h>
#include <wlc_dump.h>
#ifdef WL_GLOBAL_RCLASS
#include <bcmwifi_rclass.h>
#endif /* WL_GLOBAL_RCLASS */
#ifdef WDS
#include <wlc_wds.h>
#endif /* WDS */
#include <wlc_assoc.h>
#ifdef MBO_AP
#include <wlc_mbo.h>
#endif /* MBO_AP */
#include <wlc_pm.h>
#if BAND6G || (defined(WL_OCE) && defined(WL_OCE_AP))
#include <oce.h>
#include <fils.h>
#if defined(WL_OCE) && defined(WL_OCE_AP)
#include <wlc_oce.h>
#endif /* WL_OCE && WL_OCE_AP */
#endif /* WL_WIFI_6GHZ || (WL_OCE && WL_OCE_AP) */
#include <wlc_txbf.h>
#if defined(WLDFS) && defined(BGDFS)
#include <wlc_dfs.h>
#endif /* WLDFS && BGDFS */
#if defined(TD_STEER) && defined(CONFIG_TENDA_PRIVATE_WLAN)
#include "td_wl_core_symb.h"
#if (!defined(CONFIG_UGW_BCM_DONGLE)) || defined(DONGLEBUILD)
#include <td_multiap_steer.h>
#endif
#endif

/*
 * Structure to store the interface_create specific handler registration
 */

typedef struct bsscfg_interface_info {
    iface_create_hdlr_fn    iface_create_fn;    /* Func ptr for creating the interface */
    iface_remove_hdlr_fn    iface_remove_fn;    /* Func ptr for removing the interface */
    void            *iface_module_ctx;    /* Context of the module */
} bsscfg_interface_info_t;

/** structure for storing global bsscfg module state */
struct bsscfg_module {
    wlc_info_t    *wlc;        /**< pointer to wlc */
    uint        cfgtotsize;    /**< total bsscfg size including container */
    uint8        ext_cap_max;    /**< extend capability max length */
    wlc_cubby_info_t *cubby_info;    /**< cubby client info */
    bcm_notif_h    state_upd_notif_hdl;    /**< state change notifier handle. */
    bcm_notif_h pretbtt_query_hdl;    /**< pretbtt query handle. */
    bcm_notif_h  rxbcn_nh; /* beacon notifier handle. */
    uint16    next_bsscfg_ID;

    /* TO BE REPLACED, LEAVE IT AT THE END */
    bcm_notif_h up_down_notif_hdl;    /**< up/down notifier handle. */
    bsscfg_interface_info_t *iface_info;    /**< bsscfg interface info */
    wl_interface_type_t iface_info_max;    /**< Maximum number of interface type supported */
};

/** Flags that should not be cleared on AP bsscfg up */
#define WLC_BSSCFG_PERSIST_FLAGS (0 | \
        WLC_BSSCFG_WME_DISABLE | \
        WLC_BSSCFG_PRESERVE | \
        WLC_BSSCFG_NOBCMC | \
        WLC_BSSCFG_NOIF | \
        WLC_BSSCFG_11N_DISABLE | \
        WLC_BSSCFG_11H_DISABLE | \
        WLC_BSSCFG_NATIVEIF | \
        WLC_BSSCFG_SRADAR_ENAB | \
        WLC_BSSCFG_DYNBCN | \
        WLC_BSSCFG_AP_NORADAR_CHAN | \
        WLC_BSSCFG_BSSLOAD_DISABLE | \
        WLC_BSSCFG_TX_SUPR_ENAB | \
        WLC_BSSCFG_NO_AUTHENTICATOR | \
        WLC_BSSCFG_PSINFO | \
        WLC_BSSCFG_VHT_DISABLE | \
        WLC_BSSCFG_HE_DISABLE | \
        0)
/* Clear non-persistant flags; by default, HW beaconing and probe resp */
#define WLC_BSSCFG_FLAGS_INIT(cfg) do { \
        (cfg)->flags &= WLC_BSSCFG_PERSIST_FLAGS; \
        (cfg)->flags |= (WLC_BSSCFG_HW_BCN | WLC_BSSCFG_HW_PRB); \
    } while (0)

/* Flags2 that should not be cleared on AP bsscfg up */
#define WLC_BSSCFG_PERSIST_FLAGS2 (0 | \
        WLC_BSSCFG_FL2_MFP_CAPABLE | \
        WLC_BSSCFG_FL2_MFP_REQUIRED | \
        WLC_BSSCFG_FL2_FBT_1X | \
        WLC_BSSCFG_FL2_FBT_PSK | \
        WLC_BSSCFG_FL2_NO_KM | \
        WLC_BSSCFG_FL2_SPLIT_ASSOC_REQ | \
        WLC_BSSCFG_FL2_SPLIT_ASSOC_RESP | \
        WLC_BSSCFG_FL2_BLOCK_HE | \
        WLC_BSSCFG_FL2_BLOCK_HE_MAC | \
        WLC_BSSCFG_FL2_RSN_CAP_SPPC | \
        WLC_BSSCFG_FL2_RSN_CAP_SPPR | \
        WLC_BSSCFG_FL2_BLOCK_STA_ON_MAPBH | \
        WLC_BSSCFG_FL2_EMPTY_SSID_ON_CLOSED_NW | \
        0)
/* Clear non-persistant flags2 */
#define WLC_BSSCFG_FLAGS2_INIT(cfg) do { \
        (cfg)->flags2 &= WLC_BSSCFG_PERSIST_FLAGS2; \
    } while (0)

/* debugging macros */
#define WL_BCB(x) WL_APSTA_UPDN(x)

/* Local Functions */

static int wlc_bsscfg_wlc_up(void *ctx);

#if defined(BCMDBG)
static int wlc_bsscfg_dump(wlc_info_t *wlc, struct bcmstrbuf *b);
#endif
#if defined(BCMDBG) || defined(DNG_DBGDUMP)
static int
wlc_bsscfg_multimac_dump(wlc_info_t *wlc, struct bcmstrbuf *b);
#endif

static int wlc_bsscfg_init_int(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
    wlc_bsscfg_type_t *type, uint flags, uint32 flags2, struct ether_addr *ea);
static int wlc_bsscfg_init_intf(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);

static void wlc_bsscfg_deinit_intf(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);

static int wlc_bsscfg_bcmcscballoc(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);

#ifdef AP
static int wlc_bsscfg_ap_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
static void wlc_bsscfg_ap_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#endif
#ifdef STA
static int wlc_bsscfg_sta_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
static void wlc_bsscfg_sta_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#endif

static int wlc_bsscfg_alloc_int(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, int idx,
    wlc_bsscfg_type_t *type, uint flags, uint32 flags2, struct ether_addr *ea);

static bool wlc_bsscfg_preserve(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

static wlc_bsscfg_t *wlc_bsscfg_malloc(wlc_info_t *wlc);
static void wlc_bsscfg_mfree(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
static void wlc_bsscfg_info_get(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_info_t *info);
static void wlc_bsscfg_set_main_ap(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

/* module */

/**
 * BSS CFG specific IOVARS
 * Creating/Removing an interface is all about creating and removing a
 * BSSCFG. So its better if the interface create/remove IOVARS live here.
 */
enum {
    IOV_INTERFACE_CREATE = 1,    /* Creat an interface i.e bsscfg */
    IOV_INTERFACE_REMOVE = 2,    /* Remove an interface i.e bsscfg */
    IOV_BSSCFG_INFO = 3,        /* bsscfg info */
    IOV_LAST            /* In case of a need to check Max ID number */
};

/* BSSCFG IOVars */
static const bcm_iovar_t wlc_bsscfg_iovars[] = {
    {"interface_create", IOV_INTERFACE_CREATE,
    IOVF_OPEN_ALLOW, 0, IOVT_BUFFER, sizeof(uint32)},
    {"interface_remove", IOV_INTERFACE_REMOVE, IOVF_OPEN_ALLOW, 0,
    IOVT_VOID, 0},
    {"bsscfg_info", IOV_BSSCFG_INFO, 0, 0, IOVT_BUFFER, sizeof(wlc_bsscfg_info_t)},
    {NULL, 0, 0, 0, 0, 0}
};

/* This includes the auto generated ROM IOCTL/IOVAR patch handler C source file (if auto patching is
 * enabled). It must be included after the prototypes and declarations above (since the generated
 * source file may reference private constants, types, variables, and functions).
 */
#include <wlc_patch.h>
#ifdef WL_TRAFFIC_THRESH
#define WL_TRAFFIC_THRESH_SCB_NS 5
#define WL_TRAFFIC_THRESH_SCB_TH 30
#define WL_TRAFFIC_THRESH_BSS_NS 5
#define WL_TRAFFIC_THRESH_BSS_TH 150
#define WL_TRAFFIC_THRESH_BSS_TO_NS 5
#define WL_TRAFFIC_THRESH_BSS_TO_TH 250
#define WL_FAR_STA_RSSI -75
void
wlc_bsscfg_traffic_thresh_set_defaults(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int i;
    for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
        bsscfg->trf_scb_params[i].num_secs = WL_TRAFFIC_THRESH_SCB_NS;
        bsscfg->trf_scb_params[i].thresh = WL_TRAFFIC_THRESH_SCB_TH;
        bsscfg->trf_cfg_params[i].num_secs = WL_TRAFFIC_THRESH_BSS_NS;
        bsscfg->trf_cfg_params[i].thresh = WL_TRAFFIC_THRESH_BSS_TH;
        bsscfg->trf_cfg_data[i].cur = 0;
        bsscfg->trf_cfg_data[i].count = 0;
    }
    bsscfg->trf_cfg_params[WL_TRF_TO].num_secs = WL_TRAFFIC_THRESH_BSS_TO_NS;
    bsscfg->trf_cfg_params[WL_TRF_TO].thresh = WL_TRAFFIC_THRESH_BSS_TO_TH;
    bsscfg->trf_scb_enable = ((1 << WL_TRF_VI) | (1 << WL_TRF_VO));
    bsscfg->trf_cfg_enable = ((1 << WL_TRF_VI) | (1 << WL_TRF_VO));
}
int
wlc_bsscfg_traffic_thresh_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int i;
    for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
        if (bsscfg->trf_cfg_params[i].num_secs != 0) {
            if ((bsscfg->trf_cfg_data[i].num_data = MALLOCZ(wlc->osh,
                    sizeof(uint16) * bsscfg->trf_cfg_params[i].num_secs))
                    == NULL) {
                WL_ERROR(("wl%d: not enough memory\n",
                        wlc->pub->unit));
                return BCME_NORESOURCE;
            }
        }
    }
    return BCME_OK;
}
void
wlc_bsscfg_traffic_thresh_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int i;
    for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
        if (bsscfg->trf_cfg_data[i].num_data != NULL) {
            MFREE(wlc->osh, bsscfg->trf_cfg_data[i].num_data,
                sizeof(uint16) * bsscfg->trf_cfg_params[i].num_secs);
            bsscfg->trf_cfg_data[i].num_data = NULL;
        }
    }
}
#endif /* WL_TRAFFIC_THRESH */

static int
wlc_bsscfg_doiovar(void *hdl, uint32 actionid,
    void *params, uint p_len, void *arg, uint len, uint val_size, struct wlc_if *wlcif)
{
    bsscfg_module_t *bcmh = hdl;
    wlc_info_t *wlc = bcmh->wlc;
    wlc_bsscfg_t *bsscfg;
    int err = 0;
    int32 int_val = 0;

    BCM_REFERENCE(val_size);

    /* update bsscfg w/provided interface context */
    bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);
    ASSERT(bsscfg != NULL);

    /* convenience int and bool vals for first 8 bytes of buffer */
    if (p_len >= (int)sizeof(int_val))
        bcopy(params, &int_val, sizeof(int_val));

    /* update wlcif pointer */
    if (wlcif == NULL)
        wlcif = bsscfg->wlcif;
    ASSERT(wlcif != NULL);
    /* Do the actual parameter implementation */
    switch (actionid) {

    case IOV_GVAL(IOV_INTERFACE_CREATE): {
        wl_interface_create_t if_buf;
        wl_interface_info_t *wl_info = arg;
        int32 *ret_int_ptr = arg;

        if (wl_info == NULL) {
            err = BCME_BADADDR;
            break;
        }

        bcopy((char *)params, (char*)&if_buf, sizeof(if_buf));

        /* version check */
        if (if_buf.ver != WL_INTERFACE_CREATE_VER) {
            *ret_int_ptr = WL_INTERFACE_CREATE_VER;
            err = BCME_VERSION;
            break;
        }

        if (if_buf.flags & WL_INTERFACE_IF_INDEX_USE) {
            /* Set the if_index in software context */
            if (if_buf.if_index == 0) {
                err = BCME_RANGE;
                break;
            }
        }

        if ((uint)p_len < sizeof(if_buf)) {
            WL_ERROR(("wl%d: input buffer too short\n", wlc->pub->unit));
            err = BCME_BUFTOOSHORT;
            break;
        }

        if (if_buf.iftype >= bcmh->iface_info_max) {
            WL_ERROR(("wl%d: invalid interface type %d\n", WLCWLUNIT(wlc),
                if_buf.iftype));
            err = BCME_RANGE;
            break;
        }

        if (bcmh->iface_info[if_buf.iftype].iface_create_fn == NULL) {
            WL_ERROR(("wl%d: Callback function not registered! iftype:%d\n",
                WLCWLUNIT(wlc), if_buf.iftype));
            err = BCME_NOTFOUND;
            break;
        }
        bcmh->iface_info[if_buf.iftype].iface_create_fn(
            bcmh->iface_info[if_buf.iftype].iface_module_ctx, &if_buf, wl_info, &err);

        WL_INFORM(("wl%d: Interface Create status = %d\n", WLCWLUNIT(wlc), err));
        break;
    }

    case IOV_SVAL(IOV_INTERFACE_REMOVE):
        if (BSSCFG_IS_PRIMARY(bsscfg)) {
            WL_ERROR(("wl%d: if_del failed: cannot delete primary bsscfg\n",
                WLCWLUNIT(wlc)));
            err = BCME_EPERM;
            break;
        }
        if ((wlcif->iface_type < bcmh->iface_info_max) &&
            (bcmh->iface_info[wlcif->iface_type].iface_remove_fn)) {
            err = bcmh->iface_info[wlcif->iface_type].iface_remove_fn(wlc, wlcif);
        } else {
            WL_ERROR(("wl%d: Failed to remove iface_type:%d\n", WLCWLUNIT(wlc),
                wlcif->iface_type));
            err =  BCME_NOTFOUND;
        }

        WL_INFORM(("wl%d: Interface remove status = %d\n", WLCWLUNIT(wlc), err));
        break;

    case IOV_GVAL(IOV_BSSCFG_INFO):
        wlc_bsscfg_info_get(wlc, bsscfg, (wlc_bsscfg_info_t *)arg);
        break;

    default:
        err = BCME_UNSUPPORTED;
        break;
    }

    return err;
}

bsscfg_module_t *
BCMATTACHFN(wlc_bsscfg_attach)(wlc_info_t *wlc)
{
    bsscfg_module_t *bcmh;

    if ((bcmh = MALLOCZ(wlc->osh, sizeof(bsscfg_module_t))) == NULL) {
        WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
            wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }
    bcmh->wlc = wlc;

    if ((bcmh->cubby_info = wlc_cubby_attach(wlc->osh, wlc->pub->unit,
            wlc->pub->tunables->maxbsscfgcubbies)) == NULL) {
        WL_ERROR(("wl%d: %s: wlc_cubby_attach failed\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    /* Create notification list for bsscfg state change event. */
    if (bcm_notif_create_list(wlc->notif, &bcmh->state_upd_notif_hdl) != BCME_OK) {
        WL_ERROR(("wl%d: %s: bcm_notif_create_list failed (updn)\n",
                 wlc->pub->unit, __FUNCTION__));
        goto fail;
    }
    /* Create notification list for bsscfg up/down events. */
    if (bcm_notif_create_list(wlc->notif, &bcmh->up_down_notif_hdl) != BCME_OK) {
        WL_ERROR(("wl%d: %s: bcm_notif_create_list failed (updn)\n",
                 wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    /* Create callback list for pretbtt query. */
    if (bcm_notif_create_list(wlc->notif, &bcmh->pretbtt_query_hdl) != BCME_OK) {
        WL_ERROR(("wl%d: %s: bcm_notif_create_list failed (pretbtt)\n",
                 wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    /* create notification list for beacons. */
    if ((bcm_notif_create_list(wlc->notif, &bcmh->rxbcn_nh)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: bcm_notif_create_list() failed\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    /* Interface create list allocation */
    bcmh->iface_info = MALLOCZ(wlc->osh,
        (sizeof(*bcmh->iface_info) * WL_INTERFACE_TYPE_MAX));
    if (bcmh->iface_info == NULL) {
        WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
            WLCWLUNIT(wlc), __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }
    bcmh->iface_info_max = WL_INTERFACE_TYPE_MAX;

    bcmh->cfgtotsize = ROUNDUP(sizeof(wlc_bsscfg_t), PTRSZ);
    bcmh->cfgtotsize += ROUNDUP(DOT11_EXTCAP_LEN_MAX, PTRSZ);
    bcmh->ext_cap_max = DOT11_EXTCAP_LEN_MAX;

#if defined(BCMDBG)
    wlc_dump_register(wlc->pub, "bsscfg", (dump_fn_t)wlc_bsscfg_dump, (void *)wlc);
#endif

#if defined(BCMDBG) || defined(DNG_DBGDUMP)
    if (wlc_dump_register(wlc->pub, "multimac", (dump_fn_t)wlc_bsscfg_multimac_dump,
        (void*)wlc) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_dumpe_register() failed\n",
                  wlc->pub->unit, __FUNCTION__));
    }
#endif
    if (wlc_module_register(wlc->pub, wlc_bsscfg_iovars, "bsscfg", bcmh,
        wlc_bsscfg_doiovar, NULL, wlc_bsscfg_wlc_up,
        NULL) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_module_register() failed\n",
                  wlc->pub->unit, __FUNCTION__));
        goto fail;
    }
    return bcmh;

fail:
    MODULE_DETACH(bcmh, wlc_bsscfg_detach);
    return NULL;
}

void
BCMATTACHFN(wlc_bsscfg_detach)(bsscfg_module_t *bcmh)
{
    wlc_info_t *wlc;

    if (bcmh == NULL)
        return;

    wlc = bcmh->wlc;

    wlc_module_unregister(wlc->pub, "bsscfg", bcmh);

    /* Delete event notification list. */
    if (bcmh->pretbtt_query_hdl != NULL)
        bcm_notif_delete_list(&bcmh->pretbtt_query_hdl);
    if (bcmh->up_down_notif_hdl != NULL)
        bcm_notif_delete_list(&bcmh->up_down_notif_hdl);
    if (bcmh->state_upd_notif_hdl != NULL)
        bcm_notif_delete_list(&bcmh->state_upd_notif_hdl);
    if (bcmh->rxbcn_nh != NULL)
        bcm_notif_delete_list(&bcmh->rxbcn_nh);
    if (bcmh->iface_info != NULL) {
        MFREE(wlc->osh, bcmh->iface_info,
            (sizeof(*bcmh->iface_info) * bcmh->iface_info_max));
    }
    MODULE_DETACH(bcmh->cubby_info, wlc_cubby_detach);
    MFREE(wlc->osh, bcmh, sizeof(bsscfg_module_t));
}

static int
wlc_bsscfg_wlc_up(void *ctx)
{
#ifdef STA
    bsscfg_module_t *bcmh = (bsscfg_module_t *)ctx;
    wlc_info_t *wlc = bcmh->wlc;
    int idx;
    wlc_bsscfg_t *cfg;

#ifdef WLMCNX
    if (MCNX_ENAB(wlc->pub))
        return BCME_OK;
#endif

    /* Update tsf_cfprep if associated and up */
    FOREACH_AS_STA(wlc, idx, cfg) {
        if (cfg->up) {
            uint32 bi;

            /* get beacon period from bsscfg and convert to uS */
            bi = cfg->current_bss->beacon_period << 10;
            /* update the tsf_cfprep register */
            /* since init path would reset to default value */
            W_REG(wlc->osh, D11_CFPRep(wlc), (bi << CFPREP_CBI_SHIFT));

            /* Update maccontrol PM related bits */
            wlc_set_ps_ctrl(cfg);

            break;
        }
    }
#endif /* STA */

    return BCME_OK;
}

#ifdef BCMDBG
/* undefine the BCMDBG helper macros so they will not interfere with the function definitions */
#undef wlc_bsscfg_cubby_reserve
#undef wlc_bsscfg_cubby_reserve_ext
#endif

/**
 * Reduced parameter version of wlc_bsscfg_cubby_reserve_ext().
 *
 * Return value: negative values are errors.
 */
#ifdef BCMDBG
int
BCMATTACHFN(wlc_bsscfg_cubby_reserve)(wlc_info_t *wlc, uint size,
    bsscfg_cubby_init_t fn_init, bsscfg_cubby_deinit_t fn_deinit,
    bsscfg_cubby_dump_t fn_dump, void *ctx,
    const char *func)
#else
int
BCMATTACHFN(wlc_bsscfg_cubby_reserve)(wlc_info_t *wlc, uint size,
    bsscfg_cubby_init_t fn_init, bsscfg_cubby_deinit_t fn_deinit,
    bsscfg_cubby_dump_t fn_dump, void *ctx)
#endif /* BSSCFG */
{
    bsscfg_cubby_params_t params;
    int ret;

    bzero(&params, sizeof(params));

    params.context = ctx;
    params.fn_init = fn_init;
    params.fn_deinit = fn_deinit;
    params.fn_dump = fn_dump;

#ifdef BCMDBG
    ret = wlc_bsscfg_cubby_reserve_ext(wlc, size, &params, func);
#else
    ret = wlc_bsscfg_cubby_reserve_ext(wlc, size, &params);
#endif
    return ret;
}

/**
 * Multiple modules have the need of reserving some private data storage related to a specific BSS
 * configuration. During ATTACH time, this function is called multiple times, typically one time per
 * module that requires this storage. This function does not allocate memory, but calculates values
 * to be used for a future memory allocation instead. The private data is located at the end of a
 * bsscfg.
 *
 * Returns the offset of the private data to the beginning of an allocated bsscfg structure,
 * negative values are errors.
 */
int
#ifdef BCMDBG
BCMATTACHFN(wlc_bsscfg_cubby_reserve_ext)(wlc_info_t *wlc, uint size,
    bsscfg_cubby_params_t *params, const char *func)
#else
BCMATTACHFN(wlc_bsscfg_cubby_reserve_ext)(wlc_info_t *wlc, uint size,
    bsscfg_cubby_params_t *params)
#endif
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    wlc_cubby_fn_t fn;
    int offset;

    ASSERT((bcmh->cfgtotsize % PTRSZ) == 0);

    bzero(&fn, sizeof(fn));
    fn.fn_init = (cubby_init_fn_t)params->fn_init;
    fn.fn_deinit = (cubby_deinit_fn_t)params->fn_deinit;
    fn.fn_secsz = (cubby_secsz_fn_t)NULL;
    fn.fn_dump = (cubby_dump_fn_t)params->fn_dump;
#if defined(BCMDBG)
    fn.name = func;
#endif

    if ((offset = wlc_cubby_reserve(bcmh->cubby_info, size, &fn, params->context)) < 0) {
        WL_ERROR(("wl%d: %s: wlc_cubby_reserve failed with err %d\n",
                  wlc->pub->unit, __FUNCTION__, offset));
        return offset;
    }

    return bcmh->cfgtotsize + offset;
}

/**
 * wlc_bsscfg_state_upd_register()
 *
 * This function registers a callback that will be invoked when a bsscfg
 * changes its state (enable/disable/up/down).
 *
 * Parameters
 *    wlc       Common driver context.
 *    fn        Callback function  to invoke on state change event.
 *    arg       Client specified data that will provided as param to the callback.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int
BCMATTACHFN(wlc_bsscfg_state_upd_register)(wlc_info_t *wlc,
    bsscfg_state_upd_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->state_upd_notif_hdl;

    return bcm_notif_add_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

/**
 * wlc_bsscfg_state_upd_unregister()
 *
 * This function unregisters a bsscfg state change event callback.
 *
 * Parameters
 *    wlc       Common driver context.
 *    fn        Callback function that was previously registered.
 *    arg       Client specified data that was previously registerd.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int
BCMATTACHFN(wlc_bsscfg_state_upd_unregister)(wlc_info_t *wlc,
    bsscfg_state_upd_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->state_upd_notif_hdl;

    return bcm_notif_remove_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

/**
 * wlc_bsscfg_updown_register()
 *
 * This function registers a callback that will be invoked when either a bsscfg
 * up or down event occurs.
 *
 * Parameters
 *    wlc       Common driver context.
 *    callback  Callback function  to invoke on up/down events.
 *    arg       Client specified data that will provided as param to the callback.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int
BCMATTACHFN(wlc_bsscfg_updown_register)(wlc_info_t *wlc, bsscfg_up_down_fn_t callback, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->up_down_notif_hdl;

    return bcm_notif_add_interest(hdl, (bcm_notif_client_callback)callback, arg);
}

/**
 * wlc_bsscfg_updown_unregister()
 *
 * This function unregisters a bsscfg up/down event callback.
 *
 * Parameters
 *    wlc       Common driver context.
 *    callback  Callback function that was previously registered.
 *    arg       Client specified data that was previously registerd.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int
BCMATTACHFN(wlc_bsscfg_updown_unregister)(wlc_info_t *wlc, bsscfg_up_down_fn_t callback, void *arg)
{
    bcm_notif_h hdl;

    ASSERT(wlc->bcmh != NULL);
    hdl = wlc->bcmh->up_down_notif_hdl;

    return bcm_notif_remove_interest(hdl, (bcm_notif_client_callback)callback, arg);
}

/**
 * wlc_bsscfg_iface_register()
 *
 * This function registers a callback that will be invoked while creating an interface.
 *
 * Parameters
 *    wlc       Common driver context.
 *    if_type    Type of interface to be created.
 *    callback  Invoke respective interface creation callback function.
 *    ctx       Context of the respective callback function.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int
BCMATTACHFN(wlc_bsscfg_iface_register)(wlc_info_t *wlc, wl_interface_type_t if_type,
        iface_create_hdlr_fn create_cb, iface_remove_hdlr_fn remove_cb, void *ctx)
{
    bsscfg_interface_info_t *iface_info = wlc->bcmh->iface_info;

    if (if_type >= wlc->bcmh->iface_info_max) {
        WL_ERROR(("wl%d: invalid interface type\n", WLCWLUNIT(wlc)));
        return (BCME_RANGE);
    }

    iface_info[if_type].iface_create_fn = create_cb;
    iface_info[if_type].iface_remove_fn = remove_cb;
    iface_info[if_type].iface_module_ctx = ctx;

    return (BCME_OK);
}

/**
 * wlc_bsscfg_iface_unregister()
 *
 * This function unregisters a callback that will be invoked while creating an interface.
 *
 * Parameters
 *    wlc       Common driver context.
 *    if_type    Type of interface to be created.
 * Returns:
 *    BCME_OK on success, else BCME_xxx error code.
 */
int32
BCMATTACHFN(wlc_bsscfg_iface_unregister)(wlc_info_t *wlc, wl_interface_type_t if_type)
{
    bsscfg_interface_info_t *iface_info = wlc->bcmh->iface_info;

    if (if_type >= wlc->bcmh->iface_info_max) {
        WL_ERROR(("wl%d: invalid interface type\n", WLCWLUNIT(wlc)));
        return (BCME_RANGE);
    }

    iface_info[if_type].iface_create_fn = NULL;
    iface_info[if_type].iface_remove_fn = NULL;
    iface_info[if_type].iface_module_ctx = NULL;

    return (BCME_OK);
}

/**
 * These functions register/unregister/invoke the callback
 * when a pretbtt query is requested.
 */
int
BCMATTACHFN(wlc_bss_pretbtt_query_register)(wlc_info_t *wlc, bss_pretbtt_query_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->pretbtt_query_hdl;

    return bcm_notif_add_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

int
BCMATTACHFN(wlc_bss_pretbtt_query_unregister)(wlc_info_t *wlc, bss_pretbtt_query_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->pretbtt_query_hdl;

    return bcm_notif_remove_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

static void
wlc_bss_pretbtt_max(uint *max_pretbtt, bss_pretbtt_query_data_t *notif_data)
{
    if (notif_data->pretbtt > *max_pretbtt)
        *max_pretbtt = notif_data->pretbtt;
}

uint
wlc_bss_pretbtt_query(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint minval)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    bss_pretbtt_query_data_t notif_data;
    uint max_pretbtt = minval;

    notif_data.cfg = cfg;
    notif_data.pretbtt = minval;
    bcm_notif_signal_ex(bcmh->pretbtt_query_hdl, &notif_data,
                        (bcm_notif_server_callback)wlc_bss_pretbtt_max,
                        (bcm_notif_client_data)&max_pretbtt);

    return max_pretbtt;
}

/* on-channel beacon reception notification */
int
BCMATTACHFN(wlc_bss_rx_bcn_register)(wlc_info_t *wlc, bss_rx_bcn_notif_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->rxbcn_nh;

    return bcm_notif_add_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

int
BCMATTACHFN(wlc_bss_rx_bcn_unregister)(wlc_info_t *wlc, bss_rx_bcn_notif_fn_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->bcmh->rxbcn_nh;

    return bcm_notif_remove_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

void
wlc_bss_rx_bcn_signal(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct scb *scb,
    wlc_d11rxhdr_t *wrxh, uint8 *plcp, struct dot11_management_header *hdr,
    uint8 *body, int bcn_len, bcn_notif_data_ext_t *data_ext)
{
    bcm_notif_h hdl = wlc->bcmh->rxbcn_nh;
    bss_rx_bcn_notif_data_t data;

    data.cfg = cfg;
    data.scb = scb;
    data.wrxh = wrxh;
    data.plcp = plcp;
    data.hdr = hdr;
    data.body = body;
    data.bcn_len = bcn_len;
    data.data_ext = data_ext;
    bcm_notif_signal(hdl, &data);
}

/** Return the number of AP bsscfgs that are UP */
int
wlc_ap_bss_up_count(wlc_info_t *wlc)
{
    uint16 i, apbss_up = 0;
    wlc_bsscfg_t *bsscfg;

    FOREACH_UP_AP(wlc, i, bsscfg) {
        apbss_up++;
    }

    return apbss_up;
}

int
wlc_bsscfg_up(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    int ret = BCME_OK;
    bsscfg_state_upd_data_t st_data;
    bool stop = FALSE;

    ASSERT(cfg != NULL);
    ASSERT(cfg->enable);

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_up(%s): stas/aps/associated %d/%d/%d"
                   "flags = 0x%x\n", wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
                   (BSSCFG_AP(cfg) ? "AP" : "STA"),
                   wlc->stas_associated, wlc->aps_associated, wlc->pub->associated,
                   cfg->flags));

    bzero(&st_data, sizeof(st_data));
    st_data.cfg = cfg;
    st_data.old_enable = cfg->enable;
    st_data.old_up = cfg->up;

#ifdef AP
    if (BSSCFG_AP(cfg)) {
        /* AP mode operation must have the driver up before bringing
         * up a configuration
         */
        if (!wlc->pub->up) {
            ret = BCME_NOTUP;
            stop = TRUE;
            goto end;
        }

        /* wlc_ap_up() only deals with getting cfg->target_bss setup correctly.
         * This should not have any affects that need to be undone even if we
         * don't end up bring the AP up.
         */
        ret = wlc_ap_up(wlc->ap, cfg);
        if (ret != BCME_OK) {
            stop = TRUE;
            goto end;
        }
        /* if radio is in apsta mode and upstream connection is not present,
         * check home chanspec or target chanspec of ap bsscfg. Based on
         * different keep_ap_up options decide whether ap bsscfg up is
         * required or not.
         */
        ret = wlc_bsscfg_apsta_is_downstream_ap_up_possible(wlc, cfg);
        if (ret != BCME_OK) {
            stop = TRUE;
            goto end;
        }

        /* No SSID configured yet... */
        if (cfg->SSID_len == 0) {
            cfg->up = FALSE;
            /* XXX Do not return an error for this case.  MacOS UTF
             * tests first enable the bsscfg and then set its SSID.
             */
            stop = TRUE;
            goto end;
        }

#ifdef STA
        if (APSTA_ENAB(wlc->pub) && !(wlc->primary_bsscfg->associated)) {
            wlc_set_home_chanspec(wlc, cfg->target_bss->chanspec);
        }

        /* defer to any STA association in progress */
        if (APSTA_ENAB(wlc->pub) && !wlc_apup_allowed(wlc) &&
            (wlc->keep_ap_up || !(wlc->primary_bsscfg->associated))) {
            cfg->up = FALSE;
            stop = TRUE;
            /* XXX Do not return an error for this case.
             */
            goto end;
        }
#endif /* STA */

        /* it's ok to update beacon from onwards */
        /* bsscfg->flags &= ~WLC_BSSCFG_DEFER_BCN; */
        /* will be down next anyway... */

        /* Init (non-persistant) flags */
        WLC_BSSCFG_FLAGS_INIT(cfg);
        /* Init (non-persistant) flags2 */
        WLC_BSSCFG_FLAGS2_INIT(cfg);
        if (cfg->flags & WLC_BSSCFG_DYNBCN)
            cfg->flags &= ~WLC_BSSCFG_HW_BCN;

        WL_APSTA_UPDN(("wl%d: wlc_bsscfg_up(%s): flags = 0x%x\n",
            wlc->pub->unit, (BSSCFG_AP(cfg) ? "AP" : "STA"), cfg->flags));

        wlc_bsscfg_set_main_ap(wlc, cfg);
#ifdef MBSS
        if (wlc_mbss_bsscfg_up(wlc, cfg) != BCME_OK) {
            stop = TRUE;
            goto end;
        }
#endif /* MBSS */

#if defined(TD_STEER) && defined(CONFIG_TENDA_PRIVATE_WLAN)
        TDCORE_FUNC_IF(maps_vap_available_handler) {
            TDCORE_FUNC(maps_vap_available_handler)(cfg);
        }
#endif

end:
        if (stop || ((ret = wlc_bss_up(wlc->ap, cfg)) != BCME_OK)) {
            WL_ERROR(("wl%d: wlc_bss_up for AP failed, %d\n", WLCWLUNIT(wlc), ret));
        }
#ifdef STA
        wlc_set_wake_ctrl(wlc);
#endif
    }
#endif /* AP */

#ifdef STA
    /* cfg->up for an AP happens in wlc_ap_up_upd */
    if (!BSSCFG_AP(cfg)) {
        cfg->up = TRUE;
    }
#endif

    if (!stop) {
        /* only for AP or IBSS */
        if ((ret = wlc_bsscfg_bcmc_scb_init(wlc, cfg)) != BCME_OK) {
            cfg->up = FALSE;
            /* AP */
            wlc_bss_down(wlc->ap, cfg);
            return ret;
        }
    } else if (ret != BCME_OK) {
        WL_ERROR(("wl%d: %s BSS %d not up\n", WLCWLUNIT(wlc), __FUNCTION__,
            WLC_BSSCFG_IDX(cfg)));
        return ret;
    }

    /* invoke bsscfg state change callbacks */
    WL_BCB(("wl%d.%d: %s: notify clients of bsscfg state change. enable %d>%d up %d>%d.\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__,
            st_data.old_enable, cfg->enable, st_data.old_up, cfg->up));

    /* state change callback for an AP is executed in wlc_ap_up_upd */
    if (!BSSCFG_AP(cfg)) {
        wlc_bsscfg_notif_signal(cfg, &st_data);
    }

    return ret;
}

/* Count real IBSS (beaconing by PSM) connections.
 * This could have been done through checking HWBCN flag of the bsscfg
 * without the conditional compilation...
 */
static uint
wlc_ibss_cnt(wlc_info_t *wlc)
{
    uint ibss_bsscfgs = wlc->ibss_bsscfgs;

#ifdef WL_PROXDETECT
    {
        int idx;
        wlc_bsscfg_t *cfg;
        /* infrastructure STA(BSS) can co-exist with PROXD STA(IBSS) */
        FOREACH_AS_STA(wlc, idx, cfg) {
            if (BSS_PROXD_ENAB(wlc, cfg))
                ibss_bsscfgs--;
        }
    }
#endif /*  WL_PROXDETECT */

    return ibss_bsscfgs;
}

/** Enable: always try to force up */
int
wlc_bsscfg_enable(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    bsscfg_state_upd_data_t st_data;
#ifdef STA
    bool mpc_out = wlc->mpc_out;
#endif
    int ret = BCME_OK;

    ASSERT(bsscfg != NULL);

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_enable: currently %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
        (bsscfg->enable ? "ENABLED" : "DISABLED")));

#ifdef MBSS
    if (MBSS_ENAB(wlc->pub)) {
        ; /* do nothing */
    } else
#endif
    {
        /* block simultaneous IBSS and AP connection */
        if (BSSCFG_AP(bsscfg)) {
            uint ibss_bsscfgs = wlc_ibss_cnt(wlc);
            if (ibss_bsscfgs) {
                WL_ERROR(("wl%d: Cannot enable AP bsscfg with a IBSS\n",
                    wlc->pub->unit));
                return BCME_ERROR;
            }
        }
    }

    bzero(&st_data, sizeof(st_data));
    st_data.cfg = bsscfg;
    st_data.old_enable = bsscfg->enable;
    st_data.old_up = bsscfg->up;

#ifdef STA
    /* For AP+STA combo build */
    if (BSSCFG_AP(bsscfg)) {
        /* bringup the driver */
        wlc->mpc_out = TRUE;
        wlc_radio_mpc_upd(wlc);
    }
#endif

    bsscfg->enable = TRUE;

    if (BSSCFG_AP(bsscfg)) {
#ifdef MBSS
        /* make sure we don't exceed max */
        if (MBSS_ENAB(wlc->pub) &&
            ((uint32)AP_BSS_UP_COUNT(wlc) >= (uint32)WLC_MAX_AP_BSS(wlc->pub->corerev))) {
            WL_ERROR(("wl%d: max %d ap bss allowed\n",
                      wlc->pub->unit, WLC_MAX_AP_BSS(wlc->pub->corerev)));
            bsscfg->enable = FALSE;
            ret = BCME_ERROR;
            goto fail;
        }
#endif /* MBSS */
    }

    /* invoke bsscfg state change callbacks */
    WL_BCB(("wl%d.%d: %s: notify clients of bsscfg state change. enable %d>%d up %d>%d.\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
            st_data.old_enable, bsscfg->enable, st_data.old_up, bsscfg->up));
    bcm_notif_signal(bcmh->state_upd_notif_hdl, &st_data);

    if (BSSCFG_AP(bsscfg)) {
        ret = wlc_bsscfg_up(wlc, bsscfg);
    }

#ifdef MBSS
fail:
#endif

#ifdef STA
    /* For AP+STA combo build */
    if (BSSCFG_AP(bsscfg)) {
        wlc->mpc_out = mpc_out;
        wlc_radio_mpc_upd(wlc);
    }
#endif

    /* wlc_bsscfg_up() will be called for STA assoication code:
     * - for IBSS, in wlc_join_start_ibss() and in wlc_join_BSS()
     * - for BSS, in wlc_assoc_complete()
     */
    /*
     * if (BSSCFG_STA(bsscfg)) {
     *    return BCME_OK;
     * }
     */

    return ret;
}

static void
wlc_bsscfg_down_cbpend_sum(uint *cbpend_sum, bsscfg_up_down_event_data_t *notif_data)
{
    *cbpend_sum = *cbpend_sum + notif_data->callbacks_pending;
}

static void
wlc_bsscfg_state_upd_cbpend_sum(uint *cbpend_sum, bsscfg_state_upd_data_t *notif_data)
{
    *cbpend_sum = *cbpend_sum + notif_data->callbacks_pending;
}

int
wlc_bsscfg_down(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    int callbacks = 0;
    bsscfg_module_t *bcmh = wlc->bcmh;
    bsscfg_state_upd_data_t st_data;
    uint cbpend_sum = 0;
    wlc_bsscfg_t *cfg_loop;
    uint i;

    ASSERT(cfg != NULL);

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_down: currently %s %s; stas/aps/associated %d/%d/%d\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
                   (cfg->up ? "UP" : "DOWN"), (BSSCFG_AP(cfg) ? "AP" : "STA"),
                   wlc->stas_associated, wlc->aps_associated, wlc->pub->associated));

    if (!cfg->up) {
#ifdef AP
        if (BSSCFG_AP(cfg) && cfg->associated) {
            /* For AP, cfg->up can be 0 but down never called.
             * Thus, it's best to check for both !up and !associated
             * before we decide to skip the down procedures.
             */
            WL_APSTA_UPDN(("wl%d: AP cfg up = %d but associated, "
                           "continue with down procedure.\n",
                           wlc->pub->unit, cfg->up));
        }
        else
#endif
        return 0;
    }

#if defined(WLDFS) && defined(BGDFS)
    /* Abort if any BGDFS is running */
    if (wlc_dfs_scan_in_progress(wlc->dfs)) {
        WL_DFS(("wl%d: %s DFS SCAN/BGDFS was in progress. Aborting now.\n",
            wlc->pub->unit, __FUNCTION__));
        wlc_dfs_scan_abort(wlc->dfs);
    }
#endif /* WLDFS && BGDFS */

    bzero(&st_data, sizeof(st_data));
    st_data.cfg = cfg;
    st_data.old_enable = cfg->enable;
    st_data.old_up = cfg->up;

    /* bring down this config */
    cfg->up = FALSE;

    if (APSTA_ENAB(wlc->pub) && BSSCFG_STA(cfg) &&
            BSSCFG_IS_PRIMARY(cfg) && wlc->apsta_muted) {
        WL_REGULATORY(("wl%d.%d wlc_bsscfg_down reset apsta_muted\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(cfg)));
        wlc->apsta_muted = FALSE;
    }

    if (wlc_bsscfg_preserve(wlc, cfg)) {
        goto do_down;
    }

    /* invoke bsscfg down callbacks */
    /* TO BE REPLACED BY BELOW STATE CHANGE CALLBACK */
    {
    bsscfg_up_down_event_data_t evt_data;
    memset(&evt_data, 0, sizeof(evt_data));
    evt_data.bsscfg = cfg;
    bcm_notif_signal_ex(bcmh->up_down_notif_hdl, &evt_data,
        (bcm_notif_server_callback)wlc_bsscfg_down_cbpend_sum,
        (bcm_notif_server_context)&cbpend_sum);
    /* Clients update the number of pending asynchronous callbacks in the
     * driver down path.
     */
    callbacks += cbpend_sum;
    }

    /* invoke bsscfg state change callbacks */
    WL_BCB(("wl%d.%d: %s: notify clients of bsscfg state change. enable %d>%d up %d>%d.\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__,
            st_data.old_enable, cfg->enable, st_data.old_up, cfg->up));
    bcm_notif_signal_ex(bcmh->state_upd_notif_hdl, &st_data,
        (bcm_notif_server_callback)wlc_bsscfg_state_upd_cbpend_sum,
        (bcm_notif_server_context)&cbpend_sum);
    /* Clients update the number of pending asynchronous callbacks in the
     * driver down path.
     */
#if defined(TD_STEER) && defined(CONFIG_TENDA_PRIVATE_WLAN)
    TDCORE_FUNC_IF(maps_vap_unavailable_handler) {
        TDCORE_FUNC(maps_vap_unavailable_handler)(cfg);
    }
#endif
    callbacks += cbpend_sum;

do_down:
#ifdef AP
    if (BSSCFG_AP(cfg)) {

        callbacks += wlc_ap_down(wlc->ap, cfg);

#ifdef MBSS
        wlc_mbss_bsscfg_down(wlc, cfg);
#endif /* MBSS */
        if (cfg == wlc->main_ap_bsscfg) {
            FOREACH_UP_AP(wlc, i, cfg_loop) {
                if (cfg_loop == wlc->main_ap_bsscfg) {
                    continue;
                }
                break;
            }
            wlc->main_ap_bsscfg = cfg_loop;
#ifdef MBSS
            if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
                /* beacon init as transmit bss has changed.
                */
                if (wlc->main_ap_bsscfg) {
                    wlc_mbss_beacon_init(wlc, wlc->main_ap_bsscfg, wlc->mbss);
                    wlc_bss_update_beacon(wlc, wlc->main_ap_bsscfg, TRUE);
                }
            }
#endif /* MBSS */
        }

#ifdef STA
        wlc_set_wake_ctrl(wlc);
#endif
    }
#endif /* AP */

    /* Delay freeing BCMC SCBs if there are packets in the fifo */
    if (!BCMC_PKTS_QUEUED(cfg)) {
        /* BCMC SCBs are de-allocated */
        wlc_bsscfg_bcmcscbfree(wlc, cfg);
    }
#ifdef WL_GLOBAL_RCLASS
    if (BSSCFG_AP(cfg)) {
        cfg->scb_without_gbl_rclass = 0;
    }
#endif /* WL_GLOBAL_RCLASS */

    if (cfg->rnr_nbr_ap_info) {
        /* resetting it to populate again with periodic call:
         * wlc_ap_update_static_rnr_nbr_ap_info
         */
        cfg->rnr_nbr_ap_info_size = 0;
    }
    return callbacks;
}

int
wlc_bsscfg_disable(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    bsscfg_state_upd_data_t st_data;
    int callbacks = 0;
    uint cbpend_sum = 0;

    ASSERT(bsscfg != NULL);

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_disable: currently %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
        (bsscfg->enable ? "ENABLED" : "DISABLED")));

    /* XXX [NDIS restruct] Consider merging here from Falcon code
     * (under if (BSSCFG_AP(bsscfg))
     */

    /* If a bss is already disabled, don't do anything */
    if (!bsscfg->enable) {
        ASSERT(!bsscfg->up);
        return 0;
    }

    callbacks += wlc_bsscfg_down(wlc, bsscfg);
    ASSERT(!bsscfg->up);

    bzero(&st_data, sizeof(st_data));
    st_data.cfg = bsscfg;
    st_data.old_enable = bsscfg->enable;
    st_data.old_up = bsscfg->up;

    bsscfg->flags &= ~WLC_BSSCFG_PRESERVE;

    bsscfg->enable = FALSE;

    /* invoke bsscfg state change callbacks */
    WL_BCB(("wl%d.%d: %s: notify clients of bsscfg state change. enable %d>%d up %d>%d.\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
            st_data.old_enable, bsscfg->enable, st_data.old_up, bsscfg->up));
    bcm_notif_signal_ex(bcmh->state_upd_notif_hdl, &st_data,
        (bcm_notif_server_callback)wlc_bsscfg_state_upd_cbpend_sum,
        (bcm_notif_server_context)&cbpend_sum);
    /* Clients update the number of pending asynchronous callbacks in the
     * driver down path.
     */
    callbacks += cbpend_sum;

#ifdef STA
    /* For AP+STA combo build */
    if (BSSCFG_AP(bsscfg)) {
        /* force an update to power down the radio */
        wlc_radio_mpc_upd(wlc);
    }
#endif

#ifdef AP
    WL_APSTA_UPDN(("wl%d.%d: %s Disabling mute_ap for bsscfg \n",
        wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
    wlc_ap_mute(wlc, FALSE, bsscfg, -1);
#endif /* AP */

    return callbacks;
}

static int
wlc_bsscfg_cubby_init(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    uint len;

    /* reset all cubbies */
    len = wlc_cubby_totsize(bcmh->cubby_info);
    memset((uint8 *)cfg + bcmh->cfgtotsize, 0, len);

    return wlc_cubby_init(bcmh->cubby_info, cfg, NULL, NULL, NULL);
}

static void
wlc_bsscfg_cubby_deinit(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;

    wlc_cubby_deinit(bcmh->cubby_info, cfg, NULL, NULL, NULL);
}

#ifdef AP
static int
wlc_bsscfg_ap_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int ret;
    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_ap_init:\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));

    /* Init flags: Beacons/probe resp in HW by default */
    bsscfg->flags |= (WLC_BSSCFG_HW_BCN | WLC_BSSCFG_HW_PRB);
    if (bsscfg->flags & WLC_BSSCFG_DYNBCN)
        bsscfg->flags &= ~WLC_BSSCFG_HW_BCN;

    /* Hard Coding the cfg->flags to not support Radar channel operation */
    if (!RADAR_ENAB(wlc->pub) && !ISSIM_ENAB(wlc->pub->sih)) {
        bsscfg->flags |= WLC_BSSCFG_AP_NORADAR_CHAN;
    }

#if defined(MBSS) || defined(WLP2P)
    bsscfg->maxassoc = wlc->pub->tunables->maxscb;
#endif /* MBSS || WLP2P */
#if defined(MBSS)
    if (MBSS_ENAB(wlc->pub)) {
        wlc_mbss_bcmc_reset(wlc, bsscfg);
    }
#endif

    BSSCFG_SET_PSINFO(bsscfg);

#ifdef BCMPCIEDEV
    if (BCMPCIEDEV_ENAB())
        bsscfg->ap_isolate = AP_ISOLATE_SENDUP_ALL;
#endif

    if (bsscfg->wlcif)
        bsscfg->wlcif->iface_type = WL_INTERFACE_TYPE_AP;
#ifdef WL_TRAFFIC_THRESH
    /* Enabling bsscfg by default */
    bsscfg->traffic_thresh_enab = 1;

#ifdef CONFIG_TENDA_PRIVATE_WLAN
    bsscfg->prev_rxframe = 0;
#endif /* CONFIG_TENDA_PRIVATE_WLAN */

    bsscfg->far_sta_rssi = WL_FAR_STA_RSSI;
    /* set default values for traffic_thresh */
    wlc_bsscfg_traffic_thresh_deinit(wlc, bsscfg); /* deinit before init,
                            * To clear any residual
                            * values if present on
                            * re-assoc.
                            */
    wlc_bsscfg_traffic_thresh_set_defaults(wlc, bsscfg);
    if (wlc->pub->_traffic_thresh && bsscfg->traffic_thresh_enab) {
        ret = wlc_bsscfg_traffic_thresh_init(wlc, bsscfg);
        if (ret != BCME_OK) {
            WL_ERROR(("wl%d.%d: %s: traffic_thresh failed with err %d\n",
                wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                __FUNCTION__, ret));
            return ret;
        }
    }
#endif /* WL_TRAFFIC_THRESH */
    /* invoke bsscfg cubby init function */
    if ((ret = wlc_bsscfg_cubby_init(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_cubby_init failed with err %d\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ret));
        return ret;
    }

    return BCME_OK;
}

static void
wlc_bsscfg_ap_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_ap_deinit:\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));

    /* invoke bsscfg cubby deinit function */
    wlc_bsscfg_cubby_deinit(wlc, bsscfg);
#ifdef WL_TRAFFIC_THRESH
    wlc_bsscfg_traffic_thresh_deinit(wlc, bsscfg);
#endif /* WL_TRAFFIC_THRESH */

    bsscfg->flags &= ~(WLC_BSSCFG_HW_BCN | WLC_BSSCFG_HW_PRB);
}
#endif /* AP */

#ifdef STA
static int
wlc_bsscfg_sta_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int ret;

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_sta_init:\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
#ifdef DWDS
    bsscfg->dwds_loopback_filter = FALSE;
#endif /* DWDS */

    /* invoke bsscfg cubby init function */
    if ((ret = wlc_bsscfg_cubby_init(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_cubby_init failed with err %d\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ret));
        return ret;
    }

    if (bsscfg->wlcif)
        bsscfg->wlcif->iface_type = WL_INTERFACE_TYPE_STA;

    return BCME_OK;
}

static void
wlc_bsscfg_sta_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_sta_deinit:\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));

#ifdef DWDS
    bsscfg->dwds_loopback_filter = FALSE;
#endif /* DWDS */
    /* invoke bsscfg cubby deinit function */
    wlc_bsscfg_cubby_deinit(wlc, bsscfg);

    if (bsscfg->current_bss != NULL) {
        wlc_bss_info_t *current_bss = bsscfg->current_bss;
        if (current_bss->bcn_prb != NULL) {
            MFREE(wlc->osh, current_bss->bcn_prb, current_bss->bcn_prb_len);
            current_bss->bcn_prb = NULL;
            current_bss->bcn_prb_len = 0;
        }
    }

    wlc_bsscfg_scan_params_reset(wlc, bsscfg);
}
#endif /* STA */

static int
wlc_bsscfg_bss_rsinit(wlc_info_t *wlc, wlc_bss_info_t *bi, uint8 rates, uint8 bw, uint8 mcsallow)
{
    wlc_rateset_t *src = &wlc->band->hw_rateset;
    wlc_rateset_t *dst = &bi->rateset;

    wlc_rateset_filter(src, dst, FALSE, rates, RATE_MASK_FULL, mcsallow);
    if (dst->count == 0)
        return BCME_NORESOURCE;

    wlc_rateset_ht_bw_mcs_filter(dst, bw);
    wlc_rate_lookup_init(wlc, dst);

    return BCME_OK;
}

int
wlc_bsscfg_rateset_init(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint8 rates, uint8 bw, uint8 mcsallow)
{
    int err;

    if ((err = wlc_bsscfg_bss_rsinit(wlc, cfg->target_bss, rates, bw, mcsallow)) != BCME_OK)
        return err;
    if ((err = wlc_bsscfg_bss_rsinit(wlc, cfg->current_bss, rates, bw, mcsallow)) != BCME_OK)
        return err;

    return err;
}

#ifdef STA
/* map wlc_bss_info_t::bss_type to bsscfg_type_t */
/* There's not enough information to map bss_type to bsscfg type uniquely
 * so limit it to primary bsscfg and STA only.
 */
static const wlc_bsscfg_type_t bsscfg_cfg_types[] = {
    /* DOT11_BSSTYPE_INFRASTRUCTURE */ {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_STA},
    /* DOT11_BSSTYPE_INDEPENDENT */ {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_IBSS},
    /* DOT11_BSSTYPE_ANY */ {0, 0},    /* place holder */
    /* DOT11_BSSTYPE_MESH */ {BSSCFG_TYPE_MESH, BSSCFG_SUBTYPE_NONE}
};

int
wlc_bsscfg_type_bi2cfg(wlc_info_t *wlc, wlc_bsscfg_t *cfg,
    wlc_bss_info_t *bi, wlc_bsscfg_type_t *type)
{
    BCM_REFERENCE(wlc);
    BCM_REFERENCE(cfg);

    if (bi->bss_type < ARRAYSIZE(bsscfg_cfg_types) &&
        bi->bss_type != DOT11_BSSTYPE_ANY) {
        type->type = bsscfg_cfg_types[bi->bss_type].type;
        type->subtype = bsscfg_cfg_types[bi->bss_type].subtype;

        return BCME_OK;
    }

    return BCME_BADARG;
}
#endif /* STA */

/* is the BSS with type & subtype of AP */
static const wlc_bsscfg_type_t bsscfg_ap_types[] = {
    {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_AP},
    {BSSCFG_TYPE_P2P, BSSCFG_P2P_GO}
};

static bool
wlc_bsscfg_type_isAP(wlc_bsscfg_type_t *type)
{
    uint i;

    for (i = 0; i < ARRAYSIZE(bsscfg_ap_types); i++) {
        if (bsscfg_ap_types[i].type == type->type &&
            bsscfg_ap_types[i].subtype == type->subtype) {
            return TRUE;
        }
    }

    return FALSE;
}

/* is the BSS with type & subtype of Infra BSS */
static const wlc_bsscfg_type_t bsscfg_bss_types[] = {
    {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_AP},
    {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_STA},
    {BSSCFG_TYPE_P2P, BSSCFG_P2P_GO},
    {BSSCFG_TYPE_P2P, BSSCFG_P2P_GC},
    {BSSCFG_TYPE_PSTA, BSSCFG_SUBTYPE_NONE}
};

static bool
wlc_bsscfg_type_isBSS(wlc_bsscfg_type_t *type)
{
    uint i;

    for (i = 0; i < ARRAYSIZE(bsscfg_bss_types); i++) {
        if (bsscfg_bss_types[i].type == type->type &&
            (bsscfg_bss_types[i].subtype == BSSCFG_SUBTYPE_NONE ||
             bsscfg_bss_types[i].subtype == type->subtype)) {
            return TRUE;
        }
    }

    return FALSE;
}

/* Configure the main AP bsscfg. Main AP bsscfg is the bsscfg with lowest idx
 * which is configured as AP and isup.
 */
static void wlc_bsscfg_set_main_ap(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    wlc_bsscfg_t *cfg_loop;
    int i = 0;
    bool update_main_ap = FALSE;

    if (!wlc->main_ap_bsscfg) {
        if (wlc->pub->_pref_transmit_bss != WLC_INVALID_BSS_IDX) {
            if (cfg->_idx == wlc->pub->_pref_transmit_bss) {
                wlc->main_ap_bsscfg = cfg;
                update_main_ap = TRUE;
            }

        } else {
            FOREACH_AP(wlc, i, cfg_loop) {
                if (cfg_loop == cfg) {
                    wlc->main_ap_bsscfg = cfg;
                    update_main_ap = TRUE;
                    break;
                }
            }
        }
    } else {
        if (wlc->pub->_pref_transmit_bss == WLC_INVALID_BSS_IDX) {
            /*  Case when New bss is up. and transmit bss was already set.
             *  need to check if we have to move the transmit bss
             */
            FOREACH_AP(wlc, i, cfg_loop) {
                if (cfg_loop == wlc->main_ap_bsscfg) {
                    break;
                }

                if (cfg_loop == cfg) {
                    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
                        wlc_mbss_spt_deinit(wlc, wlc->main_ap_bsscfg);
                    }
                    wlc->main_ap_bsscfg = cfg;
                    update_main_ap = TRUE;
                    break;
                }
            }
        }
    }
#if BAND6G || defined(WL_OCE_AP)
    if (update_main_ap) {
        wlc_ap_check_tx_bss_for_upr_fd(wlc);
    }
#endif /* BAND6G || WL_OCE_AP */
    BCM_REFERENCE(update_main_ap);
}

/* map type & subtype variables to _ap & BSS variables */
static void
wlc_bsscfg_type_set(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_type_t *type)
{
    cfg->type = type->type;
    cfg->subtype = type->subtype;
    cfg->_ap = wlc_bsscfg_type_isAP(type);
    cfg->BSS = wlc_bsscfg_type_isBSS(type);
}

#ifdef BCMDBG
static const char *_wlc_bsscfg_type_name(bsscfg_type_t type);
static const char *_wlc_bsscfg_subtype_name(uint subtype);

static const char *
wlc_bsscfg_type_name(wlc_bsscfg_type_t *type)
{
    return _wlc_bsscfg_type_name(type->type);
}

static const char *
wlc_bsscfg_subtype_name(wlc_bsscfg_type_t *type)
{
    return _wlc_bsscfg_subtype_name(type->subtype);
}
#endif /* BCMDBG */

/* TODO: fold this minimum reinit into the regular reinit logic to make sure
 * it won't diverge in the future...
 */
static int
wlc_bsscfg_reinit_min(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_type_t *type, uint flags)
{
    if (!BSSCFG_STA(cfg)) {
        WL_ERROR(("wl%d.%d: %s: Bad param as_in_prog for non STA bsscfg\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));
        return BCME_NOTSTA;
    }

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_reinit_min: flags 0x%x type %s subtype %s\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(cfg), flags,
                   wlc_bsscfg_type_name(type), wlc_bsscfg_subtype_name(type)));

    cfg->flags = flags;

    wlc_bsscfg_type_set(wlc, cfg, type);

    wlc_if_event(wlc, WLC_E_IF_CHANGE, cfg->wlcif);

    return BCME_OK;
}

/* BCMC SCB allocation for IBSS-STA / BSS-AP */
int
wlc_bsscfg_bcmc_scb_init(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    int ret = BCME_OK;

    if (!cfg->up || !(BSSCFG_IBSS(cfg) || BSSCFG_AP(cfg))) {
        return ret;
    }
    if (!(cfg->flags & WLC_BSSCFG_NOBCMC)) {
        ret = wlc_bsscfg_bcmcscballoc(wlc, cfg);
        if (ret != BCME_OK) {
            WL_ERROR(("wl%d: wlc_bsscfg_bcmc_scb_init fails %d\n",
            wlc->pub->unit, ret));
        }
    }
#ifdef WL_MBSSID
    if (ret == BCME_OK) {
        if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
            cfg->bcmc_scb->aid = cfg->_idx;
        } else {
            cfg->bcmc_scb->aid = 0;
        }
    }
#endif /* WL_MBSSID */
    return ret;
}

/* Fixup bsscfg type according to the given type and subtype.
 * Only Infrastructure and Independent BSS types can be switched if min_reinit
 * (minimum reinit) is TRUE.
 */
int
wlc_bsscfg_type_fixup(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_type_t *type,
    bool min_reinit)
{
    int err = BCME_OK;

    ASSERT(cfg != NULL);

    /* Fixup BSS (Infra vs Indep) only when we are in association process */
    if (min_reinit) {
        if ((err = wlc_bsscfg_reinit_min(wlc, cfg, type, cfg->flags)) != BCME_OK) {
            WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_reinit_min failed, err %d\n",
                      wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__, err));
            return err;
        }
    }
    /* It's ok to perform a full reinit */
    else {
        if ((err = wlc_bsscfg_reinit(wlc, cfg, type, cfg->flags)) != BCME_OK) {
            WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_reinit failed, err %d\n",
                      wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__, err));
            return err;
        }
    }

    /* AP has these flags set in wlc_bsscfg_ap_init() */
    if (BSSCFG_STA(cfg)) {
        /* IBSS deploys PSM bcn/prbrsp */
        if (BSSCFG_IBSS(cfg) &&
            !(cfg->flags & (WLC_BSSCFG_SW_BCN | WLC_BSSCFG_SW_PRB))) {
            cfg->flags |= (WLC_BSSCFG_HW_BCN | WLC_BSSCFG_HW_PRB);
        /* reset in case of a role change between Infra STA and IBSS STA */
        } else {
            cfg->flags &= ~(WLC_BSSCFG_HW_BCN | WLC_BSSCFG_HW_PRB);
    }
    }
    err = wlc_bsscfg_bcmc_scb_init(wlc, cfg);
    return err;
}

int
wlc_bsscfg_get_free_idx(wlc_info_t *wlc)
{
    int idx;

    for (idx = 0; idx < WLC_MAXBSSCFG; idx++) {
        if (wlc->bsscfg[idx] == NULL) {
            return idx;
        }
    }

    return BCME_ERROR;
}

void
wlc_bsscfg_ID_assign(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;

    bsscfg->ID = bcmh->next_bsscfg_ID;
    bcmh->next_bsscfg_ID ++;
}

/**
 * After all the modules indicated how much cubby space they need in the bsscfg, the actual
 * wlc_bsscfg_t can be allocated. This happens one time fairly late within the attach phase, but
 * also when e.g. communication with a new remote party is started.
 */
static wlc_bsscfg_t *
wlc_bsscfg_malloc(wlc_info_t *wlc)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    osl_t *osh;
    wlc_bsscfg_t *cfg;
    int malloc_size;
    uint offset;
    uint len;

    osh = wlc->osh;

    len = bcmh->cfgtotsize + wlc_cubby_totsize(bcmh->cubby_info);
    if ((cfg = (wlc_bsscfg_t *)MALLOCZ(osh, len)) == NULL) {
        malloc_size = len;
        goto fail;
    }

    offset = ROUNDUP(sizeof(*cfg), PTRSZ);
    cfg->ext_cap = (uint8 *)cfg + offset;
    /* offset += ROUNDUP(bcmh->ext_cap_max, PTRSZ); */

    /* XXX optimization opportunities:
     * some of these subsidaries are STA specific therefore allocate them only for
     * STA bsscfg...
     */

    if ((cfg->multicast = (struct ether_addr *)
         MALLOCZ(osh, (sizeof(struct ether_addr)*MAXMULTILIST))) == NULL) {
        malloc_size = sizeof(struct ether_addr) * MAXMULTILIST;
        goto fail;
    }
#ifdef WL_MBSSID
    cfg->mbssid_transbss_amtidx = -1; /* initialize to invalid value */
#endif /* WL_MBSSID */

    malloc_size = sizeof(wlc_bss_info_t);
    if ((cfg->current_bss = (wlc_bss_info_t *) MALLOCZ(osh, malloc_size)) == NULL) {
        goto fail;
    }
    if ((cfg->target_bss = (wlc_bss_info_t *) MALLOCZ(osh, malloc_size)) == NULL) {
        goto fail;
    }
    /* BRCM IE */
    if ((cfg->brcm_ie = (uint8 *)
         MALLOCZ(osh, WLC_MAX_BRCM_ELT)) == NULL) {
        malloc_size = WLC_MAX_BRCM_ELT;
        goto fail;
    }
#ifdef  MULTIAP
    /* Multi-Ap IE */
    if ((cfg->multiap_ie = (uint8 *)
        MALLOCZ(osh, MAP_IE_MAX_LEN)) == NULL) {
        goto fail;
    }
#endif  /* MULTIAP */

    if ((cfg->rsnxe_info = (rsnxe_ie_info_t *)
                MALLOCZ(osh, sizeof(rsnxe_ie_info_t))) == NULL) {
        goto fail;
    }
    if ((cfg->ext_rsn_cap = (uint8 *)
                MALLOCZ(osh, EXT_RSN_CAP_MAX_LEN)) == NULL) {
        goto fail;
    }

    return cfg;

fail:
    WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__, malloc_size, MALLOCED(wlc->osh)));
    wlc_bsscfg_mfree(wlc, cfg);
    return NULL;
}

static void
wlc_bsscfg_mfree(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    osl_t *osh;
    uint len;

    if (cfg == NULL)
        return;

    osh = wlc->osh;

    if (cfg->mbssid_tag_list) {
        MFREE(osh, cfg->mbssid_tag_list, sizeof(wlc_mbssid_tag_list_t));
        cfg->mbssid_tag_list = NULL;
    }

    if (cfg->multicast) {
        MFREE(osh, cfg->multicast, (sizeof(struct ether_addr) * MAXMULTILIST));
        cfg->multicast = NULL;
    }

    if (cfg->current_bss != NULL) {
        wlc_bss_info_t *current_bss = cfg->current_bss;
        /* WES FIXME: move this to detach so that assoc_recreate will have a bcn_prb
         * over a down/up transition.
         */
        if (current_bss->bcn_prb != NULL) {
            MFREE(osh, current_bss->bcn_prb, current_bss->bcn_prb_len);
        }
        MFREE(osh, current_bss, sizeof(wlc_bss_info_t));
        cfg->current_bss = NULL;
    }
    if (cfg->target_bss != NULL) {
        MFREE(osh, cfg->target_bss, sizeof(wlc_bss_info_t));
        cfg->target_bss = NULL;
    }

    /* BRCM IE */
    if (cfg->brcm_ie != NULL) {
        MFREE(osh, cfg->brcm_ie, WLC_MAX_BRCM_ELT);
        cfg->brcm_ie = NULL;
    }

#ifdef  MULTIAP
    /* Multi-AP IE */
    if (cfg->multiap_ie != NULL) {
        MFREE(osh, cfg->multiap_ie, MAP_IE_MAX_LEN);
        cfg->multiap_ie = NULL;
    }
#endif  /* MULTIAP */

    /* XXX:
     * It's not difficult to imagine a better scheme for memory alloc/free
     * of all these little pieces.
     * FIXME
     */

#ifdef WL11AX
    wlc_bsscfg_mfree_block_he(wlc, cfg);
#endif /* WL11AX */

    if (cfg->rsnxe_info != NULL) {
        if (cfg->rsnxe_info->rsnxe) {
            MFREE(osh, cfg->rsnxe_info->rsnxe, cfg->rsnxe_info->rsnxe_len);
            cfg->rsnxe_info->rsnxe = NULL;
        }
        MFREE(osh, cfg->rsnxe_info, sizeof(rsnxe_ie_info_t));
        cfg->rsnxe_info = NULL;
    }

    if (cfg->ext_rsn_cap != NULL) {
        MFREE(osh, cfg->ext_rsn_cap, EXT_RSN_CAP_MAX_LEN);
        cfg->ext_rsn_cap = NULL;
    }

    wlc_assoc_dh_params_free(wlc, cfg);
    len = bcmh->cfgtotsize + wlc_cubby_totsize(bcmh->cubby_info);
    MFREE(osh, cfg, len);
}

/**
 * Called when e.g. a wireless interface is added, during the ATTACH phase, or as a result of an
 * IOVAR.
 */
wlc_bsscfg_t *
wlc_bsscfg_alloc(wlc_info_t *wlc, int idx, wlc_bsscfg_type_t *type,
    uint flags, uint32 flags2, struct ether_addr *ea)
{
    wlc_bsscfg_t *bsscfg;
    int err;
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 freemem_before, bytes_allocated;

    hnd_get_heapuse(&mu);
    freemem_before = mu.arena_free;
#endif /* MEM_ALLOC_STATS */

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_alloc: flags 0x%08x type %s subtype %s\n",
                   wlc->pub->unit, idx, flags,
                   wlc_bsscfg_type_name(type), wlc_bsscfg_subtype_name(type)));

    if (idx < 0 || idx >= WLC_MAXBSSCFG) {
        return NULL;
    }

    if ((bsscfg = wlc_bsscfg_malloc(wlc)) == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
                  wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }

    if ((err = wlc_bsscfg_alloc_int(wlc, bsscfg, idx, type, flags, flags2,
            ea != NULL ? ea : &wlc->pub->cur_etheraddr)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_alloc_int failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, err));
        goto fail;
    }

    if ((bsscfg->mbssid_tag_list = (wlc_mbssid_tag_list_t *) MALLOCZ(wlc->osh,
            sizeof(wlc_mbssid_tag_list_t))) == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
                  wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }

    /* Initialize invalid iface_type */
    bsscfg->wlcif->iface_type = wlc->bcmh->iface_info_max;

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    bytes_allocated = freemem_before - mu.arena_free;
    bsscfg->mem_bytes += bytes_allocated;
    if (bytes_allocated > mu.max_bsscfg_alloc) {
        mu.max_bsscfg_alloc = bytes_allocated;
    }
    mu.total_bsscfg_alloc += bytes_allocated;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

    if (!BSSCFG_IS_PRIMARY(bsscfg)) {
        bsscfg->bfe_capable = wlc->primary_bsscfg->bfe_capable;
    }
    return bsscfg;

fail:
    if (bsscfg != NULL)
        wlc_bsscfg_mfree(wlc, bsscfg);
    return NULL;
}

/* force to reset the existing bsscfg to given type+flags+ea */
int
wlc_bsscfg_reset(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, wlc_bsscfg_type_t *type,
    uint flags, uint32 flags2, struct ether_addr *ea)
{
    int err;
    wlc_bsscfg_type_t old_type;

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_reset: flags 0x%08x type %s subtype %s\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), flags,
                   wlc_bsscfg_type_name(type), wlc_bsscfg_subtype_name(type)));

    wlc_bsscfg_type_get(wlc, bsscfg, &old_type);

    if (old_type.type == type->type &&
        old_type.subtype == type->subtype &&
        bsscfg->flags == flags) {
        return BCME_OK;
    }

    wlc_bsscfg_deinit(wlc, bsscfg);

    /* clear SSID */
    memset(bsscfg->SSID, 0, DOT11_MAX_SSID_LEN);
    bsscfg->SSID_len = 0;

    /* clear all configurations */
    if ((err = wlc_bsscfg_init_int(wlc, bsscfg, type, flags, flags2, ea)) != BCME_OK) {
        WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_init_int failed, err %d\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, err));
        return err;
    }

    if ((err = wlc_bsscfg_init(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_init failed, err %d\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, err));
        return err;
    }

    wlc_if_event(wlc, WLC_E_IF_CHANGE, bsscfg->wlcif);

    return err;
}

void
wlc_bsscfg_bcmcscbfree(struct wlc_info *wlc, wlc_bsscfg_t *bsscfg)
{
    if (!BSSCFG_HAS_BCMC_SCB(bsscfg))
        return;

    WL_INFORM(("bcmc_scb: free internal scb for 0x%p\n",
               OSL_OBFUSCATE_BUF(bsscfg->bcmc_scb)));

    wlc_bcmcscb_free(wlc, bsscfg->bcmc_scb);
    bsscfg->bcmc_scb = NULL;
}

void
wlc_bsscfg_deinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    /* Some scb cubby users allocate/free secondary cubbies based on
     * bsscfg's role e.g. allocate/free certain size of memory if
     * the bsscfg is AP and allocate/free a different size of memory
     * if the bsscfg is STA. The current scb cubby infrastructure
     * treats hwrs scbs no difference, hence here makes sure free of
     * memory matches the allocated size before changing the bsscfg
     * role.
     *
     * hwrs scbs are freed here; bcmc scbs are taken care in
     * wlc_bsscfg_ap|sta_deinit(); regular scbs are taken care in
     * wlc_scb_bsscfg_deinit().
     *
     * hwrs scbs are recreated in wlc_bsscfg_init(); bcmc scbs are
     * recreated in wlc_bsscfg_ap|sta_init(); regular scbs are created
     * by their users at appropriate times.
     */
    if (BSSCFG_IS_PRIMARY(bsscfg)) {
        wlc_hwrsscbs_free(wlc);
    }

    wlc_bsscfg_bcmcscbfree(wlc, bsscfg);
    if (BSSCFG_AP(bsscfg)) {
#ifdef AP
        wlc_bsscfg_ap_deinit(wlc, bsscfg);
#endif
    }
    else {
#ifdef STA
        wlc_bsscfg_sta_deinit(wlc, bsscfg);
#endif
    }
    bsscfg->_ap = FALSE;
}

/* delete network interface and free wlcif structure */
static void
wlc_bsscfg_deinit_intf(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    if (!BSSCFG_IS_PRIMARY(bsscfg) && bsscfg->wlcif != NULL) {
        wlc_if_event(wlc, WLC_E_IF_DEL, bsscfg->wlcif);
        if (bsscfg->wlcif->wlif != NULL) {
            wl_del_if(wlc->wl, bsscfg->wlcif->wlif);
            bsscfg->wlcif->wlif = NULL;
        }
    }
    wlc_wlcif_free(wlc, wlc->osh, bsscfg->wlcif);
    bsscfg->wlcif = NULL;
}
void
wlc_bsscfg_free(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int idx;
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 membytes = bsscfg->mem_bytes;
#endif /* MEM_ALLOC_STATS */

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_free: flags = 0x%x\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), bsscfg->flags));

    /* XXX [NDIS restruct] Consider merging here from Falcon p2p code
     * (under WLP2P || WLDSTA)
     */
#ifdef CONFIG_TENDA_PRIVATE_WLAN
    wlc_deinit_dfs_cfg(wlc, bsscfg);
#endif
    wlc_bsscfg_deinit(wlc, bsscfg);

    /* delete the upper-edge driver interface */
    wlc_bsscfg_deinit_intf(wlc, bsscfg);

    /* free the wlc_bsscfg struct if it was an allocated one */
    idx = bsscfg->_idx;
    wlc_bsscfg_mfree(wlc, bsscfg);
    wlc->bsscfg[idx] = NULL;
    if (bsscfg == wlc->primary_bsscfg) {
        wlc->primary_bsscfg = NULL;
    }
#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    mu.total_bsscfg_alloc -= membytes;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

}

int
wlc_bsscfg_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    int ret;

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_init:\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));

#if defined(AP) && defined(STA)
    if (BSSCFG_AP(bsscfg)) {
        ret = wlc_bsscfg_ap_init(wlc, bsscfg);
    } else {
        ret = wlc_bsscfg_sta_init(wlc, bsscfg);
    }
#elif defined(AP)
    ret = wlc_bsscfg_ap_init(wlc, bsscfg);
#elif defined(STA)
    ret = wlc_bsscfg_sta_init(wlc, bsscfg);
#endif

#if WL_BEAMFORMING
    wlc_txbf_bfecap_init(wlc->txbf, bsscfg);
#endif
    if (ret == BCME_OK) {
        if (BSSCFG_IS_PRIMARY(bsscfg)) {
            ret = wlc_hwrsscbs_alloc(wlc);
        }
    }
    return ret;
}

/* reset the existing bsscfg to the given type, generate host event */
int
wlc_bsscfg_reinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, wlc_bsscfg_type_t *type,
    uint flags)
{
    int ret;
    wlc_bsscfg_type_t old_type;

    WL_APSTA_UPDN(("wl%d.%d: wlc_bsscfg_reinit: flags 0x%x type %s subtype %s\n",
                   wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), flags,
                   wlc_bsscfg_type_name(type), wlc_bsscfg_subtype_name(type)));

    wlc_bsscfg_type_get(wlc, bsscfg, &old_type);

    if (old_type.type == type->type &&
        old_type.subtype == type->subtype &&
        bsscfg->flags == flags) {
        return BCME_OK;
    }

    wlc_bsscfg_deinit(wlc, bsscfg);

    bsscfg->flags = flags;

    wlc_bsscfg_type_set(wlc, bsscfg, type);

    if ((ret = wlc_bsscfg_init(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d.%d: %s: wlc_bsscfg_init failed, err %d\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ret));
        return ret;
    }

    wlc_if_event(wlc, WLC_E_IF_CHANGE, bsscfg->wlcif);

    wlc_bss_mac_event(wlc, bsscfg, WLC_E_PWR_SAVE_SYNC, &bsscfg->BSSID,
            0, 0, 0, &bsscfg->pm->PM, sizeof(uint8));

    return ret;
}

/* query type & subtype */
void
wlc_bsscfg_type_get(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_type_t *type)
{
    type->type = cfg->type;
    type->subtype = 0;

    if (type->type == BSSCFG_TYPE_GENERIC) {
        if (BSSCFG_AP(cfg)) {
            type->subtype = BSSCFG_GENERIC_AP;
        } else if (cfg->BSS) {
            type->subtype = BSSCFG_GENERIC_STA;
        } else {
            type->subtype = BSSCFG_GENERIC_IBSS;
        }
    }
    else if (type->type == BSSCFG_TYPE_P2P) {
        if (P2P_GO(wlc, cfg)) {
            type->subtype = BSSCFG_P2P_GO;
        }
        else if (P2P_CLIENT(wlc, cfg)) {
            type->subtype = BSSCFG_P2P_GC;
        }
        else if (BSS_P2P_DISC_ENAB(wlc, cfg)) {
            type->subtype = BSSCFG_P2P_DISC;
        }
    }
}

#ifdef WL11AX
void
wlc_bsscfg_mfree_block_he(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    if (cfg->block_he_list) {
        struct ether_addr *ether = cfg->block_he_list->etheraddr;
        if (ether != NULL) {
            MFREE(wlc->osh, ether, sizeof(struct ether_addr)
                    * cfg->block_he_list->block_he_list_size);
        }
        MFREE(wlc->osh, cfg->block_he_list, sizeof(wlc_block_he_mac_t));
        cfg->block_he_list = NULL;
    }
}
#endif /* WL11AX */

/* query info */
static void
wlc_bsscfg_info_get(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_bsscfg_info_t *info)
{
    wlc_bsscfg_type_t type;
    wlc_bsscfg_type_get(wlc, cfg, &type);
    info->type = type.type;
    info->subtype = type.subtype;
}

/**
 * Get a bsscfg pointer, failing if the bsscfg does not alreay exist.
 * Sets the bsscfg pointer in any event.
 * Returns BCME_RANGE if the index is out of range or BCME_NOTFOUND
 * if the wlc->bsscfg[i] pointer is null
 */
wlc_bsscfg_t *
wlc_bsscfg_find(wlc_info_t *wlc, int idx, int *perr)
{
    wlc_bsscfg_t *bsscfg;

    if ((idx < 0) || (idx >= WLC_MAXBSSCFG)) {
        if (perr)
            *perr = BCME_RANGE;
        return NULL;
    }

    bsscfg = wlc->bsscfg[idx];
    if (perr)
        *perr = bsscfg ? 0 : BCME_NOTFOUND;

    return bsscfg;
}

/**
 * allocs/inits that can't go in wlc_bsscfg_malloc() (which is
 * called when wlc->wl for example may be invalid...)
 * come in here.
 */
static int
wlc_bsscfg_alloc_int(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, int idx,
    wlc_bsscfg_type_t *type, uint flags, uint32 flags2, struct ether_addr *ea)
{
    int err;

    wlc->bsscfg[idx] = bsscfg;
    bsscfg->_idx = (int8)idx;

    wlc_bsscfg_ID_assign(wlc, bsscfg);

    if ((err = wlc_bsscfg_init_int(wlc, bsscfg, type, flags, flags2, ea)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_init_int() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, err));
        goto fail;
    }

    /* create a new upper-edge driver interface */
    if ((err = wlc_bsscfg_init_intf(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_init_intf() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, err));
        goto fail;
    }

    return BCME_OK;

fail:
    return err;
}

/**
 * Called fairly late during wlc_attach(), when most modules already completed attach.
 */
int
BCMATTACHFN(wlc_bsscfg_primary_init)(wlc_info_t *wlc)
{
    wlc_bsscfg_t *bsscfg = NULL;
    int err;
    int idx;
    wlc_bsscfg_type_t type = {BSSCFG_TYPE_GENERIC, BSSCFG_SUBTYPE_NONE};
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 freemem_before, bytes_allocated;

    hnd_get_heapuse(&mu);
    freemem_before = mu.arena_free;
#endif /* MEM_ALLOC_STATS */

    idx = wlc_bsscfg_get_free_idx(wlc);
    if (idx == BCME_ERROR) {
        WL_ERROR(("wl%d: no free bsscfg\n", wlc->pub->unit));
        err = BCME_NORESOURCE;
        goto fail;
    }

    if ((bsscfg = wlc_bsscfg_malloc(wlc)) == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
                  wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        err = BCME_NOMEM;
        goto fail;
    }
    wlc->primary_bsscfg = bsscfg;

    type.subtype = wlc->pub->_ap ? BSSCFG_GENERIC_AP : BSSCFG_GENERIC_STA;

    if ((err = wlc_bsscfg_alloc_int(wlc, bsscfg, idx, &type, 0, 0,
            &wlc->pub->cur_etheraddr)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_alloc_int() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, err));
        goto fail;
    }

    if ((err = wlc_bsscfg_init(wlc, bsscfg)) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_init() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, err));
        goto fail;
    }

    if ((bsscfg->mbssid_tag_list = (wlc_mbssid_tag_list_t *) MALLOCZ(wlc->osh,
            sizeof(wlc_mbssid_tag_list_t))) == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
            wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    bytes_allocated = freemem_before - mu.arena_free;
    bsscfg->mem_bytes += bytes_allocated;
    if (bytes_allocated > mu.max_bsscfg_alloc) {
        mu.max_bsscfg_alloc = bytes_allocated;
    }
    mu.total_bsscfg_alloc += bytes_allocated;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

    return BCME_OK;

fail:
    if (bsscfg != NULL)
        wlc_bsscfg_free(wlc, bsscfg);
    return err;
}

static bool
wlc_wlcif_valid(wlc_info_t *wlc, wlc_if_t *wlcif)
{
    wlc_if_t *p;
    bool valid = FALSE;

    if (wlcif == NULL) {
        WL_ERROR(("%s: wlcif is NULL\n", __FUNCTION__));
        return FALSE;
    }

    if (wlc == NULL) {
        /* still a valid wlcif */
        return TRUE;
    }

    p = wlc->wlcif_list;
    while (p != NULL) {
        if (p == wlcif) {
            valid = TRUE;
            break;
        }
        p = p->next;
    }

    if (!valid) {
        WL_ERROR(("%s: wl%d: Not a valid wlcif %p\n",
            __FUNCTION__, wlc->pub->unit, wlcif));
    }

    return valid;
}

/*
 * Find a bsscfg from matching cur_etheraddr, BSSID, SSID, or something unique.
 */

/** match wlcif */
wlc_bsscfg_t *
wlc_bsscfg_find_by_wlcif(wlc_info_t *wlc, wlc_if_t *wlcif)
{
    wlc_bsscfg_t *ret = NULL; /**< return value */

    /* wlcif being NULL implies primary interface hence primary bsscfg */
    if (wlcif == NULL) {
        if (wlc != NULL) {
            ret = wlc->primary_bsscfg;
        }
    } else {
        if (!wlc_wlcif_valid(wlc, wlcif)) {
            return NULL;
        }

        switch (wlcif->type) {
        case WLC_IFTYPE_BSS:
            ret = wlcif->u.bsscfg;
            break;
#ifdef AP
        case WLC_IFTYPE_WDS:
            ret = SCB_BSSCFG(wlcif->u.scb);
            break;
#endif
        default:
            WL_ERROR(("wl%d: Unknown wlcif %p type %d\n",
                (wlc) ? (wlc->pub->unit) : -1,
                OSL_OBFUSCATE_BUF(wlcif), wlcif->type));
            break;
        }
    }

    ASSERT(ret != NULL);
    return ret;
} /* wlc_bsscfg_find_by_wlcif */

/* match cur_etheraddr */
wlc_bsscfg_t * BCMFASTPATH
wlc_bsscfg_find_by_hwaddr(wlc_info_t *wlc, struct ether_addr *hwaddr)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    if (ETHER_ISNULLADDR(hwaddr) || ETHER_ISMULTI(hwaddr)) {
        return NULL;
    }

    FOREACH_BSS(wlc, i, bsscfg) {
        if (eacmp(hwaddr->octet, bsscfg->cur_etheraddr.octet)) {
            continue;
        }
        return bsscfg;
    }

    return NULL;
}

/** match BSSID */
wlc_bsscfg_t *
wlc_bsscfg_find_by_bssid(wlc_info_t *wlc, const struct ether_addr *bssid)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    if (ETHER_ISNULLADDR(bssid) || ETHER_ISMULTI(bssid)) {
        return NULL;
    }

    FOREACH_BSS(wlc, i, bsscfg) {
        if (eacmp(bssid->octet, bsscfg->BSSID.octet)) {
            continue;
        }
        return bsscfg;
    }

    return NULL;
}

/** match cur_etheraddr and BSSID */
wlc_bsscfg_t * BCMFASTPATH
wlc_bsscfg_find_by_hwaddr_bssid(wlc_info_t *wlc,
    const struct ether_addr *hwaddr, const struct ether_addr *bssid)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    if (ETHER_ISMULTI(hwaddr) || ETHER_ISMULTI(bssid)) {
        return NULL;
    }

    FOREACH_BSS(wlc, i, bsscfg) {
        if (eacmp(hwaddr->octet, bsscfg->cur_etheraddr.octet)) {
            continue;
        }
        if (eacmp(bssid->octet, bsscfg->BSSID.octet)) {
            continue;
        }
        return bsscfg;
    }

    return NULL;
}

/** match target_BSSID */
wlc_bsscfg_t *
wlc_bsscfg_find_by_target_bssid(wlc_info_t *wlc, const struct ether_addr *bssid)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    if (ETHER_ISNULLADDR(bssid) || ETHER_ISMULTI(bssid)) {
        return NULL;
    }

    FOREACH_STA(wlc, i, bsscfg) {
        if (bsscfg->target_bss->max_mbssid_indicator &&
            (eacmp(bssid->octet, bsscfg->target_bss->mbssid_transmitted.octet) == 0)) {
            return bsscfg;
        }
        if (eacmp(bssid->octet, bsscfg->target_bss->BSSID.octet)) {
            continue;
        }
        return bsscfg;
    }

    return NULL;
}

/** match current_mbssid */
wlc_bsscfg_t *
wlc_bsscfg_find_by_current_mbssid(wlc_info_t *wlc, const struct ether_addr *bssid)
{
    int i;
    wlc_bsscfg_t *bsscfg;
    wlc_bss_info_t *current_bss;

    if (ETHER_ISNULLADDR(bssid) || ETHER_ISMULTI(bssid))
        return NULL;

    FOREACH_BSS(wlc, i, bsscfg) {
        if (!BSSCFG_STA(bsscfg) || !bsscfg->associated)
            continue;

        current_bss = bsscfg->current_bss;
        if (current_bss && current_bss->max_mbssid_indicator && current_bss->mbssid_index &&
                (eacmp(bssid->octet, current_bss->mbssid_transmitted.octet) == 0)) {
            return bsscfg;
        }
    }

    return NULL;
}

/** match SSID */
wlc_bsscfg_t *
wlc_bsscfg_find_by_ssid(wlc_info_t *wlc, uint8 *ssid, int ssid_len)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    if (!ssid_len) {
        return NULL;
    }

    FOREACH_BSS(wlc, i, bsscfg) {
        if (ssid_len != bsscfg->SSID_len) {
            continue;
        }
        if (bcmp(ssid, bsscfg->SSID, ssid_len)) {
            continue;
        }
        return bsscfg;
    }
    return NULL;
}

/** match ID */
wlc_bsscfg_t *
wlc_bsscfg_find_by_ID(wlc_info_t *wlc, uint16 id)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    FOREACH_BSS(wlc, i, bsscfg) {
        if (bsscfg->ID != id) {
            continue;
        }
        return bsscfg;
    }

    return NULL;
}

static void
wlc_bsscfg_bss_init(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    wlc_bss_info_t * bi = wlc->default_bss;

    bcopy((char*)bi, (char*)bsscfg->target_bss, sizeof(wlc_bss_info_t));
    bcopy((char*)bi, (char*)bsscfg->current_bss, sizeof(wlc_bss_info_t));
}

/* reset all configurations */
static int
wlc_bsscfg_init_int(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
    wlc_bsscfg_type_t *type, uint flags, uint32 flags2, struct ether_addr *ea)
{
    brcm_ie_t *brcm_ie;
#ifdef MULTIAP
    multiap_ie_t *multiap_ie = NULL;
#endif    /* MULTIAP */

    ASSERT(bsscfg != NULL);
    ASSERT(ea != NULL);

    bsscfg->wlc = wlc;

    bsscfg->flags = flags;
    bsscfg->flags2 = flags2;

    wlc_bsscfg_type_set(wlc, bsscfg, type);

    bcopy(ea, &bsscfg->cur_etheraddr, ETHER_ADDR_LEN);

    /* Match Wi-Fi default of true for aExcludeUnencrypted,
     * instead of 802.11 default of false.
     */
    bsscfg->wsec_restrict = TRUE;

    /* disable 802.1X authentication by default */
    bsscfg->eap_restrict = FALSE;

    /* disable WPA by default */
    bsscfg->WPA_auth = WPA_AUTH_DISABLED;
#ifdef ACKSUPR_MAC_FILTER
    /* acksupr initialization */
    bsscfg->acksupr_mac_filter = FALSE;
#endif /* ACKSUPR_MAC_FILTER */
    wlc_bsscfg_bss_init(wlc, bsscfg);

    /* initialize our proprietary elt */
    brcm_ie = (brcm_ie_t *)&bsscfg->brcm_ie[0];
    bzero((char*)brcm_ie, sizeof(brcm_ie_t));
    brcm_ie->id = DOT11_MNG_PROPR_ID;
    brcm_ie->len = BRCM_IE_LEN - TLV_HDR_LEN;
    bcopy(BRCM_OUI, &brcm_ie->oui[0], DOT11_OUI_LEN);
    brcm_ie->ver = BRCM_IE_VER;

    wlc_bss_update_brcm_ie(wlc, bsscfg);

    /* Copy the  edca_update_count value from primary to all the VAP if the primary is
     * an AP. Issue was every rc restart/ nvram restart edca_update_count was getting updated
     * in primary, As all the VAP are recreated after the rc restart/nvram restart
     * different values in primary and VAP are causing MBSSID to add the vendor specific IE
     * in non transmitted profile.
     */
    if (BSSCFG_AP(bsscfg) && wlc->primary_bsscfg && !BSSCFG_IS_PRIMARY(bsscfg) &&
            BSSCFG_AP(wlc->primary_bsscfg)) {
        bsscfg->edca_update_count = wlc->primary_bsscfg->edca_update_count;
    }
#ifdef MULTIAP
    /* initialize multiap  ie */
    multiap_ie = (multiap_ie_t *)&bsscfg->multiap_ie[0];
    multiap_ie->id = DOT11_MNG_VS_ID;
    multiap_ie->len = MAP_IE_FIXED_LEN;
    memcpy(&multiap_ie->oui[0], WFA_OUI,  DOT11_OUI_LEN);
    multiap_ie->type = WFA_OUI_TYPE_MULTIAP;
    wlc_update_multiap_ie(wlc, bsscfg);
#endif  /* MULTIAP */

    if (D11REV_GE(wlc->pub->corerev, 128)) {
        bsscfg->flags2 |= WLC_BSSCFG_FL2_EMPTY_SSID_ON_CLOSED_NW;
    }

    return BCME_OK;
}

/* allocate wlcif and create network interface if necessary */
static int
wlc_bsscfg_init_intf(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    /* Allocate the memory if it not through interface_create command */
    if (bsscfg->wlcif == NULL) {
        bsscfg->wlcif = wlc_wlcif_alloc(wlc, wlc->osh, WLC_IFTYPE_BSS,
            wlc->def_primary_queue);
    }
    if (bsscfg->wlcif == NULL) {
        WL_ERROR(("wl%d: %s: failed to alloc wlcif\n",
                  wlc->pub->unit, __FUNCTION__));
        return BCME_NOMEM;
    }
    bsscfg->wlcif->u.bsscfg = bsscfg;

    /* create an OS interface for non-primary bsscfg */
    if (BSSCFG_IS_PRIMARY(bsscfg)) {
        /* primary interface has an implicit wlif which is assumed when
         * the wlif pointer is NULL.
         */
        /* XXX WES: really want to make this assumption go away and
         * have the wlcif of the primary interface have a non-null
         * wlif pointer.
         */
        bsscfg->wlcif->if_flags |= WLC_IF_LINKED;
    }
    else if (!BSSCFG_HAS_NOIF(bsscfg)) {
        uint idx = WLC_BSSCFG_IDX(bsscfg);
#ifdef MBSS
        if (MBSS_ENAB(wlc->pub) &&
            !memcmp(&wlc->pub->cur_etheraddr, &bsscfg->cur_etheraddr, ETHER_ADDR_LEN)) {
            wlc_bsscfg_macgen(wlc, bsscfg);
        }
#endif
        bsscfg->wlcif->wlif = wl_add_if(wlc->wl, bsscfg->wlcif, idx, NULL);
        if (bsscfg->wlcif->wlif == NULL) {
            WL_ERROR(("wl%d: %s: wl_add_if failed for"
                      " index %d\n", wlc->pub->unit, __FUNCTION__, idx));
            return BCME_ERROR;
        }
        bsscfg->wlcif->if_flags |= WLC_IF_VIRTUAL;
        bsscfg->wlcif->if_flags |= WLC_IF_LINKED;
#ifdef WL_CFG80211_NIC
        bsscfg->wlcif->index = idx;
#endif /* WL_CFG80211_NIC */
        wlc_if_event(wlc, WLC_E_IF_ADD, bsscfg->wlcif);
    }

    return BCME_OK;
}

static int
wlc_bsscfg_bcmcscbinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, enum wlc_bandunit bandunit)
{
    ASSERT(bsscfg != NULL);
    ASSERT(wlc != NULL);

    if (!bsscfg->bcmc_scb) {
        bsscfg->bcmc_scb =
            wlc_bcmcscb_alloc(wlc, bsscfg, wlc->bandstate[bandunit]);
        WL_INFORM(("wl%d: wlc_bsscfg_bcmcscbinit: band %d: alloc internal scb 0x%p for "
               "bsscfg 0x%p\n", wlc->pub->unit, bandunit, bsscfg->bcmc_scb, bsscfg));
    }

    if (!bsscfg->bcmc_scb) {
        WL_ERROR(("wl%d: %s: fail to alloc scb for bsscfg 0x%p\n",
                  wlc->pub->unit, __FUNCTION__, bsscfg));
        return BCME_NOMEM;
    }

    return BCME_OK;
}

static int
wlc_bsscfg_bcmcscballoc(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    enum wlc_bandunit bandunit = wlc->band->bandunit;

    ASSERT(bsscfg != NULL);
    ASSERT(wlc != NULL);

    /* If BCMC SCB is allocated previously then do not allocate */
    if (!BSSCFG_HAS_BCMC_SCB(bsscfg)) {
        if (wlc_bsscfg_bcmcscbinit(wlc, bsscfg, bandunit)) {
            WL_ERROR(("wl%d: %s: init failed for band %s\n",
                wlc->pub->unit, __FUNCTION__, wlc_bandunit_name(bandunit)));
            return BCME_NOMEM;
        }
    }

    /* Initialize the band unit to the one corresponding to the wlc */
    bsscfg->bcmc_scb->bandunit = bandunit;

    return BCME_OK;
}

#ifdef STA
/** Set/reset association parameters */
int
wlc_bsscfg_assoc_params_set(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
    wl_join_assoc_params_t *assoc_params, int assoc_params_len)
{

    wl_join_assoc_params_t *new = NULL;

    ASSERT(wlc != NULL);
    ASSERT(bsscfg != NULL);

    /* If actually setting data, malloc and prepare the block */
    if (assoc_params && assoc_params_len) {
        if ((new = MALLOCZ(wlc->osh, assoc_params_len)) == NULL) {
            WL_ERROR((WLC_BSS_MALLOC_ERR, WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg),
                      __FUNCTION__, assoc_params_len, MALLOCED(wlc->osh)));
            return BCME_NOMEM;
        }
        bcopy(assoc_params, new, assoc_params_len);
    }

    /* Remove and release any old one */
    if (bsscfg->assoc_params != NULL) {
        MFREE(wlc->osh, bsscfg->assoc_params, bsscfg->assoc_params_len);
        bsscfg->assoc_params = NULL;
        bsscfg->assoc_params_len = 0;
    }

    /* Put the new one in the bsscfg */
    if (new) {
        bsscfg->assoc_params = new;
        bsscfg->assoc_params_len = (uint16)assoc_params_len;
    }

    return BCME_OK;
}

int
wlc_bsscfg_assoc_params_reset(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    if (bsscfg != NULL) {
        return (wlc_bsscfg_assoc_params_set(wlc, bsscfg, NULL, 0));
    }

    return BCME_ERROR;
}

/** Set/reset scan parameters */
int
wlc_bsscfg_scan_params_set(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
    wl_join_scan_params_t *scan_params)
{
    ASSERT(bsscfg != NULL);

    if (scan_params == NULL) {
        if (bsscfg->scan_params != NULL) {
            MFREE(wlc->osh, bsscfg->scan_params, sizeof(wl_join_scan_params_t));
            bsscfg->scan_params = NULL;
        }
        return BCME_OK;
    }
    else if (bsscfg->scan_params != NULL ||
             (bsscfg->scan_params = MALLOC(wlc->osh, sizeof(wl_join_scan_params_t))) != NULL) {
        bcopy(scan_params, bsscfg->scan_params, sizeof(wl_join_scan_params_t));
        return BCME_OK;
    }

    WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
        wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
    return BCME_NOMEM;
}

void
wlc_bsscfg_scan_params_reset(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    if (bsscfg != NULL)
        wlc_bsscfg_scan_params_set(wlc, bsscfg, NULL);
}
#endif /* STA */

void
wlc_bsscfg_SSID_set(wlc_bsscfg_t *bsscfg, const uint8 *SSID, int len)
{
    ASSERT(bsscfg != NULL);
    ASSERT(len <= DOT11_MAX_SSID_LEN);

    if ((bsscfg->SSID_len = (uint8)len) > 0) {
        ASSERT(SSID != NULL);
        /* need to use memove here to handle overlapping copy */
        memmove(bsscfg->SSID, SSID, len);

        if (len < DOT11_MAX_SSID_LEN)
            bzero(&bsscfg->SSID[len], DOT11_MAX_SSID_LEN - len);
             bsscfg->short_ssid = ~hndcrc32(bsscfg->SSID, bsscfg->SSID_len, CRC32_INIT_VALUE);
        return;
    }

    bzero(bsscfg->SSID, DOT11_MAX_SSID_LEN);
}

static void
wlc_bsscfg_update_ext_cap_len(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    bsscfg_module_t *bcmh = wlc->bcmh;
    int i;

    for (i = bcmh->ext_cap_max - 1; i >= 0; i--) {
        if (bsscfg->ext_cap[i] != 0)
            break;
    }

    bsscfg->ext_cap_len = i + 1;
}

int
wlc_bsscfg_set_ext_cap(wlc_bsscfg_t *bsscfg, uint32 bit, bool val)
{
    wlc_info_t *wlc = bsscfg->wlc;
    bsscfg_module_t *bcmh = wlc->bcmh;

    if (((bit + 8) >> 3) > bcmh->ext_cap_max)
        return BCME_RANGE;

    if (val)
        setbit(bsscfg->ext_cap, bit);
    else
        clrbit(bsscfg->ext_cap, bit);

    wlc_bsscfg_update_ext_cap_len(wlc, bsscfg);

    return BCME_OK;
}

bool
wlc_bsscfg_test_ext_cap(wlc_bsscfg_t *bsscfg, uint32 bit)
{
    bsscfg_module_t *bcmh = bsscfg->wlc->bcmh;

    if (((bit + 8) >> 3) > bcmh->ext_cap_max)
        return FALSE;

    return isset(bsscfg->ext_cap, bit);
}

/** helper function for per-port code to call to query the "current" chanspec of a BSS */
chanspec_t
wlc_get_home_chanspec(wlc_bsscfg_t *cfg)
{
    ASSERT(cfg != NULL);

    if (cfg->associated)
        return cfg->current_bss->chanspec;

    return cfg->wlc->home_chanspec;
}

/** helper function for per-port code to call to query the "current" bss info of a BSS */
wlc_bss_info_t *
wlc_get_current_bss(wlc_bsscfg_t *cfg)
{
    ASSERT(cfg != NULL);

    return cfg->current_bss;
}

/**
 * Do multicast filtering
 * returns TRUE on success (reject/filter frame).
 */
bool
wlc_bsscfg_mcastfilter(wlc_bsscfg_t *cfg, struct ether_addr *da)
{
    unsigned int i;

    for (i = 0; i < cfg->nmulticast; i++) {
        if (bcmp((void*)da, (void*)&cfg->multicast[i],
            ETHER_ADDR_LEN) == 0)
            return FALSE;
    }

    return TRUE;
}

#if defined(BCMDBG)

static const struct {
    bsscfg_type_t type;
    char name[12];
} bsscfg_type_names[] = {
    {BSSCFG_TYPE_GENERIC, "GENERIC"},
    {BSSCFG_TYPE_PSTA, "PSTA"},
    {BSSCFG_TYPE_P2P, "P2P"},
    {BSSCFG_TYPE_TDLS, "TDLS"},
    {BSSCFG_TYPE_PROXD, "PROXD"},
};

static const char *
_wlc_bsscfg_type_name(bsscfg_type_t type)
{
    uint i;

    for (i = 0; i < ARRAYSIZE(bsscfg_type_names); i++) {
        if (bsscfg_type_names[i].type == type)
            return bsscfg_type_names[i].name;
    }

    return "UNKNOWN";
}

static const struct {
    uint subtype;
    char name[10];
} bsscfg_subtype_names[] = {
    {BSSCFG_GENERIC_AP, "AP"},
    {BSSCFG_GENERIC_STA, "STA"},
    {BSSCFG_GENERIC_IBSS, "IBSS"},
    {BSSCFG_P2P_GO, "GO"},
    {BSSCFG_P2P_GC, "GC"},
    {BSSCFG_P2P_DISC, "DISC"},
};

static const char *
_wlc_bsscfg_subtype_name(uint subtype)
{
    uint i;

    for (i = 0; i < ARRAYSIZE(bsscfg_subtype_names); i++) {
        if (bsscfg_subtype_names[i].subtype == subtype)
            return bsscfg_subtype_names[i].name;
    }

    return "UNKNOWN";
}

static const bcm_bit_desc_t bsscfg_flags[] = {
    {WLC_BSSCFG_PRESERVE, "PRESERVE"},
    {WLC_BSSCFG_WME_DISABLE, "WME_DIS"},
    {WLC_BSSCFG_PS_OFF_TRANS, "PSOFF_TRANS"},
    {WLC_BSSCFG_SW_BCN, "SW_BCN"},
    {WLC_BSSCFG_SW_PRB, "SW_PRB"},
    {WLC_BSSCFG_HW_BCN, "HW_BCN"},
    {WLC_BSSCFG_HW_PRB, "HW_PRB"},
    {WLC_BSSCFG_NOIF, "NOIF"},
    {WLC_BSSCFG_11N_DISABLE, "11N_DIS"},
    {WLC_BSSCFG_11H_DISABLE, "11H_DIS"},
    {WLC_BSSCFG_NATIVEIF, "NATIVEIF"},
    {WLC_BSSCFG_P2P_DISC, "P2P_DISC"},
    {0, NULL}
};

static int
_wlc_bsscfg_dump(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct bcmstrbuf *b)
{
    char ssidbuf[SSID_FMT_BUF_LEN];
    char bssbuf[ETHER_ADDR_STR_LEN];
    char ifname[32];
    int i;
    int bsscfg_idx = WLC_BSSCFG_IDX(cfg);
    bsscfg_module_t *bcmh = wlc->bcmh;
    char flagstr[64];
    wlc_bsscfg_type_t type;
    uint8 cap_num_msdu;

    bcm_bprintf(b, ">>>>>>>> BSS Config %d (0x%p) <<<<<<<<\n",
        bsscfg_idx, OSL_OBFUSCATE_BUF(cfg));

    wlc_format_ssid(ssidbuf, cfg->SSID, cfg->SSID_len);

    strncpy(ifname, wl_ifname(wlc->wl, cfg->wlcif->wlif), sizeof(ifname));
    ifname[sizeof(ifname) - 1] = '\0';

    bcm_bprintf(b, "SSID): \"%s\" BSSID: %s\n",
                ssidbuf, bcm_ether_ntoa(&cfg->BSSID, bssbuf));
    if (BSSCFG_STA(cfg) && cfg->BSS)
        bcm_bprintf(b, "AID: 0x%x\n", cfg->AID);

    bcm_bprintf(b, "_ap %d BSS %d enable %d. up %d. associated %d flags 0x%x\n",
                BSSCFG_AP(cfg), cfg->BSS, cfg->enable, cfg->up, cfg->associated, cfg->flags);
    bcm_format_flags(bsscfg_flags, cfg->flags, flagstr, sizeof(flagstr));
    bcm_bprintf(b, "flags: 0x%x [%s]\n", cfg->flags, flagstr);
    wlc_bsscfg_type_get(wlc, cfg, &type);
    bcm_bprintf(b, "type: %s (%s %s)\n", _wlc_bsscfg_type_name(cfg->type),
                _wlc_bsscfg_type_name(type.type), _wlc_bsscfg_subtype_name(type.subtype));
    bcm_bprintf(b, "flags2: 0x%x\n", cfg->flags2);

    /* allmulti and multicast lists */
    bcm_bprintf(b, "allmulti %d\n", cfg->allmulti);
    bcm_bprintf(b, "nmulticast %d\n", cfg->nmulticast);
    if (cfg->nmulticast) {
        for (i = 0; i < (int)cfg->nmulticast; i++)
            bcm_bprintf(b, "%s ", bcm_ether_ntoa(&cfg->multicast[i], bssbuf));
        bcm_bprintf(b, "\n");
    }

    bcm_bprintf(b, "cur_etheraddr %s\n", bcm_ether_ntoa(&cfg->cur_etheraddr, bssbuf));
    bcm_bprintf(b, "wlcif: flags 0x%x wlif 0x%p \"%s\" qi 0x%p\n",
                cfg->wlcif->if_flags, OSL_OBFUSCATE_BUF(cfg->wlcif->wlif),
            ifname, OSL_OBFUSCATE_BUF(cfg->wlcif->qi));
    bcm_bprintf(b, "ap_isolate %d\n", cfg->ap_isolate);
    bcm_bprintf(b, "bfe_cap %d ", cfg->bfe_capable);
    bcm_bprintf(b, "closednet %d nobcnssid %d\n", cfg->closednet, cfg->nobcnssid);
    bcm_bprintf(b, "wsec 0x%x auth %d\n", cfg->wsec, cfg->auth);
    bcm_bprintf(b, "WPA_auth 0x%x wsec_restrict %d eap_restrict %d",
        cfg->WPA_auth, cfg->wsec_restrict, cfg->eap_restrict);
    bcm_bprintf(b, "\n");

    bcm_bprintf(b, "Extended Capabilities: ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_OBSS_COEX_MGMT))
        bcm_bprintf(b, "obss_coex ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_SPSMP))
        bcm_bprintf(b, "spsmp ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_PROXY_ARP))
        bcm_bprintf(b, "proxy_arp ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_CIVIC_LOC))
        bcm_bprintf(b, "civic_loc ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_LCI))
        bcm_bprintf(b, "lci ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_BSSTRANS_MGMT))
        bcm_bprintf(b, "bsstrans ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_IW))
        bcm_bprintf(b, "inwk ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_SI))
        bcm_bprintf(b, "si ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_OPER_MODE_NOTIF))
        bcm_bprintf(b, "oper_mode ");
#ifdef WL_FTM
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_FTM_RESPONDER))
        bcm_bprintf(b, "ftm-rx ");
    if (isset(cfg->ext_cap, DOT11_EXT_CAP_FTM_INITIATOR))
        bcm_bprintf(b, "ftm-tx ");
#endif /* WL_FTM */
    /* Bit 63 - 64
     * Indicates the maximum number of MSDUs in an A-MSDU that the STA is able to
     * receive from a VHT STA:
     * Set to 0 to indicate that no limit applies. (Bit64 = 0, Bit63 = 0)
     * Set to 1 for 32. (Bit64 = 0, Bit63 = 1)
     * Set to 2 for 16. (Bit64 = 1, Bit63 = 0)
     * Set to 3 for 8. (Bit64 = 1, Bit63 = 1)
     * Reserved if A-MSDU is not supported or if the STA is not an HT STA.
    */
    cap_num_msdu = isset(cfg->ext_cap, DOT11_EXT_CAP_NUM_MSDU) ? 1 : 0;
    cap_num_msdu |= isset(cfg->ext_cap, DOT11_EXT_CAP_NUM_MSDU+1) ? (1 << 1) : 0;
    bcm_bprintf(b, "msdu%d ", (64 >> cap_num_msdu));

    bcm_bprintf(b, "\n");

#ifdef MEM_ALLOC_STATS
    /* display memory usage information */
    bcm_bprintf(b, "BSSCFG Memory Allocated: %d bytes\n", cfg->mem_bytes);
#endif

    bcm_bprintf(b, "-------- bcmc scb --------\n");
    if (cfg->bcmc_scb != NULL) {
        wlc_scb_dump_scb(wlc, cfg, cfg->bcmc_scb, b, -1);
    }

#ifdef DWDS
    /* Display DWDS SA list */
    if (BSSCFG_STA(cfg) && MAP_ENAB(cfg) && cfg->dwds_loopback_filter) {
        wlc_dwds_dump_sa_list(wlc, cfg, b);
    }
#endif /* DWDS */

    /* invoke bsscfg cubby dump function */
    bcm_bprintf(b, "cfg base size: %u\n", sizeof(wlc_bsscfg_t));
    bcm_bprintf(b, "-------- bsscfg cubbies --------\n");
    wlc_cubby_dump(bcmh->cubby_info, cfg, NULL, NULL, b);

    /* display bsscfg up/down callbacks */
    bcm_bprintf(b, "-------- up/down notify list --------\n");
    bcm_notif_dump_list(bcmh->up_down_notif_hdl, b);

    /* display bsscfg state change callbacks */
    bcm_bprintf(b, "-------- state change notify list --------\n");
    bcm_notif_dump_list(bcmh->state_upd_notif_hdl, b);

    /* display bss pretbtt query update callbacks */
    bcm_bprintf(b, "-------- pretbtt query notify list ---------\n");
    bcm_notif_dump_list(bcmh->pretbtt_query_hdl, b);

    /* display bss bcn rx notify callbacks */
    bcm_bprintf(b, "-------- bcn rx notify list --------\n");
    bcm_notif_dump_list(bcmh->rxbcn_nh, b);

    return BCME_OK;
}

static int
wlc_bsscfg_dump(wlc_info_t *wlc, struct bcmstrbuf *b)
{
    int i;
    wlc_bsscfg_t *bsscfg;

    bcm_bprintf(b, "# of bsscfgs: %u\n", wlc_bss_count(wlc));

    FOREACH_BSS(wlc, i, bsscfg) {
        _wlc_bsscfg_dump(wlc, bsscfg, b);
        bcm_bprintf(b, "\n");
    }

    return 0;
}
#endif

static bool
wlc_bsscfg_preserve(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    bool preserve = FALSE;

    ASSERT(cfg);
#ifdef WOWL
    /* return false if WoWL is not enable or active */
    if (!WOWL_ENAB(wlc->pub) || !WOWL_ACTIVE(wlc->pub))
        preserve = FALSE;
#endif /* WOWL */
    return preserve;
}

/** Return the number of bsscfgs present now */
int
wlc_bss_count(wlc_info_t *wlc)
{
    uint16 i, ccount = 0;
    wlc_bsscfg_t *bsscfg;

    FOREACH_BSS(wlc, i, bsscfg) {
        ccount++;
    }

    return ccount;
}

/* Get appropriate wlc from the wlcif */
wlc_info_t *
wlc_bsscfg_get_wlc_from_wlcif(wlc_info_t *wlc, wlc_if_t *wlcif)
{
    wlc_bsscfg_t *bsscfg;

    /* this function gets called from NATOE, which is a feature that is going to be removed */
    ASSERT(0);

    bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);

    if (bsscfg != NULL) {
        return bsscfg->wlc;
    } else {
        return NULL;
    }
}

wlc_bsscfg_t*
wlc_bsscfg_alloc_iface_create(wlc_info_t *wlc, wlc_bsscfg_create_param_t *bsscfg_create_param,
    int32 *err)
{
    wlc_bsscfg_t *bsscfg;
    struct ether_addr *p_ether_addr;

    bsscfg = wlc_bsscfg_malloc(wlc);
    if (bsscfg == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
                  wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        *err = BCME_NOMEM;
        return (NULL);
    }

    ASSERT(wlc->bsscfg[bsscfg_create_param->bsscfg_idx] == NULL);
    bsscfg->_idx = bsscfg_create_param->bsscfg_idx;

    wlc_bsscfg_ID_assign(wlc, bsscfg);

    if (bsscfg_create_param->if_buf->flags & WL_INTERFACE_MAC_USE) {
        p_ether_addr = &bsscfg_create_param->if_buf->mac_addr;
    } else {
        p_ether_addr = &wlc->pub->cur_etheraddr;
    }

    wlc->bsscfg[bsscfg_create_param->bsscfg_idx] = bsscfg;

    *err = wlc_bsscfg_init_int(wlc, bsscfg, bsscfg_create_param->type,
            bsscfg_create_param->bsscfg_flag, 0, p_ether_addr);
    if (*err != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_init_int() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, *err));
        goto fail;
    }

    /* create a new upper-edge driver interface */
    bsscfg->wlcif = wlc_wlcif_alloc(wlc, wlc->osh, WLC_IFTYPE_BSS, wlc->def_primary_queue);
    if (bsscfg->wlcif == NULL) {
        WL_ERROR(("wl%d: %s: failed to alloc wlcif\n",
                  wlc->pub->unit, __FUNCTION__));
        *err = BCME_NOMEM;
        goto fail;
    }

    /* Initialize wlcif->index
     * 0, default setting to allocate next available if_index
     * otherwise, assign if_buf->if_index, if host has specified if_index
     */
    bsscfg->wlcif->index = bsscfg_create_param->if_buf->if_index;

    *err = wlc_bsscfg_init_intf(wlc, bsscfg);
    if (*err != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_init_intf() failed with %d\n",
                  wlc->pub->unit, __FUNCTION__, *err));
        goto fail;
    }

    if ((bsscfg->mbssid_tag_list = (wlc_mbssid_tag_list_t *) MALLOCZ(wlc->osh,
        sizeof(wlc_mbssid_tag_list_t))) == NULL) {
        WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
        wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        goto fail;
    }

    /* Set the interface type */
    bsscfg->wlcif->iface_type = bsscfg_create_param->if_buf->iftype;

    return (bsscfg);
fail:
    wlc->bsscfg[bsscfg_create_param->bsscfg_idx] = NULL;
    if (bsscfg != NULL) {
        wlc_bsscfg_mfree(wlc, bsscfg);
    }

    return (NULL);
}

/**
 * Function: wlc_iface_create_generic_bsscfg
 *
 * Purpose:
 *    This function creates generic bsscfg for AP or STA interface when interface_create IOVAR
 *    is issued.
 *
 * Input Parameters:
 *    wlc - wlc pointer
 *    if_buf - Interface create buffer pointer
 *    type - bsscfg type
 * Return:
 *    Success: newly created bsscfg pointer for the interface
 *    Failure: NULL pointer
 */
wlc_bsscfg_t*
wlc_iface_create_generic_bsscfg(wlc_info_t *wlc, wl_interface_create_t *if_buf,
    wlc_bsscfg_type_t *type, int32 *err)
{
    wlc_bsscfg_t *cfg;
    wlc_bsscfg_create_param_t bsscfg_create_param;
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 freemem_before, bytes_allocated;
#endif /* MEM_ALLOC_STATS */

    ASSERT((type->subtype == BSSCFG_GENERIC_AP) || (type->subtype == BSSCFG_GENERIC_STA));
    memset(&bsscfg_create_param, 0, sizeof(bsscfg_create_param));

    /* if interface idx is available from iovar fill it as bsscfg idx
     * else fetch free idx and fill it.
     */
    if (if_buf->if_index > 0) {
        bsscfg_create_param.bsscfg_idx = (int8)if_buf->if_index;
    } else {
        bsscfg_create_param.bsscfg_idx = (int8)wlc_bsscfg_get_free_idx(wlc);
    }
    if (bsscfg_create_param.bsscfg_idx < 0) {
        WL_ERROR(("wl%d: no free bsscfg\n", WLCWLUNIT(wlc)));
        *err = BCME_NORESOURCE;
        return (NULL);
    }

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    freemem_before = mu.arena_free;
#endif /* MEM_ALLOC_STATS */

    bsscfg_create_param.type = type;
    bsscfg_create_param.if_buf = if_buf;
    bsscfg_create_param.bsscfg_flag = 0;
#ifdef WL_CFG80211_NIC
    bsscfg_create_param.if_buf->if_index = bsscfg_create_param.bsscfg_idx;
#endif /* WL_CFG80211_NIC */
    cfg = wlc_bsscfg_alloc_iface_create(wlc, &bsscfg_create_param, err);
    if (cfg == NULL) {
        WL_ERROR(("wl%d: cannot allocate bsscfg\n", WLCWLUNIT(wlc)));
        return (cfg);
    }

    *err = wlc_bsscfg_init(wlc, cfg);
    if (*err != BCME_OK) {
        WL_ERROR(("wl%d: cannot init bsscfg\n", WLCWLUNIT(wlc)));
        wlc_bsscfg_free(wlc, cfg);
        return (NULL);
    }

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    bytes_allocated = freemem_before - mu.arena_free;
    cfg->mem_bytes += bytes_allocated;
    if (bytes_allocated > mu.max_bsscfg_alloc) {
        mu.max_bsscfg_alloc = bytes_allocated;
    }
    mu.total_bsscfg_alloc += bytes_allocated;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

    return (cfg);
}

int
wlc_set_iface_info(wlc_bsscfg_t *cfg, wl_interface_info_t *wl_info,
        wl_interface_create_t *if_buf)
{

    /* Assign the interface name */
    wl_info->bsscfgidx = cfg->_idx;
    memcpy(&wl_info->ifname, wl_ifname(cfg->wlc->wl, cfg->wlcif->wlif),
        sizeof(wl_info->ifname) - 1);
    wl_info->ifname[sizeof(wl_info->ifname) - 1] = 0;

    /* Copy the MAC addr */
    memcpy(&wl_info->mac_addr.octet, &cfg->cur_etheraddr.octet, ETHER_ADDR_LEN);

    /* Set the allocated interface index value */
    wl_info->if_index = cfg->wlcif->index;
    return BCME_OK;
}

/* Get corresponding ether addr of wlcif */
struct ether_addr *
wlc_bsscfg_get_ether_addr(wlc_info_t *wlc, wlc_if_t *wlcif)
{
    wlc_bsscfg_t *bsscfg;

    bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);

    return &bsscfg->cur_etheraddr;
}

bool
wlc_bsscfg_is_associated(wlc_bsscfg_t* bsscfg)
{
    return bsscfg->associated && !ETHER_ISNULLADDR(&bsscfg->current_bss->BSSID);
}

bool
wlc_bsscfg_mfp_supported(wlc_bsscfg_t *bsscfg)
{
    bool ret = FALSE;
    if (BSSCFG_INFRA_STA(bsscfg))
        ret = TRUE;
    return ret;
}

struct wlc_if*
wlcif_get_by_ifindex(wlc_info_t *wlc, int if_idx)
{
    wlc_if_t *wlcif;

    for (wlcif = wlc->wlcif_list; wlcif; wlcif = wlcif->next) {

        if (wlcif->wlif && (wlcif->index == if_idx) &&
            (wlcif->type == WLC_IFTYPE_BSS)) {

            return wlcif;
        }

    }
    return NULL;
}

#if defined(BCMDBG) || defined(DNG_DBGDUMP)
static int
wlc_bsscfg_multimac_dump(wlc_info_t *wlc, struct bcmstrbuf *b)
{
    int i;
    wlc_bsscfg_t *cfg;

    bcm_bprintf(b, "multimac entry\n");
    FOREACH_BSS(wlc, i, cfg) {
        bcm_bprintf(b, "wl%d.%d multimac_index=%d\t  bsscfg=0x%p\t"
            "ether [0x%p]["MACF"]\tbssid [0x%p]["MACF"]\t",
            wlc->pub->unit, cfg->_idx, i, cfg, &cfg->cur_etheraddr,
            ETHERP_TO_MACF(&cfg->cur_etheraddr), &cfg->BSSID,
            ETHERP_TO_MACF(&cfg->BSSID));
        bcm_bprintf(b, "\n");
    }
    return BCME_OK;
}
#endif

void
wlc_bsscfg_notif_signal(wlc_bsscfg_t *cfg, bsscfg_state_upd_data_t *st_data)
{
    wlc_info_t *wlc = cfg->wlc;
    bsscfg_module_t *bcmh = wlc->bcmh;

    bcm_notif_signal(bcmh->state_upd_notif_hdl, st_data);

    /* invoke bsscfg up callbacks */
    /* TO BE REPLACED BY ABOVE STATE CHANGE CALLBACK */
    if (cfg->up) {
        bsscfg_up_down_event_data_t evt_data;
        memset(&evt_data, 0, sizeof(evt_data));
        evt_data.bsscfg = cfg;
        evt_data.up    = TRUE;
        bcm_notif_signal(bcmh->up_down_notif_hdl, &evt_data);
    }
}

#ifdef WL_GLOBAL_RCLASS
bool
wlc_cur_opclass_global(wlc_info_t* wlc)
{
    uint8 cur_rclass = wlc_channel_get_cur_rclass(wlc);
    if (cur_rclass == BCMWIFI_RCLASS_TYPE_GBL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void
wlc_bsscfg_update_rclass(wlc_info_t* wlc)
{
    uint8 idx = 0;
    wlc_bsscfg_t *apcfg = NULL;
    bool switch_to_global_rclass = TRUE;

    /* switch to non global operating class if atleast one sta
     * which does not support global operating class associate to
     * any operational cfg of radio
     */
    FOREACH_UP_AP(wlc, idx, apcfg) {
        if (apcfg->scb_without_gbl_rclass) {
            switch_to_global_rclass = FALSE;
            break;
        }
    }
    WL_INFORM(("wl%d: switch_to_global_rclass[%d]\n", wlc->pub->unit, switch_to_global_rclass));
    /* All sta in bsscfg understand global operating class, switch to global
     * operating class, else maintain Country specific Operating class
     * operation
     */
    wlc_update_rcinfo(wlc->cmi, switch_to_global_rclass);
}
#endif /* WL_GLOBAL_RCLASS */
#ifdef MBO_AP
bool
wlc_bsscfg_is_mbo_enabled(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    return wlc_mbo_bsscfg_is_enabled(wlc, cfg);
}
#endif /* MBO_AP */

#if defined(WL_OCE) && defined(WL_OCE_AP)
bool
wlc_bsscfg_is_oce_enabled(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    return wlc_oce_bsscfg_is_enabled(wlc, cfg);
}
#endif /* wl_oce && WL_OCE_AP */

/* Check if any AP interface is up */
bool
wlc_is_ap_interface_up(wlc_info_t *wlc)
{
    int idx;
    wlc_bsscfg_t *apcfg;
    bool ap_up = FALSE;

    FOREACH_UP_AP(wlc, idx, apcfg) {
        ap_up = TRUE;
        break;
    }
    return ap_up;
}

int
wlc_bsscfg_apsta_is_downstream_ap_up_possible(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    chanspec_t chanspec;

    if (!APSTA_ENAB(wlc->pub)) {
        WL_INFORM(("wl%d: Radio is not in APSTA mode, donstream ap up possible\n",
            wlc->pub->unit));
        /* keep_ap_up options are required for APSTA mode */
        return BCME_OK;
    }

    if (wlc->primary_bsscfg->associated) {
        WL_INFORM(("wl%d: upstream connection is present, downstream ap up possible\n",
            wlc->pub->unit));
        /* let downstream ap to up irrespective of keep up options */
        return BCME_OK;
    }

    WL_INFORM(("wl%d :keep_ap_up[%d] home chanspec[%x] cfg->current_bss->chanspec[%x]"
        " target bss chanspec[%x] PI_RADIO_Chanspec[%x]\n", wlc->pub->unit,
        wlc->keep_ap_up, wlc->home_chanspec,
        (cfg->current_bss ? cfg->current_bss->chanspec : 0),
        (cfg->target_bss ? cfg->target_bss->chanspec : 0),
        phy_utils_get_chanspec(wlc->pi)));

    if (wlc->keep_ap_up == WLC_KEEP_AP_UP_NEVER) {
        WL_INFORM(("wl%d: keep_ap_up[%d] Not allowed to up ap cfg[%d] for"
            " home chanspec[%x]\n", wlc->pub->unit, wlc->keep_ap_up,
            WLC_BSSCFG_IDX(cfg), wlc->home_chanspec));
        return BCME_ERROR;
    } else if (wlc->keep_ap_up == WLC_KEEP_AP_UP_ON_NON_DFS) {
        chanspec = wlc->home_chanspec;
        if ((cfg->target_bss->chanspec != wlc->home_chanspec)) {
            chanspec = cfg->target_bss->chanspec;

        }
        /* timing condition like ap bsscfg is trying to come
         * up after sta completed scan. Lets select target chanspec
         */
        if (BAND_5G(wlc->band->bandtype) &&
            wlc_radar_chanspec(wlc->cmi, chanspec)) {

            return BCME_ERROR;
        }

        WL_INFORM(("wl%d: keep_ap_up[%d], up ap cfg[%d] for home chanspec[%x]\n",
            wlc->pub->unit, wlc->keep_ap_up, WLC_BSSCFG_IDX(cfg), wlc->home_chanspec));
    } else {
        WL_INFORM(("wl%d: keep_ap_up[%d] allowed ap cfg[%d] up for home chanspec[%x]\n",
            wlc->pub->unit, wlc->keep_ap_up, WLC_BSSCFG_IDX(cfg), wlc->home_chanspec));

        ;
    }
    return BCME_OK;
}

#ifdef WL11AX
void
wlc_addto_heblocklist(wlc_bsscfg_t *cfg, struct ether_addr *ea)
{
    int loop = cfg->block_he_list->cur_size;
    int i;
    for (i = 0; i < loop; i ++) {
        if (!ether_cmp(ea, &cfg->block_he_list->etheraddr[i])) {
            WL_INFORM(("add already in the list ea["MACF"] \n", ETHERP_TO_MACF(ea)));
            return;
        }
    }
    if (cfg->block_he_list->cur_size < cfg->block_he_list->block_he_list_size) {
        cfg->block_he_list->cur_size ++;
    }
    bcopy(ea, &cfg->block_he_list->etheraddr[cfg->block_he_list->index], ETHER_ADDR_LEN);
    if (cfg->block_he_list->index == (cfg->block_he_list->block_he_list_size - 1)) {
        cfg->block_he_list->index = 0;
    } else {
        cfg->block_he_list->index ++;
    }
}

int
wlc_isblocked_hemac(wlc_bsscfg_t *cfg, struct ether_addr *ea)
{
    int loop = cfg->block_he_list->cur_size;
    int i;

    for (i = 0; i < loop; i ++) {
        if (!ether_cmp(ea, &cfg->block_he_list->etheraddr[i])) {
            WL_INFORM(("blocked he ea["MACF"] \n", ETHERP_TO_MACF(ea)));
            return 1;
        }
    }
    return 0;
}
#endif /* WL11AX */

/*
 * This function walks through the active bsscfgs.
 * returns TRUE only when there are no active bsscfgs; Returns FALSE otherwise.
 */
bool
wlc_bsscfg_none_bss_active(wlc_info_t *wlc)
{
    wlc_bsscfg_t *bsscfg;
    int i;

    FOR_ALL_UP_BSS(wlc, i, bsscfg) {
        return FALSE;
    }
    return TRUE;
}

#if BAND6G || (defined(WL_OCE) && defined(WL_OCE_AP))
/* Extract max channel time information from FILS request parameter element
 * and decide whether probe response can be skipped with Beacon.
 */
bool
wlc_bsscfg_is_probe_response_required(wlc_bsscfg_t *cfg, uint8 *body, uint8 body_len)
{
    fils_request_parameters_element_t *pfils_params = NULL;
    bcm_tlv_t *tlv = NULL;
    uint32 now, elapsed, pending;
    uint8 max_chan_time;

    tlv = bcm_find_tlv_ext(body, body_len, DOT11_MNG_ID_EXT_ID, FILS_REQ_PARAMS_EXT_ID);

    if (!tlv) {
        return TRUE;
    }

    pfils_params = (fils_request_parameters_element_t*)tlv;

    if (pfils_params->length >  FILS_RQST_PARAM_MAX_LEN) {
        WL_ERROR(("wl%d: Invalid FILS request parameter, return\n", cfg->wlc->pub->unit));
        return TRUE;
    }

    max_chan_time = pfils_params->max_chan_time;

    /* OCE spec: 3.9 Beacon frame replaces a probe response frame:
     * An OCE AP shall not, transmit a Probe Response frame in response to a Probe
     * Request frame containing a Max Channel Time indication if a Beacon frame is
     * scheduled to be transmitted within the time period that is indicated in the
     * Max Channel Time field in the Probe Request frame.
     */
    if (max_chan_time > cfg->current_bss->beacon_period) {
        WL_INFORM(("wl%d:%d probe response is not required\n", cfg->wlc->pub->unit,
            WLC_BSSCFG_IDX(cfg)));

        return FALSE;
    }

    now = R_REG(cfg->wlc->osh, D11_TSFTimerLow(cfg->wlc));
    elapsed = now - cfg->wlc->prev_TBTT;
    elapsed /= 1000;
    pending = cfg->current_bss->beacon_period - elapsed;
    /* take account of beacon skipping exact beacon period window */
    pending += WL_PRB_RSP_CANCEL_GRACE_PERIOD;

    WL_INFORM(("wl%d:%d now[%d] elapsed[%d] pending[%d] max_chan_time[%d]\n",
        cfg->wlc->pub->unit, WLC_BSSCFG_IDX(cfg), now, elapsed, pending,
        max_chan_time));

    if (pending < max_chan_time) {
        WL_INFORM(("wl%d:%d probe response is not required\n", cfg->wlc->pub->unit,
            WLC_BSSCFG_IDX(cfg)));

        return FALSE;
    }
    return TRUE;
}
#endif /* WL_WIFI_6GHZ || (WL_OCE && WL_OCE_AP) */
