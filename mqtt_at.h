#ifndef __IOT47_SIM7600_MQTT_API__
#define __IOT47_SIM7600_MQTT_API__

/* Thư viện App MQTT
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 */

bool mqtt_connect_state;
bool mqtt_connected(){return mqtt_connect_state;}
void mqtt_handle(String cmd);
//--------------------------
String  SIM_MQTT_TOPIC;
uint8_t *payload;
uint16_t payload_len;
//-------------------------
int mqtt_sub_err_code;
int mqtt_sub_state;
uint32_t mqtt_sub_t;

int mqtt_pub_err_code;
int mqtt_pub_state;
uint32_t mqtt_pub_t;

int mqtt_unsub_err_code;
int mqtt_unsub_state;
uint32_t mqtt_unsub_t;
//------------------------/
void mqtt_loop() //check timeout
{
  if(millis() - mqtt_pub_t > 10000){mqtt_pub_state=0;mqtt_pub_err_code = -1;}
  if(millis() - mqtt_sub_t > 10000){mqtt_sub_state=0;mqtt_sub_err_code = -1;}
  if(millis() - mqtt_unsub_t > 10000){mqtt_unsub_t=0;mqtt_unsub_err_code = -1;}
}
int mqtt_ready_to_pub(){return mqtt_pub_state;}
int mqtt_ready_to_sub(){return mqtt_sub_state;}
int mqtt_ready_to_unsub(){return mqtt_unsub_state;}

void mqtt_disconnect()
{
  SIM_SERIAL.println("AT+CMQTTDISC=0,60");
  delay(20);
  SIM_SERIAL.println("AT+CMQTTREL=0");
  delay(20);
  SIM_SERIAL.println("AT+CMQTTSTOP");
  delay(20);
}

//sysnc function
int mqtt_connect(String sever,String client_id,String user, String pass, String will_topic, String will_mess, int sslOn)
{
  if(mqtt_connect_state)return -1;
  mqtt_disconnect();
  startAT_SYNC_mode();
  
  SIM_SERIAL.println("AT+CMQTTSTART");
  if(whileReponseString("+CMQTTSTART: 0",5000) == "")return stopAT_SYNC_mode(-2);
  delay(2000);

  SIM_SERIAL.println("AT+CMQTTACCQ=0,\"" + client_id + "\"," + String(sslOn));
  if(whileReponseString("OK",5000) == "")return stopAT_SYNC_mode(-3);
  delay(2000);
  
  String user_info;
  if((user != "") && (pass != ""))
  {
    user_info = ",\"" + user + "\",\"" + pass + "\"";
  }
  SIM_SERIAL.println("AT+CMQTTCONNECT=0,\"tcp://" + sever + "\",60,1" + user_info); 
  String rx = whileReponseString("+CMQTTCONNECT: 0,",10000);
  if( rx != "")
  {
    int err_code = getNumWithHeader(rx,"+CMQTTCONNECT: 0,").toInt();
    if(err_code != 0)return err_code;
  }
  else return stopAT_SYNC_mode(-4);

  if((will_topic != "") && (will_mess != ""))
  {
    SIM_SERIAL.println("AT+CMQTTWILLTOPIC=0," + String(will_topic.length()));
    whileReponse(500);delay(3);
    SIM_SERIAL.println(will_topic);
    delay((will_topic.length()/50) * 10);
    if(simCheckRecever("OK") == "")return stopAT_SYNC_mode(-5);

    SIM_SERIAL.println("AT+CMQTTWILLMSG=0," + String(will_mess.length()) + ",1");
    whileReponse(500);delay(3);
    SIM_SERIAL.println(will_mess);
    delay((will_mess.length()/50) * 10);
    if(simCheckRecever("OK") == "")return stopAT_SYNC_mode(-6);
  }

  SIM7600_add_handle(mqtt_handle); //thêm trình xử lí mqtt vào để xử các tác vụ mặc định
  SIM7600_add_user_loop(mqtt_loop); //thêm trình lặp để xử lí tác vụ custom của mqtt
  mqtt_connect_state=1;

  return stopAT_SYNC_mode(0);
}
int mqtt_subscribe_async(String topic,int qos)
{
  if(mqtt_sub_state)return -1;
  SIM_SERIAL.println("AT+CMQTTSUBTOPIC=0,"+ String(topic.length()) + "," + String(qos));
  delay(5);
  
  SIM_SERIAL.println(topic);
  delay((topic.length()/50) * 10 + 5);
  
  SIM_SERIAL.println("AT+CMQTTSUB=0");delay(1);
  mqtt_sub_t = millis();
  mqtt_sub_state = 1;
  return 0;
}
int mqtt_subscribe(String topic,int qos)
{
  mqtt_subscribe_async(topic,qos);
  while(1)
  {
    delay(10);
    SIM7600_loop();
    if(mqtt_ready_to_sub() == 0)return mqtt_sub_err_code;
  }
}
int mqtt_unsub_async(String topic)
{
  if(mqtt_unsub_state)return -1;
  SIM_SERIAL.println("AT+CMQTTUNSUBTOPIC=0,"+ String(topic.length()));
  whileReponse(500);delay(3);

  SIM_SERIAL.println(topic);
  delay((topic.length()/50) * 10 + 5);
  SIM_SERIAL.println("AT+CMQTTUNSUB=0,0");delay(1);
  mqtt_unsub_t = millis();
  mqtt_unsub_state = 1;
  return 0;
}
int mqtt_unsub(String topic)
{
  mqtt_unsub_async(topic);
  while(1)
  {
    delay(10);
    SIM7600_loop();
    if(mqtt_ready_to_unsub() == 0)return mqtt_unsub_err_code;
  }
}
int mqtt_publish_async(String topic, uint8_t * dat, uint16_t len)
{
  if(mqtt_pub_state)return mqtt_pub_state;
  SIM_SERIAL.println("AT+CMQTTTOPIC=0," + String(topic.length()));
  delay(3);

  SIM_SERIAL.println(topic); //sent topic
  delay((topic.length()/50) * 10 + 5);

  SIM_SERIAL.println("AT+CMQTTPAYLOAD=0," + String(len));
  delay(3);

  for(int i=0;i<len;i++)SIM_SERIAL.write(dat[i]); //sent payload
  delay((len/50) * 10 + 5);

  SIM_SERIAL.println("AT+CMQTTPUB=0,1,60");delay(1);

  mqtt_pub_t = millis();
  mqtt_pub_state = 1;
  
  return 0;
}
int mqtt_publish(String topic, uint8_t * dat, uint16_t len)
{
  mqtt_publish_async(topic,dat,len);
  while(1)
  {
    delay(10);
    SIM7600_loop();
    if(mqtt_ready_to_pub() == 0)return mqtt_pub_err_code;
  }
}

typedef void (*mqtt_on_mess_t)(String topic, uint8_t *payload,uint16_t len);
mqtt_on_mess_t mqtt_on_mess_f;
void MQTT_on_mess_callback(mqtt_on_mess_t _callback){mqtt_on_mess_f = _callback;}

typedef void (*mqtt_callback_t)(void *arg);
mqtt_callback_t mqtt_on_disconnect_f;
void MQTT_on_disconnect_callback(mqtt_callback_t _callback){mqtt_on_disconnect_f = _callback;}

void mqtt_handle(String cmd) //tay cầm mqtt
{
  if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'M') && (cmd[3] == 'Q') && (cmd[4] == 'T') && (cmd[5] == 'T')) //check command hearder
  {
    //-----------------xử lí phản hồi tin nhắn-----------
    int index = cmd.indexOf("+CMQTTRXTOPIC: 0,");
    if(index != -1)
    {
      int topic_len = getNumByStartIndex(cmd,index+17).toInt();
      wait_data((topic_len/50) * 10);
      SIM_MQTT_TOPIC = read_to_len(topic_len);
      SIM_SKIP_CRLF();
      return;
    }
    index = cmd.indexOf("+CMQTTRXPAYLOAD: 0,");
    if(index != -1)
    {
      payload_len = getNumByStartIndex(cmd,index+19).toInt(); 
      wait_data((payload_len/50) * 10 + 1);
      if(mqtt_on_mess_f!=0)
      {
        payload = (uint8_t *)malloc(payload_len);
        for(int d=0;d<payload_len;d++)
        {
          payload[d] = SIM_BUFFER[SIM_READ_index];
          if(payload[d] == '\n')SIM_SKIP_LF();
          else SIM_READ_INDEX_UP();
        }
        mqtt_on_mess_f(SIM_MQTT_TOPIC,payload,payload_len); 
        free(payload);
      }
      //if(SIM_READ_index>=SIM_BUFF_LENGTH)SIM_READ_index-=SIM_BUFF_LENGTH; //kiểm tra tránh tràn buffer
      SIM_SKIP_CRLF();//nếu data được module sim kết thúc bằng CRLF thì phải gửi lệnh này
      return;
    }
    index = cmd.indexOf("+CMQTTSUB: 0,"); //phản hồi subscribe
    if(index != -1)
    {
      int err_code = getNumByStartIndex(cmd,index+13).toInt();
      mqtt_sub_err_code = err_code;
      #ifdef SIM_ENABLE_LOG 
      Serial.println("[LOG]MQTT subscribe:" + String(err_code));
      #endif
      mqtt_sub_state=0;
      return;
    }
    //-----------------xử lí phản hổi publish-----------
    index = cmd.indexOf("+CMQTTPUB: 0,");
    if(index != -1)
    {
      int err_code = getNumByStartIndex(cmd,index+13).toInt();
      mqtt_pub_err_code = err_code;
      #ifdef SIM_ENABLE_LOG 
      Serial.println("[LOG]MQTT publish:" + String(err_code));
      #endif
      mqtt_pub_state = 0;
      return;
    }
    //-----------------xử lí phản hổi unsub-----------
    index = cmd.indexOf("+CMQTTUNSUB: 0,");
    if(index != -1)
    {
      int err_code = getNumByStartIndex(cmd,index+15).toInt();
      mqtt_unsub_err_code = err_code;
      #ifdef SIM_ENABLE_LOG 
      Serial.println("[LOG]MQTT unsub:" + String(err_code));
      #endif
      mqtt_unsub_state = 0;
      return;
    }
    //-----------------xử lí các ngoại lệ----------------
    index = cmd.indexOf("+CMQTTCONNLOST: 0,"); //ngắt kết nối và kết nối lại
    if(index != -1)
    {
      int err_code = getNumByStartIndex(cmd,index+18).toInt(); 
      mqtt_connect_state=0;
      SIM7600_delete_handle(mqtt_handle); //xóa trinh xử lí mqtt
      MQTT_on_disconnect_callback(0);
      return;
    }
    index = cmd.indexOf("+CMQTTNONET"); //kết nối lại
    if(index != -1)
    {
      mqtt_connect_state=0;
      SIM7600_delete_handle(mqtt_handle); //xóa trinh xử lí mqtt
      MQTT_on_disconnect_callback(0);
      return;
    }
    index = cmd.indexOf("+CMQTTCONNECT: 0,");
    if(index != -1)
    {
      int err_code = getNumByStartIndex(cmd,index+18).toInt();
      if(err_code != 0)
      {
        #ifdef SIM_ENABLE_LOG 
        Serial.println("[LOG]MQTT connect err code:" + String(err_code));
        #endif
      }
      return;
    }
  }
}

#endif


