#ifndef __IOT47_SIM7600_HTTPS_API__
#define __IOT47_SIM7600_HTTPS_API__

/* Thư viện http và https
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Lưu ý: data trả về phải dưới 1500 byte, nếu quá sẽ lỗi
 */
typedef void (*https_callback_t)(int http_code,uint8_t *payload, int len);
https_callback_t https_callback_f;
int https_state;
String https_request;
void https_handle(String cmd);
void https_loop();
uint32_t timeOutHttps;
int https_code;

int https_flag=0;
void init_http()
{
  if(https_flag==0)
  {
    https_flag = 1;
    SIM7600_add_user_loop(https_loop);
    SIM_SERIAL.println("AT+HTTPINIT");
    delay(5);
  }
}
int https_call(int action,String url,https_callback_t _callback)
{
  if(https_state != 0) return -1;
  https_callback_f = _callback;
  SIM7600_add_handle(https_handle);
  init_http();
  SIM_SERIAL.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  delay((url.length()/50) * 10 + 5);
  SIM_SERIAL.println("AT+HTTPACTION=" + String(action)); //get
  timeOutHttps = millis();
  #ifdef SIM_ENABLE_LOG
  Serial.println("[LOG]http sent request..");
  #endif
  https_state = 1;
  return 0;
}
int https_get(String url,https_callback_t _callback)
{
  return https_call(0,url,_callback);
}
int https_post(String url,https_callback_t _callback)
{
  return https_call(1,url,_callback);
}
int https_head(String url,https_callback_t _callback)
{
  return https_call(2,url,_callback);
}
int https_delete(String url,https_callback_t _callback)
{
  return https_call(3,url,_callback);
}
void https_call_error()
{
  https_state = 0;
  SIM7600_delete_handle(https_handle);
  if(https_callback_f != 0)https_callback_f(-1,0,0);
}
void https_loop()
{
  if(https_state == 2)
  {
    if(millis() - timeOutHttps > 200)
    {
      https_state=0;
    }
  }
  else if(https_state == 1)
  {
    if(millis() - timeOutHttps > 8000)
    {
      #ifdef SIM_ENABLE_LOG
      Serial.println("[LOG]http timeout");
      #endif
      SIM_SERIAL.println("AT+HTTPTERM");
      delay(10);
      https_flag = 0;
      https_call_error();
    }
  }
}

void https_read(String cmd,int offset)
{
  int https_payload_len = getNumByStartIndex(cmd,offset).toInt();
  if(https_payload_len>0) //có data
  {
    uint8_t *payload = (uint8_t *)malloc(https_payload_len);
    wait_data((https_payload_len/50) * 10 + 1); //wait data
    for(int d=0;d<https_payload_len;d++)
    {
      payload[d] = SIM_BUFFER[SIM_READ_index];
      if(payload[d] == '\n')SIM_SKIP_LF();
      else SIM_READ_INDEX_UP();
    }
    https_state = 2;
    timeOutHttps = millis();
    SIM7600_delete_handle(https_handle);
    if(https_callback_f != 0)https_callback_f(https_code,payload,https_payload_len);
    https_callback_f = 0; 
    free(payload);
    SIM_SKIP_CRLF();//nếu data được module sim kết thúc bằng CRLF thì phải gửi lệnh này
  }
  else //không có data
  {
    https_state = 0;
    SIM7600_delete_handle(https_handle);
    if(https_callback_f != 0)https_callback_f(https_code,0,0);
  }
}

void https_handle(String cmd) //tay cầm https
{
  if((cmd[0] == '+') && (cmd[1] == 'H') && (cmd[2] == 'T') && (cmd[3] == 'T') && (cmd[4] == 'P'))
  {
    if(cmd.indexOf("+HTTPACTION: 0,") != -1)
    {
      https_code = getNumByStartIndex(cmd,15).toInt();
      if(https_code >= 100)
      {
        int https_payload_len = getNumByStartIndex(cmd,19).toInt();
        if(https_payload_len < 1500)SIM_SERIAL.println("AT+HTTPREAD=" + String(https_payload_len));
        else https_call_error();
      }
      else https_call_error();
      return;
    }
    if(cmd.indexOf("+HTTPREAD: DATA,") != -1)
    {
      https_read(cmd,16);
      return;
    }
    if(cmd.indexOf("+HTTPREAD: ") != -1)
    {
      https_read(cmd,11);
      return;
    }
    //---------------xử lí ngoại lệ----------------------
    if(cmd.indexOf("+HTTPTERM:") != -1)
    {
      SIM_SERIAL.println("AT+HTTPTERM");
      delay(10);
      https_flag = 0;
    }
  }
}


#endif