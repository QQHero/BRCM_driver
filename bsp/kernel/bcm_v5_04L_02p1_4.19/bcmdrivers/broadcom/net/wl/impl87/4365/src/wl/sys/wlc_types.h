/*
 * Forward declarations for commonly used wl driver structs
 *
 * Copyright (C) 2022, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: wlc_types.h 730443 2017-11-07 08:52:23Z $
 */

#ifndef _wlc_types_h_
#define _wlc_types_h_

/* forward declarations */

typedef struct wlc_info wlc_info_t;
typedef struct wlcband wlcband_t;
typedef struct wlc_cmn_info wlc_cmn_info_t;
typedef struct wlc_assoc_info wlc_assoc_info_t;

typedef struct wlc_bsscfg wlc_bsscfg_t;
typedef struct wlc_mbss_info wlc_mbss_info_t;
typedef struct wlc_spt wlc_spt_t;
typedef struct scb scb_t;
typedef struct scb_iter scb_iter_t;
typedef struct vndr_ie_listel vndr_ie_listel_t;
typedef struct wlc_if wlc_if_t;
typedef struct wl_if wl_if_t;
typedef struct led_info led_info_t;
typedef struct bmac_led bmac_led_t;
typedef struct bmac_led_info bmac_led_info_t;
typedef struct seq_cmds_info wlc_seq_cmds_info_t;
typedef struct ota_test_info ota_test_info_t;
typedef struct wlc_ccx ccx_t;
typedef struct wlc_ccx_rm ccx_rm_t;
typedef struct apps_wlc_psinfo apps_wlc_psinfo_t;
typedef struct scb_module scb_module_t;
typedef struct ba_info ba_info_t;
typedef struct wlc_frminfo wlc_frminfo_t;
typedef struct amsdu_info amsdu_info_t;
typedef struct txq_info txq_info_t;
typedef struct wlc_mux_srcs mux_srcs_t;
typedef struct txq txq_t;
typedef struct cram_info cram_info_t;
typedef struct wlc_extlog_info wlc_extlog_info_t;
typedef struct wlc_txq_info wlc_txq_info_t;
typedef struct wlc_hrt_info wlc_hrt_info_t;
typedef struct wlc_hrt_to wlc_hrt_to_t;
typedef struct wlc_cac wlc_cac_t;
typedef struct ampdu_tx_info ampdu_tx_info_t;
typedef struct ampdu_rx_info ampdu_rx_info_t;
typedef struct wlc_ratesel_info wlc_ratesel_info_t;
typedef struct ratesel_info ratesel_info_t;
#ifdef WLFIPS
typedef struct wl_fips_info wl_fips_info_t;
#endif
typedef struct wlc_ap_info wlc_ap_info_t;
typedef struct cs_info cs_info_t;
typedef struct wlc_scan_info wlc_scan_info_t;
typedef struct wlc_scan_cmn_info wlc_scan_cmn_t;
typedef struct dpt_info dpt_info_t;
#ifdef WLAWDL
typedef struct wlc_awdl_info wlc_awdl_info_t;
#endif
typedef struct tdls_info tdls_info_t;
typedef struct dls_info dls_info_t;
typedef struct l2_filter_info l2_filter_info_t;
typedef struct wlc_auth_info wlc_auth_info_t;
typedef struct wlc_sup_info wlc_sup_info_t;
typedef struct wlc_fbt_info wlc_fbt_info_t;
typedef struct wlc_ccxsup_info wlc_ccxsup_info_t;
typedef struct wlc_psta_info wlc_psta_info_t;
typedef struct wlc_psa wlc_psa_t;
typedef struct wlc_mcnx_info wlc_mcnx_info_t;
typedef struct wlc_p2p_info wlc_p2p_info_t;
typedef struct mchan_info mchan_info_t;
typedef struct wlc_mchan_context wlc_mchan_context_t;
typedef struct bta_info bta_info_t;
typedef struct wowl_info wowl_info_t;
typedef struct wowlpf_info wowlpf_info_t;
typedef struct wlc_plt_info wlc_plt_pub_t;
typedef struct authenticator authenticator_t;
typedef struct antsel_info antsel_info_t;
#if defined(WLC_HIGH) && !defined(WLC_LOW)
typedef struct rpctx_info rpctx_info_t;
#endif
#ifdef WLC_LOW
typedef struct bmac_pmq bmac_pmq_t;
#endif
typedef struct wlc_wmf_instance wlc_wmf_instance_t;
typedef struct wmf_info wmf_info_t;
typedef struct wlc_rrm_info wlc_rrm_info_t;
typedef struct rm_info rm_info_t;

struct d11init;

typedef struct wlc_dpc_info wlc_dpc_info_t;

typedef struct wlc_11h_info wlc_11h_info_t;
typedef struct wlc_tpc_info wlc_tpc_info_t;
typedef struct wlc_csa_info wlc_csa_info_t;
typedef struct wlc_quiet_info wlc_quiet_info_t;
typedef struct cca_info cca_info_t;
typedef struct itfr_info itfr_info_t;

typedef struct wlc_wnm_info wlc_wnm_info_t;
typedef struct wlc_ol_info_t wlc_ol_info_t;
typedef struct wlc_11d_info wlc_11d_info_t;
typedef struct wlc_cntry_info wlc_cntry_info_t;

typedef struct wlc_dfs_info wlc_dfs_info_t;

typedef struct bsscfg_module bsscfg_module_t;

typedef struct wlc_prq_info_s wlc_prq_info_t;

typedef struct wlc_prot_info wlc_prot_info_t;
typedef struct wlc_prot_g_info wlc_prot_g_info_t;
typedef struct wlc_prot_n_info wlc_prot_n_info_t;
typedef struct wlc_prot_obss_info wlc_prot_obss_info_t;
typedef struct wlc_obss_dynbw wlc_obss_dynbw_t;
typedef struct wlc_11u_info wlc_11u_info_t;
typedef struct wlc_probresp_info wlc_probresp_info_t;
typedef struct wlc_wapi_info wlc_wapi_info_t;

typedef struct wlc_tbtt_info wlc_tbtt_info_t;
typedef struct wlc_nic_info wlc_nic_info_t;

typedef struct wlc_bssload_info wlc_bssload_info_t;

typedef struct wlc_pcb_info wlc_pcb_info_t;
typedef struct wlc_txc_info wlc_txc_info_t;

typedef struct wlc_trf_mgmt_ctxt    wlc_trf_mgmt_ctxt_t;
typedef struct wlc_trf_mgmt_info    wlc_trf_mgmt_info_t;

typedef struct wlc_net_detect_ctxt  wlc_net_detect_ctxt_t;

typedef struct wlc_powersel_info wlc_powersel_info_t;
typedef struct powersel_info powersel_info_t;

typedef struct wlc_lpc_info wlc_lpc_info_t;
typedef struct lpc_info lpc_info_t;
typedef struct rate_lcb_info rate_lcb_info_t;
typedef struct wlc_txbf_info wlc_txbf_info_t;
typedef struct wlc_mutx_info wlc_mutx_info_t;
typedef struct wlc_murx_info wlc_murx_info_t;

typedef struct wlc_olpc_eng_info_t wlc_olpc_eng_info_t;
/* used by olpc to register for callbacks from stf */
typedef void (*wlc_stf_txchain_evt_notify)(wlc_info_t *wlc);

typedef struct wlc_rfc wlc_rfc_t;
typedef struct wlc_pktc_info wlc_pktc_info_t;

typedef struct wlc_mfp_info wlc_mfp_info_t;

typedef struct wlc_dngl_ol_info wlc_dngl_ol_info_t;
typedef struct wlc_dngl_ol_bcn_info wlc_dngl_ol_bcn_info_t;
typedef struct wlc_dngl_ol_pkt_filter_info wlc_dngl_ol_pkt_filter_info_t;
typedef struct wlc_dngl_ol_wowl_info wlc_dngl_ol_wowl_info_t;
typedef struct wlc_dngl_ol_l2keepalive_info wlc_dngl_ol_l2keepalive_info_t;
typedef struct wlc_dngl_ol_gtk_info wlc_dngl_ol_gtk_info_t;
typedef struct wlc_dngl_ol_mdns_info wlc_dngl_ol_mdns_info_t;
typedef struct wlc_dngl_ol_rssi_info wlc_dngl_ol_rssi_info_t;
typedef struct wlc_dngl_ol_ltr_info wlc_dngl_ol_ltr_info_t;
typedef struct wlc_dngl_ol_eventlog_info wlc_dngl_ol_eventlog_info_t;

typedef struct wlc_macfltr_info wlc_macfltr_info_t;
typedef struct wlc_bmon_info wlc_bmon_info_t;

typedef struct wlc_nar_info wlc_nar_info_t;
typedef struct wlc_bs_data_info wlc_bs_data_info_t;

typedef struct wlc_keymgmt wlc_keymgmt_t;
typedef struct wlc_key	wlc_key_t;
typedef struct wlc_key_info wlc_key_info_t;

typedef struct wlc_hw wlc_hw_t;
typedef struct wlc_hw_info wlc_hw_info_t;
typedef struct wlc_hwband wlc_hwband_t;

typedef struct wlc_rmc_info wlc_rmc_info_t;

typedef struct wlc_iem_info wlc_iem_info_t;

typedef struct wlc_ier_info wlc_ier_info_t;
typedef struct wlc_ier_reg wlc_ier_reg_t;

typedef struct wlc_ht_info wlc_ht_info_t;
typedef struct wlc_obss_info wlc_obss_info_t;
typedef struct wlc_vht_info wlc_vht_info_t;
typedef struct wlc_akm_info wlc_akm_info_t;
typedef struct wlc_srvsdb_info wlc_srvsdb_info_t;

typedef struct wlc_bss_info wlc_bss_info_t;

typedef struct wlc_hs20_info wlc_hs20_info_t;
typedef struct wlc_pmkid_info	wlc_pmkid_info_t;
typedef struct wlc_btc_info wlc_btc_info_t;

typedef struct wlc_txh_info wlc_txh_info_t;
typedef union wlc_txd wlc_txd_t;

typedef struct wlc_staprio_info wlc_staprio_info_t;
typedef struct wlc_stamon_info wlc_stamon_info_t;
typedef struct wlc_monitor_info wlc_monitor_info_t;

typedef struct wlc_nan_info wlc_nan_info_t;
typedef struct wlc_bcn_clsg_info wlc_bcn_clsg_info_t;
typedef struct obj_registry obj_registry_t;

typedef struct wlc_wds_info wlc_wds_info_t;
typedef struct okc_info okc_info_t;
typedef struct wlc_aibss_info wlc_aibss_info_t;
typedef struct wlc_ipfo_info wlc_ipfo_info_t;
typedef struct wlc_stats_info wlc_stats_info_t;
typedef struct wlc_ioctl_cmd wlc_ioctl_cmd_t;

typedef struct wlc_pps_info wlc_pps_info_t;

typedef struct duration_info duration_info_t;
typedef struct wlc_taf_info wlc_taf_info_t;
typedef struct wlc_pdsvc_info wlc_pdsvc_info_t;
typedef struct wlc_mbo_info wlc_mbo_info_t;
typedef struct wlc_mbo_oce_info wlc_mbo_oce_info_t;

typedef struct wlc_supp_channels wlc_supp_channels_t;

/* For LTE Coex */
typedef struct wlc_ltecx_info wlc_ltecx_info_t;

typedef struct wlc_probresp_mac_filter_info wlc_probresp_mac_filter_info_t;

typedef struct wlc_ltr_info wlc_ltr_info_t;

typedef struct bwte_info bwte_info_t;

typedef struct tbow_info tbow_info_t;

typedef struct wlc_modesw_info wlc_modesw_info_t;

typedef struct wlc_pm_mute_tx_info wlc_pm_mute_tx_t;

typedef struct wlc_bcntrim_info wlc_bcntrim_info_t;

typedef struct chanim_interfaces chanim_interfaces_t;

typedef struct wlc_ulb_info wlc_ulb_info_t;

typedef struct wlc_macdbg_info wlc_macdbg_info_t;
#ifdef ACKSUPR_MAC_FILTER
typedef struct wlc_addrmatch_info wlc_addrmatch_info_t;
#endif /* ACKSUPR_MAC_FILTER */

#if defined(WL_AIR_IQ)
typedef struct airiq_info airiq_info_t;
#endif
#endif	/* _wlc_types_h_ */
