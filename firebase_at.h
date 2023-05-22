#ifndef __IOT47_SIM7600_FIREBASE_API__
#define __IOT47_SIM7600_FIREBASE_API__

/* Thư viện App firbase
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Chú ý: Độ dài data gửi nhận < 1500
 */

const char* firebase_pem= \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n" \
"-----END CERTIFICATE-----";

#include "https_at.h"

typedef void (*firebase_callback_t)(void *arg);
firebase_callback_t firebase_timeout_f;
void firebase_timeout_callback(firebase_callback_t _callback)
{
  firebase_timeout_f = _callback;
}

firebase_callback_t firebase_connected_f;
void firebase_connected_callback(firebase_callback_t _callback)
{
  firebase_connected_f = _callback;
}
firebase_callback_t firebase_disconnect_f;
void firebase_disconnect_callback(firebase_callback_t _callback)
{
  firebase_disconnect_f = _callback;
}

typedef void (*firebase_stream_t)(uint8_t *payload, uint16_t len);
firebase_stream_t firebase_stream_f;
firebase_stream_t firebase_get_f;
void firebase_stream_callback(firebase_stream_t _callback)
{
  firebase_stream_f = _callback;
}

int firebase_connect_state;  int firebase_connected(){return firebase_connect_state;};
int firebase_state;
uint32_t firebase_timeout;
void firebase_handle(String cmd);
void firebase_loop();

//sysnc
char https_add_ssl(const char * root_ca,uint16_t len,String path)
{
  #ifdef SIM_ENABLE_LOG
  Serial.println("[LOG]Nạp chứng chỉ CA");
  Serial.println("AT+CCERTDOWN=\"" + path + "\"," + String(len));
  #endif
  startAT_SYNC_mode();
  SIM_SERIAL.println("AT+CCERTDOWN=\"" + path + "\"," + String(len));
  whileReponse(1000);simCheckRecever(">");
  for(int i=0;i<len;i++)SIM_SERIAL.write(root_ca[i]);
  whileReponse(1000);delay(3);
  String rx = simCheckRecever("OK");
  if( rx != "")
  {
     #ifdef SIM_ENABLE_LOG
     Serial.println("[LOG]Nạp chứng chỉ thành công");
     #endif
     return stopAT_SYNC_mode(1);
   }
   else
   {
     #ifdef SIM_ENABLE_LOG
     Serial.println("[LOG]Nạp chứng chỉ thất bại");
     #endif
     SIM_SERIAL.println("AT+CCERTDELE=\"" + path + "\"");
     delay(100);while(SIM_SERIAL.available())SIM_SERIAL.read(); //xoá chứng chỉ
     return stopAT_SYNC_mode(0);
  }
}

String firebase_link;
String firebase_path;
String firebase_auth;

//sysnc
int firebase_begin(String _firebase_link,String _firebase_path,String _firebase_auth)
{
  static int flag_init = 0;
  if(firebase_state != 0)return -1;
  firebase_link = _firebase_link;
  firebase_path = _firebase_path;
  firebase_auth = _firebase_auth;
  
  startAT_SYNC_mode();
  SIM_SERIAL.println("AT+CCERTLIST");
  whileReponse(1000);delay(3);
  String rx = simCheckRecever("OK");
  if(rx != "")
  {
    if(rx.indexOf("firebase.pem") != -1)  //đã nạp chứng chỉ
    {
       #ifdef SIM_ENABLE_LOG
       //Serial.println("[LOG]Đã có chứng chỉ CA FIREBASE");
       #endif
    }
    else
    {
      https_add_ssl(firebase_pem,1911,"firebase.pem"); //nạp chứng chỉ
    }
  }
 
  if(flag_init==0)
  { 
    flag_init = 1;
    
    #ifdef SIM_ENABLE_LOG
    Serial.println(F("[LOG]CONNECT TO FIREBASE..."));
    #endif
    SIM_SERIAL.println("AT+CCHCLOSE=0");dumpSimReponse();
    SIM_SERIAL.println("AT+CCHCLOSE=1");dumpSimReponse();
    //gọi 1 số lệnh set up ssl
    SIM_SERIAL.println("AT+CSSLCFG=\"sslversion\",0,4");dumpSimReponse();
    SIM_SERIAL.println("AT+CSSLCFG=\"authmode\",0,1");dumpSimReponse();
    SIM_SERIAL.println("AT+CSSLCFG=\"cacert\",0,\"firebase.pem\"");dumpSimReponse();
    SIM_SERIAL.println("AT+CCHSET=1");dumpSimReponse();
    SIM_SERIAL.println("AT+CCHSTART");dumpSimReponse();
    SIM_SERIAL.println("AT+CCHSSLCFG=0,0");dumpSimReponse();
  }

  firebase_state=1;
  SIM7600_add_handle(firebase_handle); //thêm trình xử lí firebase
  SIM7600_add_user_loop(firebase_loop);
  firebase_timeout = millis();
  
  //connect
  SIM_SERIAL.println("AT+CCHOPEN=0,\"www." + firebase_link + "\",443,2"); 
  return stopAT_SYNC_mode(0);
}
void firebase_get(String path,firebase_stream_t callback)
{
  firebase_get_f = callback;
  https_get("https://"+firebase_link + firebase_path + ".json?auth=" + firebase_auth,[](int http_code,uint8_t *payload, int len){
        if(http_code>0)firebase_get_f(payload,len);   
  });
}


int firebase_flag_tx;
String firebase_tx_path;
String firebase_tx_data;
uint32_t firebase_tx_time;
uint32_t firebase_tx_timeout;

int firebase_ready_put()
{
  return firebase_flag_tx;
}
void firebase_write(String path,String dat)
{
  if(firebase_connect_state)
  {
    if(firebase_flag_tx==0)
    {
      SIM_SERIAL.println("AT+CCHOPEN=1,\"www." + firebase_link + "\",443,2"); //mở kết nối
      firebase_flag_tx = 1;
      firebase_tx_data = dat;
      firebase_tx_path = path;
      firebase_tx_timeout=millis();
    }
  }
}
void firebase_put(String path,String dat)
{
  firebase_write(path,dat);
}
void firebase_put(String path,int dat)
{
  firebase_write(path,String(dat));
}
void firebase_put(String path,float dat)
{
  firebase_write(path,String(dat));
}
void firebase_put(String path,double dat)
{
  firebase_write(path,String(dat));
}
void firebase_put(String path,uint32_t dat)
{
  firebase_write(path,String(dat));
}
void firebase_put(String path,long unsigned int dat)
{
  firebase_write(path,String(dat));
}

void firebase_loop()
{
  if(firebase_state == 1)
  {
    if(millis() - firebase_timeout > 15000)
    {
       firebase_state = 0;
       firebase_connect_state = 0;
       if(firebase_timeout_f!=0)firebase_timeout_f(0);
    }
  }
  if(firebase_flag_tx == 1 )
  {
    if(millis() - firebase_tx_timeout > 8000)
    {
      firebase_flag_tx=0;
    }
  }
  if(firebase_flag_tx == 2 )
  {
    if(millis() - firebase_tx_time > 50)
    {
      String req = "PUT " + firebase_tx_path + ".json?auth=" + firebase_auth + " HTTP/1.1\r\nHost: " + firebase_link + "\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\nContent-Length: "
        + String(firebase_tx_data.length()) + "\r\n\r\n" + firebase_tx_data;
      SIM_SERIAL.println("AT+CCHSEND=1," + String(req.length()));
      delay(3);
      SIM_SERIAL.print(req);
      firebase_flag_tx = 0;
    }
  }
}
void firebase_handle(String cmd) //tay cầm firebase
{
  if(cmd[0] == '+')
  {
    //-------------------tx--------------
    if(cmd.indexOf("+CCHOPEN: 1,") != -1)
    {
      int err_code = getNumByStartIndex(cmd,12).toInt();
      if(err_code == 0)
      {
        firebase_flag_tx = 2;
        firebase_tx_time = millis();
      }
      else firebase_flag_tx=0;
    }
    //-------------------rx---------------
    if(cmd.indexOf("+CCHOPEN: 0,") != -1)
    {
      int err_code = getNumByStartIndex(cmd,12).toInt();
      Serial.println("[LOG] CCHOPEN: " + String(err_code));
      if(err_code == 0)
      {
       String req = "GET " + firebase_path + ".json?auth=" + firebase_auth + " HTTP/1.1\r\nHost: " + firebase_link + "\r\nAccept: text/event-stream\r\n\r\n\r\n";
       SIM_SERIAL.println("AT+CCHSEND=0," + String(req.length()));
       delay(3);
       SIM_SERIAL.print(req);
       firebase_connect_state = 1;
       firebase_state = 2;
      }
      return;
    }
    if(cmd.indexOf("+CCHRECV: DATA,0,") != -1)
    {
      int payload_len = getNumByStartIndex(cmd,17).toInt();
      if(payload_len<1500)
      {
        uint8_t *payload = (uint8_t *)malloc(payload_len);
        wait_data((payload_len/50) * 10 +1); //wait data
        for(int d=0;d<payload_len;d++)
        {
          payload[d] = SIM_BUFFER[SIM_READ_index];
          if(payload[d] == '\n')SIM_SKIP_LF();
          else SIM_READ_INDEX_UP();
        }
        if( (payload[0] == 'e') && (payload[1] == 'v') && (payload[2] == 'e') && (payload[3] == 'n') && (payload[4] == 't'))
        {
          if((payload[7] == 'p')) //put or path
          {
            uint8_t *index_start_data = (uint8_t *)strstr((const char *)payload,(const char *)"\ndata: {");
            uint8_t *index_end_data = (uint8_t *)strstr((const char *)payload,(const char *)"}\n\n");
            if((index_start_data!=0) && (index_end_data!=0))
            {
               index_start_data+=7;
               if(firebase_stream_f !=0) firebase_stream_f(index_start_data,index_end_data-index_start_data+1);
            }
          }
        }
        free(payload);
      }
      else
      {
        #ifdef SIM_ENABLE_LOG
        Serial.println(F("[LOG]Du lieu qua lon"));
        #endif
      }
      return;
    }

    //-----------xử lí các ngoại lệ--------------
    if(cmd.indexOf("+CCH_PEER_CLOSED: 0") != -1) //ngắt kết nối firebase
    {
      firebase_state=0;
      firebase_connect_state = 0;
      if(firebase_disconnect_f != 0)firebase_disconnect_f(0);
    }
    if(cmd.indexOf("+CCH_PEER_CLOSED: 1") != -1)
    {
      firebase_flag_tx = 0;
    }
  }
}

#endif