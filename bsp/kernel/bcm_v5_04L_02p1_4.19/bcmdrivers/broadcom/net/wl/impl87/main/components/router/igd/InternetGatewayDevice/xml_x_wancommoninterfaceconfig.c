/*
 * Broadcom IGD module, xml_x_wancommoninterfaceconfig.c
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
 * $Id: xml_x_wancommoninterfaceconfig.c 241182 2011-02-17 21:50:03Z $
 */

char xml_x_wancommoninterfaceconfig[]=
	"<?xml version=\"1.0\"?>\r\n"
	"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"
	"\t<specVersion>\r\n"
	"\t\t<major>1</major>\r\n"
	"\t\t<minor>0</minor>\r\n"
	"\t</specVersion>\r\n"
	"\t<actionList>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>SetEnabledForInternet</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewEnabledForInternet</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>EnabledForInternet</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetEnabledForInternet</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewEnabledForInternet</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>EnabledForInternet</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetCommonLinkProperties</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewWANAccessType</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>WANAccessType</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewLayer1UpstreamMaxBitRate</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>Layer1UpstreamMaxBitRate</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewLayer1DownstreamMaxBitRate</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>Layer1DownstreamMaxBitRate</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewPhysicalLinkStatus</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>PhysicalLinkStatus</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetMaximumActiveConnections</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewMaximumActiveConnections</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>MaximumActiveConnections</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetTotalBytesSent</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewTotalBytesSent</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>TotalBytesSent</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetTotalPacketsSent</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewTotalPacketsSent</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>TotalPacketsSent</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetTotalBytesReceived</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewTotalBytesReceived</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>TotalBytesReceived</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetTotalPacketsReceived</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewTotalPacketsReceived</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>TotalPacketsReceived</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t\t<action>\r\n"
	"\t\t\t<name>GetActiveConnections</name>\r\n"
	"\t\t\t\t<argumentList>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewActiveConnectionIndex</name>\r\n"
	"\t\t\t\t\t\t<direction>in</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>NumberOfActiveConnections</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewActiveConnDeviceContainer</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>ActiveConnectionDeviceContainer</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t\t<argument>\r\n"
	"\t\t\t\t\t\t<name>NewActiveConnectionServiceID</name>\r\n"
	"\t\t\t\t\t\t<direction>out</direction>\r\n"
	"\t\t\t\t\t\t<relatedStateVariable>ActiveConnectionServiceID</relatedStateVariable>\r\n"
	"\t\t\t\t\t</argument>\r\n"
	"\t\t\t\t</argumentList>\r\n"
	"\t\t</action>\r\n"
	"\t</actionList>\r\n"
	"\t<serviceStateTable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>WANAccessType</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t\t<allowedValueList>\r\n"
	"\t\t\t\t<allowedValue>DSL</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>POTS</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Cable</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Ethernet</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Other</allowedValue>\r\n"
	"\t\t\t</allowedValueList>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>Layer1UpstreamMaxBitRate</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>Layer1DownstreamMaxBitRate</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"yes\">\r\n"
	"\t\t\t<name>PhysicalLinkStatus</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t\t<allowedValueList>\r\n"
	"\t\t\t\t<allowedValue>Up</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Down</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Initializing</allowedValue>\r\n"
	"\t\t\t\t<allowedValue>Unavailable</allowedValue>\r\n"
	"\t\t\t</allowedValueList>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"yes\">\r\n"
	"\t\t\t<name>EnabledForInternet</name>\r\n"
	"\t\t\t<dataType>boolean</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>MaximumActiveConnections</name>\r\n"
	"\t\t\t<dataType>ui2</dataType>\r\n"
	"\t\t\t<allowedValueRange>\r\n"
	"\t\t\t\t<minimum>1</minimum>\r\n"
	"\t\t\t\t<maximum></maximum>\r\n"
	"\t\t\t\t<step>1</step>\r\n"
	"\t\t\t</allowedValueRange>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>NumberOfActiveConnections</name>\r\n"
	"\t\t\t<dataType>ui2</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>TotalBytesSent</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>TotalBytesReceived</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>TotalPacketsSent</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>TotalPacketsReceived</name>\r\n"
	"\t\t\t<dataType>ui4</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>ActiveConnectionDeviceContainer</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t\t<stateVariable sendEvents=\"no\">\r\n"
	"\t\t\t<name>ActiveConnectionServiceID</name>\r\n"
	"\t\t\t<dataType>string</dataType>\r\n"
	"\t\t</stateVariable>\r\n"
	"\t</serviceStateTable>\r\n"
	"</scpd>\r\n"
	"\r\n";
