
// led pin names
#define D1  PD_15
#define D3  PE_9
#define D4  PF_13
#define D5  PF_12

//Simulated gps/bps data via uart connector
#define HAT_UART_RX     PD_6
#define HAT_UART_INT    PD_7
#define HAT_UNUSED      PD_5            //attached to uart tx pin that we will be ignoring, has to be tied to the same hardware as rx pd_6 & pd_5

//tsl5556
#define HAT_TSL_ADDR   0x29
#define HAT_TSL_SDA PB_9
#define HAT_TSL_SCL PB_8

//bno055
#define HAT_BNO_ADDR    0x28
#define HAT_BNO_SDA     PB_11
#define HAT_BNO_SCL     PB_10
#define HAT_BNO_INT     PE_14
#define HAT_BNO_RST     PE_15

//CAN BUS
//Spi TRANSLATION VIA MCP2515
#define HAT_SPI_SCL         PA_5
#define HAT_SPI_MOSI        PA_7
#define HAT_SPI_MISO        PA_6
#define HAT_SPI_MCP2515_CS  PD_14
#define HAT_MCP_INT         PE_11

//CAN TRANSLATION HARDWARE ON STM
#define HAT_STM_CANRX   PD_0
#define HAT_STM_CANTX   PD_1

//CAN ADM3054 voltage error detect pins
/// \warning there's a jumper between this digital input and the adm3054 digital output
#define HAT_ADM_1_VDD_SENSE PF_14
#define HAT_ADM_2_VDD_SENSE PE_13