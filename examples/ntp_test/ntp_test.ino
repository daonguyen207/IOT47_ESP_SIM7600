#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "ntp_at.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Khi LTE sẵn sàng hãy khởi tạo ntp bằng ntp_begin(time_zone);
 *  3.Dùng ntp_getTime để lấy thời gian, nó là hàm bất đồng bộ nên sẽ bỏ qua lần gọi hàm tiếp theo nếu đang bận
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

bool LTE_OK = false;

void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");
  ntp_begin(7); //sau khi begin hãy đợi tầm 10s rồi hẵng gọi ntp_getTime vì module sim cần 1 chút thời gian để tiến hành update time tự động
}
void SIM_on_lte_fail(void *arg)
{
  LTE_OK = false;
  Serial.println("Đã mất mạng");
}
void SIM_on_ready(void *arg)
{
  Serial.println("Modul sim ready");
}

void setup() 
{
  Serial.begin(115200);
  SIM_SERIAL.setRxBufferSize(2048);
  SIM_SERIAL.begin(115200);
  SIM7600_on_ready_callback(SIM_on_ready);
  SIM7600_on_lte_ready_callback(SIM_on_lte);
  SIM7600_on_lte_fail_callback(SIM_on_lte_fail);
}

uint32_t tSent=0;
void loop() 
{
  SIM7600_loop();
  if(LTE_OK)
  {
    if(ntp_ready())
    {
      if(millis() - tSent > 999) //1s lấy time 1 lần
      {
          tSent = millis();
          ntp_getTime([](void *arg){
            int8_t *time = (int8_t *)arg;
            Serial.print("Time: ");
            Serial.print((int)time[2]); Serial.print(":");
            Serial.print((int)time[1]); Serial.print(":");
            Serial.print((int)time[0]); Serial.print(" ");
    
            Serial.print((int)time[4]); Serial.print("/");
            Serial.print((int)time[5]); Serial.print("/");
            Serial.print((int)time[6]); Serial.print(" Z=");
            Serial.println((int)time[7]);
        });
      }
    }
  }
}
