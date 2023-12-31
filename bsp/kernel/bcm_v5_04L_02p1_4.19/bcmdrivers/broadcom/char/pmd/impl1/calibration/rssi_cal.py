# $language = "python"
# $interface = "1.0"

'''
Copyright 2016-2020 Broadcom Corporation
<:label-BRCM:2020:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
'''

#from add_api import *
from telnet_api import *
from math import floor


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def main(log_file, board_serial_number='', configuration_data=''):
    #crt.Screen.Synchronous = True
    # setting up user configuration_data
    olt_to_onu_att1_db = float( configuration_data.olt_to_onu_att1_db )
    onu_to_rx_opm_att_db = float( configuration_data.onu_to_rx_opm_att_db )


    # opening a log file
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    start_time = time.time( )
    log_file_header( log_file, "RSSI_cal_log" )
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    # starting calibration
    echo( "$$$ RSSI calibration starts  $$$############### ", 1 )


    #Run rssi calibration

    #First point
    # Block incoming light to DUT
    block_incoming_light()

    #Configure optical power
    send2shell("laser msg --set b2 2 0000")

    #Operate FW, first parameter
    send2shell("laser msg --set ae 2 000b")

    #Wait for process to end
    OPC()

    read_internal_runtime(log_file)

    #Second point

    # Unblock incoming light to DUT
    unblock_incoming_light()
    # Set attenuator attenuation level for incoming light level to DUT to -27dBm
    set_attenuation_level( olt_to_onu_att1_db )

    #Measure the actual rx power
    opm_dbm_rssi = float( measure_rx_optical_power_dbm() ) + onu_to_rx_opm_att_db

    op_uw_rssi = (1000 * (10 ** (float( opm_dbm_rssi ) / 10)))*16
    log_file.write( "opm dbm read=%f \t op_uw_rssi=%f \n" % (opm_dbm_rssi, op_uw_rssi) )

    #Set to FW the actual rx power
    op_uw_hex = format( int( round( op_uw_rssi ) ), '04x' )
    send2shell( "laser msg --set b2 2 %s" % (op_uw_hex) )    

    #Operate FW, second parameter
    send2shell("laser msg --set ae 2 100b")

    #Wait for process to end
    OPC()

    read_internal_runtime(log_file)

    # calibration complete message
    echo( "$$$   RSSI calibration complete  $$$###############", 1 )

    # timing the script
    end_time = time.time( )
    log_file.write( "\ntime of script is %f seconds\n" % (end_time - start_time) )
