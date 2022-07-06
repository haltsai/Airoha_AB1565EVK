
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "task_def.h"
#include "hal.h"
#include "hal_gpt.h"
#include "airo_key_event.h"
#include "airo_key_event_internal.h"
#include "bsp_inear_abov_A96T516.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE

#ifdef BSP_INEAR_USE_A96T349DF
#include "bsp_inear_abov_A96T349DF_FW.h"
#else
#include "bsp_inear_abov_A96T516_FW.h"
#endif

#endif


#ifdef MTK_IN_EAR_FEATURE_ENABLE
bsp_inear_context_t bsp_inear_context;
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t bsp_inear_send_buffer[50];
ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t bsp_inear_receive_buffer[50];

static void bsp_inear_send_state(uint32_t key_data,airo_key_driven_t state);
static void bsp_inear_handle_inear_key(void);

void bsp_inear_mutex_take(void);
void bsp_inear_mutex_give(void);
void bsp_inear_set_mode(uint8_t mode);

void bsp_inear_iic_call_back (uint8_t slave_address,hal_i2c_callback_event_t event,void *user_data)
{
	uint8_t count;
	count = *((uint8_t *)user_data);
	//log_hal_msgid_error("[BSP_INEAR] cs_press_iic_call_back count=%d event =%d\n",2,count,event);

	if(count%2)
		bsp_inear_context.dma_write_flag = 1;
	else
		bsp_inear_context.dma_read_flag = 1;

	bsp_inear_context.iic_cb_event = event;
    if(HAL_I2C_EVENT_ACK_ERROR == event) {
        //ACK error handler;
    } else if(HAL_I2C_EVENT_NACK_ERROR == event) {
        //NACK error handler;
    } else if(HAL_I2C_EVENT_TIMEOUT_ERROR == event) {
        // Timeout handler;
    } else if(HAL_I2C_EVENT_SUCCESS == event) {
        // Do something;
        // The slave_address indicates which device is using I2C.
    }
}

void bsp_inear_delay_ms(uint32_t time_ms)
{
	hal_gpt_delay_ms(time_ms);
}

static char bsp_inear_iic_write(uint8_t regAddress ,const uint8_t *dat, uint32_t length)
{
    hal_i2c_status_t ret;
   	//uint8_t final_data[50];
	uint8_t trial = 0;
	uint8_t i2c_event_wait_count = 0;

	bsp_inear_context.dma_write_flag = 0;
	if(bsp_inear_context.call_back_count%2)
		bsp_inear_context.call_back_count += 2;
	else
		bsp_inear_context.call_back_count += 1;
	if(bsp_inear_context.call_back_count >200)
		bsp_inear_context.call_back_count = 1;
	memset(bsp_inear_send_buffer,0x0,sizeof(bsp_inear_send_buffer));
	bsp_inear_send_buffer[0]=regAddress;
	memcpy(bsp_inear_send_buffer+1,dat,length);
	//for(int i = 0;i<length+1; i++)
	//	log_i2c_error("[BSP_INEAR]  %x\n",1,final_data[i]);
#ifdef BSP_INEAR_USE_POLLING
    ret = hal_i2c_master_send_polling(HAL_I2C_MASTER_2, ABOV_SLAVE_ADDR, bsp_inear_send_buffer, length+1);
#else
    do{
		ret = hal_i2c_master_send_dma(HAL_I2C_MASTER_2, ABOV_SLAVE_ADDR, bsp_inear_send_buffer, length+1);
		while(bsp_inear_context.dma_write_flag == 0 && i2c_event_wait_count < BSP_INEAR_I2C_DMA_TRIAL)
		{
			i2c_event_wait_count ++ ;
			bsp_inear_delay_ms(BSP_INEAR_I2C_DMA_WAIT_TIME);
		}
		i2c_event_wait_count = 0;
		trial++;
    }while(bsp_inear_context.iic_cb_event!=HAL_I2C_EVENT_SUCCESS && trial<BSP_INEAR_I2C_DMA_TRIAL);
#endif
	//log_hal_msgid_error("[BSP_INEAR] dma_write_flag =%d iic_cb_event=%d\n",2,bsp_inear_context.dma_write_flag,bsp_inear_context.iic_cb_event);
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_write ret=%d trial=%d\n",2,ret,trial);
    if (HAL_I2C_STATUS_OK != ret) {
	    //Error handler;
	}
	//cs_press_iic_deinit();
    return ret;

}


static char bsp_inear_iic_write_flash(uint8_t regAddress ,const uint8_t *dat, uint32_t length)
{
    hal_i2c_status_t ret;
   	//uint8_t final_data[50];
	uint8_t trial = 0;
	uint8_t i2c_event_wait_count = 0;

	bsp_inear_context.dma_write_flag = 0;
	if(bsp_inear_context.call_back_count%2)
		bsp_inear_context.call_back_count += 2;
	else
		bsp_inear_context.call_back_count += 1;
	if(bsp_inear_context.call_back_count >200)
		bsp_inear_context.call_back_count = 1;
	memset(bsp_inear_send_buffer,0x0,sizeof(bsp_inear_send_buffer));
	memcpy(bsp_inear_send_buffer,dat,length);
	//for(int i = 0;i<length+1; i++)
	//	log_i2c_error("[BSP_INEAR]  %x\n",1,final_data[i]);
#ifdef BSP_INEAR_USE_POLLING
    ret = hal_i2c_master_send_polling(HAL_I2C_MASTER_2, ABOV_SLAVE_ADDR, bsp_inear_send_buffer, length);
#else
    do{
		ret = hal_i2c_master_send_dma(HAL_I2C_MASTER_2, ABOV_SLAVE_ADDR, bsp_inear_send_buffer, length);
		while(bsp_inear_context.dma_write_flag == 0 && i2c_event_wait_count < BSP_INEAR_I2C_DMA_TRIAL)
		{
			i2c_event_wait_count ++ ;
			bsp_inear_delay_ms(BSP_INEAR_I2C_DMA_WAIT_TIME);
		}
		i2c_event_wait_count = 0;
		trial++;
    }while(bsp_inear_context.iic_cb_event!=HAL_I2C_EVENT_SUCCESS && trial<BSP_INEAR_I2C_DMA_TRIAL);
#endif
	//log_hal_msgid_error("[BSP_INEAR] dma_write_flag =%d iic_cb_event=%d\n",2,bsp_inear_context.dma_write_flag,bsp_inear_context.iic_cb_event);
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_write ret=%d trial=%d\n",2,ret,trial);
    if (HAL_I2C_STATUS_OK != ret) {
	    //Error handler;
	}
	//cs_press_iic_deinit();
    return ret;

}

static char bsp_inear_iic_read_no_reg(uint8_t regAddress ,uint8_t *dat, uint32_t length)
{
	hal_i2c_status_t ret;
#ifdef BSP_INEAR_USE_POLLING
	hal_i2c_send_to_receive_config_t config;
#endif
	uint8_t trial = 0;
	uint8_t i2c_event_wait_count = 0;
	memset(bsp_inear_send_buffer,0,sizeof(bsp_inear_send_buffer));
	memset(bsp_inear_receive_buffer,0,sizeof(bsp_inear_receive_buffer));
	bsp_inear_send_buffer[0] = regAddress;
#ifdef BSP_INEAR_USE_POLLING
	config.slave_address = ABOV_SLAVE_ADDR;
	config.send_data = bsp_inear_send_buffer;
	config.send_length = 1;
	config.receive_buffer = bsp_inear_receive_buffer;
	config.receive_length = length;
#endif
	bsp_inear_context.dma_read_flag = 0;
	if(bsp_inear_context.call_back_count%2)
		bsp_inear_context.call_back_count += 1;
	else
		bsp_inear_context.call_back_count += 2;
	if(bsp_inear_context.call_back_count >200)
		bsp_inear_context.call_back_count = 0;
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_read regAddress =%d\n",1,&regAddress);
#ifdef BSP_INEAR_USE_POLLING
	ret = hal_i2c_master_send_to_receive_polling(HAL_I2C_MASTER_2,&config);
#else
	do
	{
		ret = hal_i2c_master_receive_dma(HAL_I2C_MASTER_2,ABOV_SLAVE_ADDR, bsp_inear_receive_buffer, length);
		//cs_press_delay_ms(CS_PRESS_I2C_DMA_WAIT_TIME);
		while(bsp_inear_context.dma_read_flag == 0 && i2c_event_wait_count < BSP_INEAR_I2C_DMA_TRIAL)
		{
			i2c_event_wait_count ++ ;
			bsp_inear_delay_ms(BSP_INEAR_I2C_DMA_WAIT_TIME);
		}
		i2c_event_wait_count = 0;
		trial++;
	}while(bsp_inear_context.iic_cb_event != HAL_I2C_EVENT_SUCCESS && trial<BSP_INEAR_I2C_DMA_TRIAL);
#endif
	if(bsp_inear_context.iic_cb_event == HAL_I2C_EVENT_SUCCESS)
	{
		memcpy(dat,bsp_inear_receive_buffer,length);
	}

	//log_hal_msgid_error("[BSP_INEAR] dma_read_flag =%d iic_cb_event=%d\n",2,bsp_inear_context.dma_read_flag,bsp_inear_context.iic_cb_event);
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_read  ret=%d trial=%d\n",2,ret,trial);
	for(uint32_t i = 0;i<length; i++)
		log_hal_msgid_info("[BSP_INEAR]  %x\n",1,dat[i]);
	return ret;
}


static char bsp_inear_iic_read(uint8_t regAddress ,uint8_t *dat, uint32_t length)
{
	hal_i2c_status_t ret;
	hal_i2c_send_to_receive_config_t config;
	uint8_t trial = 0;
	uint8_t i2c_event_wait_count = 0;
	memset(bsp_inear_send_buffer,0,sizeof(bsp_inear_send_buffer));
	memset(bsp_inear_receive_buffer,0,sizeof(bsp_inear_receive_buffer));
	bsp_inear_send_buffer[0] = regAddress;
	config.slave_address = ABOV_SLAVE_ADDR;
	config.send_data = bsp_inear_send_buffer;
	config.send_length = 1;
	config.receive_buffer = bsp_inear_receive_buffer;
	config.receive_length = length;
	bsp_inear_context.dma_read_flag = 0;
	if(bsp_inear_context.call_back_count%2)
		bsp_inear_context.call_back_count += 1;
	else
		bsp_inear_context.call_back_count += 2;
	if(bsp_inear_context.call_back_count >200)
		bsp_inear_context.call_back_count = 0;
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_read regAddress =%d\n",1,&regAddress);
#ifdef BSP_INEAR_USE_POLLING
	ret = hal_i2c_master_send_to_receive_polling(HAL_I2C_MASTER_2,&config);
#else
	do
	{
		ret = hal_i2c_master_send_to_receive_dma(HAL_I2C_MASTER_2,&config);
		//cs_press_delay_ms(CS_PRESS_I2C_DMA_WAIT_TIME);
		while(bsp_inear_context.dma_read_flag == 0 && i2c_event_wait_count < BSP_INEAR_I2C_DMA_TRIAL)
		{
			i2c_event_wait_count ++ ;
			bsp_inear_delay_ms(BSP_INEAR_I2C_DMA_WAIT_TIME);
		}
		i2c_event_wait_count = 0;
		trial++;
	}while(bsp_inear_context.iic_cb_event != HAL_I2C_EVENT_SUCCESS && trial<BSP_INEAR_I2C_DMA_TRIAL);
#endif
	if(bsp_inear_context.iic_cb_event == HAL_I2C_EVENT_SUCCESS)
	{
		memcpy(dat,bsp_inear_receive_buffer,length);
	}

	//log_hal_msgid_error("[BSP_INEAR] dma_read_flag =%d iic_cb_event=%d\n",2,bsp_inear_context.dma_read_flag,bsp_inear_context.iic_cb_event);
	//log_hal_msgid_error("[BSP_INEAR] bsp_inear_iic_read  ret=%d trial=%d\n",2,ret,trial);
	//for(uint32_t i = 0;i<length; i++)
	//	log_hal_msgid_info("[BSP_INEAR]  %x\n",1,dat[i]);
	return ret;
}

void bsp_inear_set_reg(uint8_t reg,uint8_t value)
{
	uint8_t send_buf[2];
	int8_t ret = 0;
	uint8_t confirm;
	uint8_t retry_time = 0;
	do
	{
		send_buf[0] = value;
		ret = bsp_inear_iic_write(reg, send_buf, 1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_reg:%d value:%d fail ret=%d\n",3,reg,value,ret);
		}
		bsp_inear_delay_ms(5);

		ret = bsp_inear_iic_read(reg,&confirm,1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_reg %d read fail!", 1,reg);
		}
		retry_time ++;
		if(confirm == value)
			return;
		else
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_reg  fail! value=%d confirm=%d",2,value,confirm);
	}while(retry_time < 3);
}

void bsp_inear_power_on_to_boot(void)
{
	hal_rtc_gpio_set_output(HAL_RTC_GPIO_2,0);
	//hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_GPIO10);
	//hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_OUTPUT);
	//hal_gpio_set_output(HAL_GPIO_10, HAL_GPIO_DATA_LOW);
	bsp_inear_delay_ms(50);
	hal_rtc_gpio_set_output(HAL_RTC_GPIO_2,1);
	//hal_gpio_set_output(HAL_GPIO_10, HAL_GPIO_DATA_HIGH);
	bsp_inear_delay_ms(45);

}

void bsp_inear_power_on(uint8_t on)
{
	if(on)
	{
		log_hal_msgid_info("[BSP_INEAR]bsp_inear_power_on\n",0);
		hal_rtc_gpio_set_output(HAL_RTC_GPIO_2,1);
		//hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_GPIO10);
		//hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_OUTPUT);
		//hal_gpio_set_output(HAL_GPIO_10, HAL_GPIO_DATA_HIGH);
		bsp_inear_delay_ms(200);
	}
	else
	{
		log_hal_msgid_info("[BSP_INEAR]bsp_inear_power_off\n",0);
		//hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_GPIO10);
		//hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_OUTPUT);
		//hal_gpio_set_output(HAL_GPIO_10, HAL_GPIO_DATA_LOW);
		hal_rtc_gpio_set_output(HAL_RTC_GPIO_2,0);
	}

}


void bsp_inear_reset(void)
{
	bsp_inear_power_on(0);
	bsp_inear_delay_ms(100);
	bsp_inear_power_on(1);
	bsp_inear_delay_ms(500);
#ifdef BSP_INEAR_HEARTBEAT_RESET
	bsp_inear_enable_ic_heartbeat(1);
#endif
	log_hal_msgid_error("[BSP_INEAR] bsp_inear_reset \n",0);
}


uint8_t bsp_inear_iic_init(void)
{
	hal_i2c_status_t ret = 0;
	hal_i2c_config_t i2c_config;


	i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
	hal_gpio_init(HAL_GPIO_6);
	hal_gpio_init(HAL_GPIO_7);

	hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_I2C2_SCL);
	hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_I2C2_SDA);
	ret = hal_i2c_master_init(HAL_I2C_MASTER_2, &i2c_config);
	log_hal_msgid_info("[BSP_INEAR] hal_i2c_master_init status= %d\n",1,ret);
	if(ret)
		return 1;
#ifndef  BSP_INEAR_USE_POLLING
	ret = hal_i2c_master_register_callback(HAL_I2C_MASTER_2, bsp_inear_iic_call_back, (void *) &bsp_inear_context.call_back_count);//Register a user callback
	log_hal_msgid_info("[BSP_INEAR] hal_i2c_master_register_callback status= %d\n",1,ret);
	if(ret)
		return 1;
#endif
	return 0;
}

void bsp_inear_rtc_gpio_eint_test(void)
{
	#if 1
	static uint8_t count = 0;
	if(count == 100)
		count = 0;
	if(!(count %2))
		bsp_inear_send_state(DEVICE_KEY_1,AIRO_KEY_DRIVEN_PRESS);
	else
		bsp_inear_send_state(DEVICE_KEY_1,AIRO_KEY_DRIVEN_RELEASE);
	count++;
	#endif
	//bsp_inear_handle_inear_key();
}

void bsp_inear_rtc_gpio_eint_handler(void *user_data)
{
	// BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
	//log_hal_msgid_info("[BSP_INEAR]bsp_inear_rtc_gpio_eint_handler",0);
	LOG_W(MPLOG,"[BSP_INEAR]bsp_inear_rtc_gpio_eint_handler\r\n");
	#if 0
	bsp_inear_queue_t queue_item;
	assert(bsp_inear_context.queue);
	queue_item.msg = BSP_INEAR_QUEUE_MSG_TOUCH_EVENT;
	xQueueSendFromISR(bsp_inear_context.queue, &queue_item, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
	#endif
	if(bsp_inear_context.handle_key_timer != NULL)
	{
		xTimerStopFromISR(bsp_inear_context.handle_key_timer,0);
		xTimerStartFromISR(bsp_inear_context.handle_key_timer,0);
	}
}

void bsp_inear_rtc_gpio_setting(bool int_enable)
{
	hal_rtc_eint_config_t rtc_eint_config;
	bsp_inear_mutex_take();
#if 0
	if(int_enable)
	{
		if(bsp_inear_context.charger_status)
		{
			bsp_inear_mutex_give();
			return ;
		}

	}
#endif
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_rtc_gpio_setting int_enable %x", 1,(uint8_t)int_enable);
	rtc_eint_config.rtc_gpio                = HAL_RTC_GPIO_0;
    rtc_eint_config.is_enable_rtc_eint      = int_enable;
    rtc_eint_config.is_falling_edge_active  = true;
    rtc_eint_config.is_enable_debounce      = true;
	hal_rtc_eint_init(&rtc_eint_config);
	hal_rtc_eint_register_callback(HAL_RTC_GPIO_0, bsp_inear_rtc_gpio_eint_handler, (void *)1);
	bsp_inear_mutex_give();

}


uint8_t bsp_inear_get_inear_state(void)
{
	return bsp_inear_context.inear_state;
}

static void bsp_inear_send_state(uint32_t key_data,airo_key_driven_t state)
{
    uint32_t time;
    airo_key_mapping_event_t key_event;
	if(key_data == DEVICE_KEY_0)
	{
		if(bsp_inear_context.key_state == state)
			return ;
		bsp_inear_context.key_state = state;
	}
	else if(key_data == DEVICE_KEY_1)
	{
        //不上报入耳键值mask if
        if(bsp_inear_context.inear_state== state)
			return ;
#ifdef BSP_INEAR_KEY_DISCARD_FEATURE
		if(state == AIRO_KEY_DRIVEN_RELEASE)
		{
			bsp_inear_context.ch2 = 0;
			bsp_inear_context.key_discard = 1;
		}
		else if(state == AIRO_KEY_DRIVEN_PRESS)
		{
			xTimerStopFromISR(bsp_inear_context.key_discard_timer,0);
			xTimerStartFromISR(bsp_inear_context.key_discard_timer,0);
		}
#endif
		bsp_inear_context.inear_state = state;
	}
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
    key_event.state      = state;
    key_event.key_data   = key_data;
    key_event.time_stamp = time;
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_send_state key_data=%d state=%d\r\n", 2, key_data,(uint32_t)state);
    airo_key_process_key(&key_event, AIRO_KEY_INEAR_TOUCH);
	if(key_data == DEVICE_KEY_0 && state == 1)
		LOG_W(MPLOG,"key_press: time=0x%x\n",time);
	if(key_data == DEVICE_KEY_0 && state == 0)
		LOG_W(MPLOG,"key_relese: time=0x%x\n",time);
}

uint8_t bsp_inear_get_inear_status(void)
{
	uint8_t status = 0;
	int8_t ret = 0;
	uint8_t ch0,ch1,ch2;

	ret = bsp_inear_iic_read(REG_keyValue,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_get_inear_status read fail!", 0);
		return ret;
	}

	ch0 = status & 0x1;
	ch1 = (status >>1) & 0x1;
#ifdef 	BSP_INEAR_USE_A96T349DF
	ch2 = (status >>4) & 0x1;
#else
	ch2 = (status >>2) & 0x1;
#endif
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_get_inear_status ch0:%d ch1:%d ch2:%d\n",3,ch0,ch1,ch2);
	return ret;
}

static void bsp_inear_handle_inear_key(void)
{
	uint8_t status = 0;
	int8_t ret = 0;
	uint8_t ch0,ch1,ch2;

	ret = bsp_inear_iic_read(REG_keyValue,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_handle_inear_key read fail!", 0);
		return;
	}
	LOG_W(MPLOG,"[BSP_INEAR]bsp_inear_handle_inear_key 0x00=%x\n",status);
	ch0 = status & 0x1;
	ch1 = (status >>1) & 0x1;
#ifdef 	BSP_INEAR_USE_A96T349DF
	ch2 = (status >>4) & 0x1;
#else
	ch2 = (status >>2) & 0x1;
#endif
	if(bsp_inear_context.charger_status)
	{
		if(bsp_inear_context.inear_state == AIRO_KEY_DRIVEN_PRESS)
		{
			bsp_inear_send_state(DEVICE_KEY_1,AIRO_KEY_DRIVEN_RELEASE);
			log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key release inear key\n",0);
		}
		log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key charge in return\n",0);
		return ;
	}
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key ch0:%d ch1:%d ch2:%d\n",3,ch0,ch1,ch2);
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key pre_ch0:%d pre_ch1:%d pre_ch2:%d\n",
		3,bsp_inear_context.ch0,bsp_inear_context.ch1,bsp_inear_context.ch2);
	if(ch0 != bsp_inear_context.ch0 ||
		ch1 != bsp_inear_context.ch1||
		ch2 != bsp_inear_context.ch2 )
	{

		if(ch0 != bsp_inear_context.ch0 ||
			ch1 != bsp_inear_context.ch1)
		{
			if(ch0 && ch1 )
			{
				log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key ear in!", 0);
				bsp_inear_send_state(DEVICE_KEY_1,AIRO_KEY_DRIVEN_PRESS);

			}
			else if((!ch0 && !ch1) ||(!ch0 && ch1))
			//else if(bsp_inear_context.ch0 &&  bsp_inear_context.ch1)
			{
				log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key ear out!", 0);
				bsp_inear_send_state(DEVICE_KEY_1,AIRO_KEY_DRIVEN_RELEASE);
#ifdef BSP_INEAR_KEY_RELATED
				if(bsp_inear_context.ch2)
				{
#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
					bsp_inear_context.istouchon = 0;
#endif
					log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key key release!", 0);
					bsp_inear_send_state(DEVICE_KEY_0,AIRO_KEY_DRIVEN_RELEASE);
					bsp_inear_context.ch2 = 0;
				}
#endif
			}
			bsp_inear_context.ch0 = ch0;
			bsp_inear_context.ch1 = ch1;
			return ;
		}
		if(ch2 != bsp_inear_context.ch2)
		{
#ifdef BSP_INEAR_KEY_DISCARD_FEATURE
			if(bsp_inear_context.key_discard)
			{
				log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key key discard!", 0);
				return;
			}
#endif
			if(ch2)
			{
#ifdef BSP_INEAR_KEY_RELATED
				if(bsp_inear_context.ch0 && bsp_inear_context.ch1)
#endif
				{
#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
					xTimerStopFromISR(bsp_inear_context.false_trigger_timer,0);
					xTimerStartFromISR(bsp_inear_context.false_trigger_timer,0);
					bsp_inear_context.istouchon = 1;
#endif
					log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key key press!", 0);
					bsp_inear_send_state(DEVICE_KEY_0,AIRO_KEY_DRIVEN_PRESS);
					bsp_inear_context.ch2 = ch2;
				}

			}
			else
			{
#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
				bsp_inear_context.istouchon = 0;
				bsp_inear_context.is_false_trigger = 0;
				bsp_inear_context.touch_on_timer_count = 0;
				xTimerStopFromISR(bsp_inear_context.false_trigger_timer,0);

#endif
				log_hal_msgid_info("[BSP_INEAR]bsp_inear_handle_inear_key key release!", 0);
				bsp_inear_send_state(DEVICE_KEY_0,AIRO_KEY_DRIVEN_RELEASE);
				bsp_inear_context.ch2 = ch2;
			}

		}

	}

}

void bsp_inear_mutex_creat(void)
{
    bsp_inear_context.mutex= xSemaphoreCreateMutex();
    if (bsp_inear_context.mutex == NULL) {
        log_hal_msgid_error("[BSP_INEAR]bsp_inear_mutex_creat fail!!\n", 0);
    }
}

void bsp_inear_key_mutex_creat(void)
{
    bsp_inear_context.key_mutex= xSemaphoreCreateMutex();
    if (bsp_inear_context.key_mutex == NULL) {
        log_hal_msgid_error("[BSP_INEAR]bsp_inear_key_mutex_creat fail!!\n", 0);
    }
}


void bsp_inear_mutex_take(void)
{

    if(bsp_inear_context.mutex != NULL)
    {
        if (xSemaphoreTake(bsp_inear_context.mutex, portMAX_DELAY) == pdFALSE) {
            log_hal_msgid_error("[BSP_INEAR] bsp_inear_mutex_take fail!!\n", 0);
        }
		else
		{
			log_hal_msgid_info("[BSP_INEAR] bsp_inear_mutex_take\n", 0);
		}
    }

}
void bsp_inear_mutex_give(void)
{
    if(bsp_inear_context.mutex != NULL)
    {
        if (xSemaphoreGive(bsp_inear_context.mutex) == pdFALSE) {
            log_hal_msgid_error("[BSP_INEAR] bsp_inear_mutex_give fail!!\n", 0);
        }
		else
		{
			log_hal_msgid_info("[BSP_INEAR] bsp_inear_mutex_give\n", 0);
		}
    }

}

void bsp_inear_key_mutex_take(void)
{

    if(bsp_inear_context.key_mutex != NULL)
    {
        if (xSemaphoreTake(bsp_inear_context.key_mutex, portMAX_DELAY) == pdFALSE) {
            log_hal_msgid_error("[BSP_INEAR] bsp_inear_key_mutex_take fail!!\n", 0);
        }
		else
		{
			log_hal_msgid_info("[BSP_INEAR] bsp_inear_key_mutex_take\n", 0);
		}
    }

}

void bsp_inear_key_mutex_give(void)
{
    if(bsp_inear_context.key_mutex != NULL)
    {
        if (xSemaphoreGive(bsp_inear_context.key_mutex) == pdFALSE) {
            log_hal_msgid_error("[BSP_INEAR] bsp_inear_key_mutex_give fail!!\n", 0);
        }
		else
		{
			log_hal_msgid_info("[BSP_INEAR] bsp_inear_key_mutex_give\n", 0);
		}
    }

}


#ifndef BSP_INEAR_USE_A96T349DF
void bsp_inear_print_thre_diff_online(void)
{
	uint16_t ch0_th,ch1_th,ch2_th;
	uint16_t ch0_diff,ch1_diff,ch2_diff;
	uint8_t temp[6];
	for(int i=0x0c;i<=0x11;i++)
		bsp_inear_iic_read(i,&temp[i-0xc],1);
	ch0_th = (temp[0]<<8)|temp[1];
	ch1_th = (temp[2]<<8)|temp[3];
	ch2_th = (temp[4]<<8)|temp[5];

	for(int i=0x24;i<=0x29;i++)
		bsp_inear_iic_read(i,&temp[i-0x24],1);
	ch0_diff = (temp[0]<<8)|temp[1];
	ch1_diff = (temp[2]<<8)|temp[3];
	ch2_diff = (temp[4]<<8)|temp[5];
	LOG_W(MPLOG,"bsp_inear_print_thre_diff_online thod:%d %d %d\n",ch0_th,ch1_th,ch2_th);
	LOG_W(MPLOG,"bsp_inear_print_thre_diff_online diff:%d %d %d\n",ch0_diff,ch1_diff,ch2_diff);

}

void bsp_inear_print_check_bypass_cap_online(void)
{
	uint8_t temp[6];
	uint16_t ch0_cap,ch1_cap,ch2_cap;
	for(int i=0x2a;i<=0x2f;i++)
		bsp_inear_iic_read(i,&temp[i-0x2a],1);
	ch0_cap = (temp[0]<<8)|temp[1];
	ch1_cap = (temp[2]<<8)|temp[3];
	ch2_cap = (temp[4]<<8)|temp[5];
	LOG_W(MPLOG,"bsp_inear_cap:%d %d %d\n",ch0_cap,ch1_cap,ch2_cap);
}

#else
void bsp_inear_print_total_cap_online(void)
{
	uint8_t temp[10];
	uint16_t ch0_cap,ch1_cap,ch2_cap,ch3_cap,ref_cap;
	for(int i=0x40;i<=0x49;i++)
		bsp_inear_iic_read(i,&temp[i-0x40],1);
	ch0_cap = (temp[0]<<8)|temp[1];
	ch1_cap = (temp[2]<<8)|temp[3];
	ch2_cap = (temp[4]<<8)|temp[5];
	ch3_cap = (temp[6]<<8)|temp[7];
	ref_cap = (temp[8]<<8)|temp[9];
	LOG_W(MPLOG,"bsp_inear_cap:%d %d %d %d %d\n",ch0_cap,ch1_cap,ch2_cap,ch3_cap,ref_cap);
}

uint16_t bsp_inear_get_key_diff(void)
{
	uint8_t temp[2];
	uint16_t ch2_diff;
	for(int i=0x34;i<=0x35;i++)
		bsp_inear_iic_read(i,&temp[i-0x34],1);
	ch2_diff = (temp[0]<<8)|temp[1];
	LOG_W(MPLOG,"bsp_inear_get_key_diff:%d\n",ch2_diff);
	return ch2_diff;
}

void bsp_inear_print_diff_online(void)
{
	uint8_t temp[8];
	uint16_t ch0_diff,ch1_diff,ch2_diff,ch3_diff;
	for(int i=0x30;i<=0x37;i++)
		bsp_inear_iic_read(i,&temp[i-0x30],1);
	ch0_diff = (temp[0]<<8)|temp[1];
	ch1_diff = (temp[2]<<8)|temp[3];
	ch2_diff = (temp[4]<<8)|temp[5];
	ch3_diff = (temp[6]<<8)|temp[7];
	LOG_W(MPLOG,"bsp_inear_diff:%d %d %d %d\n",ch0_diff,ch1_diff,ch2_diff,ch3_diff);
}

void bsp_inear_print_rawdata(void)
{
	uint8_t temp[8];
	uint16_t ch0_raw,ch1_raw,ch2_raw;
	for(int i=0x20;i<=0x25;i++)
		bsp_inear_iic_read(i,&temp[i-0x20],1);
	ch0_raw = (temp[0]<<8)|temp[1];
	ch1_raw = (temp[2]<<8)|temp[3];
	ch2_raw = (temp[4]<<8)|temp[5];
	LOG_W(MPLOG,"bsp_inear_print_rawdata:%d %d %d\n",ch0_raw,ch1_raw,ch2_raw);

}


void bsp_inear_print_baseline(void)
{
	uint8_t temp[8];
	uint16_t ch0_base,ch1_base,ch2_base;
	for(int i=0x28;i<=0x2d;i++)
		bsp_inear_iic_read(i,&temp[i-0x28],1);
	ch0_base = (temp[0]<<8)|temp[1];
	ch1_base = (temp[2]<<8)|temp[3];
	ch2_base = (temp[4]<<8)|temp[5];
	LOG_W(MPLOG,"bsp_inear_print_baseline:%d %d %d\n",ch0_base,ch1_base,ch2_base);

}

void bsp_inear_print_init_status(void)
{
	uint8_t value;
	bsp_inear_iic_read(0xf8,&value,1);
	LOG_W(MPLOG,"bsp_inear_print_init_status:%d\n",value);

}

void bsp_inear_debug_uart(void)
{
	uint8_t value[32];
	for(int i=0xb0;i<=0xcf;i++)
	{
		bsp_inear_iic_read(i,&value[i-0xb0],1);
	}
	LOG_W(MPLOG,"b0-bf %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
		value[0],value[1],value[2],value[3],value[4],value[5],value[6],value[7],value[8],
		value[9],value[10],value[11],value[12],value[13],value[14],value[15]);
	LOG_W(MPLOG,"c0-cf %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
		value[16],value[17],value[18],value[19],value[20],value[21],value[22],value[23],value[24],
		value[25],value[26],value[27],value[28],value[29],value[30],value[31]);
}


void bsp_inear_print_preraw(void)
{
	uint8_t value[4];
	uint16_t ch0,ch1;
	for(int i=0xba;i<=0xbd;i++)
	{
		bsp_inear_iic_read(i,&value[i-0xba],1);
	}
	ch0 = value[0]*256 + value[1];
	ch1 = value[2]*256 + value[3];
	LOG_W(MPLOG,"ch0 %d ch1 %d\n",ch0,ch1);
}



#endif

void bsp_inear_test_recal(void)
{
	int8_t ret = 0;
	uint8_t send_buf[2];
	send_buf[0] = 1;
	ret = bsp_inear_iic_write(ABOV_RECALI_REG, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_test_recal fail ret=%d\n",1,ret);
	}

}


uint8_t bsp_inear_read_firmware_version(void)
{
	uint8_t status = 0;
	int8_t ret = 0;

	ret = bsp_inear_iic_read(0x02,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_read_firmware_version read fail!", 0);
		return 1;
	}
	return status;
}

uint8_t bsp_inear_read_mode(void)
{
	uint8_t status = 0;
	int8_t ret = 0;

	ret = bsp_inear_iic_read(ABOV_CTRL_CHANNEL_REG,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_read_mode read fail!", 0);
	}
	return ret;
}

void bsp_inear_key_handler(TimerHandle_t pxExpiredTimer)
{
	bsp_inear_key_mutex_take();
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_key_handler\n", 0);
	xTimerStopFromISR(bsp_inear_context.handle_key_timer,0);
	bsp_inear_handle_inear_key();
	bsp_inear_key_mutex_give();
}


void bsp_inear_enable_ic_heartbeat(uint8_t enable)
{
#ifdef BSP_INEAR_HEARTBEAT_RESET
	uint8_t send_buf[2];
	int8_t ret = 0xff;
	uint8_t status = 0;

	log_hal_msgid_info("[BSP_INEAR]bsp_inear_enable_ic_heartbeat enable=%d\n",1,enable);
	send_buf[0] = enable;
	ret = bsp_inear_iic_write(ABOV_HOST_RESET_CHECK_EN, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_enable_ic_heartbeat enable heartbeat fail! ret=%d\n",1,ret);
	}

	ret = bsp_inear_iic_read(ABOV_HOST_RESET_CHECK_EN,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_enable_ic_heartbeat confirm heartbeat fail! ret =%d\n",1,ret);
	}
	bsp_inear_context.heartbeat_enabled = enable;
#endif

}

#ifdef BSP_INEAR_HEARTBEAT_RESET
uint8_t bsp_inear_check_IC_state(void)
{
	uint8_t status = 0;
	uint8_t send_buf[2];
	int8_t ret = 0;

	if( !bsp_inear_context.heartbeat_enabled )
		return 0;
	ret = bsp_inear_iic_read(ABOV_HOST_RESET_CHECK,&status,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_check_IC_state read fail! ret =%d\n",1,ret);
		return 1;
	}
	if(status != BSP_INEAR_ABOV_MAGIC_NO)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_check_IC_state abov no wrong! no=%x\n",1,status);
		return 1;
	}

	#if 1
	send_buf[0] = BSP_INEAR_HOST_MAGIC_NO;
	ret = bsp_inear_iic_write(ABOV_HOST_RESET_CHECK, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_check_IC_state write host no fail! ret=%d\n",1,ret);
	}
	#endif
	return 0;

}
#endif

void bsp_inear_chargein_action(void)
{
	bsp_inear_mutex_take();
	if(!bsp_inear_context.charger_status)
	{
		bsp_inear_context.charger_status = 1;
		bsp_inear_context.ch0 = 0;
		bsp_inear_context.ch1 = 0;
		bsp_inear_context.ch2 = 0;
		//bsp_inear_rtc_gpio_setting(0);
		if(bsp_inear_context.handle_key_timer != NULL)
		{
			xTimerStopFromISR(bsp_inear_context.handle_key_timer,0);
			xTimerStartFromISR(bsp_inear_context.handle_key_timer,0);
		}
		log_hal_msgid_info("[BSP_INEAR]bsp_inear_chargein_action \n",0);
	}
	bsp_inear_mutex_give();

}

void bsp_inear_chargeout_action(void)
{
	bsp_inear_mutex_take();
	if(bsp_inear_context.charger_status)
	{
		bsp_inear_context.charger_status = 0;
		bsp_inear_set_mode(BSP_INEAR_MODE_NORMAL);
		//bsp_inear_set_reg(0x7d,1);
		//bsp_inear_test_recal();
		//bsp_inear_rtc_gpio_setting(1);
		log_hal_msgid_info("[BSP_INEAR]bsp_inear_chargeout_action \n",0);
	}
	bsp_inear_mutex_give();

}

//mode 0:off 1:on
void bsp_inear_enable_lpsd(uint8_t enable)
{
#ifdef	BSP_INEAR_LPSD_ENABLE
	uint8_t send_buf[2];
	int8_t ret = 0;
	uint8_t confirm;
	uint8_t retry_time = 0;
	do
	{
		send_buf[0] = enable;
		ret = bsp_inear_iic_write(ABOV_HOST_LPSD_CHECK, send_buf, 1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_enable_lpsd %d fail ret=%d\n",2,enable,ret);
		}
		bsp_inear_delay_ms(5);

		ret = bsp_inear_iic_read(ABOV_HOST_LPSD_CHECK,&confirm,1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_enable_lpsd  read fail!", 0);
		}
		retry_time ++;
		if(confirm == enable)
			return;
		else
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_enable_lpsd enable fail! enable=%d confirm=%d",2,enable,confirm);
	}while(retry_time < 3);
#endif
}

//0:normal 1:sleep
void bsp_inear_set_mode(uint8_t mode)
{
	uint8_t send_buf[2];
	int8_t ret = 0;
	uint8_t confirm;
	uint8_t retry_time = 0;
	do
	{
		send_buf[0] = mode;
		ret = bsp_inear_iic_write(ABOV_CTRL_CHANNEL_REG, send_buf, 1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_mode %d fail ret=%d\n",2,mode,ret);
		}
		bsp_inear_delay_ms(5);

		ret = bsp_inear_iic_read(ABOV_CTRL_CHANNEL_REG,&confirm,1);
		if(ret != 0)
		{
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_mode  read fail!", 0);
		}
		retry_time ++;
		if(confirm == mode)
			return;
		else
			log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_mode  fail! mode=%d confirm=%d",2,mode,confirm);
	}while(retry_time < 3);

}
void bsp_inear_lidopen_action(void)
{

	bsp_inear_mutex_take();
	if(bsp_inear_context.lid_status)
	{
		bsp_inear_set_mode(BSP_INEAR_MODE_NORMAL);
		//log_hal_msgid_info("[BSP_INEAR]bsp_inear_lidopen_action \n",0);
		bsp_inear_context.lid_status = 0;
		LOG_W(MPLOG,"[BSP_INEAR]bsp_inear_lidopen_action\n");
	}
	bsp_inear_mutex_give();
}

void bsp_inear_lidclose_action(void)
{
	bsp_inear_mutex_take();
	if(!bsp_inear_context.lid_status)
	{
		bsp_inear_set_mode(BSP_INEAR_MODE_SLEEP);
		//log_hal_msgid_info("[BSP_INEAR]bsp_inear_lidclose_action \n",0);
		bsp_inear_context.lid_status = 1;
		LOG_W(MPLOG,"[BSP_INEAR]bsp_inear_lidclose_action\n");
	}
	bsp_inear_mutex_give();
}

void bsp_inear_set_key_press_threshold(uint16_t threshold)
{
	uint8_t send_buf[2];
	int8_t ret = 0;

	log_hal_msgid_info("[BSP_INEAR]bsp_inear_set_key_press_threshold threshold=%d\n",1,threshold);
	send_buf[0] = (threshold >>8)& 0xff;
	ret = bsp_inear_iic_write(ABOV_KEY_PRESS_THR_H, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_key_press_threshold write thresh high fail! ret=%d\n",1,ret);
		return;
	}

	send_buf[0] = threshold & 0xff;
	ret = bsp_inear_iic_write(ABOV_KEY_PRESS_THR_L, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_key_press_threshold write thresh low fail! ret=%d\n",1,ret);
		return;
	}

}

void bsp_inear_set_key_release_threshold(uint16_t threshold)
{
	uint8_t send_buf[2];
	int8_t ret = 0;

	log_hal_msgid_info("[BSP_INEAR]bsp_inear_set_key_release_threshold threshold=%d\n",1,threshold);
	send_buf[0] = (threshold >>8)& 0xff;
	ret = bsp_inear_iic_write(ABOV_KEY_RELEASE_THR_H, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_key_release_threshold write thresh high fail! ret=%d\n",1,ret);
		return;
	}

	send_buf[0] = threshold & 0xff;
	ret = bsp_inear_iic_write(ABOV_KEY_RELEASE_THR_L, send_buf, 1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_set_key_release_threshold write thresh low fail! ret=%d\n",1,ret);
		return;
	}

}

uint16_t bsp_inear_get_key_press_threshold(void)
{
	uint8_t th_h=0,th_l=0;
	int8_t ret = 0;
	uint16_t value = 0;

	ret = bsp_inear_iic_read(ABOV_KEY_PRESS_THR_H,&th_h,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_get_key_press_threshold  high read fail!", 0);
		return 1;
	}
	ret = bsp_inear_iic_read(ABOV_KEY_PRESS_THR_L,&th_l,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_get_key_press_threshold low read fail!", 0);
		return 1;
	}
	value = (th_h<<8)|th_l;
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_get_key_press_threshold threshold=%d\n",1,value);
	return value;

}


uint16_t bsp_inear_get_key_release_threshold(void)
{
	uint8_t th_h=0,th_l=0;
	int8_t ret = 0;
	uint16_t value = 0;

	ret = bsp_inear_iic_read(ABOV_KEY_RELEASE_THR_H,&th_h,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_get_key_release_threshold  high read fail!", 0);
		return 1;
	}
	ret = bsp_inear_iic_read(ABOV_KEY_RELEASE_THR_L,&th_l,1);
	if(ret != 0)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_get_key_release_threshold low read fail!", 0);
		return 1;
	}
	value = (th_h<<8)|th_l;
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_get_key_release_threshold threshold=%d\n",1,value);
	return value;

}


uint32_t random_get_value(uint32_t min, uint32_t max)
{
    uint32_t random_count;

    while (hal_trng_init() != HAL_TRNG_STATUS_OK) {
        vTaskDelay(50);
    }

    do {
        hal_trng_get_generated_random_number(&random_count);
        random_count %= max;
    } while (random_count < min);

    hal_trng_deinit();

    return random_count;
}

#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER

void bsp_inear_false_trigger_timer_handler( TimerHandle_t pxExpiredTimer )
{
	bsp_inear_mutex_take();
	xTimerStopFromISR(bsp_inear_context.false_trigger_timer,0);
	bsp_inear_context.touch_on_timer_count++;
	//LOG_W(MPLOG,"bsp_inear_false_trigger_timer_handler enter count=%d\n",bsp_inear_context.touch_on_timer_count);
	if(bsp_inear_context.istouchon )
	{
		if(bsp_inear_get_key_diff()< ABOV_KEY_FALSE_TRIGGER_PRESS_VALUE)
		{
			//LOG_W(MPLOG,"bsp_inear_false_trigger_timer_handler 1\n");
			bsp_inear_context.is_false_trigger = 1;
			//xTimerStopFromISR(bsp_inear_context.false_trigger_timer,0);
			//bsp_inear_context.istouchon = 0;

		}
		if(bsp_inear_context.touch_on_timer_count < BSP_INEAR_FALSETRIGGER_TIMER_COUNT)
		{
			//LOG_W(MPLOG,"bsp_inear_false_trigger_timer_handler 2\n");
			xTimerStartFromISR(bsp_inear_context.false_trigger_timer,0);
		}
		else
		{
			if(bsp_inear_context.is_false_trigger)
			{
				//LOG_W(MPLOG,"bsp_inear_false_trigger_timer_handler 3\n");
				bsp_inear_send_state(DEVICE_KEY_0,AIRO_KEY_DRIVEN_RELEASE);
				bsp_inear_context.is_false_trigger = 0;
			}
			//LOG_W(MPLOG,"bsp_inear_false_trigger_timer_handler 4\n");
			bsp_inear_context.touch_on_timer_count = 0;
		}

	}
	bsp_inear_mutex_give();
}
#endif

#ifdef BSP_INEAR_KEY_DISCARD_FEATURE
void bsp_inear_key_discard_timer_handler( TimerHandle_t pxExpiredTimer )
{
	bsp_inear_context.key_discard = 0;
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_key_discard_timer_handler \n",0);
}
#endif

void bsp_inear_heart_beat_timer_handler( TimerHandle_t pxExpiredTimer )
{
	bsp_inear_queue_t queue_item;
#ifdef BSP_INEAR_HEARTBEAT_RESET
	uint8_t need_reset;
#endif
	// uint8_t data;
	// uint8_t mode;
	BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
	// uint8_t ver=0x0;
	UNUSED(pxExpiredTimer);
	bsp_inear_mutex_take();
	xTimerStopFromISR(bsp_inear_context.heartbeat_timer,0);
#ifdef BSP_INEAR_HEARTBEAT_RESET
	need_reset = bsp_inear_check_IC_state();
	if(need_reset)
	{
		bsp_inear_reset();
	}
#endif

	//bsp_inear_debug_uart();
#if 0
	bsp_inear_debug();

	if(abov_send_finish == 0)
	{
		abov_start_send_count ++ ;
		if(abov_start_send_count > 200)
		{
			//for(int j = 0;j<ABOV_DEBUG_LEN/16;j++)
			//{
				LOG_W(MPLOG,"%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",abov_test_data[abov_sent_lenth*16+0],
				abov_test_data[abov_sent_lenth*16+1],abov_test_data[abov_sent_lenth*16+2],abov_test_data[abov_sent_lenth*16+3],abov_test_data[abov_sent_lenth*16+4],
				abov_test_data[abov_sent_lenth*16+5],abov_test_data[abov_sent_lenth*16+6],abov_test_data[abov_sent_lenth*16+7],abov_test_data[abov_sent_lenth*16+8],
				abov_test_data[abov_sent_lenth*16+9],abov_test_data[abov_sent_lenth*16+10],abov_test_data[abov_sent_lenth*16+11],abov_test_data[abov_sent_lenth*16+12],
				abov_test_data[abov_sent_lenth*16+13],abov_test_data[abov_sent_lenth*16+14],abov_test_data[abov_sent_lenth*16+15]);
				abov_sent_lenth ++;
			//}
		}
		if(abov_sent_lenth >= ABOV_DEBUG_LEN/16)
			abov_send_finish = 1;
	}
#endif

	//ver = bsp_inear_read_firmware_version();
	//mode = bsp_inear_read_mode();
	//hal_rtc_gpio_get_input(HAL_RTC_GPIO_0, &data);
	//LOG_W(MPLOG,"bsp_inear_heart_beat_timer_handler gpio value =%d \n",data);
	//LOG_W(MPLOG,"bsp_inear_heart_beat_timer_handler ver =%x mode=%x\n",ver,mode);
	//bsp_inear_print_total_cap_online();
	//bsp_inear_print_init_status();
	//bsp_inear_print_baseline();
	//bsp_inear_print_rawdata();
	//bsp_inear_print_preraw();
	bsp_inear_print_diff_online();
	//log_hal_msgid_info("[BSP_INEAR]bsp_inear_heart_beat_timer_handler\n", 0);
	queue_item.msg = BSP_INEAR_QUEUE_MSG_HEARTBEAT_EVENT;
	xQueueSendFromISR(bsp_inear_context.queue, &queue_item, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
	bsp_inear_mutex_give();
}
uint8_t bsp_inear_get_version(void)
{
	uint8_t regaddr = ABOV_VERSION_REG;
	uint8_t read_buf[2]={0};
	//log_hal_msgid_error("[BSP_INEAR] abov_fw_update_check regaddr =%d\n",1,&regaddr);
	bsp_inear_iic_read(regaddr, read_buf, 1);
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_get_version version =%d\n",1,read_buf[0]);
	return read_buf[0];
}

int8_t bsp_inear_hardware_init(void)
{
	int ret = 0;
	ret = bsp_inear_iic_init();
	if(ret)
		goto fail_init;
	//bsp_inear_delay_ms(500);
	//bsp_inear_test_read();
	//bsp_inear_get_inear_status();
	//bsp_inear_get_version();
	//bsp_inear_test_recal();
	bsp_inear_power_on(1);
	#ifndef BSP_INEAR_USE_POLLING
	ret = abov_sar_init();
	if(ret)
		goto fail_init;
	#endif

	bsp_inear_context.handle_key_timer = xTimerCreate( "bsp_inear_handle_key", pdMS_TO_TICKS(BSP_INEAR_HANDLE_KEY_TIME_MS), pdFALSE, NULL, bsp_inear_key_handler);
	if(NULL == bsp_inear_context.handle_key_timer)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_hardware_init handle key timer start fail\n", 0);
		goto fail_init;
	}

	bsp_inear_context.heartbeat_timer = xTimerCreate( "bsp_inear_heartbeat", pdMS_TO_TICKS(BSP_INEAR_HEARTBEAT_TIME_MS), pdFALSE, NULL, bsp_inear_heart_beat_timer_handler);
	if(NULL == bsp_inear_context.heartbeat_timer)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_hardware_init heartbeat timer start fail\n", 0);
		//goto fail_init;
	}

#ifdef BSP_INEAR_KEY_DISCARD_FEATURE
	bsp_inear_context.key_discard_timer = xTimerCreate( "bsp_inear_key_discard", pdMS_TO_TICKS(BSP_INEAR_KEY_DISCARD_TIME_MS), pdFALSE, NULL, bsp_inear_key_discard_timer_handler);
	if(NULL == bsp_inear_context.key_discard_timer)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_hardware_init key discard timer start fail\n", 0);
		//goto fail_init;
	}
#endif

#ifdef	BSP_INEAR_ANTI_FALSE_TRIGGER
	bsp_inear_context.false_trigger_timer= xTimerCreate( "bsp_inear_false_trigger_timer", pdMS_TO_TICKS(BSP_INEAR_FALSETRIGGER_TIMER_MS), pdFALSE, NULL, bsp_inear_false_trigger_timer_handler);
	if(NULL == bsp_inear_context.false_trigger_timer)
	{
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_hardware_init false trigger timer start fail\n", 0);
		//goto fail_init;
	}
#endif
	bsp_inear_mutex_creat();
	bsp_inear_key_mutex_creat();

#ifdef BSP_INEAR_HEARTBEAT_RESET
	bsp_inear_enable_ic_heartbeat(1);
#endif
#ifdef BSP_INEAR_LPSD_ENABLE
	bsp_inear_enable_lpsd(1);
#endif
	bsp_inear_set_key_press_threshold(ABOV_KEY_PRESS_THR_VALUE);
	bsp_inear_set_key_release_threshold(ABOV_KEY_RELEASE_THR_VALUE);
	//xTimerStartFromISR(bsp_inear_context.heartbeat_timer,0);
	bsp_inear_context.is_init = 1;
#ifdef BSP_INEAR_KEY_DISCARD_FEATURE
	bsp_inear_context.key_discard = 1;
#endif
	bsp_inear_rtc_gpio_setting(1);
	return 0;
 fail_init:
 	bsp_inear_power_on(0);
 	log_hal_msgid_error("[BSP_INEAR]bsp_inear_hardware_init fail!!\n",0);
	return 1;


}

static void bsp_inear_loop(void * data)
{
	bsp_inear_queue_t queue_item;
	int8_t ret=0;
	assert(bsp_inear_context.queue);
	log_hal_msgid_info("[BSP_INEAR]bsp_inear_loop enter!", 0);
	while(1) {
 		if(xQueueReceive(bsp_inear_context.queue, &queue_item, portMAX_DELAY)) {
			if(queue_item.msg == BSP_INEAR_QUEUE_MSG_HARDWARE_INIT)
			{
				log_hal_msgid_info("[BSP_INEAR]BSP_INEAR_QUEUE_MSG_HARDWARE_INIT\n", 0);
				ret = bsp_inear_hardware_init();
				if(ret)
				{
					log_hal_msgid_error("[BSP_INEAR]bsp_inear_loop hardware init fail\n", 0);
					break;
				}
				#if 1
				//if(!(0x04 & pmu_get_power_on_reason()))
				{
					bsp_inear_mutex_take();
					xTimerStopFromISR(bsp_inear_context.handle_key_timer,0);
					xTimerStartFromISR(bsp_inear_context.handle_key_timer,0);
					bsp_inear_mutex_give();
				}
				#endif
			}
			#if 0
			else if(queue_item.msg == BSP_INEAR_QUEUE_MSG_TOUCH_EVENT )//&& bsp_inear_context.charger_status == 0 )
			{
				log_hal_msgid_info("[BSP_INEAR]BSP_INEAR_QUEUE_MSG_TOUCH_EVENT\n", 0);
				bsp_inear_mutex_take();
				xTimerStopFromISR(bsp_inear_context.handle_key_timer,0);
				xTimerStartFromISR(bsp_inear_context.handle_key_timer,0);
				bsp_inear_mutex_give();
			}
			#endif
			else if(queue_item.msg == BSP_INEAR_QUEUE_MSG_HEARTBEAT_EVENT)
			{
				bsp_inear_mutex_take();
				xTimerStopFromISR(bsp_inear_context.heartbeat_timer,0);
				xTimerStartFromISR(bsp_inear_context.heartbeat_timer,0);
				bsp_inear_mutex_give();
			}

		}

	}
	log_hal_msgid_error("[BSP_INEAR]bsp_inear_loop exit!", 0);
	vTaskSuspend(bsp_inear_context.thread);
}

void bsp_inear_init(void)
{
	int ret;
	bsp_inear_queue_t queue_item;
	BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
	if(bsp_inear_context.is_init)
		return ;
	memset(&bsp_inear_context,  0,  sizeof(bsp_inear_context_t));
	bsp_inear_context.iic_cb_event = HAL_I2C_NON_BLOCKING_FUNCTION_ERROR;
	bsp_inear_context.queue = xQueueCreate(BSP_INEAR_QUEUE_LENGTH, sizeof(bsp_inear_queue_t));
	if (NULL == bsp_inear_context.queue) {
		log_hal_msgid_error("[BSP_INEAR]Failed to create bsp inear queue!", 0);
		return ;
	}
	ret = xTaskCreate(bsp_inear_loop,
                        "bsp_inear",
                        2048 / sizeof(StackType_t),
                        NULL,
                        TASK_PRIORITY_NORMAL,
                        &bsp_inear_context.thread);

	if( ret != pdPASS ){
		log_hal_msgid_error("[BSP_INEAR]bsp_inear_init create task fail,ret =%d",1,ret);
		return ;
	}
	queue_item.msg = BSP_INEAR_QUEUE_MSG_HARDWARE_INIT;
	xQueueSendFromISR(bsp_inear_context.queue, &queue_item, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


//---------------------------fw update function start-----------------------------------
uint32_t abov_fw_bootmode_enter(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_bootmode_enter\n",0);
	uint32_t retcode;
	uint8_t retry_count = 10;
    uint8_t regaddr = 0;
	uint8_t send_buf[2];

retry:
    send_buf[0] = 0xF0;
	send_buf[1] = 0xAA;
	retcode = bsp_inear_iic_write_flash(regaddr, send_buf, 2);
	if (retcode != 0)
	{
	   if (retry_count-- > 0)
	   {
			goto retry;
	   }
	   return 1;
	}

	return retcode;
}


uint32_t abov_fw_downloadermode_enter(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_downloadermode_enter\n",0);
	uint32_t retcode;
    uint8_t regaddr = 0;
	uint8_t send_buf[2];
	uint8_t read_buf[2];


	bsp_inear_power_on_to_boot();

	regaddr = 0xAC;
	send_buf[0] = 0x5B;

	retcode = bsp_inear_iic_write(regaddr, send_buf, 1);

	if (retcode != 0)
	{
		return 1;
	}

	bsp_inear_delay_ms(5); // 5ms

    regaddr = 0x00;
    retcode = bsp_inear_iic_read_no_reg(regaddr, read_buf, 1);
#ifdef BSP_INEAR_USE_A96T349DF
	if ((read_buf[0] == 0x39)|| (read_buf[0] == 0x3D))
#else
	if ((read_buf[0] == 0x50))
#endif
	{
		return 0;
	}
	log_hal_msgid_info("[BSP_INEAR]allen:abov_fw_downloadermode_enter fail\n",0);
	return 1;
}

uint32_t abov_fw_flash_erase(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_flash_erase\n",0);
	uint32_t retcode;
    uint8_t regaddr = 0;
	uint8_t send_buf[2];
    send_buf[0] = 0xAC;
	send_buf[1] = 0x2D;

    retcode = bsp_inear_iic_write_flash(regaddr, send_buf, 2);

	return retcode;
}


uint32_t abov_fw_flash_write(uint8_t *addrH, uint8_t *addrL, uint8_t *val)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_flash_write,addr:0x%0x,%0x\n",2,*addrH,*addrL);
    uint8_t length = 36;
	uint32_t retcode;
    uint8_t regaddr = 0;
	uint8_t send_buf[36];

    send_buf[0] = 0xAC;
	send_buf[1] = 0x7A;
	memcpy(&send_buf[2], addrH, 1);
	memcpy(&send_buf[3], addrL, 1);
	memcpy(&send_buf[4], val, 32);

    retcode = bsp_inear_iic_write_flash(regaddr, send_buf, length);
	if (retcode != 0)
	{
		return 1;
	}
	bsp_inear_delay_ms(3); // 3ms

	return retcode;
}


uint32_t abov_fw_checksum_verify(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_checksum_verify\n",0);
	uint32_t retcode;
    uint8_t regaddr = 0;
	uint8_t send_buf[6];
	uint8_t read_buf[6];

	bsp_inear_context.checksum_h = 0x00;
	bsp_inear_context.checksum_l = 0x00;
	send_buf[0] = 0xAC;
	send_buf[1] = 0x9E;
	send_buf[2] = 0x00;
	send_buf[3] = 0x00;
	send_buf[4] = bsp_inear_context.checksum_h_bin;
	send_buf[5] = bsp_inear_context.checksum_l_bin;

	retcode = bsp_inear_iic_write_flash(regaddr, send_buf, 6);
	if (retcode != 0)
	{
		return 1;
	}
	bsp_inear_delay_ms(5); // 5ms

	regaddr = 0x00;
	send_buf[0] = 0x00;

	retcode = bsp_inear_iic_write_flash(regaddr, send_buf, 1);
	if (retcode != 0)
	{
	    log_hal_msgid_info("[BSP_INEAR]allen:================================================\n",0);
		return 1;
	}
	bsp_inear_delay_ms(5); // 5ms

    regaddr = 0x00;
    retcode = bsp_inear_iic_read(regaddr, read_buf, 6);
	if (retcode != 0)
	{
		return 1;
	}

	bsp_inear_context.checksum_h = read_buf[4];
	bsp_inear_context.checksum_l = read_buf[5];
	for(int i=0;i<6;i++)
	{
	    log_hal_msgid_info("[BSP_INEAR]allen:read_buf[%d]=0x%0x\n",2,i,read_buf[i]);
	}

	return retcode;
}

uint32_t abov_fw_downloadermode_exit(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_downloadermode_exit\n",0);
	uint32_t retcode;
    uint8_t regaddr = 0;
	uint8_t send_buf[2];
    send_buf[0] = 0xAC;
	send_buf[1] = 0xE1;

    retcode = bsp_inear_iic_write_flash(regaddr, send_buf, 2);
    log_hal_msgid_info("[BSP_INEAR]allen:==================retcode = 0x%0x\n",1,retcode);
	return retcode;
}

uint32_t abov_fw_update(const uint8_t *image, uint32_t size)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_update\n",0);
    uint32_t retcode;
	uint32_t ii = 0;
	uint32_t count;
	uint32_t address;
	uint8_t addrH, addrL;
	uint8_t data[32] = {0, };

#if 0
	if (abov_fw_bootmode_enter() != 0)
	{
		return 1;
	}

	bsp_inear_delay_ms(45); // 45ms
#endif
	for (ii = 0; ii < ABOV_FW_UPDATA_TRIAL; ii++)
	{
		if (abov_fw_downloadermode_enter() != 0)
		{
			bsp_inear_delay_ms(40); // 40ms
			continue;
		}
		break;
	}

	if (ABOV_FW_UPDATA_TRIAL <= ii) {
		return 1;
	}

	if (abov_fw_flash_erase() != 0)
	{
		return 1;
	}

	bsp_inear_delay_ms(1400); // 1400ms

	address = 0x800;
	count = size / 32;
	log_hal_msgid_info("[BSP_INEAR]allen:count======0x%0x\n",1,count);

	for (ii = 0; ii < count; ii++)
	{
		/* first 32byte is header */
		addrH = (uint8_t)((address >> 8) & 0xFF);
		addrL = (uint8_t)(address & 0xFF);
		memcpy(data, &image[ii * 32], 32);
		retcode = abov_fw_flash_write(&addrH, &addrL, data);
		if (retcode != 0)
		{
		    log_hal_msgid_info("[BSP_INEAR]allen:++++++++++++++++++++++++++++++++++++\n",0);
			return 1;
		}
		address += 0x20;
		memset(data, 0, 32);
		//bsp_inear_delay_ms(5);
	}

	retcode = abov_fw_checksum_verify();
	retcode = abov_fw_downloadermode_exit();//retcode==0,ok
	log_hal_msgid_info("[BSP_INEAR]allen:checksum_h=0x%0x,checksum_l=0x%0x-----------------\n",2,bsp_inear_context.checksum_h,bsp_inear_context.checksum_l);
	log_hal_msgid_info("[BSP_INEAR]allen:checksum_h_bin=0x%0x,checksum_l_bin=0x%0x-----------------\n",2,bsp_inear_context.checksum_h_bin,bsp_inear_context.checksum_l_bin);
	if ((bsp_inear_context.checksum_h == bsp_inear_context.checksum_h_bin)
		&& (bsp_inear_context.checksum_l == bsp_inear_context.checksum_l_bin))
	{
       retcode = 0;
	}
	else
	{
	   retcode = 1;
	}
    log_hal_msgid_info("[BSP_INEAR]allen:retcode=0x%0x-----------------\n",1,retcode);
	bsp_inear_delay_ms(100); // 100ms

	return retcode;
}

#if 1

uint8_t abov_checksum_recalc_verify(uint8_t sum_l,uint8_t sum_h)
{
	uint8_t regaddr = 0;
	uint32_t retcode;
	uint8_t send_buf[2];
	uint8_t check_sum_l,check_sum_h;

	regaddr = ABOV_RECALC_REG;
	send_buf[0] = 0x10;
	retcode = bsp_inear_iic_write(regaddr, send_buf, 1);
	if(retcode != 0)
		return 1;

	bsp_inear_delay_ms(100);

	regaddr = ABOV_CHECKSUM_H;
	retcode = bsp_inear_iic_read(regaddr, &check_sum_h, 1);
	if(retcode != 0)
		return 1;

	regaddr = ABOV_CHECKSUM_L;
	retcode = bsp_inear_iic_read(regaddr, &check_sum_l, 1);
	if(retcode != 0)
		return 1;

	log_hal_msgid_info("[BSP_INEAR]sum_h:%x check_sum_h:%x\n",2,sum_h,check_sum_h);
	log_hal_msgid_info("[BSP_INEAR]sum_l:%x check_sum_l:%x\n",2,sum_l,check_sum_l);
	if(sum_h == check_sum_h && sum_l == check_sum_l)
		return 0;

	return 1;

}

uint32_t abov_fw_update_check(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_fw_update_check\n",0);
	uint32_t retcode;
	uint8_t regaddr = 0;
	uint8_t update_loop;
	uint8_t fw_upgrade = 0;
	uint8_t fw_version = 0;
	uint8_t fw_file_version = 0;
	uint8_t fw_modelno = 0;
	uint8_t fw_file_modeno = 0;
	uint8_t read_buf[2];

    regaddr = ABOV_VERSION_REG;
	//log_hal_msgid_error("[BSP_INEAR] abov_fw_update_check regaddr =%d\n",1,&regaddr);
	retcode = bsp_inear_iic_read(regaddr, read_buf, 1);
	fw_version = read_buf[0];

	regaddr = ABOV_MODELNO_REG;
	retcode = bsp_inear_iic_read(regaddr, read_buf, 1);
	fw_modelno = read_buf[0];

	fw_file_modeno = abovFW[1];
	fw_file_version = abovFW[5];
	bsp_inear_context.checksum_h_bin = abovFW[8];
	bsp_inear_context.checksum_l_bin = abovFW[9];

	log_hal_msgid_info("[BSP_INEAR]fw_version:%x fw_file_version:%x\n",2,fw_version,fw_file_version);
	log_hal_msgid_info("[BSP_INEAR]fw_modelno:%x fw_file_modeno:%x\n",2,fw_modelno,fw_file_modeno);

	if ((fw_version != fw_file_version) || (fw_modelno != fw_modelno))
	{
	  fw_upgrade = 1;
	}
	else
	{
		fw_upgrade = 0;
		retcode = 0;
	}
	if(!fw_upgrade)
	{
		retcode = abov_checksum_recalc_verify(bsp_inear_context.checksum_l_bin,bsp_inear_context.checksum_h_bin);
		if(retcode)
			fw_upgrade = 1;
	}
	//fw_upgrade = 1;
	if (fw_upgrade)
	{
		for (update_loop = 0; update_loop < ABOV_FW_UPDATA_TRIAL; update_loop++)
		{
			retcode = abov_fw_update(&abovFW[32], ABOV_FW_SIZE-32);
			if (retcode == 0)
			{
				break;
			}
			bsp_inear_delay_ms(400); // 400ms
		}
		if (update_loop >= ABOV_FW_UPDATA_TRIAL)
		{
			retcode = 1;
		}
	}

	return retcode;
}

//---------------------------fw update function end-----------------------------------

uint8_t abov_sar_init(void)
{
	log_hal_msgid_info("[BSP_INEAR]allen:enter abov_sar_init\n",0);
 	uint32_t retcode;
	//uint8_t regaddr;
	//uint8_t regvalue[2];

    //bsp_inear_delay_ms(100); // 100ms  make sure sensor run, need delay 100ms after sensor get power
	retcode = abov_fw_update_check();
	if(retcode)
	{
		log_hal_msgid_info("[BSP_INEAR]allen: abov_fw_update_check failed, retcode=%d\n",1, retcode);
		bsp_inear_power_on(0);
		return 1 ;
	}
	//bsp_inear_delay_ms(300); // 300ms   make sure calibration completed
	return 0 ;
/*
    regaddr = ABOV_CTRL_MODE_REG;
	regvalue[0] = 0x00;
    retcode = bsp_inear_iic_write(regaddr,regvalue, 1);
	if (retcode != 0)
	{
		log_hal_msgid_info("[BSP_INEAR][%s] add ABOV_CTRL_MODE_REG success! retcode=%d",2,__func__,retcode);
		return ;
	}

	regaddr = ABOV_CTRL_CHANNEL_REG;
	regvalue[0] = 0x07;
    retcode = bsp_inear_iic_write(regaddr, regvalue, 1);
	if (retcode != 0)
	{
		log_hal_msgid_info("[BSP_INEAR][%s] add ABOV_CTRL_CHANNEL_REG success! retcode=%d",2,__func__,retcode);
		return ;
	}

	regaddr = ABOV_RECALI_REG;
	regvalue[0] = 0x01;
    retcode = bsp_inear_iic_write(regaddr, regvalue, 1);
	if (retcode != 0)
	{
		log_hal_msgid_info("[BSP_INEAR][%s] add ABOV_RECALI_REG success! retcode=%d",2,__func__,retcode);
		return ;
	}
*/
}
#endif
void abov_enter_factory_test(uint8_t enter)
{
	if(enter)
		bsp_inear_context.factory_test = 1;
	else
		bsp_inear_context.factory_test = 0;
}

int8_t abov_read_reg(abov_factory_reg_type reg_type, uint16_t *data)
{
	int8_t ret = 0;
	uint8_t status = 0;
	uint16_t read_data = 0;
	uint8_t reg_l = 0;
	uint8_t reg_h = 0;
	uint8_t send_buf[2];

	if(bsp_inear_context.is_init == 0)
	{

		ret = bsp_inear_hardware_init();
		if(ret != 0)
			return -2;
	}
	LOG_W(MPLOG,"abov_read_reg %d\n",reg_type);
	//log_hal_msgid_info("[BSP_INEAR]abov_read_reg %d\n",1,reg_type);
	bsp_inear_mutex_take();
#if 0
	if(reg_type == ABOV_VERSION)
	{
		ret = bsp_inear_iic_read(ABOV_MODELNO_REG, &status, 1);
		if(ret != 0)
		{
			printf("[BSP_InEar_Abov]read_abov_reg read reg_h error\n");
			return -1;
		}
		*data = status;
		return 0;
	}
#endif

	switch(reg_type)
	{
		case CH0_DIFF:
			reg_h = 0x30;
			reg_l = 0x31;
			break;

		case CH1_DIFF:
			reg_h = 0x32;
			reg_l = 0x33;
			break;

		case CH2_DIFF:
			reg_h = 0x34;
			reg_l = 0x35;
			break;

		case CH0_CAPA:
			reg_h = 0x40;
			reg_l = 0x41;
			break;

		case CH1_CAPA:
			reg_h = 0x42;
			reg_l = 0x43;
			break;

		case CH2_CAPA:
			reg_h = 0x44;
			reg_l = 0x45;
			break;

		case REF_CAPA:
			reg_h = 0x48;
			reg_l = 0x49;
			break;

		case ABOV_VERSION:
			ret = bsp_inear_iic_read(ABOV_VERSION_REG, &status, 1);
			if(ret != 0)
			{
				printf("[BSP_INEAR]read_abov_reg read reg_h error\n");
				bsp_inear_mutex_give();
				return -1;
			}
			*data = status;
			bsp_inear_mutex_give();
			return 0;

		case ABOV_RE_CAL:
			send_buf[0] = 1;
			ret = bsp_inear_iic_write(ABOV_RECALI_REG, send_buf, 1);
			if(ret != 0)
			{
				printf("[BSP_INEAR]read_abov_reg RE CAL error\n");
				bsp_inear_mutex_give();
				return -1;
			}
			bsp_inear_mutex_give();
			return 0;


		default:
			printf("[BSP_INEAR]read_abov_reg  parameter error\n");
			bsp_inear_mutex_give();
			return -1;

	}
	ret = bsp_inear_iic_read(reg_h,&status,1);
	if(ret != 0)
	{
		printf("[BSP_InEar_Abov]read_abov_reg read reg_h error\n");
		bsp_inear_mutex_give();
		return -1;
	}
	read_data= status*256;
	ret = bsp_inear_iic_read(reg_l,&status,1);
	if(ret != 0)
	{
		printf("[BSP_InEar_Abov]read_abov_reg read reg_l error\n");
		bsp_inear_mutex_give();
		return -1;
	}
	read_data +=status;

	*data = read_data;
	bsp_inear_mutex_give();
	return 0;
}

#endif
