#include "mfrc522.h"
#include "config.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char* TAG="RC522";
static spi_device_handle_t spi;

static void cs_low(){ gpio_set_level(RC522_CS,0); }
static void cs_high(){ gpio_set_level(RC522_CS,1); }

static void reg_write(uint8_t reg, uint8_t val){
    cs_low();
    uint8_t d[2]={(reg<<1)&0x7E,val};
    spi_transaction_t t={.length=16,.tx_buffer=d};
    spi_device_transmit(spi,&t);
    cs_high();
}
static uint8_t reg_read(uint8_t reg){
    cs_low();
    uint8_t tx[2]={((reg<<1)&0x7E)|0x80,0};
    uint8_t rx[2]={0};
    spi_transaction_t t={.length=16,.tx_buffer=tx,.rx_buffer=rx};
    spi_device_transmit(spi,&t);
    cs_high();
    return rx[1];
}
static void fifo_write(uint8_t v){ reg_write(0x09,v); }

void mfrc522_init(void){
    gpio_set_direction(RC522_CS,GPIO_MODE_OUTPUT);
    gpio_set_direction(RC522_RST,GPIO_MODE_OUTPUT);
    gpio_set_level(RC522_CS,1);
    gpio_set_level(RC522_RST,0);
    vTaskDelay(50/portTICK_PERIOD_MS);
    gpio_set_level(RC522_RST,1);
    vTaskDelay(100/portTICK_PERIOD_MS);

    spi_bus_config_t buscfg={
       .mosi_io_num=RC522_MOSI,
       .miso_io_num=RC522_MISO,
       .sclk_io_num=RC522_SCK,
       .quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=32
    };
    spi_bus_initialize(SPI2_HOST,&buscfg,SPI_DMA_CH_AUTO);

    spi_device_interface_config_t dev={
       .clock_speed_hz=500000,
       .mode=0,
       .spics_io_num=-1,
       .queue_size=1
    };
    spi_bus_add_device(SPI2_HOST,&dev,&spi);

    reg_write(0x01,0x0F);
    vTaskDelay(50/portTICK_PERIOD_MS);
    reg_write(0x2A,0x8D);
    reg_write(0x2B,0x3E);
    reg_write(0x2D,30);
    reg_write(0x2C,0);
    reg_write(0x15,0x40);
    reg_write(0x11,0x3D);
    reg_write(0x14, reg_read(0x14)|0x03);
    ESP_LOGI(TAG,"RC522 Ready Ver=0x%02X",reg_read(0x37));
}

int mfrc522_read_uid(uint8_t *uid, uint8_t *len){
    reg_write(0x01,0x00);
    reg_write(0x04,0x00);
    reg_write(0x0A,0x80);
    reg_write(0x0D,0x07);
    fifo_write(0x26);
    reg_write(0x01,0x0C);
    reg_write(0x0D,0x87);
    int to=100;
    while(to--){ if(reg_read(0x04)&0x30) break; vTaskDelay(2/portTICK_PERIOD_MS); }
    if(to<=0) return -1;

    reg_write(0x01,0x00);
    reg_write(0x04,0x00);
    reg_write(0x0A,0x80);
    reg_write(0x0D,0x00);
    fifo_write(0x93);
    fifo_write(0x20);
    reg_write(0x01,0x0C);
    reg_write(0x0D,0x80);
    to=100;
    while(to--){ if(reg_read(0x04)&0x30) break; vTaskDelay(2/portTICK_PERIOD_MS); }
    if(to<=0) return -1;

    vTaskDelay(15/portTICK_PERIOD_MS);
    if(reg_read(0x0A)!=5) return -1;

    uint8_t d[5];
    for(int i=0;i<5;i++){ d[i]=reg_read(0x09); vTaskDelay(2/portTICK_PERIOD_MS); }
    memcpy(uid,d,4);
    *len=4;
    reg_write(0x01,0x00);
    return 0;
}