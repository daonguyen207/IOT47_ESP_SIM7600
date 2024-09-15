#ifndef __IOT47_SIM7600_API__
#define __IOT47_SIM7600_API__

/* Thư viện SIM7600 lowlevel
 * Thiết kế bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 */

int rssi;
const int16_t dBm[]={-113,-111,-110,-107,-105,-103,-101,-99,-97,-95,-93,-91,-89,-87,-85,-83,-81,-79,-77,-75,-73,-71,-69,-67,-65,-63,-61,-59,-57,-55,-53,-51,
-50,-49,-48,-47,-46,-45,-44,-43,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,
0,
-116,-115,-114,-113,-112,-111,-110,-109,-108,-107,-106,-105,-104,-103,-102,-101,-100,-99,-98,-97,-96,-95,-94,-93,-92,-91,-90,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-74,-73,-72,-71,-70,-69,-68,-67,-66,-65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,-25,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
String simIP;
bool SIM_init_ok = false;
int simInitFlag=0;
bool noLTE = false;
void SIM7600_noLTE(bool _lte) //gọi hàm này ở setup -> true nếu không muốn mở mạng 4G LTE( chỉ sms, call)
{
  noLTE = _lte;
  if(noLTE == false)
  {
    SIM_init_ok = false;
    simInitFlag=2;
  }
}
void sim_loop();

String getNum(String rx,char CharEnd)
{
  String value;
  for(int i=0;i<10;i++)
  {
    if(rx[i] == CharEnd)
      return value;
    value+=(char)rx[i];
  }
  return "";
}
String getNumWithHeader(String rx,String header)
{
  int index = rx.indexOf(header);
  if(index != -1)
  {
    index+=header.length();
    String value;
    for(int i=0;i<10;i++)
    {
      if(rx[i+index]==' ')continue;
      if((rx[i+index]>='0') && (rx[i+index]<='9'))value+=(char)rx[i+index];
      else return value;
    }
  }
  return "";
}
String getNumByStartIndex(String rx,int index)
{
  if(index != -1)
  {
    String value;
    for(int i=0;i<10;i++)
    {
      if((rx[i+index]>='0') && (rx[i+index]<='9'))value+=(char)rx[i+index];
      else return value;
    }
  }
  return "";
}
String getNumByStartIndex(uint8_t *rx,int index)
{
  if(index != -1)
  {
    String value;
    for(int i=0;i<10;i++)
    {
      if((rx[i+index]>='0') && (rx[i+index]<='9'))value+=(char)rx[i+index];
      else return value;
    }
  }
  return "";
}
String get_string(String rx,char CharEnd,int maxLength)
{
  String value;
  for(int i=0;i<maxLength;i++)
  {
    if(rx[i] == CharEnd)
      return value;
    value+=(char)rx[i];
  }
  return "";
}
void get_rssi(String rep)
{
  int index = rep.indexOf("+CSQ: ");
  if(index != -1)
  {
    int value = (rep[6 + index]-48);
    if(rep[7 + index] == ',') //1 digit
    {
      
    }
    else if( rep[7 + index] != ',' && rep[8 + index] == ',') //2 digit
    {
      value*=10;
      value+= rep[7 + index]-48;
    }
    else if( rep[7 + index] != ',' && rep[8 + index] != ',' && rep[9 + index] == ',') //3 digit
    {
      value*=10;
      value+= rep[7 + index]-48;
      value*=10;
      value+= rep[8 + index]-48;
    }
    
    rssi=value;
    if(rssi==99)rssi=0;
    else rssi = dBm[value];
  }
}
void getIP(String rep)
{
  int index = rep.indexOf("+IPADDR: ");
  if(index != -1)
  {
     simIP = rep.substring(index+9);
     simIP.replace("\r","");
     simIP.replace("\n","");
     simIP.replace("OK","");
  }
}
String endline_waitString(String reponse,uint32_t timeout)
{
  uint32_t t = millis();
  String rx;
  uint32_t tlog = millis();
  while(1)
  {
    if(millis() -  t > timeout)return "";
    if(SIM_SERIAL.available())
    {
      char c = (char)SIM_SERIAL.read();
      rx+=c;
      if(c == '\n')
      {
        if(rx.indexOf(reponse) != -1)return rx;
        else rx="";
      }
    }
    delay(0);
    if(timeout == 100000)
    {
      if(millis() - tlog > 1000)
      {
        tlog = millis();
        #ifdef SIM_ENABLE_LOG
        Serial.print(".");
        #endif
      }
    }
  }
}
String whileReponseString(String reponse,uint32_t timout) //chờ cho tới khi có phản hồi 
{
  uint32_t t = millis();
  String rx;
  while(1)
  {
    delay(5);
    if(millis() - t  > timout)return "";
    while(SIM_SERIAL.available())
    {
      char c = (char)SIM_SERIAL.read();
      rx+=c;
      #ifdef SIM_ENABLE_LOG
      Serial.write(c); 
      #endif 
    }
    if(rx.indexOf(reponse) != -1)return rx;
  }
}
int whileReponse(uint32_t timout)
{
  uint32_t t = millis();
  while(1)
  {
    if( millis() - t  > timout)return 0;
    if(SIM_SERIAL.available())return 1;
    delay(5);
  }
}
int whileReponseNoDelay(uint32_t timout)
{
  uint32_t t = millis();
  while(1)
  {
    if( millis() - t  > timout)return 0;
    if(SIM_SERIAL.available())return 1;
  }
}
void ClearSimReponse()
{
  while(SIM_SERIAL.available())SIM_SERIAL.read();
}
void dumpSimReponse()
{
  whileReponse(500);
  while(SIM_SERIAL.available())
  {
    char c=(char)SIM_SERIAL.read();
    #ifdef SIM_ENABLE_LOG
    Serial.print(c);
    #endif
  }
}
void dumpSimReponse(int timeout)
{
  whileReponse(timeout);
  while(SIM_SERIAL.available())
  {
    char c=(char)SIM_SERIAL.read();
    #ifdef SIM_ENABLE_LOG
    Serial.print(c);
    #endif
  }
}
void dumpSimReponseNoDelay()
{
  while(SIM_SERIAL.available())
  {
    char c=(char)SIM_SERIAL.read();
    #ifdef SIM_ENABLE_LOG
    Serial.print(c);
    #endif
  }
}
String simCheckRecever(String value)
{
  if(SIM_SERIAL.available())
  {
     delay(5);
     String dat="";
     while(SIM_SERIAL.available())dat+=(char)SIM_SERIAL.read();
     #ifdef SIM_ENABLE_LOG
     Serial.println(dat);
     #endif
     if(dat.indexOf(value) != -1)
       return dat;
  }
  return "";
}
String simCheckRecever(String value,String value2)
{
  if(SIM_SERIAL.available())
  {
     delay(5);
     String dat="";
     while(SIM_SERIAL.available())dat+=(char)SIM_SERIAL.read();
     #ifdef SIM_ENABLE_LOG
     Serial.println(dat);
     #endif
     if(dat.indexOf(value) != -1)
       return dat;
     else if(dat.indexOf(value2) != -1)
       return dat;
  }
  return "";
}
String simGetRecever()
{
  if(SIM_SERIAL.available())
  {
     delay(5);
     String dat="";
     while(SIM_SERIAL.available())dat+=(char)SIM_SERIAL.read();
     #ifdef SIM_ENABLE_LOG
     Serial.println(dat);
     #endif
  }
  return "";
}
String simGetRecever(String value,String value2)
{
  if(SIM_SERIAL.available())
  {
     delay(5);
     String dat="";
     while(SIM_SERIAL.available())dat+=(char)SIM_SERIAL.read();
     #ifdef SIM_ENABLE_LOG
     Serial.println(dat);
     #endif
     if(dat.indexOf(value) != -1)
       return value;
     else if(dat.indexOf(value2) != -1)
       return value2;
  }
  return "";
}
char simCheckRecever_AndWait_CheckRecever(String check1,int timeout1,String check2 , int timeout2)
{
  whileReponse(timeout1);
  if(SIM_SERIAL.available())
  {
     String dat="";
     while(SIM_SERIAL.available()) dat+=(char)SIM_SERIAL.read(); 
     #ifdef SIM_ENABLE_LOG
     Serial.println(dat);
     #endif
     if((dat.indexOf(check1) != -1) || (dat.indexOf(check2) != -1)) //nếu check 1 ok
     {
       whileReponse(timeout2);
       if(SIM_SERIAL.available())
       {
          while(SIM_SERIAL.available()) 
          {
            char c = (char)SIM_SERIAL.read();
            dat+=c;
            #ifdef SIM_ENABLE_LOG
            Serial.print(c);
            #endif
          }
          if(dat.indexOf(check2) != -1) return 1;
       }
     }
  }
  return 0;
}

void simSoftReset()
{
  SIM_SERIAL.println("AT");       delay(10);
  SIM_SERIAL.println("AT+CRESET");delay(10);
}

typedef void (*SIM7600_callback_t)(void *arg);
SIM7600_callback_t SIM7600_on_lte_ready_f;
void SIM7600_on_lte_ready_callback(SIM7600_callback_t _callback)
{
  SIM7600_on_lte_ready_f = _callback;
}
SIM7600_callback_t SIM7600_on_lte_fail_f;
void SIM7600_on_lte_fail_callback(SIM7600_callback_t _callback)
{
  SIM7600_on_lte_fail_f = _callback;
}

SIM7600_callback_t SIM7600_ready_f;
void SIM7600_on_ready_callback(SIM7600_callback_t _callback)
{
  SIM7600_ready_f = _callback;
}

void SIM7600_loop()
{
  static uint32_t tResend=0;
  if(simInitFlag==0) //check hardwave
  {
     if(millis() - tResend > 200)
     {
        tResend=millis();
        String rx = simCheckRecever("OK");
        if( rx != "")
        {
           simInitFlag=1;
           #ifdef SIM_ENABLE_LOG
           Serial.println("SIM7600 connect ok");
           #endif
           return;
        }
        //---------------------//
        #ifdef SIM_ENABLE_LOG
        Serial.println("ATE0");
        #endif
        SIM_SERIAL.println("ATE0");
     }
  }
  else if(simInitFlag==1) //init sim ( check singer)
  {
    if(millis() - tResend > 1000)
    {
        tResend=millis();
        String rx = simCheckRecever("+CSQ: ");
        if(rx != "")
        {
           get_rssi(rx);
           if( rssi != 0)
           {
             #ifdef SIM_ENABLE_LOG
             Serial.println("SIM7600 rssi:" + String(rssi));  
             #endif        
             if(SIM7600_ready_f !=0) SIM7600_ready_f(0);
             simInitFlag=2;
             if(noLTE)
             {
                simInitFlag = -1;
                SIM_init_ok=true;
                #ifdef SIM_ENABLE_LOG
                Serial.println("SIM INIT DONE");   
                #endif
             }   
             return;
           }
        }
        //---------------------//   
        #ifdef SIM_ENABLE_LOG
        Serial.println("AT+CSQ"); 
        #endif
        SIM_SERIAL.println("AT+CSQ");
    }
  }
  else if(simInitFlag==2)
  {
    if(millis() - tResend > 300)
    {
       tResend=millis();
       if(simCheckRecever("+NETOPEN: 0","Network is already opened") != "")
       {
          simInitFlag=3;
          return;
       }
       //---------------------//    
       #ifdef SIM_ENABLE_LOG 
       Serial.println("AT+NETOPEN"); 
       #endif
       SIM_SERIAL.println("AT+NETOPEN");
    }
  }
  else if(simInitFlag==3) //get ip
  {
    if(millis() - tResend > 300)
    {
      tResend=millis();
      String rx = simCheckRecever("+IPADDR:");
      if(rx != "")
      {        
          simInitFlag=-1;
          SIM_init_ok=true;   
          getIP(rx);
          #ifdef SIM_ENABLE_LOG 
          Serial.println("IP:" + simIP);
          #endif
          if(SIM7600_on_lte_ready_f != 0)SIM7600_on_lte_ready_f(0);
          return;
      }
      #ifdef SIM_ENABLE_LOG 
      Serial.println("AT+IPADDR");
      #endif
      SIM_SERIAL.println("AT+IPADDR");
    }
  }
  //----------------------------------
  if(SIM_init_ok)
  {
    sim_loop();
  }
}
bool SIM7600_connected()
{
  return SIM_init_ok;
}
//----------------------------------------------
//hander 
#define SIM_BUFF_LENGTH 2048*4
uint8_t SIM_BUFFER[SIM_BUFF_LENGTH];
int     SIM_WRITE_index;
int     SIM_COUNT_CMD;
int     SIM_READ_index;


#define SIM7600_MAX_HANDLE 10
typedef void (*SIM7600_handle_t)(String cmd);
SIM7600_handle_t list_handle[SIM7600_MAX_HANDLE];
int counter_handle;
void SIM7600_add_handle(SIM7600_handle_t my_handle) //thêm trình xử lí
{
  if(counter_handle == SIM7600_MAX_HANDLE)return;
  for(int i=0;i<SIM7600_MAX_HANDLE;i++)if(list_handle[i] == my_handle)return;
  
  list_handle[counter_handle] = my_handle;
  counter_handle++;
}
void SIM7600_delete_handle(SIM7600_handle_t my_handle)
{
  int i=0;
  for(;i<SIM7600_MAX_HANDLE;i++)
  {
    if( list_handle[i] == my_handle )
    {
      counter_handle--;
      break;
    }
  }
  for(;i<SIM7600_MAX_HANDLE-1;i++)
  {
     list_handle[i] = list_handle[i+1];
  }
}


#define SIM7600_MAX_LOOP 10
typedef void (*SIM7600_loop_t)();
SIM7600_loop_t list_user_loop[SIM7600_MAX_LOOP];
int counter_user_loop;
void SIM7600_add_user_loop(SIM7600_loop_t my_loop) //thêm trình lap
{
  if(counter_user_loop == SIM7600_MAX_LOOP)return;
  for(int i=0;i<SIM7600_MAX_LOOP;i++)if(list_user_loop[i] == my_loop)return;
  
  list_user_loop[counter_user_loop] = my_loop;
  counter_user_loop++;
}
void SIM7600_delete_user_loop(SIM7600_loop_t my_loop)
{
  int i=0;
  for(;i<SIM7600_MAX_LOOP;i++)
  {
    if( list_user_loop[i] == my_loop )
    {
      counter_user_loop--;
      break;
    }
  }
  for(;i<SIM7600_MAX_LOOP-1;i++)
  {
     list_user_loop[i] = list_user_loop[i+1];
  }
}
//----------------------------------------------------
void SIM_READ_INDEX_UP()
{
  SIM_READ_index++; if(SIM_READ_index >= SIM_BUFF_LENGTH) SIM_READ_index = 0;
}
void get_data_to_buffer()
{
  while( SIM_SERIAL.available() )
  {
    char c = (char)SIM_SERIAL.read();  
    #ifdef SIM_LOG_RX_UART
    Serial.write(c);
    #endif
    SIM_BUFFER[SIM_WRITE_index] = c;
    if(SIM_BUFFER[SIM_WRITE_index] == '\n')SIM_COUNT_CMD++;
    SIM_WRITE_index++; if(SIM_WRITE_index >= SIM_BUFF_LENGTH) SIM_WRITE_index = 0;
  }
}
void SIM_SKIP_CR()
{
  SIM_READ_INDEX_UP();
}
void SIM_SKIP_LF()
{
  SIM_READ_INDEX_UP();
  SIM_COUNT_CMD--;
}
void SIM_SKIP_CRLF()
{
  SIM_SKIP_CR();
  SIM_SKIP_LF();
}
String read_to_LF()
{
  String cmd;
  SIM_COUNT_CMD--;
  while(1)
  {
    char c = (char)SIM_BUFFER[SIM_READ_index];
    cmd+=c;
    SIM_READ_INDEX_UP();
    if(c == '\n')
    {
      return cmd;
    }
  }
  return cmd;
}
String read_to_CRLF()
{
  String cmd;
  char flagCR = 0;
  while(1)
  {
    char c = (char)SIM_BUFFER[SIM_READ_index];
    cmd+=c;
    SIM_READ_INDEX_UP();
    if(c == '\n')
    {
      SIM_COUNT_CMD--;
      if(flagCR==1)return cmd;
    }
    else if(c == '\r')flagCR=1; 
  }
  return cmd;
}
String read_to_len(int len)
{
  String cmd;
  int count = 0;
  while(1)
  {
    char c = (char)SIM_BUFFER[SIM_READ_index];
    cmd+=c;
    SIM_READ_INDEX_UP();
    count++;if(count>=len)
    {
      return cmd;
    }
  }
  return cmd;
}
void wait_data(int wait)//khi module sim truyền data ra, tốc độ truyền khá chậm nên sẽ chờ data tùy theo độ dài dữ liệu 
{
  delay(wait);
  get_data_to_buffer();
}
void sim_loop()
{
  get_data_to_buffer();
  while(SIM_COUNT_CMD>0)
  {
    String cmd = read_to_LF();
    //Serial.print("[");Serial.print(cmd);Serial.print("]"); 
    if(cmd[0] == '+') //system handle
    {
      if(cmd.indexOf("+CIPEVENT: NETWORK CLOSED UNEXPECTEDLY") != -1)
      {
        if(SIM7600_on_lte_fail_f!=0)SIM7600_on_lte_fail_f(0);
        return;
      }
    }
    for(int i=0;i<counter_handle;i++)if( list_handle[i] != 0) list_handle[i](cmd); //run app handle
  }
  for(int i=0;i<counter_user_loop;i++)if( list_user_loop[i] != 0) list_user_loop[i](); //run loop user
}

void startAT_SYNC_mode()
{
  ClearSimReponse();
}
int stopAT_SYNC_mode(int err_code)
{
  SIM_COUNT_CMD=0;
  return err_code;
}


/*------Viết app hanler-----
 * Ở chế độ nhận dữ liệu, bạn phải gọi hàm SIM_SKIP_LF() với mỗi kĩ tự \n có trong data
 * Ở chế độ nhận dữ liệu, nếu data được kết thúc với \r\n thì phải gọi SIM_SKIP_CRLF()
 * Ở chế độ nhận dữ liệu, trước khi bắt đầu đọc phải gọi hàm wait_data(x) với x là độ dài data / 50 * 10   bởi uart truyền data chậm hơn tốc độ xử lí của cpu
 * Nếu các hàm gửi được viết theo cơ chế sysnc ( gửi lệnh - chờ phản hồi) thì phải gọi hàm startAT_SYNC_mode(); đầu hàm và gọi stopAT_SYNC_mode(err_code); sau khi kết thúc hàm đó
 */

#endif
