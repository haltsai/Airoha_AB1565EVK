/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

/**
 * @addtogroup ATCMD
 * @{
 */

/** @brief
 * This enum only helps explain HAL AT command.
 */
typedef enum {
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Get ADC sample value and voltage.
     * <b>[Command]</b>
     *    AT+EAUXADC=(adc_channel_num),(continue_sampling_times)
     * <b>[Parameter]</b>
     *   (adc_channel_num): Defined adc channel number in datasheet.
     *   (continue_sampling_times): The sample times in an atcmd, but it's smaller than 20.
     * <b>[Response]</b>
     *    adc_channel = %c, adc_data = 0x%04x, adc_voltage = %dmV
     * <b>[Example]</b>
     *    Get ADC channel 1,sampling 1 times.
     * @code
     *    Send:
     *       AT+EAUXADC=1,1
     *    Response:
     *       adc_channel = 1, adc_data = 0x%04x, adc_voltage = %dmV
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EAUXADC,
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Set PWM mode.
     * <b>[Command]</b>
     *    AT+EPWM=SET,(channel),(source_clk),(frequency),(duty)
     * <b>[Parameter]</b>
     *    (channel): PWM channel which want to set.
     *    (source_clk): PWM source clock, please see reference manual.
     *    (frequency): PWM output frequency.
     *    (duty): PWM output duty.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Set PWM channel 0,source clock 32k, PWM frequency 1000Hz,duty 50%.
     * @code
     *    Send:
     *        AT+EPWM=SET,0,32000,1000,50
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EPWM,
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Test RTC Feature.
     * <b>[Command]</b>
     *    AT+ERTC=REG,<OP>,<Address>,<Value>
     *    AT+ERTC=TEST,<Mode>,[Para]
     * <b>[Parameter]</b>
     *    (OP): W(for write), R(for read).
     *    (Mode):
     *        <0>: Get rtc time.
     *        <1>: Enter rtc mode.
     *        <2>,<second>: Set alarm time.
     *        <3>: Get alarm time.
     *        <4>,<date>,<time>: Set rtc time.
     *        <5>: Get power on reason.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Write 0x1 to address of 0xA2080000:
     *         AT+ERTC=REG,W,0xA2080000,0x1
     *    Set alarm 5 second later trigger
     *         AT+ERTC=TEST,2,5
     *    Set RTC date time to 21-1-13 12:00:00
     *         AT+ERTC=TEST,4,21-1-13,12:00:00
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_ERTC,
    /** <pre>
    * <b>[Category]</b>
    *    GPIO
    * <b>[Description]</b>
    *    Get GPIO information.
    * <b>[Command]</b>
    *    AT+EGPIO=GPIO_GET:m~n
    *    AT+EGPIO=GPIO_SET_MODE:g,m
    *    AT+EGPIO=GPIO_SET_DIR:g,d
    *    AT+EGPIO=GPIO_SET_PULL:g,p
    *    AT+EGPIO=GPIO_SET_PUPD:g,pupd,r0,r1
    *    AT+EGPIO=GPIO_SET_OD:g,d
    *    AT+EGPIO=GPIO_SET_DRV:g,drv
    *    AT+EGPIO=GPIO_SET_ALL:g,m,dir,do,p,drv
    * <b>[Parameter]</b>
    *    None
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EGPIO=GPIO_GET: 0~0
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EGPIO,
    AT_EPMUREG,
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Test AES.
     * <b>[Command]</b>
     *    AT+ECRYPTO=AES,(bits),(mode)?
     * <b>[Parameter]</b>
     *    (bits): 128, 192 or 256 for AES calculation bits.
     *    (mode): CBC or ECB for different modes.
     * <b>[Response]</b>
     *    AES CBC/ECB TEst 128b ok or AES CBC/ECT TEst 128b fail.
     * <b>[Example]</b>
     *    Set 128 bits with CBC mode.
     * @code
     *    Send:
     *        AT+ECRYPTO=AES,128,CBC?
     *    Response:
     *        AES CBC/ECB TEst 128b ok
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_ECRYPTO,
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Security status operation.
     * <b>[Command]</b>
     *    AT+SEC=(op),(security_level),(variable_attribute),(cmd)
     * <b>[Parameter]</b>
     *    (op): read.
     *    (security_level): 0, for extension in the future.
     *    (variable_attribute): 0, for extension in the future.
     *    (cmd): the operation target item, item list is in efuse setting.
     * <b>[Response]</b>
     *    (cmd)=value
     * <b>[Example]</b>
     *    Read USBDL auto-detect disable in BROM status.
     * @code
     *    Send:
     *       AT+SEC=read,0,0,Disable_USBDL_By_Auto_Detect
     *    Response:
     *       USBDL Auto-Detect Disable = 1 OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_SEC,
    /** <pre>
     * <b>[Category]</b>
     *    HAL
     * <b>[Description]</b>
     *    Sleep manager operation.
     * <b>[Command]</b>
     *    AT+SM=0
     *    AT+SM=BASE
     *    AT+SM=STATUS
     *    AT+SM=AT+SM=DEBUGIO,(DEBUG_MONITOR_SETTING)
     *    AT+SM=HANDLE_LOCK,(SLEEP_HANDLE_INDEX)
     *    AT+SM=HANDLE_UNLOCK,(SLEEP_HANDLE_INDEX)
     *    AT+SM=LOCK,(LOCK_OP),SLP
     *    AT+SM=SET_GPIO,(GPIO_NUM),(GPIO_MODE)
     *    AT+SM=PWR,(MTCMOS),(MTCMOS_OP)
     *    AT+SM=RTC,(RTC_OP)
     * <b>[Parameter]</b>
     *    (DEBUG_MONITOR_SETTING): A 32-bit value which is used to configure the output settings of the debug monitor.
     *    (SLEEP_HANDLE_INDEX): The sleep handle index used by different modules. (Can be found at hal_sleep_manager_platform.h)
     *    (LOCK_OP): 1: Lock, 0: Unlock.
     *    (GPIO_NUM): The GPIO number to be configured.
     *    (GPIO_MODE): The GPIO mode to be configured.
     *    (MTCMOS): The MTCMOS to be configured. (Can be found at hal_spm.h)
     *    (MTCMOS_OP): 1: Enable, 0: Disable.
     *    (RTC_OP): 1: Enter RTC mode with RTC alarm, 0: Enter RTC mode.
     * <b>[Response]</b>
     *    OK, ERROR, or Not support. Response of "AT+SM=STATUS" will show on syslog.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+SM=LOCK,1,SLP
     *    Response:
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_SM,
    /** <pre>
    * <b>[Category]</b>
    *    HAL
    * <b>[Description]</b>
    *    Read or wirte the captouch register.
    * <b>[Command]</b>
    *    AT+ECPT=RW_RG:[R/W],[ADDR],[DATA]\0d\0a
    * <b>[Parameter]</b>
    *    [R/W]: "R", read; "W", write.
    *    [ADDR]: Register address.
    *    [DATA]: Register data.
    * <b>[Response]</b>
    *    Register data, OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+ECPT=RW_RG:W,0xa2110004,0x12345678\0d\0a
    *    Response:
    *        OK
    *    Send:
    *        AT+ECPT=RW_RG:R,0xa2110004,NULL\0d\0a
    *    Response:
    *        Read Captouch HIF addr:0xa2110004 = 0x12345678 done!
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_CAPTOUCH,
} AT_CMD_HAL;

/** @brief
 * This enum only helps explain SYSTEM AT command.
 */
typedef enum {
    /** <pre>
    * <b>[Category]</b>
    *    System Service
    * <b>[Description]</b>
    *    Show system information and trigger system reboot or crash.
    * <b>[Command]</b>
    *    AT+SYSTEM=(module)
    * <b>[Parameter]</b>
    *    (module) string type
    *    task: Show all freeRTOS tasks information.
    *    mem: Show heap status.
    *    crash: Trigger system crash to dump system information.
    *    reboot: Trigger system to hard-reboot.
    * <b>[Response]</b>
    *    OK with data or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+SYSTEM=task
    *    Response:
    *        +SYSTEM:
    *        Parameter meaning:
    *        1: pcTaskName
    *        2: cStatus(Ready/Blocked/Suspended/Deleted)
    *        3: uxCurrentPriority
    *        4: usStackHighWaterMark (unit is sizeof(StackType_t))
    *        5: xTaskNumber
    *        OK
    *        +SYSTEM:
    *        ATCI  X 5 594 2
    *        IDLE  R 0 223 5
    *        Tmr S B 14 326 6
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    MTK_SYSTEM_AT_COMMAND_ENABLE should be defined in y in project's feature.mk.
    * </pre>
    */
    AT_SYSTEM,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    AT+SYSLOG - Show or modify syslog cpu and module filter level and switch.
     * <b>[Command]</b>
     *    Config cpu's level:AT+SYSLOG=0,(cpu),(log_switch),(print_level)
     *    Config module's setting:AT+SYSLOG=1,(cpu),(module),(log_switch),(print_level)
     *    Save cpu/module's filter setting to NVDM: AT+SYSLOG=2
     *    Config log to flash setting:  AT+SYSLOG=3,(dump_switch),(dump_level)
     *    Config log to flash setting:  AT+SYSLOG=4,(log_save_interval(ms)),(assert_interval(s))
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+SYSLOG=0,0,0,1
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_SYSLOG,
    /** <pre>
    * <b>[Category]</b>
    *    System Service
    * <b>[Description]</b>
    *    Profile CPU utilization.
    * <b>[Command]</b>
    *    AT+UTILIZATION=(op),(tick),(count)
    * <b>[Parameter]</b>
    *    (op): String type, must be 'start', 'stop' or 'duration'.
    *    (tick): Integer type, means the profiling duration, the unit is system tick, 1tick is 1ms, only needed when (op)=duration.
    *    (count): Integer type, means the profiling counts, only needed when (op)=duration.
    * <b>[Response]</b>
    *    OK with profiling data or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+UTILIZATION= duration,5000,10
    *    Response:
    *        +UTILIZATION:
    *        profiling begin, duration is 5000 tick, the tick's unit is 1/configTICK_RATE_HZ
    *        OK
    *        +UTILIZATION:
    *        parameter meaning:
    *        1: pcTaskName
    *        2: count(unit is 32k of gpt)
    *        3: ratio
    *        +UTILIZATION:
    *        CPU   0  <1%
    *        IDLE  163943 99%
    *        Tmr S 89  <1%
    *        ATCI  19  <1%
    * @endcode
    * <b>[Note]</b>
    *    MTK_OS_CPU_UTILIZATION_ENABLE should be defined in y in project's feature.mk.
    * </pre>
    */
    AT_UTILIZATION,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    AT+EPORT - show or modify serial port assignment and switch.
     * <b>[Command]</b>
     *    Serial port show:  AT+EPORT=0
     *    Serial port assign: AT+EPORT=1,(owner_name),(device_id)
     *    Serial port switch: AT+EPORT=2,(owner_name),(device_id)
     * <b>[Parameter]</b>
     *    Parameters modify: AT+EPORT=3,(device_id),(parameters)
     *    Parameters show: AT+EPORT=4
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+EPORT=4
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EPORT,
    /** <pre>
     * <b>[Category]</b>
     *    System Service
     * <b>[Description]</b>
     *    Enable or disable SWLA at runtime.
     * <b>[Command]</b>
     *    AT+SWLA=(op)
     * <b>[Parameter]</b>
     *    (op) string type, must be 'enable' or 'disable'.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+SWLA=enable
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    MTK_SWLA_ENABLE should be defined in y in project's feature.mk.
     * </pre>
     */
	AT_SWLA,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    AT+MPORT - show or modify MUX port assignment and switch.
     * <b>[Command]</b>
     *    MUX port show current setting: AT+MPORT?
     *    MUX port show nvdm setting: AT+MPORT=0
     *    MUX port assign single-user: AT+MPORT=1,(owner_name),(device_id)
     *    MUX port show nvdm setting: AT+MPORT=2,(device_id),(on/off)
     *    MUX port assign multi-user: AT+MPORT=3,(device_1),(owner_name1),..,(owner_namex),(device_2),(owner_name1),..,(owner_namex)
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+MPORT=0\0d\0a  (show nvdm setting.)
     *        AT+MPORT=1,atci,1\0d\0a
     *        AT+MPORT=2,0,1\0d\0a  (disconnect uart0.)
     *        AT+MPORT=3,0,ATCI,2,SYSLOG,RACE_CMD\0d\0a  (port switch.)
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_MPORT,
    AT_CPUFSET,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Initialize eint key.
     * <b>[Command]</b>
     *    AT+EKEY=INIT,1: Initialize eint key.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+EKEY=INIT,1
     *    Response:
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EKEY,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Send key event.
     * <b>[Command]</b>
     *    AT+AIROKEY=APPEVENT:(key_even),(key_data) : Send final key event to upper layer, it does not go through the airo key middleware.
     *    AT+AIROKEY=EVENT:(key_even),(key_data)    : Send original press and release event to airo key middleware.
     * <b>[Parameter]</b>
     *      key_event:
     *        For AT+AIROKEY=APPEVENT: The value of key_event is defined in airo_key_event_t
     *        For AT+AIROKEY=EVENT: 0 means release key, 1 means press key
     *      key_data:
     *        eint_key: EINT_KEY_0 <= key_data <= EINT_KEY_7
     *        captouch: DEVICE_KEY_A <= key_data <= DEVICE_KEY_H
     *        powerkey: key_data = DEVICE_KEY_POWER
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+AIROKEY=APPEVENT:0x21,0x18  : Send AIRO_KEY_LONG_PRESS_1 event by power key to upper layer.
     *       AT+AIROKEY=EVENT:1,0x18        : Send AIRO_KEY_DRIVEN_PRESS event by power key to airo key middleware.
     *       AT+AIROKEY=EVENT:0,0x18        : Send AIRO_KEY_DRIVEN_RELEASE event by power key to airo key middleware.
     *    Response:
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_AIROKEY,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Access Register.
     * <b>[Command]</b>
     *    AT+EREG=?
     *    AT+EREG=op,address,(data)
     *    Operations:
     *    0:READ
     *      AT+EREG=0,address
     *    1:WRITE
     *      AT+EREG=1,address,data
     *    2:READ RF Register
     *       AT+EREG=2,address
     *    3:WRITE RF Register
     *       AT+EREG=0,address,data
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+EREG=0,register address
     *    Response:
     *       Register value
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EREG,
    AT_SM,
#if 0
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Switch AT to RACE.
     * <b>[Command]</b>
     *    AT+SWITCH=1,race_command
     * <b>[Example]</b>
     *    Switch AT to RACE.
     * @code
     *    Send:
     *        AT+SWITCH=1,race_command
     *    Response:
     *        AT switch to RACE command mode!
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_SWITCH,
#endif
    /** <pre>
     * <b>[Category]</b>
     *    NVDM
     * <b>[Description]</b>
     *    Add, modify and query data items stored in NVDM.
     * <b>[Command]</b>
     *    AT+ENVDM=0,(group_name),(data_item_name): read binary value.
     *    AT+ENVDM=1,(group_name),(data_item_name),(length),(data): write binary value into NVDM.
     *    AT+ENVDM=2,(group_name): reset items specified by group_name.
     *    AT+ENVDM=R,(group_name),(data_item_name): read binary value and the maximum supported single data item length is 4096.
     *    AT+ENVDM=CI,(group_name),(data_item_name): delete one item.
     *    AT+ENVDM=CG,(group_name): delete all data items in the group specified by group_name.
     *    AT+ENVDM=CA: delete all items in NVDM.
     *    AT+ENVDM=D: dump all items information in NVDM and display its group_name, data_item_name and binary data.
     *    AT+ENVDM=WB,(group_name),(data_item_name),(length),(data): write binary value into NVDM.
     *    AT+ENVDM=WS,(group_name),(data_item_name),(length),(string): write string value into NVDM.
     *    AT+ENVDM=I: show the usage, user setting of NVDM.
     * <b>[Parameter]</b>
     *    (group_name): ascii code used to specify the group name of the data item to be operated.
     *    (data_item_name): ascii code used to specify the item name of the data item to be operated.
     *    (length): ascii code to specify the length of data the user wants to write.
     *    (data): The data the user wants to write. The incoming contiguous ascii code is internally converted to binary data and stored.
     *    (string): When using AT+ENVDM=WS, user can write ascii string value into NVDM.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+ENVDM=1,nvdm_at_test,test_value,2,3335
     *    Response:
     *        OK
     *    Meaning:
     *        Add an NVDM data item. Its group name is nvdm_at_test and its item name is test_value. It has 2 bytes of data: 0x33, 0x35.
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_ENVDM,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Crystal Trim.
     * <b>[Command]</b>
     *    AT+TRIM=?
     *    AT+TRIM=CRYSTAL,(uart_port),(output_freqency),(running time)
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+TRIM=CRYSTAL ,1,312500,10000
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_TRIM,
    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Crystal Trim.
     * <b>[Command]</b>
     *    AT+CLOCK=?
     *    AT+CLOCK=(op),(param0),(param1)
     *    Operations:
     *    0:METER
     *       AT+CLOCK=1,100,(ck_id)
     *    1:MUX
     *       AT+CLOCK=1,(mux_id),1
     *    2:CG
     *       AT+CLOCK=2,(cg_id),1
     *    3:TEST
     *       AT+CLOCK=3,dcxo_mode,(mode)
     *       AT+CLOCK=3,periodical,1
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+CLOCK=3,dcxo_mode,0
     *    Response:
     *       dcxo mode : LowPwr
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_CLOCK,

    /** <pre>
     * <b>[Category]</b>
     *    SYSTEM
     * <b>[Description]</b>
     *    Led Control.
     * <b>[Command]</b>
     *    AT+ELED=?
     *    AT+ELED=(led_id),(mode)
     * <b>[Parameter]</b>
     *    (led_id): The number of led which want to operate.
     *    (mode): The type of operation(mode0:off, mode1:on, mode2:blink).
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *       AT+ELED=1,1\0d\0a
     *    Response:
     *       OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_ELED,
} AT_CMD_SYSTEM;

/** @brief
 * This enum only helps explain AUDIO AT command.
 */
typedef enum {
    /** <pre>
     * <b>[Category]</b>
     *    Aduio HW RG DUMP
     * <b>[Description]</b>
     *    Dump audio HW RG.
     * <b>[Command]</b>
     *    AT+EAUDIO=AUD_DIG_RG_DUMP
     *    AT+EAUDIO=AUD_ANA_RG_DUMP
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+EAUDIO=AUD_ANA_RG_DUMP
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_EAUDIO_RG,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Select audio PEQ mode.
    * <b>[Command]</b>
    *    AT+EAUDIO=PEQ_MODE,(group_id),(phase_id)
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=PEQ_MODE, 0, 0
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_PEQ,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Select multi-mic device & interface.
    * <b>[Command]</b>
    *    AT+EAUDIO=MULTIMIC_SELR,(mic_no),(device),(interface)
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=MULTIMIC_SEL,1,1,1
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_MULTIMIC,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Vendor sound effect & Vendor volume table test command.
    * <b>[Command]</b>
    *    AT+EAUDIO=VENDOR_SE
    *    AT+EAUDIO=VENDOR_VOL
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=VENDOR_SE
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_VENDOR,
#if 0
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Detachable mic test command.
    * <b>[Command]</b>
    *    AT+EAUDIO=SET_DETACHABLE_MIC, 0
    *    AT+EAUDIO=SET_DETACHABLE_MIC, 1
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=SET_DETACHABLE_MIC, 0
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_DETACHABLE_MIC,
#endif
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Init/Start/Stop/deinit audio transmitter by scenario type & scenario sub id.
    * <b>[Command]</b>
    *    AT+EAUDIO=AUDIO_TRANSMITTER,(init/start/stop/deinit),(scenario_type),(scenario_sub_id),(option_param)
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=AUDIO_TRANSMITTER,init,4,2,16000,8,640
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_TRANSMITTER,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Enable/Disable game ull latency measurement.
    * <b>[Command]</b>
    *    AT+EAUDIO=GAME_ULL_LATENCY_ON
    *    AT+EAUDIO=GAME_ULL_LATENCY_OFF
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=GAME_ULL_LATENCY_ON
    *        AT+EAUDIO=GAME_ULL_LATENCY_OFF
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_GAME_ULL_LATENCY,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Enable/Disable ble audio dongle latency measurement.
    * <b>[Command]</b>
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_1: enable latency measurement on USB speaker port 1.
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_1: disable latency measurement on USB speaker port 1.
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_2: enable latency measurement on USB speaker port 2.
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_2: disable latency measurement on USB speaker port 2.
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_ON_1: enable latency measurement on USB micphone port 1.
    *    AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_OFF_1: disable latency measurement on USB micphone port 1.
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_1
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_1
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_2
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_2
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_ON_1
    *        AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_OFF_1
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_BLE_AUDIO_DONGLE_LATENCY,
#if 0
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Start/Stop audio transmitter.
    * <b>[Command]</b>
    *    AT+EAUDIO=DATA_TRANSMIT_OPEN_1: start receive data
    *    AT+EAUDIO=DATA_TRANSMIT_CLOSE_1:stop receive data
    *    AT+EAUDIO=DATA_TRANSMIT_OPEN_2: start send data
    *    AT+EAUDIO=DATA_TRANSMIT_CLOSE_2:stop send data
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=DATA_TRANSMIT_OPEN_1
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_DATA_TRANSMIT,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Init/Start/Stop/deinit/set_runtime_config audio transmitter by scenario type & scenario sub id.
    * <b>[Command]</b>
    *    AT+EAUDIO=AUDIO_TRANSMITTER,(init/start/stop/deinit/set_runtime_config),(scenario type),(scenario sub id),(option param)
    * <b>[Parameter]</b>
    *    None.
    * <b>[Response]</b>
    *    OK or ERROR;
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=AUDIO_TRANSMITTER,init,3,1
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_TRANSMITTER,
    /** <pre>
    * <b>[Category]</b>
    *    User trigger adaptive FF
    * <b>[Description]</b>
    *    Start user trigger adaptive FF record.
    * <b>[Command]</b>
    *    AT+EAUDIO=USER_TRIGGER_FF_SET_PARAM,(FF_Noise_thd, FB_Noise_thd, stepsize, Shaping_switch, time_end): save params in nvdm
    *    AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM: read filter 3 params from nvdm
    *    AT+EAUDIO=USER_TRIGGER_FF: start  User trigger adaptive FF record
    * <b>[Parameter]</b>
    *    <FF_Noise_thd>: 2bytes
    *    <FB_Noise_thd>: 2bytes
    *    <stepsize>: 2bytes
    *    <Shaping_switch>: 2bytes
    *    <time_end>: 2bytes
    * <b>[Response]</b>
    *    OK
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=USER_TRIGGER_FF_SET_PARAM,1000,1000,20000,0,10000
    *        AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM
    *        AT+EAUDIO=USER_TRIGGER_FF
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_UTFF_V1,
#endif
/** <pre>
    * <b>[Category]</b>
    *    User trigger adaptive FF for 156x
    * <b>[Description]</b>
    *    Start user trigger adaptive FF record.
    * <b>[Command]</b>
    *    AT+EAUDIO=AT+EAUDIO=USER_TRIGGER_FF_PZ_SET_PARAM,(mode_setting, PZ_thd, PZ_VAD_control, PZ_learn_thd, PZ_max_time, PZ_min_time, PZ_offset, PZ_ENG_POW_INIT, PZ_ENG_VAD_THR, PZ_ENG_VAD_HANG, PZ_effective_frame, PZ_stop_frame): save params in nvdm
    *    AT+EAUDIO=AT+EAUDIO=USER_TRIGGER_FF_SZ_SET_PARAM,(SZ_delay_i, SZ_thd, SZ_VAD_control, SZ_learn_thd, SZ_max_time, SZ_min_time, SZ_offset): save params in nvdm
    *    AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM: read adaptive filter params from nvdm
    *    AT+EAUDIO=USER_TRIGGER_FF,(mode_setting): start  User trigger adaptive FF record
    * <b>[Parameter]</b>
    *    (mode_setting): 1bytes
    *    (PZ_thd): 2bytes
    *    (PZ_VAD_control): 2bytes
    *    (PZ_learn_thd): 2bytes
    *    (PZ_max_time): 2bytes
    *    (PZ_min_time): 2bytes
    *    (PZ_offset): 2bytes
    *    (PZ_ENG_POW_INIT): 2bytes
    *    (PZ_ENG_VAD_THR): 2bytes
    *    (PZ_ENG_VAD_HANG): 2bytes
    *    (PZ_effective_frame): 2bytes
    *    (PZ_stop_frame): 2bytes
    *    (SZ_delay_i): 2bytes
    *    (SZ_thd): 2bytes
    *    (SZ_VAD_control): 2bytes
    *    (SZ_learn_thd): 2bytes
    *    (SZ_max_time): 2bytes
    *    (SZ_min_time): 2bytes
    *    (SZ_offset): 2bytes
    * <b>[Response]</b>
    *    OK
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=USER_TRIGGER_FF_PZ_SET_PARAM,0,9830,1,4,2,1,33,-7680,717,0,35,58
    *        AT+EAUDIO=USER_TRIGGER_FF_SZ_SET_PARAM,74,9830,1,8,5,2,3
    *        AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM
    *        AT+EAUDIO=USER_TRIGGER_FF,0
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_UTFF_V2,

/** <pre>
    * <b>[Category]</b>
    *    User Unaware for 156x
    * <b>[Description]</b>
    *    Update User Unaware parameters to nvdm.
    * <b>[Command]</b>
    *    AT+EAUDIO=USER_UNAWARE_SET_PARA,(alpha_par, thd, switch_par): save param in nvdm
    * <b>[Parameter]</b>
    *    alpha_par: 2bytes
    *    thd: 4bytes
    *    switch_par: 4bytes
    * <b>[Response]</b>
    *    OK
    * <b>[Example]</b>
    * @code
    *    Send:
    *    AT+EAUDIO=USER_UNAWARE_SET_PARA,983,500000,52233
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_USER_UNAWARE,

/** <pre>
    * <b>[Category]</b>
    *    Adaptive ANC(WND+UU+ED) for ab156x
    * <b>[Description]</b>
    *    Open or Close Adaptive ANC stream.
    * <b>[Command]</b>
    *    AT+EAUDIO=ADAPTIVE_ANC_STREAM,OPEN
    *    AT+EAUDIO=ADAPTIVE_ANC_STREAM,CLOSE
    * <b>[Parameter]</b>
    * <b>[Response]</b>
    *    OK
    * <b>[Example]</b>
    * @code
    *    Send:
    *    AT+EAUDIO=ADAPTIVE_ANC_STREAM,OPEN
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_ADAPTIVE_ANC_STREAM,

/** <pre>
    * <b>[Category]</b>
    *    User for LE audio feature
    * <b>[Description]</b>
    *    Start LE audio related commands.
    * <b>[Command]</b>
    *    AT+LEAUDIO=BROADCAST,L,(BMS_address): synchronize to BMS
    *    AT+LEAUDIO=BROADCAST,STOP: stop synchronizing to BMS
    *    AT+LEAUDIO=BROADCAST,CONFIG,(BIS_index_count),(BIS_indices): config BIS index for synchronizing
    * <b>[Parameter]</b>
    *    (BMS_address): 6bytes
    *    (BIS_index_count): 1byte
    *    (BIS_indices): 1byte
    * <b>[Response]</b>
    *    OK
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+LEAUDIO=BROADCAST,L,06:AA:14:99:0B:BE\0d\0a
    *        AT+LEAUDIO=BROADCAST,STOP\0d\0a
    *        AT+LEAUDIO=BROADCAST,CONFIG,1,2\0d\0a
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_LE,
    /** <pre>
    * <b>[Category]</b>
    *    Audio
    * <b>[Description]</b>
    *    Loopback Test DAC & Mic founctional ability
    * <b>[Command]</b>
    *    AT+EAUDIO=1KTone_check,AMIC,$par1(adc_id),$par2(amic_mode)\0d\0a
    *    AT+EAUDIO=1KTone_check,DMIC,$par1(adc_id)\0d\0a
    *    AT+EAUDIO=1KTone_check,help\0d\0a :print support cmd at log
    * <b>[Parameter]</b>
    *    $par1(adc_id): choose which mic to test, 0~5
    *    $par2(amic_mode): choose amic mode, 0: ACC10k, 1:ACC20k, 2:DCC
    * <b>[Response]</b>
    *    Freq, Magnitude, OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+EAUDIO=1KTone_check,AMIC,0,0\0d\0a
    *    Response:
    *        Freq=1000, Mag=9684977, OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_EAUDIO_1KTone,
} AT_CMD_AUDIO;

/** @brief
 * This enum only helps explain BT AT command.
 */
typedef enum {
    /** <pre>
     * <b>[Category]</b>
     *    Earbuds RHO Test
     * <b>[Description]</b>
     *    Trigger RHO or start a BT external timer for RHO test.
     * <b>[Command]</b>
     *    AT+RHO=(cmd)
     * <b>[Parameter]</b>
     *    (cmd): TIMER.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Trigger RHO.
     * @code
     *    Send:
     *        AT+RHO
     *    Response:
     *        OK
     * @endcode
     *    Start a bt external timer for RHO test, so the external timer module can test RHO.
     * @code
     *    Send:
     *        AT+RHO=TIMER
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_RHO,
	/** <pre>
     * <b>[Catagory]</b>
     *    LE
     * <b>[Description]</b>
     *    Operate LE ADV.
     * <b>[Command]</b>
     *    AT+BLEADV=ON:  enable LE advertising.
     *    AT+BLEADV=OFF: disable LE advertising.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Enable a advertising.
     * @code
     *      Send:
     *          AT+BLEADV=ON
     *      Response:
     *          OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_LE_ADV,
    /** <pre>
     * <b>[Catagory]</b>
     *    LE
     * <b>[Description]</b>
     *    Operate LE Scan.
     * <b>[Command]</b>
     *    AT+BLESCAN=ON:  enable LE scan.
     *    AT+BLESCAN=OFF: disable LE scan.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Enable LE scan.
     * @code
     *      Send:
     *          AT+BLESCAN=ON
     *      Response:
     *          OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_LE_SCAN,
    /** <pre>
     * <b>[Catagory]</b>
     *    LE
     * <b>[Description]</b>
     *    Cancel a connection being created.
     * <b>[Command]</b>
     *    AT+BLECANCELCONN
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Cancel a connection.
     * @code
     *      Send:
     *          AT+BLECANCELCONN
     *      Response:
     *          OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_LE_CANCEL_CONN,
    /** <pre>
     * <b>[Catagory]</b>
     *    LE
     * <b>[Description]</b>
     *    Get the local random address.
     * <b>[Command]</b>
     *    AT+BLERANDOMADDR=GET: get the local random address.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Get the local random address.
     * @code
     *      Send:
     *          AT+BLERANDOMADDR=GET
     *      Response:
     *          OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_LE_RANDOM_ADDR,
    /** <pre>
     * <b>[Catagory]</b>
     *    LE
     * <b>[Description]</b>
     *    Operate LE extended advertising.
     * <b>[Command]</b>
     *    AT+EXTBLEADV=ON:  enable extended advertising.
     *    AT+EXTBLEADV=OFF: disable extended advertising.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    enable extended advertising.
     * @code
     *      Send:
     *          AT+EXTBLEADV=ON
     *      Response:
     *          OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
	AT_LE_EXT_ADV,
	/** <pre>
     * <b>[Catagory]</b>
	 *    Sink
	 * <b>[Description]</b>
	 *    Operate calls.
	 * <b>[Command]</b>
	 *    AT+BTSINKIT=ANS: answer an incoming call.
	 *    AT+BTSINKIT=REJ: reject an incoming call.
	 *    AT+BTSINKIT=HAG: hang up an outgoing call or an active call.
	 *    AT+BTSINKIT=DLN: dial the last dialed number.
	 *    AT+BTSINKIT=RAH: release all held calls or sets User Determined User Busy for a waiting call.
	 *    AT+BTSINKIT=RAA: release all active calls and accept the other held or waiting call.
	 *    AT+BTSINKIT=HAA: place all active calls on hold and accept the other held or waiting call.
	 *    AT+BTSINKIT=SAP: switch audio path.
	 *    AT+BTSINKIT=SAD: swith audio between two devices.
	 *    AT+BTSINKIT=RSI(index): release a specific call.
	 *    AT+BTSINKIT=HSI(index): place a specific call on hold.
	 *    AT+BTSINKIT=AHTC: add a held call to the conversation.
	 *    AT+BTSINKIT=ECT: connect the two calls and disconnect the AG from both calls.
	 *    AT+BTSINKIT=VRA(activate): activate voice recognition.
	 *    AT+BTSINKIT=ECNR(activate): activate the echo canceling and noise reduction function.
	 *    AT+BTSINKIT=CALLVUP: increase the call volume one level.
	 *    AT+BTSINKIT=CALLVDN: decrease the call volume one level.
	 *    AT+BTSINKIT=CALLVMIN: decrease the call volume to the MIN.
	 *    AT+BTSINKIT=CALLVMAX: increase the call volume to the MAX.
	 *    AT+BTSINKIT=EHFP: enable HFP SDP record.
	 *    AT+BTSINKIT=DHFP: disable HFP SDP record.
	 *    AT+BTSINKIT=ACUQA,(ms): delay sending HFP AT commands for ACUQA tesing.
	 *    AT+BTSINKIT=SIRI: get the Siri state from Apple-specific devices.
	 *    AT+BTSINKIT=MICVUP: for PTS testing.
	 *    AT+BTSINKIT=MICVDN: for PTS testing.
	 *    AT+BTSINKIT=MICVMAX: for PTS testing.
	 *    AT+BTSINKIT=MICVMIN: for PTS testing.
	 *    AT+BTSINKIT=PTS-ON: for PTS testing.
	 *    AT+BTSINKIT=HSP-ON: disable HFP SDP record, and enable HSP SDP record.
	 *    AT+BTSINKIT=HSP-OFF: disable HSP SDP record, and enable HFP SDP record.
	 *    AT+BTSINKIT=HSP-PRESS: send key press event via HSP.
	 * <b>[Response]</b>
	 *    OK or ERROR.
	 * <b>[Example]</b>
	 *    Answer an incoming call.
	 * @code
	 *      Send:
	 *          AT+BTSINKIT=ANS
	 *      Response:
	 *          OK
	 * @endcode
	 * <b>[Note]</b>
	 *    None.
	 * </pre>
	 */
	 AT_BTSINKIT_CALL,
    /** <pre>
     * <b>[Category]</b>
     *    HFP
     * <b>[Description]</b>
     *    Set or get the HFP codec.
     * <b>[Command]</b>
     *    AT+HFPCODEC=get
     *    AT+HFPCODEC=set,(codec)
     * <b>[Parameter]</b>
     *    (codec): CVSD, set current codec to CVSD.
     *    (codec): MSBC, set current codec to MSBC.
     * <b>[Response]</b>
     *    OK / code mask: 0x%0x.
     * <b>[Example]</b>
     *    Set the current HFP codec.
     * @code
     *    Send:
     *        AT+HFPCODEC=set,MSBC
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_HFPCODEC,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth connection manager.
      * <b>[Description]</b>
      *    Operate calls.
      * <b>[Command]</b>
      *    AT+BTCMIT=DSC: enable discoverable.
      *    AT+BTCMIT=EIXT_DSC: disable discoverable.
      *    AT+BTCMIT=CLD: connect the latest connected device.
      *    AT+BTCMIT=CON,(addr): connect the indicated device by addr.
      *    AT+BTCMIT=DIS: disconnect all of the connected devices.
      *    AT+BTCMIT=RESET: clear all of the paired list.
      *    AT+BTCMIT=GRRSSI,(addr): get the raw RSSI about the indicated device.
      *    AT+BTCMIT=GRSSI,(addr): get the RSSI about the indicated device.
      *    AT+BTCMIT=BT_STANDBY: standby the bluetooth.
      *    AT+BTCMIT=BT_ACTIVE: active the bluetooth.
      *    AT+BTCMIT=reset_sniff_timer,(ms): set the timer of active enter to sniff mode in bluetooth link idle state.
      *    AT+BTCMIT=sniff_param,(max interval, min interval, sniff attemp, sniff timeout): set the default parameters of the sniff mode.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Enable/Disable discoverable.
      * @code
      *      Send:
      *          AT+BTCMIT=DSC
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT_BTCMIT,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth local public address.
      * <b>[Description]</b>
      *    Set or get local public address.
      * <b>[Command]</b>
      *    AT+BTLOCALADDR=GET
      *    AT+BTLOCALADDR=SET,(address)
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Set local public address.
      * @code
      *      Send:
      *          AT+BTLOCALADDR=set,112233445566
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT+BTLOCALADDR,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth AWS peer address.
      * <b>[Description]</b>
      *    Set or get AWS peer address.
      * <b>[Command]</b>
      *    AT+BTAWSPEERADDR=GET
      *    AT+BTAWSPEERADDR=SET,(address)
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Set AWS peer address.
      * @code
      *      Send:
      *          AT+BTAWSPEERADDR=SET,112233445566
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT+BTAWSPEERADDR,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth AWS role.
      * <b>[Description]</b>
      *    Set or get AWS role.
      * <b>[Command]</b>
      *    AT+BTAWSROLE=GET
      *    AT+BTAWSROLE=SET,(AGENT or PARTNER)
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Set AWS role.
      * @code
      *      Send:
      *          AT+BTAWSROLE=SET,AGENT
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT+BTAWSROLE,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth AWS key.
      * <b>[Description]</b>
      *    Set or get AWS key.
      * <b>[Command]</b>
      *    AT+BTAWSKEY=GET
      *    AT+BTAWSKEY=SET,(16bytes AWS key)
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Set AWS key.
      * @code
      *      Send:
      *          AT+BTAWSKEY=SET,00112233445566778899AABBCCDDEEFF
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT+BTAWSKEY,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth HID.
      * <b>[Description]</b>
      *    Monitor HID key operation.
      * <b>[Command]</b>
      *    AT+HID=bt hid (left/right/forward/up/down)
      *    AT+HID=bt hid lp (left/right/forward/up/down)
      * <b>[Response]</b>
      *    None.
      * <b>[Example]</b>
      *    Set remote to go home.
      * @code
      *      Send:
      *          AT+HID=bt hid home
      *      Response:
      *          None
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT+HID,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth Host.
      * <b>[Description]</b>
      *    Bluetooth Host test.
      * <b>[Command]</b>
      *    AT+EBTAT=enter_test_mode: disable the Bluetooth SDK power manager strategy.
      *    AT+EBTAT=exit_test_mode: exit test mode, enable the Bluetooth SDK power manager strategy.
      *    AT+EBTAT=bt_power_on: power on both Bluetooth Host and Controller.
      *    AT+EBTAT=bt_power_off: power off both Bluetooth Host and Controller.
      *    AT+EBTAT=bt_search: start Bluetooth inquiry.
      *    AT+EBTAT=dut: enable Bluetooth DUT mode.
      *    AT+EBTAT=set_dut_addr,(addr): set the BD address of Bluetooth in DUT mode.
      *    AT+EBTAT=tx_config,(tx_power): set the TX power of Bluetooth.
      *    AT+EBTAT=tx_power_gc,(GC): set the TX power GC of Bluetooth.
      *    AT+EBTAT=g_tx_power_gc: get the TX power GC of Bluetooth.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Power on both Bluetooth Host and Controller.
      * @code
      *      Send:
      *          AT+EBTAT=bt_power_on
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT_EBTAT,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth Controller.
      * <b>[Description]</b>
      *    Bluetooth Controller test.
      * <b>[Command]</b>
      *    AT+EBTPW=0: power off Bluetooth Controller only, and disable BTIF command mode.
      *    AT+EBTPW=1: power on Bluetooth Controller only, and enable BTIF command mode.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    Power on Bluetooth Controller.
      * @code
      *      Send:
      *          AT+EBTPW=1
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT_EBTPW,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth Controller.
      * <b>[Description]</b>
      *    Bluetooth Controller relay test.
      * <b>[Command]</b>
      *    AT+EBTER=SBUAD: set the uart port baud rate in relay mode.
      *    AT+EBTER=GBUAD: get the uart port baud rate in relay mode.
      *    AT+EBTER=EXIT: exit relay mode.
      *    AT+EBTER=port_number(for instance:2): enter relay mode with uart port.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    enter relay mode with uart port 2.
      * @code
      *      Send:
      *          AT+EBTER=2
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT_EBTER,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth Controller.
      * <b>[Description]</b>
      *    Send HCI command raw data to Bluetooth Controller via BTIF.
      * <b>[Command]</b>
      *    AT+EBTSHC=op_code, param_length, raw_data_of_param: send Bluetooth Controller HCI command raw data.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    send HCI write class of device command to Bluetooth Controller via BTIF.
      * @code
      *      Send:
      *          AT+EBTSHC=0C24,03,040424
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    Must send AT+EBTPW=1 before use AT+EBTSHC.
      * </pre>
      */
    AT_EBTSHC,
    /** <pre>
      * <b>[Catagory]</b>
      *    Bluetooth Controller.
      * <b>[Description]</b>
      *    Send HCI command raw data to Bluetooth Controller via Bluetoot HCI.
      * <b>[Command]</b>
      *    AT+EBTSHCD=raw_data_of_hci_cmd: send Bluetooth Controller HCI command raw data.
      * <b>[Response]</b>
      *    OK or ERROR.
      * <b>[Example]</b>
      *    send HCI write class of device command to Bluetooth Controller via Bluetooth HCI.
      * @code
      *      Send:
      *          AT+EBTSHC=240C03040424
      *      Response:
      *          OK
      * @endcode
      * <b>[Note]</b>
      *    None.
      * </pre>
      */
    AT_EBTSHCD,
} AT_CMD_BT;


/** @brief
 * This enum only helps explain feature related AT command.
 */
typedef enum {
#if 0
    /** <pre>
     * <b>[Category]</b>
     *    Speaker
     * <b>[Description]</b>
     *    Set speaker BROADCAST mode LS addr.
     * <b>[Command]</b>
     *    AT+BLEAWS=LSADDR,(addr): set LS addr.
     * <b>[Parameter]</b>
     *    <addr>: 6bytes, the LS addr to be set as.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+BLEAWS=LSADDR,121314151617
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_BLEAWS,
#endif
    /** <pre>
     * <b>[Category]</b>
     *    SmartCharger
     * <b>[Description]</b>
     *    Send AT CMD to simulate SmartCharger operation.
     * <b>[Command]</b>
     *    AT+SMC=(cmd): SmartCharger command.
     * <b>[Parameter]</b>
     *    (cmd): pwr_key_boot, boot via power_key, headset will be out of case.
     *    (cmd): chg_in_boot, boot via charger_in, headset will boot in the case.
     *    (cmd): chg_in, charger in (put into case).
     *    (cmd): chg_out, charger out (out of case).
     *    (cmd): chg_off, charger off (charge complete).
     *    (cmd): lid_close, send user_data1 event, then send lid_close event after 3 sec.
     *    (cmd): lid_open, send lid_open event.
     *    (cmd): key, send charger key event.
     *    (cmd): user_data1, send user_data1 event.
     *    (cmd): status, get SmartCharger APP information.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+SMC=lid_open
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_SMC,
    /** <pre>
    * <b>[Category]</b>
    *    APP
    * <b>[Description]</b>
    *    Configure APP features.
    * <b>[Command]</b>
    *    AT+APPFEATURE=SET,(feature),(1/0)
    *    AT+APPFEATURE=GET,(feature)
    * <b>[Parameter]</b>
    *    (feature): AUTORHO, 0 means disable auto RHO.
    *    (feature): NOCONN_SLEEP, 1 means do BT power off instead of system power off.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+APPFEATURE=SET,AUTORHO,0
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_APPFEATURE,
    /** <pre>
    * <b>[Category]</b>
    *    APP
    * <b>[Description]</b>
    *    Send simulated charger exist event.
    * <b>[Command]</b>
    *    AT+BATCHARGING=(cmd)
    * <b>[Parameter]</b>
    *    (cmd): 1 means charger in, 0 means charger out.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+BATCHARGING=1
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_BATCHARGING,
    /** <pre>
    * <b>[Category]</b>
    *    APP
    * <b>[Description]</b>
    *    Send simulated charging state event.
    * <b>[Command]</b>
    *    AT+BATCHARGINGSTATE=(cmd)
    * <b>[Parameter]</b>
    *    (cmd): Must be the value in battery_managerment_charger_state_t in MTK\battery_management\port\ab155x\inc\battery_management_core.h.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+BATCHARGINGSTATE=2
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_BATCHARGINGSTATE,
    /** <pre>
    * <b>[Category]</b>
    *    APP
    * <b>[Description]</b>
    *    Send simulated battery percentage event.
    * <b>[Command]</b>
    *    AT+BATPERCENT=(cmd)
    * <b>[Parameter]</b>
    *    (cmd): The percentage of battery.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+BATPERCENT=80
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_BATPERCENT,
    /** <pre>
    * <b>[Category]</b>
    *    APP
    * <b>[Description]</b>
    *    Send simulated battery voltage.
    * <b>[Command]</b>
    *    AT+BATVOLTAGE=(cmd)
    * <b>[Parameter]</b>
    *    (cmd): Voltage value, unit is mV.
    * <b>[Response]</b>
    *    OK or ERROR.
    * <b>[Example]</b>
    * @code
    *    Send:
    *        AT+Voltage=4200
    *    Response:
    *        OK
    * @endcode
    * <b>[Note]</b>
    *    None.
    * </pre>
    */
    AT_BATVOLTAGE,
    /** <pre>
     * <b>[Category]</b>
     *    FOTA
     * <b>[Description]</b>
     *    Cancel FOTA.
     * <b>[Command]</b>
     *    AT+FOTA=CANCEL
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+FOTA=CANCEL
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_FOTA,
#if 0
    /** <pre>
     * <b>[Category]</b>
     *    FOTA
     * <b>[Description]</b>
     *    Set the FOTA update flag.
     * <b>[Command]</b>
     *    AT+FOTA=2,GO
      * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     * @code
     *    Send:
     *        AT+FOTA=2,GO
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_FOTA,
#endif
    /** <pre>
     * <b>[Category]</b>
     *    Earbuds Gsound SPP RHO Test
     * <b>[Description]</b>
     *    Set/Remove Gsound SPP RHO flag.
     * <b>[Command]</b>
     *    AT+GSOUND=1,(cmd)
     * <b>[Parameter]</b>
     *    <cmd>: 1 means set Gsound SPP RHO flag. 2 means remove Gsound SPP RHO flag.
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Before Trigger RHO.
     * @code
     *    Send:
     *        AT+GSOUND=1,1
     *    Response:
     *        OK
     * @endcode
     *    After RHO done.
     * @code
     *    Send:
     *        AT+GSOUND=1,2
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_GSOUND,
#if 0
    /** <pre>
     * <b>[Category]</b>
     *    MFI
     * <b>[Description]</b>
     *    Check whether the MFI could read the cerificate data.
     * <b>[Command]</b>
     *    AT+GETMFICERT
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Try to read the cerificate
     * @code
     *    Send:
     *        AT+GETMFICERT
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_GETMFICERT,
    /** <pre>
     * <b>[Category]</b>
     *    MFI
     * <b>[Description]</b>
     *    Check whether the MFI could generate the challenge response.
     * <b>[Command]</b>
     *    AT+GETMFIRESP
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Try to generate the challenge response.
     * @code
     *    Send:
     *        AT+GETMFIRESP
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_GETMFIRESP,
#endif
    /** <pre>
     * <b>[Category]</b>
     *    MFI
     * <b>[Description]</b>
     *    Check whether the MFI chip is goode.
     * <b>[Command]</b>
     *    AT+CHECKMFICHIP
     * <b>[Response]</b>
     *    OK or ERROR.
     * <b>[Example]</b>
     *    Check the mfi chip.
     * @code
     *    Send:
     *        AT+CHECKMFICHIP
     *    Response:
     *        OK
     * @endcode
     * <b>[Note]</b>
     *    None.
     * </pre>
     */
    AT_CHECKMFICHIP,
} AT_CMD_FEATURE;

/**
*@}
*/
