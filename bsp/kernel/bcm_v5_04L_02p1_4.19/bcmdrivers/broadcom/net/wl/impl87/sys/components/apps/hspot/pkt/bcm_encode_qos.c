/*
 * Encoding of QoS packets.
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
 * $Id:$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "802.11.h"
#include "trace.h"
#include "bcm_hspot.h"
#include "bcm_encode_qos.h"

/* encode QoS map */
int bcm_encode_qos_map(bcm_encode_t *pkt,
	uint8 except_length, uint8 *except_data,
	uint8 up0_low, uint8 up0_high,
	uint8 up1_low, uint8 up1_high,
	uint8 up2_low, uint8 up2_high,
	uint8 up3_low, uint8 up3_high,
	uint8 up4_low, uint8 up4_high,
	uint8 up5_low, uint8 up5_high,
	uint8 up6_low, uint8 up6_high,
	uint8 up7_low, uint8 up7_high)
{
	int initLen = bcm_encode_length(pkt);

	bcm_encode_byte(pkt, DOT11_ACTION_CAT_QOS);
	bcm_encode_byte(pkt, DOT11_QOS_ACTION_QOS_MAP);
	bcm_encode_byte(pkt, DOT11_MNG_QOS_MAP_ID);
	bcm_encode_byte(pkt, 16 + except_length);
	if (except_length > 0)
		bcm_encode_bytes(pkt, except_length, except_data);
	bcm_encode_byte(pkt, up0_low);
	bcm_encode_byte(pkt, up0_high);
	bcm_encode_byte(pkt, up1_low);
	bcm_encode_byte(pkt, up1_high);
	bcm_encode_byte(pkt, up2_low);
	bcm_encode_byte(pkt, up2_high);
	bcm_encode_byte(pkt, up3_low);
	bcm_encode_byte(pkt, up3_high);
	bcm_encode_byte(pkt, up4_low);
	bcm_encode_byte(pkt, up4_high);
	bcm_encode_byte(pkt, up5_low);
	bcm_encode_byte(pkt, up5_high);
	bcm_encode_byte(pkt, up6_low);
	bcm_encode_byte(pkt, up6_high);
	bcm_encode_byte(pkt, up7_low);
	bcm_encode_byte(pkt, up7_high);

	return bcm_encode_length(pkt) - initLen;
}
