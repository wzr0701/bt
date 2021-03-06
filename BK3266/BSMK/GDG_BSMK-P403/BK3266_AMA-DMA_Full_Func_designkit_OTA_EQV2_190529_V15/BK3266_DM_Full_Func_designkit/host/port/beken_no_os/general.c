/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "includes.h"
#include "bk3000_reg.h"
#include "tra_hcit.h"
#include "tc_const.h"
#include "app_beken_includes.h"
#if (CONFIG_DRC == 1)
#include "app_drc.h"
#endif
#ifdef CONFIG_PRODUCT_TEST_INF
#include "driver_icu.h"
#endif

#if PTS_TESTING
#include "pts/pts.h"
#endif


#if (USER_EX_EQ == 1)
#define TX_FIFO_THRD                0x80
#define RX_FIFO_THRD                0x80
#else
#define TX_FIFO_THRD                0x40
#define RX_FIFO_THRD                0x40
#endif



#if (DEBUG_BASEBAND_MONITORS == 1)
extern void TC_Read_Local_Baseband_Monitors_Via_App(void);
extern void LSLCstat_Reset_Monitors();
#endif
#if (DEBUG_SCATTERNET_MONITORS == 1)
extern void TC_Read_Local_LMPconfig_Monitors_Via_App(void);
#endif

extern void TRAhcit_Rx_Char(u_int8 ch);
extern void juart_receive(u_int8 *buf, u_int16 size);
#ifdef BEKEN_DEBUG
extern void app_env_dump(void);
#endif
extern void app_print_linkkey(void);
extern u_int32 XVR_analog_reg_save[16];

// new_test
extern void app_bt_enable_dut_mode( int8 enable);
extern void app_bt_enable_fcc_mode( int8 mode,uint8 chnl);

unsigned char uart_rx_buf[RX_FIFO_THRD]={0};
unsigned char uart_tx_buf[TX_FIFO_THRD]={0};
volatile boolean uart_rx_done = FALSE;
volatile unsigned int uart_rx_index = 0;
HCI_COMMAND_PACKET *pHCIrxBuf = (HCI_COMMAND_PACKET *)(&uart_rx_buf[0]);
HCI_EVENT_PACKET   *pHCItxBuf = (HCI_EVENT_PACKET *)(&uart_tx_buf[0]);
#ifdef	CONFIG_BLUETOOTH_AVDTP_SCMS_T
extern void security_control_cp_support_print(void);  //print the array's param for check
#endif

enum
{
    DBG_HCI_STATE_RX_TYPE = 0,
    DBG_HCI_STATE_RX_COMMAND_OPCODE1,
    DBG_HCI_STATE_RX_COMMAND_OPCODE2,
    DBG_HCI_STATE_RX_COMMAND_LENGTH,
    DBG_HCI_STATE_RX_DATA_START,
    DBG_HCI_STATE_RX_DATA_CONTINUE,
    DBG_HCI_STATE_RX_DATA_COMMIT,
	DBG_HCI_STATE_RX_DATA_DIRECT, 
};
//u_int8 dbg_hci_rx_buf[255] = {0};
static u_int8 s_dbg_hci_rx_state;
static volatile u_int8* s_dbg_hci_rx_pdu_buf = (u_int8 *)&uart_rx_buf[4];
static volatile u_int16 s_dbg_hci_rx_pdu_length;
static u_int8 s_dbg_hci_rx_length;
u_int8* s_dbg_hci_rx_head_buf = (u_int8 *) &uart_rx_buf[0];

extern void show_bt_stack_status(void);
#ifdef BEKEN_OTA
extern int app_button_ota_enable_action( void );
#endif

/*
 * uart_initialise
 *
 * This function initialises the UART registers & UART driver paramaters.
 */
void uart_initialise(u_int32 baud_rate) {
    u_int32 baud_divisor;

    baud_divisor           = UART_CLOCK_FREQ_26M/baud_rate;
    baud_divisor           = baud_divisor-1;

    REG_APB3_UART_CFG      = (   (DEF_STOP_BIT    << sft_UART_CONF_STOP_LEN)
                                 | (DEF_PARITY_MODE << sft_UART_CONF_PAR_MODE)
                                 | (DEF_PARITY_EN   << sft_UART_CONF_PAR_EN)
                                 | (DEF_DATA_LEN    << sft_UART_CONF_UART_LEN)
                                 | (baud_divisor    << sft_UART_CONF_CLK_DIVID)
                                 | (DEF_IRDA_MODE   << sft_UART_CONF_IRDA)
                                 | (DEF_RX_EN       << sft_UART_CONF_RX_ENABLE)
                                 | (DEF_TX_EN       << sft_UART_CONF_TX_ENABLE));

    REG_APB3_UART_FIFO_THRESHOLD = ((RX_FIFO_THRD << sft_UART_FIFO_CONF_RX_FIFO)
                                    | (TX_FIFO_THRD << sft_UART_FIFO_CONF_TX_FIFO));
    REG_APB3_UART_INT_ENABLE=0;             /* Disable UART Interrupts */
    REG_APB3_UART_INT_ENABLE = bit_UART_INT_RX_NEED_READ | bit_UART_INT_RX_STOP_END; //enable Rx interrupt
    BK3000_GPIO_0_CONFIG = 0x70;
    BK3000_GPIO_1_CONFIG = 0x7C;
}

void uart_send (unsigned char *buff, unsigned int len) {
    
#ifdef CONFIG_APP_DATA_CAPTURE

	if( !app_aec_get_data_capture_mode() ) {
		while (len--) {
			while(!UART_TX_WRITE_READY);
                UART_WRITE_BYTE(*buff++);
		}
	}
    
#else

    while (len--) {
        while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(*buff++);
    }
    
#endif

}

#if (CONFIG_DEBUG_PCM_TO_UART == 1)
void uart_send_ppp(unsigned char *buff, unsigned char fid,unsigned short len)
{
    unsigned char xsum = 0;
    unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.

    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(0x7E);
    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(fid);
    xsum ^= fid;
    while (len--)
    {
        xsum ^= *buff;
        if(0x7E == *buff)
        {   while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x7D);
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x5e);
        }
        else if(0x7D == *buff)
        {
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x7D);
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x5D);
        }
        else
        {
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(*buff);
        }
        buff++;
    }
    if(0x7E == xsum)
    {   while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x7D);
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x5e);
    }
    else if(0x7D == xsum)
    {
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x7D);
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x5D);
    }
    else
    {
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(xsum);
    }
    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(0x7E);
    set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.

}
#endif
#define PRINT_BUF_PREPARE(rc, buf, fmt)             \
    int rc;                                         \
    va_list args;                                   \
    va_start(args, fmt);                            \
    rc = vsnprintf(buf, sizeof(buf), fmt, args);    \
    va_end(args);                                   \
    buf[sizeof(buf) - 1] = '\0';
#define PRINT_BUF_SIZE 0X100
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
static int32 os_printf_cnt = 0;
int32 os_printf(const char *fmt, ...) {
    char buf[PRINT_BUF_SIZE];

    unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.
#if 1
    if(os_printf_cnt < 1)
    {
        PRINT_BUF_PREPARE(rc, buf, fmt);
        uart_send((unsigned char *)&buf[0], rc);
    }
    else
    {
        delay_us(1);
    }
    os_printf_cnt++;
#endif
    set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return 0;
}
#else
int32 os_printf(const char *fmt, ...) {
#if (BT_HOST_MODE == JUNGO_HOST)
    char buf[PRINT_BUF_SIZE];

    //unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    //set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.

    PRINT_BUF_PREPARE(rc, buf, fmt);
#if (CONFIG_XIAONIU_DEVICE_TEST == 1)
	if (flag_os_printf_disable == 0)
#endif
    uart_send((unsigned char *)&buf[0], rc);

    //set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return rc;
 #else
    //unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    //set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.
    delay_us(10);
    //set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return 0;
 #endif
}
#endif
int32 os_null_printf(const char *fmt, ...) {
    return 0;
}

void clear_uart_buffer(void) {
    uart_rx_index = 0;
    uart_rx_done = FALSE;
    memset(uart_rx_buf, 0, sizeof(uart_rx_buf)); /**< Clear the RX buffer */
    memset(uart_tx_buf, 0, sizeof(uart_tx_buf)); /**< Clear the TX buffer */
}

void Beken_Uart_Tx(void) {
    unsigned int tx_len       = HCI_EVENT_HEAD_LENGTH+pHCItxBuf->total;
    pHCItxBuf->code  = TRA_HCIT_EVENT;
    pHCItxBuf->event = HCI_COMMAND_COMPLETE_EVENT;
    uart_send(uart_tx_buf, tx_len);
}

static void app_debug_showstack(void) {
    extern uint32 _sbss_end;
    extern uint32 _stack;
    uint32 count;
    uint32 *ptr;
    uint32 i;

    count = (((uint32)&_stack - (uint32)&_sbss_end) >> 2) - 2;
    ptr = (uint32 *)((uint32)&_sbss_end  & (~3));

    os_printf("ShowStack:%p:%p\r\n",  &_sbss_end, &_stack);
    for(i = 0; i < count; i ++)
        os_printf("0x%x:%p\r\n", &ptr[i], ptr[i]);
}

#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
extern void _LSLCacc_Read_AGC_Param(void);
#endif
extern void hfp_app_ptr_debug_printf(void);

extern int hf_sco_handle_process(int oper);
#define HF_SCO_CONN 0
#define HF_SCO_DISCONN 1

//extern void app_bt_sdp_connect(void);
extern void app_led_dump(void);
#ifdef CONFIG_APP_EQUANLIZER
extern void app_set_eq_gain(int8 gain);
#endif
extern void set_sdp_browse_on(void);
extern result_t sdp_send_serviceSearchRequest(void);
extern void app_bt_sdp_send_serviceAttributeRequest(void);
extern uint32 XVR_reg_0x24_save;



#if (USER_EX_EQ == 1)

CONST int dspRamcoef_44100[153] =
{

	0x00000001,	0x00000000,	0x00732ae1,	0x08000000,	0x08000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x0000a230,	0x00014461,	0x0000a230,	0x0f98d7b0,	0xf8649f80,
	0x07cd0e10,	0xf065e3e0,	0x07cd0e10,	0x0f98d7b0,	0xf8649f80,
	0x00000084,	0x08000000,	0x00813855,	0x00732ae1,	0x0000006e,
	0x07ac3770,	0x07ffa0e8,	0x011c8654,	0x00000084,	0x08000000,
	0x00813855,	0x00732ae1,	0x0000006e,	0x07ac3770,	0x07ffa0e8,
	0x011c8654,	0x08000000,	0x00006851,	0x0000d0a2,	0x00006851,
	0x0fad77d0,	0xf850e6f0,	0x07d72438,	0xf051b790,	0x07d72438,
	0x0fad77d0,	0xf850e6f0,	0x01b49f28,	0x03693e50,	0x01b49f28,
	0x02b68a10,	0xfe76f956,	0x030fe42c,	0xf9e037a8,	0x030fe42c,
	0x02b68a10,	0xfe76f956,	0x08000000,	0x00000000,	0x00000000,
	0x00000000,	0x00000000,	0x08000000,	0x00000000,	0x00000000,
	0x00000000,	0x00000000,	0x0144960c,	0x2cfcc000,	0x08000000,
	0x07cd0e10,	0xf065e3e0,	0x07cd0e10,	0x0f98d7c0,	0xf8649f88,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x08000000,	0x00000000,	0x00000000,	0x00000000,	0x00000000,
	0x00000084,	0x08000000,	0x00813855,	0x00732ae1,	0x0000006e,
	0x07ac3770,	0x07ffa0e8,	0x011c8654,

};



void user_tunningApi(uint8_t *d)
{
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);

	uint8 index;
	uint32 rx_addr;
	uint32 rx_leng;

	if (!flag_L_is_LplusR)
	{
		memcpy((uint8 *)&rx_leng, (uint8 *)&d[4], 4);
		memcpy((uint8 *)&rx_addr, (uint8 *)&d[8], 4);
		index = 0;

		if (rx_leng == 0x05)				//leng is 0x05
		{
			if (rx_addr == 0x28)			//addr eq7
				index = 1;
			else if (rx_addr == 0x2d)		//addr eq8
				index = 2;
			else if (rx_addr == 0x32)		//addr eq9
				index = 3;

			if (index)
			{
				app_eq_get_on_line_para(index-1, (uint8 *)&d[12]);
				app_equalizer_a2dp_apply();
			}
		}
		else if (rx_leng == 0x1d)			//leng is 0x1d
		{
			if (rx_addr == 0x1d)			//addr send all 2 part
			{
				app_eq_get_on_line_para(0, (uint8 *)&d[56]);			//eq7
				app_eq_get_on_line_para(1, (uint8 *)&d[56+20]);			//eq8
				app_eq_get_on_line_para(2, (uint8 *)&d[56+20+20]);		//eq9
				app_equalizer_a2dp_apply();
			}
		}
	}

	tunningApi((uint8 *)d);
}


void user_leadelityEntryInit(void)
{
	static uint8 hw_eq_set = 0;
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);
	uint8_t *d;

	int* image = &dspRamcoef_44100;
	int size = sizeof(dspRamcoef_44100);
	
	if (!hw_eq_set)
	{
		hw_eq_set = 0xff;

		if (!flag_L_is_LplusR)
		{
			d = (uint8 *)image;
			app_eq_get_on_line_para(0, (uint8 *)&d[40*4]);		//eq7
			app_eq_get_on_line_para(1, (uint8 *)&d[45*4]);		//eq8
			app_eq_get_on_line_para(2, (uint8 *)&d[50*4]);		//eq9
			app_equalizer_a2dp_apply();
		}
	}

	leadelityEntryInit(image, size);
}


#endif

extern u8 power_on_flag;
extern u8 dut_set_flag;

RAM_CODE void Beken_Uart_Rx(void)
{
#if (CONFIG_XIAONIU_DEVICE_TEST == 1)
	uint8 cali_data;		
	uint8 uart_rx_data[20];
	env_charge_cali_data_t *charge_cali;
	charge_cali = app_get_env_charge_cali_data();
#endif

    if ((uart_rx_done != TRUE) || (uart_rx_index == 0))
        return;

#if (USER_EX_EQ == 1)

    if ((pHCIrxBuf->code == 'D'))
	{
		user_tunningApi(&uart_rx_buf[0]);
	}
	
#endif

	
#ifdef BEKEN_DEBUG
    #if 0
	os_printf("Beken_Uart_Rx %x,%x,%x,%x,%x,%x,%x\r\n",
		pHCIrxBuf->code,
		pHCIrxBuf->opcode.ogf,
		pHCIrxBuf->opcode.ocf,
		pHCIrxBuf->total,
		pHCIrxBuf->cmd,
		pHCIrxBuf->param[0],
		uart_rx_index);
    #endif
#endif
 loop_mode:
    //os_delay_ms(10);

    //os_printf("code=0x%x, ogf=0x%x, ocf=0x%x, count=0x%x\r\n",pHCIrxBuf->code,pHCIrxBuf->opcode.ogf,pHCIrxBuf->opcode.ocf,uart_rx_index);
    //os_printf("total=%d\r\n", pHCIrxBuf->total);
    
#if PTS_TESTING
    if ( (pHCIrxBuf->code       == TRA_HCIT_COMMAND)
      && (pHCIrxBuf->opcode.ogf == PTS_TESTING_OGF)
      && (pHCIrxBuf->opcode.ocf == BEKEN_OCF)
      && (uart_rx_index         == (HCI_COMMAND_HEAD_LENGTH+pHCIrxBuf->total))) {
       /*******************************************
        * uart cmd: 0x01 e0 40 01 10 
        *                         10~4F for hfp
        *                         50~8F for a2dp
        *******************************************/
        pts_entry(pHCIrxBuf->cmd);      
        goto ret;
    }    
#endif

    if (   (pHCIrxBuf->code       != TRA_HCIT_COMMAND)
        || (pHCIrxBuf->opcode.ogf != VENDOR_SPECIFIC_DEBUG_OGF)
        || (pHCIrxBuf->opcode.ocf != BEKEN_OCF)
        || (uart_rx_index         != (HCI_COMMAND_HEAD_LENGTH+pHCIrxBuf->total)))
        goto ret;

// 01E0FC.len.cmd.para1.para2....// len from cmd, hex fomate send
    /* os_printf("cmd=%x\r\n",pHCIrxBuf->cmd); */
    switch (pHCIrxBuf->cmd) {
    case BEKEN_UART_LINK_CHECK:
        pHCItxBuf->total = uart_rx_index;
        memcpy(pHCItxBuf->param, uart_rx_buf, pHCItxBuf->total);
        break;

    case BEKEN_UART_REGISTER_WRITE_CMD: {
        int reg_index;
        REGISTER_PARAM *rx_param        = (REGISTER_PARAM *)pHCIrxBuf->param;
        REGISTER_PARAM *tx_param        = (REGISTER_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];

        pHCItxBuf->total                = uart_rx_index-1;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        tx_param->addr                  = rx_param->addr;
        tx_param->value                 = rx_param->value;
        *(volatile unsigned int *)rx_param->addr = rx_param->value;
        reg_index                       = (rx_param->addr-BK3000_XVR_BASE_ADDR)/4;
        if ((reg_index>=0) && (reg_index<=0x0f))
            XVR_analog_reg_save[reg_index] = rx_param->value;
        if(reg_index == 0x24)
            XVR_reg_0x24_save = rx_param->value;
		//#ifdef BEKEN_DEBUG
        // os_printf("addr=%x,val=%x\r\n",rx_param->addr, rx_param->value);
		//#endif
        break;
    }

    case BEKEN_UART_REGISTER_READ_CMD: {
        int reg_index;
        REGISTER_PARAM *rx_param = (REGISTER_PARAM *)pHCIrxBuf->param;
        REGISTER_PARAM *tx_param = (REGISTER_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];

        pHCItxBuf->total         = HCI_EVENT_HEAD_LENGTH+uart_rx_index;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]      = pHCIrxBuf->cmd;
        tx_param->addr           = rx_param->addr;
        reg_index                = (rx_param->addr-BK3000_XVR_BASE_ADDR)/4;
        if ((reg_index>=0) && (reg_index<=0x0f))
            tx_param->value        = XVR_analog_reg_save[reg_index];
        else
            tx_param->value        = *(volatile unsigned int *)rx_param->addr;
		//#ifdef BEKEN_DEBUG
        // os_printf("addr=%x,val=%x\r\n",tx_param->addr, tx_param->value);
		//#endif
        break;
    }

    case BEKEN_FLASH_READ_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total         = HCI_COMMAND_HEAD_LENGTH+sizeof(FLASH_PARAM)+rx_param->len;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]      = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_read_data(tx_param->data, tx_param->addr, tx_param->len);
        break;
    }

    case BEKEN_FLASH_WRITE_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total                = HCI_COMMAND_HEAD_LENGTH+sizeof(FLASH_PARAM);
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_write_data(rx_param->data, rx_param->addr, rx_param->len);
        break;
    }

    case BEKEN_FLASH_ERASE_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total                = HCI_COMMAND_HEAD_LENGTH+sizeof(tx_param->addr);
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_erase_sector(rx_param->addr);
        break;
    }

    case BEKEN_SHOW_STACK_CMD:
        app_debug_showstack();
        goto ret;

    case BEKEN_DUMP_ENV_CMD:
#ifdef BEKEN_DEBUG
		//01E0FC05AB00000000
        app_env_dump();
#endif
        goto ret;
    case BEKEN_CLEAR_LINKKEY_CMD:
 	flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
 	break;
    case LOOP_MODE_CMD: {
        clear_uart_buffer();
        while(uart_rx_done == FALSE);
        goto loop_mode;
    }

    case BEKEN_SHOW_BT_STATUS:
        app_bt_status_show();
	#if 0//def CONFIG_BLUETOOTH_AVDTP_SCMS_T
        security_control_cp_support_print();
	#endif
        goto ret;

#ifdef BEKEN_DEBUG
    case BEKEN_SHOW_BT_DEBUG:
        app_bt_debug_show();
        goto ret;
	case BEKEN_LED_EQ_BUTTON:
        if(pHCIrxBuf->param[0]==0xff)
        {
        //app_led_dump();
        }
#ifdef CONFIG_APP_EQUANLIZER
        else if(pHCIrxBuf->param[0]==0xfe)
        app_set_eq_gain((int8)(pHCIrxBuf->param[1]));
    #if (CONFIG_PRE_EQ == 1)
        else if(pHCIrxBuf->param[0]==0xfa)
            app_set_pre_eq(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xfb)
            app_show_pre_eq();
        else if(pHCIrxBuf->param[0]==0xfc)
            app_set_pre_eq_gain(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xfd)
            bt_flag2_operate(APP_FLAG2_SW_MUTE,pHCIrxBuf->param[1]);
    #if (CONFIG_DRC == 1)
        else if(pHCIrxBuf->param[0]==0xe1)
            app_set_drc_para(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xeb)
            app_show_drc_para();
    #endif

    #endif
#endif
        //else
        //    app_button_sw_action(pHCIrxBuf->param[0]);
		goto ret;

#ifdef CONFIG_APP_AEC
	case BEKEN_SET_AEC_PARA:
        app_set_aec_para(&pHCIrxBuf->param[0]);
		goto ret;
#endif
#endif

    case BEKEN_PRINT_LINK_KEY:
        app_print_linkkey();
        goto ret;
#if 1
    case BEKEN_ENTRY_DUT_MODE:
	 //01E0FC05AF00000000
	 if(power_on_flag == 0)
        app_bt_enable_dut_mode(pHCIrxBuf->param[0]);
	 else
	 	dut_set_flag = 4;
        goto ret;
    case BEKEN_ENTRY_FCC_TESTMODE:
        app_bt_enable_fcc_mode(pHCIrxBuf->param[0],pHCIrxBuf->param[1]);
    #ifdef BEKEN_OTA
        //app_button_ota_enable_action();
    #endif
        goto ret;
#endif

#if (CONFIG_XIAONIU_DEVICE_TEST == 1)

    // 01 E0 FC 01 71
	case BEKEN_ENTRY_BATT_VLCF_READ:

		flag_os_printf_disable = 1;
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable

		if (user_batt_charger_vlcf <= 0x3f)
			cali_data = user_batt_charger_vlcf;
		else
			cali_data = charge_cali->charger_vlcf;
		
		if(cali_data>0x3f)	
			cali_data=0x3f;

		uart_rx_data[0] = 0x01;
		uart_rx_data[1] = 0xE0;
		uart_rx_data[2] = 0xFC;
		uart_rx_data[3] = 0x05;
		uart_rx_data[4] = 0x71;
		uart_rx_data[5] = cali_data;
		uart_rx_data[6] = 0xFF-cali_data;

		uart_send(&uart_rx_data[0], 7);	
		uart_send((unsigned char *)&eeprom_block_num, 2);	
        goto ret;


    // 01 E0 FC 03 75 XX YY
    case BEKEN_ENTRY_BATT_VLCF_UPDATE:

		if ((pHCIrxBuf->param[0] + pHCIrxBuf->param[1]) != 0xff)
	        goto ret;

		flag_os_printf_disable = 1;
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable

		if(pHCIrxBuf->param[0]>0x3f)	
			pHCIrxBuf->param[0]=0x3f;

		user_batt_charger_vlcf = pHCIrxBuf->param[0];
		BK3000_A3_CONFIG = ((BK3000_A3_CONFIG & ~0x3f) | pHCIrxBuf->param[0]);

		//add flash write
		User_Eeprom_Vlcf_Write(pHCIrxBuf->param[0]);

		uart_rx_data[0] = 0x01;
		uart_rx_data[1] = 0xE0;
		uart_rx_data[2] = 0xFC;
		uart_rx_data[3] = 0x03;
		uart_rx_data[4] = 0x75;
		uart_rx_data[5] = pHCIrxBuf->param[0];
		uart_rx_data[6] = pHCIrxBuf->param[1];

		uart_send(&uart_rx_data[0], 7);		
        goto ret;


    // 01 E0 FC 01 62 
    case BEKEN_CMD_MACADDR_READ:

		flag_os_printf_disable = 1;
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable

		uart_rx_data[0] = 0x01;
		uart_rx_data[1] = 0xE0;
		uart_rx_data[2] = 0xFC;
		uart_rx_data[3] = 0x0D;
		uart_rx_data[4] = 0x62;

		User_Eeprom_Btaddr_Read(&uart_rx_data[11]);
        //memcpy(&uart_rx_data[11], (uint8*)&env_h->env_cfg.bt_para.device_addr, 6);
        
		uart_rx_data[5] = uart_rx_data[16];
		uart_rx_data[6] = uart_rx_data[15];
		uart_rx_data[7] = uart_rx_data[14];
		uart_rx_data[8] = uart_rx_data[13];
		uart_rx_data[9] = uart_rx_data[12];
		uart_rx_data[10] = uart_rx_data[11];

		uart_rx_data[11] = 0xFF - uart_rx_data[5];
		uart_rx_data[12] = 0xFF - uart_rx_data[6];
		uart_rx_data[13] = 0xFF - uart_rx_data[7];
		uart_rx_data[14] = 0xFF - uart_rx_data[8];
		uart_rx_data[15] = 0xFF - uart_rx_data[9];
		uart_rx_data[16] = 0xFF - uart_rx_data[10];

		uart_send(&uart_rx_data[0], 17);
        goto ret;


    // 01 E0 FC 07 6A AA BB CC DD EE FF 
    case BEKEN_CMD_MACADDR_UPDATE:

		flag_os_printf_disable = 1;
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable

		//add flash write
		uart_rx_data[0] = pHCIrxBuf->param[5];
		uart_rx_data[1] = pHCIrxBuf->param[4];
		uart_rx_data[2] = pHCIrxBuf->param[3];
		uart_rx_data[3] = pHCIrxBuf->param[2];
		uart_rx_data[4] = pHCIrxBuf->param[1];
		uart_rx_data[5] = pHCIrxBuf->param[0];
		User_Eeprom_Btaddr_Write(&uart_rx_data[0]);
	
		uart_rx_data[0] = 0x01;
		uart_rx_data[1] = 0xE0;
		uart_rx_data[2] = 0xFC;
		uart_rx_data[3] = 0x07;
		uart_rx_data[4] = 0x6A;
		uart_rx_data[5] = pHCIrxBuf->param[0];
		uart_rx_data[6] = pHCIrxBuf->param[1];
		uart_rx_data[7] = pHCIrxBuf->param[2];
		uart_rx_data[8] = pHCIrxBuf->param[3];
		uart_rx_data[9] = pHCIrxBuf->param[4];
		uart_rx_data[10] = pHCIrxBuf->param[5];
		uart_send(&uart_rx_data[0], 11);

		//写入自动开机条件后系统复位
        Delay(100);
		rf_test_user_set_reg();
        Delay(200);
        BK3000_wdt_reset();
		while(1);

        goto ret;

   // 01 E0 FC 01 7A
    case BEKEN_CMD_CRC_READ:

		flag_os_printf_disable = 1;
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable

		uart_rx_data[0] = 0x01;
		uart_rx_data[1] = 0xE0;
		uart_rx_data[2] = 0xFC;
		uart_rx_data[3] = 0x09;
		uart_rx_data[4] = 0x7A;
		User_Eeprom_CRC_Read(&uart_rx_data[5]);

		uart_rx_data[9] = 0xFF - uart_rx_data[5];
		uart_rx_data[10] = 0xFF - uart_rx_data[6];
		uart_rx_data[11] = 0xFF - uart_rx_data[7];
		uart_rx_data[12] = 0xFF - uart_rx_data[8];
		uart_send(&uart_rx_data[0], 13);		
        goto ret;


    // 01 E0 FC 01 7F
    case BEKEN_CMD_OS_PRINTF_ENABLE:

		flag_os_printf_disable = 0;		//os printf enable
		BK3000_GPIO_0_CONFIG = 0x70;	//Tx is enable
        goto ret;

#endif

#if (DEBUG_BASEBAND_MONITORS == 1)
    case BEKEN_READ_BASEBAND_MONITORS:{
        TC_Read_Local_Baseband_Monitors_Via_App();
        goto ret;
    }
    case BEKEN_RESET_BASEBAND_MONITORS:{
        LSLCstat_Reset_Monitors();
        goto ret;
    }
#endif

#if (DEBUG_SCATTERNET_MONITORS == 1)
    case BEKEN_READ_SCATTERNET_MONITORS:
    {
        TC_Read_Local_LMPconfig_Monitors_Via_App();
        goto ret;
    }
#endif


#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
    case BEKEN_AGC_MODE_PARAM:
        _LSLCacc_Read_AGC_Param();
        goto ret;
#endif
#endif

    case BEKEN_TEMP_CMD: {
        os_printf("BEKEN_TEMP_CMD\r\n");
        hfp_app_ptr_debug_printf();
        goto ret;
    }
#ifdef CONFIG_PRODUCT_TEST_INF
    case BEKEN_RSSI_CMD: {
	os_printf("rssi:0x%d,freqoffset:0x%x\r\n",aver_rssi,aver_offset);
	goto ret;
    }
#endif

#ifdef CONFIG_TWS
    case BEKEN_TWS: {
        app_tws_debug(&pHCIrxBuf->param[0]);
        break;
    }
#endif

    case BEKEN_SHOW_SYSTEM_INFO: {
        //os_printf("BEKEN_SHOW_SYSTEM_INFO.\r\n");
        show_bt_stack_status();
        goto ret;
    }

    case BEKEN_CMD_SDP_CONNECT: {
        set_sdp_browse_on();
        //app_bt_sdp_connect();
        goto ret;
    }
    
    case BEKEN_CMD_SERVICE_SEARCH_REQUEST: {
        sdp_send_serviceSearchRequest();
        goto ret;
    }
    
    case BEKEN_CMD_SERVICE_ATTRIBUTE_REQUEST: {
        app_bt_sdp_send_serviceAttributeRequest();
        goto ret;
    }
    
    case BEKEN_CMD_A2DP_SSA_REQUEST: {
        /* for a2dp sdp_query request. */
        extern result_t send_a2dp_ssa_request(void);
        send_a2dp_ssa_request();
        goto ret;
    }
    
    case BEKEN_CMD_HFP_SSA_REQUEST: {
        /* for hfp sdp_query request. */
       // extern result_t send_hfp_ssa_request(void);
       // send_hfp_ssa_request();
        goto ret;
    }
    case BEKEN_CMD_SYS_RESET:
    {
        /* watchdog reset for uart download */
        if(pHCIrxBuf->total >= 5)
        {
            uint32 param = pHCIrxBuf->param[0]<<24 | pHCIrxBuf->param[1]<<16 | pHCIrxBuf->param[2]<<8 | pHCIrxBuf->param[3];
            if(param == 0x95279527) 
            {
                /* watchdog reset */
                BK3000_WDT_CONFIG = 0x5A0001;
                BK3000_WDT_CONFIG = 0xA50001;
            }
        }
        goto ret;
    }
        
    default:
        goto ret;
    }

    Beken_Uart_Tx();

 ret:
    clear_uart_buffer();
}
void dbg_hci_rx_char(u_int8 ch)
{
    switch(s_dbg_hci_rx_state)
    {
        case DBG_HCI_STATE_RX_DATA_CONTINUE:
__DBG_HCI_STATE_RX_DATA_CONTINUE:
            *s_dbg_hci_rx_pdu_buf = ch;
            s_dbg_hci_rx_pdu_buf++;
            s_dbg_hci_rx_pdu_length--;
            if(!s_dbg_hci_rx_pdu_length)
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_COMMIT;
                goto __DBG_HCI_STATE_RX_DATA_COMMIT;
            }
            return;

        case  DBG_HCI_STATE_RX_TYPE:
            if((ch == 0x01))   // HCI_COMMAND 
            {
                s_dbg_hci_rx_head_buf[0] = ch;
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_OPCODE1;
            }
            else if((ch == 'D'))
			{
                s_dbg_hci_rx_head_buf[0] = ch;
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_DIRECT;
				s_dbg_hci_rx_length = 1;
			}			
            else
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_TYPE;
                uart_rx_index = 0;
                uart_rx_done = FALSE;
            }
            return;

		case DBG_HCI_STATE_RX_DATA_DIRECT:
			s_dbg_hci_rx_head_buf[s_dbg_hci_rx_length] = ch;
			s_dbg_hci_rx_length++;
			if(!UART_RX_READ_READY)
			{
				s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_COMMIT;
				goto __DBG_HCI_STATE_RX_DATA_COMMIT;
			}
			return;

        case DBG_HCI_STATE_RX_COMMAND_OPCODE1:
            s_dbg_hci_rx_head_buf[1] = ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_OPCODE2;
            return;

        case DBG_HCI_STATE_RX_COMMAND_OPCODE2:
            s_dbg_hci_rx_head_buf[2] = ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_LENGTH;
            return;

        case DBG_HCI_STATE_RX_COMMAND_LENGTH:
            s_dbg_hci_rx_head_buf[3] = ch;
            s_dbg_hci_rx_pdu_length = ch;
            s_dbg_hci_rx_length = 4 + ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_START;
            
            if (s_dbg_hci_rx_pdu_length==0)
            {
                goto __DBG_HCI_STATE_RX_DATA_START;
            }
            return;

        case DBG_HCI_STATE_RX_DATA_START:
__DBG_HCI_STATE_RX_DATA_START:
            s_dbg_hci_rx_pdu_buf = (volatile u_int8*) &uart_rx_buf[4];
            if((s_dbg_hci_rx_pdu_length == 0) || (s_dbg_hci_rx_pdu_length >59))    
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_COMMIT;
                goto __DBG_HCI_STATE_RX_DATA_COMMIT;
            }
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_CONTINUE;
            goto __DBG_HCI_STATE_RX_DATA_CONTINUE;

        case DBG_HCI_STATE_RX_DATA_COMMIT:
__DBG_HCI_STATE_RX_DATA_COMMIT:
            uart_rx_done = TRUE;
            uart_rx_index = s_dbg_hci_rx_length;
            #if 0
            {
                u_int8 i;
                os_printf("===hci_dbg:\r\n");
                for(i=0;i<s_dbg_hci_rx_length;i++)
                {
                    os_printf("%02x,",uart_rx_buf[i]);
                }
                os_printf("===\r\n");
            }
            #endif
            Beken_Uart_Rx();
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_TYPE;
            return;
        default:    
            return;
    }
    return;
}
void uart_handler(void) {
    u_int32 status;
    u_int8 value;

    status = REG_APB3_UART_INT_FLAG;
    if(status & (bit_UART_INT_RX_NEED_READ|bit_UART_INT_RX_STOP_END)) {
        while (UART_RX_READ_READY) {
            value=UART_READ_BYTE();
            if (BT_HOST_MODE == NONE_CONTROLLER)
                juart_receive(&value, 1);
            else if ((BT_HOST_MODE == JUNGO_HOST)) {
                #if 0
                uart_rx_buf[uart_rx_index++] = value;
                if (uart_rx_index == RX_FIFO_THRD)
                    uart_rx_index = 0;
                uart_rx_done = TRUE;;
                #endif
                dbg_hci_rx_char(value);
            } else
                TRAhcit_Rx_Char(value);
        }
    }
    REG_APB3_UART_INT_FLAG = status;
}
