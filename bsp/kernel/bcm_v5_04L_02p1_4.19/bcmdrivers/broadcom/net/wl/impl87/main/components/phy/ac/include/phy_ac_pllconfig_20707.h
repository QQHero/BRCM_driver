/*
 * ACPHY 20707 Radio PLL configuration
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
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id$
 */

/* All other constants are defined in phy_ac_pllconfig_20707.c
 * This constant has been moved here to make it accessible
 * from phy_ac_vcocal.c
 */
#define RFPLL_VCO_CAP_MODE_DEC                  0

#ifndef _PHY_AC_20707_PLLCONFIG_H
#define _PHY_AC_20707_PLLCONFIG_H

struct pll_config_20707_tbl_s; /* forward declaration */

extern void wlc_phy_radio20707_pll_tune(phy_info_t *pi, struct pll_config_20707_tbl_s *pll,
                                        uint32 chan_freq);
extern struct pll_config_20707_tbl_s* BCMATTACHFN(phy_ac_radio20707_populate_pll_config_tbl)
						 (phy_info_t *pi);
extern void BCMATTACHFN(phy_ac_radio20707_populate_pll_config_mfree)
		       (phy_info_t *pi, struct pll_config_20707_tbl_s *pll);

#endif /* _PHY_AC_20707_PLLCONFIG_H */
