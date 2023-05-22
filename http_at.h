#ifndef __IOT47_SIM7600_HTTP_API__
#define __IOT47_SIM7600_HTTP_API__

/* Thư viện App http thường
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Lưu ý: Chỉ hỗ trợ http thường, nếu dùng http hoặc https thì dùng thư viện https_at.h, data trả về phải dưới 1500 byte, nếu quá sẽ lỗi
 */

 typedef void (*http_callback_t)(int http_code,uint8_t *payload, int len);
 http_callback_t http_callback_f;
 int http_state;
 String http_request;
 void http_handle(String cmd);
 void http_loop();
 
 #define HTTP_CIPOPEN 1
 #define HTTP_CIPSEND 2

 uint32_t timeOutHttp;
 int http_call(String server,String _request,http_callback_t _callback)
 {
    if(http_state != 0) return-1;
    http_callback_f = _callback;
    SIM7600_add_handle(http_handle);
    SIM7600_add_user_loop(http_loop);
    http_state = HTTP_CIPOPEN;
    timeOutHttp = millis();
    http_request = _request;
    SIM_SERIAL.println("AT+CIPOPEN=0,\"TCP\",\"" + server + "\",80");
    return 0;
 }
 int http_call(String server,int port,String _request,http_callback_t _callback)
 {
    if(http_state != 0) return-1;
    http_callback_f = _callback;
    SIM7600_add_handle(http_handle);
    SIM7600_add_user_loop(http_loop);
    http_state = HTTP_CIPOPEN;
    timeOutHttp = millis();
    http_request = _request;
    SIM_SERIAL.println("AT+CIPOPEN=0,\"TCP\",\"" + server + "\"," + String(port));
    return 0;
 }
 void http_call_error()
 {
    http_state = 0;
    SIM7600_delete_handle(http_handle);
    SIM_SERIAL.println("AT+CIPCLOSE=0");
    if(http_callback_f != 0)http_callback_f(-1,0,0);
 }
 void http_loop()
 {
    if(http_state != 0)
    {
      if(millis() - timeOutHttp > 8000)
      {
        #ifdef SIM_ENABLE_LOG
        Serial.println("http timeout");
        #endif
        http_state = 0;
        SIM7600_delete_handle(http_handle);
        if(http_callback_f != 0)http_callback_f(-1,0,0);
      }
    }
 }
 void http_handle(String cmd) //tay cầm http
 {
    if(cmd[0] == '+')
    {
      if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'I') && (cmd[3] == 'P'))
      {
        if(cmd.indexOf("+CIPOPEN: 0,") != -1)
        {
          int err_code = getNumByStartIndex(cmd,12).toInt();
          if(err_code == 0 || err_code == 4) //CONNECT OK
          {
            SIM_SERIAL.println("AT+CIPSEND=0," + String(http_request.length()));
            delay(3);
            SIM_SERIAL.println(http_request);
            return;
          }
          else http_call_error();
        }
        return;
      }
      if((cmd[0] == '+') && (cmd[1] == 'I') && (cmd[2] == 'P') && (cmd[3] == 'D'))
      {
         int payload_len = getNumByStartIndex(cmd,4).toInt();
         if(payload_len > 0)
         {
           uint8_t *payload = (uint8_t *)malloc(payload_len);
           wait_data((payload_len/50) * 10 +1); //wait data
           for(int d=0;d<payload_len;d++)
           {
            payload[d] = SIM_BUFFER[SIM_READ_index];
            if(payload[d] == '\n')SIM_SKIP_LF();
            else SIM_READ_INDEX_UP();
           }
           if((payload[0] = 'H') && (payload[1] = 'T') && (payload[2] = 'T') && (payload[3] = 'P'))
           {
             int http_code = getNumByStartIndex(payload,9).toInt();
             uint8_t *dat = (uint8_t *)strstr((const char *)payload,(const char *)"\r\n\r\n");
             if(dat != 0)
             {
               http_state = 0;
               SIM7600_delete_handle(http_handle);
               SIM_SERIAL.println("AT+CIPCLOSE=0");
               if(http_callback_f != 0)http_callback_f(http_code,dat+4,payload_len - (dat+4-payload));
             }
             else http_call_error();
           }
           else http_call_error();
           free(payload);
         }
         else http_call_error();
         return;
      }
    }
 }

 #endif