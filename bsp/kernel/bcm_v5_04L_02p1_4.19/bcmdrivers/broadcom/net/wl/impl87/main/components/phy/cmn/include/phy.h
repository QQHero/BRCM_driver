/*
 * PHY Core module internal interface.
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
 * $Id: phy.h 809672 2022-03-22 04:44:16Z $
 */

#ifndef _phy_h_
#define _phy_h_

#if defined(WL_DSI)
	#if defined(ROM_ENAB_RUNTIME_CHECK) || !defined(DONGLEBUILD)
		#define DSI_ENAB(pi)		((pi)->ff->_dsi)
	#elif defined(WL_DSI_DISABLED)
		#define DSI_ENAB(pi)		(0)
	#else
		#define DSI_ENAB(pi)		(1)
	#endif
#else
	#define DSI_ENAB(pi)			(0)
#endif /* WL_DSI */

#if defined(WL_WBPAPD)
	#if defined(ROM_ENAB_RUNTIME_CHECK) || !defined(DONGLEBUILD)
		#define WBPAPD_ENAB(pi)		((pi)->ff->_wbpapd)
	#elif defined(WL_WBPAPD_DISABLED)
		#define WBPAPD_ENAB(pi)		(0)
	#else
		#define WBPAPD_ENAB(pi)		((pi)->ff->_wbpapd)
	#endif
#else
	#define WBPAPD_ENAB(pi)			(0)
#endif /* WL_WBPAPD */

/* ET Check */
#if defined(WL_ETMODE)
	#if defined(ROM_ENAB_RUNTIME_CHECK) || !defined(DONGLEBUILD)
		#define ET_ENAB(pi)		((pi)->ff->_et)
	#elif defined(WL_ETMODE_DISABLED)
		#define ET_ENAB(pi)		(0)
	#else
		#define ET_ENAB(pi)		((pi)->ff->_et)
	#endif
#else
	#define ET_ENAB(pi)			(0)
#endif /* WL_WBPAPD */

/* PHY Dual band support */
#if defined(ROM_ENAB_RUNTIME_CHECK)
	#define PHY_BAND5G_ENAB(pi)   ((pi)->ff->_dband)
#elif defined(MULTIBAND)
	#define PHY_BAND5G_ENAB(pi)   (1)
#else
	#define PHY_BAND5G_ENAB(pi)   (0)
#endif

#ifdef BCMPHYCOREMASK
#define PHYCOREMASK(cm)	(BCMPHYCOREMASK)
#else
#define PHYCOREMASK(cm)	(cm)
#endif

#define PHY_INVALID_RSSI (-127)
#define DUAL_MAC_SLICES 2

#define PHY_COREMASK_SISO(cm) ((cm == 1 || cm == 2 || cm == 4 || cm == 8) ? 1 : 0)

#define DUAL_PHY_NUM_CORE_MAX 4

#define PI_SLICE(pi)		((pi)->pubpi->slice)
#define IS_SLICE_MAIN(pi)	(PI_SLICE(pi) == DUALMAC_MAIN)
#define IS_SLICE_AUX(pi)	(PI_SLICE(pi) == DUALMAC_AUX)

#endif /* _phy_h_ */
