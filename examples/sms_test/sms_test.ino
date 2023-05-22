#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "sms_call_at.h"

void SIM_on_sms(String phone, String sms)
{
  Serial.println("=>SMS RX: " + phone + "," + sms);
}
void SIM_on_ring_ring()
{
  Serial.println("[RING RING]"); //có cuộc gọi đến
}
void SIM_on_ready(void *arg)
{
  Serial.println("Modul sim ready");
  sms_set_callback(SIM_on_sms);
  call_set_callback(SIM_on_ring_ring);
}

void setup() 
{
  Serial.begin(115200);
  SIM_SERIAL.setRxBufferSize(2048);
  SIM_SERIAL.begin(115200);
  SIM7600_on_ready_callback(SIM_on_ready);
}

void loop() 
{
  SIM7600_loop();
  
  //sms_sent("0398647714","Xin chao, toi la sms gui tu esp");
  //call_calling("0398647714");
}
