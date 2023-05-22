#define SIM_SERIAL         Serial2  //khai báo serial cho module sim (bắt buộc)
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "mqtt_at.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Khi LTE sẵn sàng hãy đăng kí các calback của mqtt và gọi mqtt connect
 *  3.Để gửi bản tin mqtt thì có hai hàm  mqtt_publish_async và mqtt_publish
 *    - Hàm mqtt_publish_async là hàm bất đồng bộ, nó sẽ không block code của bạn.
 *       Bạn sẽ phải đợi quy trình publish dưới nền hoàn tất thì mới gọi hàm này tiếp được. 
 *         Có thể kiểm tra có sẵn sàng để gửi không bằng lệnh mqtt_ready_to_pub() với 0 là sẵn sàng, 1 là busy
 *    - Hàm mqtt_publish sẽ chờ cho tới khi gửi hoàn tất hoặc timeout, hàm này trả về 0 là gửi ok, -1 là timeout, tuy nhiên nên dùng mqtt_publish_async thì hơn
 *  4.Tương tự như thế với việc sub, các hàm mqtt_subscribe_async() mqtt_subscribe() mqtt_ready_to_sub() hoạt động tương tự
 *  5.Tương tự như thế với việc unsub, các hàm mqtt_unsub_async() mqtt_unsub() mqtt_ready_to_unsub() hoạt động tương tự
 *  
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

bool LTE_OK = false;

void MQTT_on_message(String topic, uint8_t *payload,uint16_t len)
{
  Serial.println("payload_len:" + String(len));
  Serial.print(topic);Serial.print(" : ");
  for(int i=0;i<len;i++)Serial.write(payload[i]);
  Serial.println();
}
void MQTT_on_disconnect(void *arg)
{
  Serial.println("Mất kết nối mqtt"); //user cần xử lí kết nối lại
}
void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");

  Serial.println("Start mqtt...");
  int err =  mqtt_connect("broker.emqx.io:8883","client_id_random_fdjh546t54g","","","iot47_willtopic","offline",1); //đối số gồm (server,client id, username, pass, last will topic, last will mess, ssl_on ( 1=ssl, 0 = thường)
  if(err==0)
  {
    MQTT_on_mess_callback(MQTT_on_message);
    MQTT_on_disconnect_callback(MQTT_on_disconnect);
    Serial.println("Mqtt connected");
    
    mqtt_subscribe("mqtt_topic_1",1); // đối số gồm topic, qos
    mqtt_subscribe("mqtt_topic_2",1); // đối số gồm topic, qos
    mqtt_subscribe("mqtt_topic_3",1); // đối số gồm topic, qos
  }
  else
  {
    Serial.println("Mqtt connect err code=" + String(err));
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
    if(mqtt_connected())
    {
      if(millis() - tSent > 10000) //gửi tin nhắn sau mỗi 10s
      {
        if( mqtt_ready_to_pub() == 0) //0 = OK, -1 = busy
        {
          tSent = millis();
          String data_sent = String(millis()) +  " Hi, Toi la mqtt tu esp";
          Serial.println(data_sent);
          mqtt_publish_async("mqtt_topic_esp_pub",(uint8_t *)&data_sent[0],data_sent.length());
        }
      }
    }
  }
}
