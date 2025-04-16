#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include <Update.h>
#include "ota_from_link.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Dùng ota_start để download file ota thông qua giao thức http: Lưu ý: 1 số module sim chỉ có cache giới hạn dưới 700KB, nếu thấy mã lỗi 707 trả về tức là file của bạn có kích thước quá lớn, bạn cần dùng ví dụ ota_link_mutil_part
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

bool LTE_OK = false;

void OTA_downloadding(uint32_t current, uint32_t total)
{
  Serial.println(String(current) + "/" + String(total));
  String proces = String((float)current/(float)total*100.0) + "%";
  Serial.println(proces);
}

void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");
  ota_downloading_callback(OTA_downloadding);
  int err = ota_start("https://raw.githubusercontent.com/daonguyen207/IOT47_ESP_SIM7600/main/examples/ota_from_link/hello.bin");
  Serial.println("ota err code: " + String(err));
  if(err == 0)
  {
    if (Update.end())
    {
      if (Update.isFinished())
      {
        Serial.println("Restart device!");
        delay(2000);
        ESP.restart();
      }
      else
      {
          Serial.println("OTA not fiished");
      }
    }
    else
    {
        Serial.println("Error occured #: " + String(Update.getError()));
    }
  }
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
  SIM_SERIAL.begin(115200,SERIAL_8N1,4,15);  //thay thành chân UART bạn dùng để kết nối với module sim
  SIM7600_on_ready_callback(SIM_on_ready);
  SIM7600_on_lte_ready_callback(SIM_on_lte);
  SIM7600_on_lte_fail_callback(SIM_on_lte_fail);
}

void loop() 
{
  SIM7600_loop();
}
