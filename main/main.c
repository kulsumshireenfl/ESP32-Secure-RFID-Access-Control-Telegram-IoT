#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "config.h"
#include "mfrc522.h"
#include "esp_crt_bundle.h"
#include <string.h>

static const char* TAG="DOORLOCK_S3";

void servo_angle(int angle){
    uint32_t duty = (500 + angle*2000/180) * 8191 / 20000;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void telegram_send(const char* message){
    char enc[700]; int j=0;
    for(int i=0; message[i] && j<600; i++){
        if(message[i]==' ') enc[j++]='+';
        else if((message[i]>='A'&&message[i]<='Z')||(message[i]>='a'&&message[i]<='z')||(message[i]>='0'&&message[i]<='9')||message[i]==':'||message[i]=='_'||message[i]=='-') enc[j++]=message[i];
        else { sprintf(&enc[j],"%%%02X",(uint8_t)message[i]); j+=3; }
    }
    enc[j]=0;
    char url[1000];
    snprintf(url,sizeof(url),"https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s",TELEGRAM_TOKEN,TELEGRAM_CHAT_ID,enc);
    esp_http_client_config_t cfg={.url=url,.method=HTTP_METHOD_GET,.timeout_ms=10000,.crt_bundle_attach=esp_crt_bundle_attach};
    esp_http_client_handle_t client=esp_http_client_init(&cfg);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void wifi_init(){
    nvs_flash_init(); esp_netif_init(); esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t c=WIFI_INIT_CONFIG_DEFAULT(); esp_wifi_init(&c);
    wifi_config_t wc={}; strcpy((char*)wc.sta.ssid,WIFI_SSID); strcpy((char*)wc.sta.password,WIFI_PASS);
    esp_wifi_set_mode(WIFI_MODE_STA); esp_wifi_set_config(WIFI_IF_STA,&wc); esp_wifi_start(); esp_wifi_connect();
    vTaskDelay(4000/portTICK_PERIOD_MS);
}

const char* check_authorized(char* uid_str){
    for(int i=0;i<AUTHORIZED_USERS_COUNT;i++) if(strcmp(uid_str,authorized_users[i].uid)==0) return authorized_users[i].name;
    return NULL;
}

void door_task(void* arg){
    ESP_LOGI(TAG,"Door Task Started - Waiting for card!");
    telegram_send("S3_DoorLock_Online_Ready");

    uint8_t uid[10]; uint8_t len;
    while(1){
        if(mfrc522_read_uid(uid,&len)==0){
            char s_space[32]="";
            for(int i=0;i<len;i++){ char b[5]; if(i==0) sprintf(b,"%02X",uid[i]); else sprintf(b," %02X",uid[i]); strcat(s_space,b); }
            ESP_LOGI(TAG,"CARD [%s]",s_space);
            const char* name = check_authorized(s_space);
            if(name){
                ESP_LOGI(TAG,"AUTH %s",name);
                gpio_set_level(LED_GREEN,1); servo_angle(90);
                char msg[150]; snprintf(msg,sizeof(msg),"Kulsum - Blue Card Entered UID: %s",s_space);
                telegram_send(msg);
                vTaskDelay(3000/portTICK_PERIOD_MS);
                servo_angle(0); gpio_set_level(LED_GREEN,0);
            }else{
                ESP_LOGW(TAG,"DENIED %s",s_space);
                gpio_set_level(LED_RED,1); gpio_set_level(BUZZER_PIN,1);
                char msg[150]; snprintf(msg,sizeof(msg),"Intruder! Unknown Card: %s",s_space);
                telegram_send(msg);
                vTaskDelay(1500/portTICK_PERIOD_MS);
                gpio_set_level(LED_RED,0); gpio_set_level(BUZZER_PIN,0);
            }
        }
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
}

void app_main(){
    ESP_LOGI(TAG,"ESP32-S3 DoorLock ESP-IDF");
    gpio_set_direction(LED_GREEN,GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_RED,GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER_PIN,GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GREEN,0); gpio_set_level(LED_RED,0); gpio_set_level(BUZZER_PIN,0);

    ESP_LOGI(TAG,"LED TEST START");
    for(int i=0;i<3;i++){ gpio_set_level(LED_GREEN,1); gpio_set_level(LED_RED,1); vTaskDelay(300/portTICK_PERIOD_MS); gpio_set_level(LED_GREEN,0); gpio_set_level(LED_RED,0); vTaskDelay(300/portTICK_PERIOD_MS); }

    ledc_timer_config_t tc={.speed_mode=LEDC_LOW_SPEED_MODE,.duty_resolution=LEDC_TIMER_13_BIT,.timer_num=LEDC_TIMER_0,.freq_hz=50};
    ledc_timer_config(&tc);
    ledc_channel_config_t cc={.gpio_num=SERVO_PIN,.speed_mode=LEDC_LOW_SPEED_MODE,.channel=LEDC_CHANNEL_0,.timer_sel=LEDC_TIMER_0,.duty=0};
    ledc_channel_config(&cc);
    servo_angle(0);

    wifi_init();
    mfrc522_init();
    xTaskCreate(door_task,"door_task",8192,NULL,5,NULL);
    while(1){ vTaskDelay(1000/portTICK_PERIOD_MS); }
}