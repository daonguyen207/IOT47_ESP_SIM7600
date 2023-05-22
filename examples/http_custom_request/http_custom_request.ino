#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "http_at.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Dùng http_call để tạo request, lưu ý đây là hàm bất đồng bộ, nó sẽ bỏ qua lần gọi hàm tiếp theo nếu đang bận
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

bool LTE_OK = false;

void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");
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
  //nên nối thêm chân reset để reset cứng moudle sim khi khởi động ở đây
  
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
    if(millis() - tSent > 999) //1s call 1 lần
    {
      tSent = millis();
      http_call("iot47.com","GET /time.php HTTP/1.1\r\nHost: thoigian.iot47.com\r\n\r\n\r\n\r\n",[](int http_code,uint8_t *payload, int len){
        Serial.println("\r\nHTTP Code:" + String(http_code));
        if(http_code>0)for(int i=0;i<len;i++)Serial.write(payload[i]);
      });
    }
  }
}
