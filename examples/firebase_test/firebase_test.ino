#define SIM_SERIAL         Serial2  //khai báo serial cho module sim (bắt buộc)
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "firebase_at.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Khi LTE sẵn sàng hãy đăng kí các calback của firebase và gọi firebase_begin
 *  3.Hàm begin yêu cầu nhập vào các tham số sau firebase_begin(server,path,token)
 *    - server không được chứa http www
 *    - path luôn bắt đầu bằng 1 dấu /
 *    - token lấy trên website của firebase
 *  4.Để gửi bản data lên firebase hãy gọi firebase_put(path,data); lưu ý là path luôn bắt đầu bằng 1 dấu gạch chéo /  còn data thì là kiểu String hoặc int, double đều đc
 *    - Hàm firebase_put là bất đồng bộ (tức là quy trình gửi sẽ được chạy dưới nền), bạn phải gọi firebase_ready_put() để kiểm tra xem có đang bận gửi không, hàm này trả về 0 là OK, 1 là busy
 *  5.Để nhận dữ liệu, có 2 cách
 *    5.1: Dùng callback stream, hàm nãy sẽ tự động gọi mỗi khi dữ liệu trên server được cập nhật (put) hoặc lần đầu connect (path)
 *    5.2: Dùng lệnh get
 *    
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

bool LTE_OK = false;

void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");
  
  firebase_connected_callback([](void *arg){
    Serial.println("Firebase connected");
  });
  firebase_disconnect_callback([](void *arg){
    Serial.println("Firebase disconnect");
  });
  firebase_timeout_callback([](void *arg){
    Serial.println("Firebase timeout");
  });
  firebase_stream_callback([](uint8_t *payload, uint16_t len){
    for(int i=0;i<len;i++)Serial.write(payload[i]); //đây là 1 json, bạn nên kết hợp với thư viện json để decode data
  });
  firebase_begin("test1-12b02.firebaseio.com","/","vb8C70YSdi6UvnM804eYlPEohqPkPFmv3vzdobxL"); //server, path , auth token
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
    if(firebase_connected())
    {
      if(firebase_ready_put() == 0)
      {
        if(millis() - tSent > 3000) //gửi tin nhắn sau mỗi 3s - do đã đăng kí kênh stream nên sẽ có event puth trả ngược về
        {
          tSent = millis();
          Serial.println("\r\nPut to /time " + String(millis()));
          firebase_put("/time",millis());
        }
      }

//lệnh này dùng để get dữ liệu 1 cách chủ động nhé, không nên dùng lệnh này, dùng kênh stream để nhận sẽ tiết kiệm băng thông
//lệnh này là bất đồng bộ, quy trình chạy sẽ được thực hiện dưới nền, nó sẽ bỏ qua lần gọi tiếp theo nếu nó đang bận
//        firebase_get("/tinnhan",[](uint8_t *payload, uint16_t len){ //nhân dự liệu từ key tinnhan
//          for(int i=0;i<len;i++)Serial.write(payload[i]);
//        });

    }
  }
}
