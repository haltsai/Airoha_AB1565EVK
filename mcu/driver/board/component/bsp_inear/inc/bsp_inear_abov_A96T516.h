#ifndef _BSP_INEAR_ABOV_A96T516_H_
#define _BSP_INEAR_ABOV_A96T516_H_
#include "hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#ifdef MTK_IN_EAR_FEATURE_ENABLE

#ifndef UNUSED
  #define UNUSED(x)  ((void)(x))
#endif

#define BSP_INEAR_USE_A96T349DF 
//#define BSP_INEAR_HEARTBEAT_RESET
//#define BSP_INEAR_ANTI_FALSE_TRIGGER
//#define BSP_INEAR_LPSD_ENABLE
// #define BSP_INEAR_KEY_RELATED
// #define BSP_INEAR_KEY_DISCARD_FEATURE

#define BSP_INEAR_I2C_DMA_WAIT_TIME 4 
#define BSP_INEAR_I2C_DMA_TRIAL  10

#define BSP_INEAR_ABOV_MAGIC_NO  0xBA
#define BSP_INEAR_HOST_MAGIC_NO  0xAB

#define BSP_INEAR_MODE_SLEEP     0x01
#define BSP_INEAR_MODE_NORMAL     0x00

#define ABOV_SLAVE_ADDR          0x20
#define ABOV_VERSION_REG         0x02
#define ABOV_MODELNO_REG         0x03
#define ABOV_CTRL_MODE_REG       0x07
#define ABOV_CTRL_CHANNEL_REG    0x08
#define ABOV_RECALI_REG          0xFB
#ifdef BSP_INEAR_USE_A96T349DF
#define ABOV_RECALC_REG			 0x71
#define ABOV_CHECKSUM_H			 0x72
#define ABOV_CHECKSUM_L			 0x73
#define ABOV_HOST_RESET_CHECK_EN 0x7A
#define ABOV_HOST_RESET_CHECK    0x7B
#define ABOV_HOST_LPSD_CHECK     0x7C
#define ABOV_KEY_PRESS_THR_H     0x14
#define ABOV_KEY_PRESS_THR_L     0x15

#define ABOV_KEY_RELEASE_THR_H   0x1C
#define ABOV_KEY_RELEASE_THR_L   0x1D

#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
#define ABOV_KEY_PRESS_THR_VALUE     350
#define ABOV_KEY_RELEASE_THR_VALUE   300
#define ABOV_KEY_FALSE_TRIGGER_PRESS_VALUE 600
#else
#define ABOV_KEY_PRESS_THR_VALUE     600
#define ABOV_KEY_RELEASE_THR_VALUE   500  
#endif

#else
#define ABOV_RECALC_REG			 0x6C
#define ABOV_CHECKSUM_H			 0x6D
#define ABOV_CHECKSUM_L			 0x6E
#endif
#define ABOV_FW_UPDATA_TRIAL     3

#define ABOV_FW_SIZE  14368
#define BSP_INEAR_QUEUE_LENGTH 10

#define REG_keyValue 0x00
#define BSP_INEAR_INIT_TIME_MS 5
#define BSP_INEAR_HANDLE_KEY_TIME_MS 1
#define BSP_INEAR_HEARTBEAT_TIME_MS 100
#define BSP_INEAR_KEY_DISCARD_TIME_MS 500


//#define BSP_INEAR_USE_POLLING
#define BSP_INEAR_QUEUE_MSG_HARDWARE_INIT    0x00000001
#define BSP_INEAR_QUEUE_MSG_TOUCH_EVENT    0x00000002
#define BSP_INEAR_QUEUE_MSG_HEARTBEAT_EVENT    0x00000003
#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
#define BSP_INEAR_QUEUE_MSG_ANTI_FALSE_TOUCH_EVENT    0x00000004
#define BSP_INEAR_FALSETRIGGER_TIMER_MS 50
#define BSP_INEAR_FALSETRIGGER_TIMER_COUNT 4
#define BSP_INEAR_FALSETRIGGER_KEY_RELEASE_TIME 200
#endif

typedef struct
{
	uint8_t checksum_h;
	uint8_t checksum_h_bin;
	uint8_t checksum_l;
	uint8_t checksum_l_bin;
	uint8_t dma_write_flag;
	uint8_t dma_read_flag;
	uint8_t call_back_count;
	hal_i2c_callback_event_t iic_cb_event;
	QueueHandle_t queue ;
	TaskHandle_t thread;
	uint8_t ch0;
	uint8_t ch1;
	uint8_t ch2;
	uint8_t inear_state;
	uint8_t key_state;
	TimerHandle_t heartbeat_timer;
	TimerHandle_t handle_key_timer;
#ifdef BSP_INEAR_KEY_DISCARD_FEATURE	
	uint8_t key_discard;
	TimerHandle_t key_discard_timer;
#endif
#ifdef BSP_INEAR_ANTI_FALSE_TRIGGER
	TimerHandle_t false_trigger_timer;
	uint8_t istouchon;
	uint8_t touch_on_timer_count;
	uint8_t is_false_trigger;
#endif	
	uint8_t is_init;
	uint8_t factory_test;
#ifdef BSP_INEAR_HEARTBEAT_RESET
	uint8_t heartbeat_enabled;
#endif
	uint8_t charger_status;
	uint8_t inear_status;
	uint8_t lid_status;
	SemaphoreHandle_t mutex;
	SemaphoreHandle_t key_mutex;
} bsp_inear_context_t;

typedef enum
{
	CH0_DIFF=1,
	CH1_DIFF,
	CH2_DIFF,
	CH0_CAPA,
	CH1_CAPA,
	CH2_CAPA,
	REF_CAPA,
	ABOV_VERSION,
	ABOV_RE_CAL,
}abov_factory_reg_type;

typedef struct {
    uint32_t msg;
    const void * pdata;
} bsp_inear_queue_t;
uint8_t abov_sar_init(void);
//void abov_sar_init(void);
void bsp_inear_init(void);
int8_t abov_read_reg(abov_factory_reg_type reg_type, uint16_t *data);
void abov_enter_factory_test(uint8_t enter);
void bsp_inear_chargein_action(void);
void bsp_inear_chargeout_action(void);
void bsp_inear_lidopen_action(void);
void bsp_inear_lidclose_action(void);
void bsp_inear_enable_ic_heartbeat(uint8_t enable);
uint8_t bsp_inear_get_inear_state(void);
#endif
#endif
