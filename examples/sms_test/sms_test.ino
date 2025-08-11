#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "sms_call_at.h"

#define TX_SIM1 19
#define RX_SIM1 21

void SIM_on_sms(String phone, String sms, String time)
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
  //SIM_SERIAL.begin(115200);
  SIM_SERIAL.begin(115200,SERIAL_8N1,TX_SIM1,RX_SIM1);
  SIM7600_on_ready_callback(SIM_on_ready);
  SIM7600_noLTE(true); // tắt mạng
}

void loop() 
{
  SIM7600_loop();
  if(Serial.available()) //nhập tin nhắn cần gửi đi theo kiểu 0394733311|Xin chào, đây là sms hỗ trợ tiếng việt\r\n
  {
    String raw="";
    String phone,message;
    while(Serial.available())raw+=(char)Serial.read();
    // Tìm vị trí xuống dòng đầu tiên
    int pos = raw.indexOf('|');
    if (pos != -1) {
      phone = raw.substring(0, pos);              // từ đầu đến trước \n
      message = raw.substring(pos + 1);           // từ sau \n tới hết
      message.trim();                                    // bỏ khoảng trắng, \r, \n thừa

      Serial.println("Gửi tin nhắn sms tới: Phone: " + phone);
      Serial.print("   Message: " + message);
    }
    int result = sms_sent_andWait(phone,message,5000);
    if(result == -1)Serial.println("Gửi sms thất bại");
    else Serial.println("Gửi sms thành công");
  }
  //call_calling("0398647714");
}

