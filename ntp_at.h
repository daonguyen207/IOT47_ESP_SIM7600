#ifndef __IOT47_SIM7600_NTP_API__
#define __IOT47_SIM7600_NTP_API__

/* Thư viện App ntp
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Chú ý: Hãy đảm bảo LTE đã được khởi động trước khi gọi hàm begin
 */

 /*   Mô tả các lệnh AT
  *   1. AT+CCLK?   Để lấy múi giờ
  *   2. AT+CTZU=1  Để tự động cập nhật giờ
  *   3. AT+CNTP="pool.ntp.org",0  Để set server thời gian
  */  

  typedef void (*ntp_callback_t)(void *arg);
  ntp_callback_t ntp_callback_f;
  void ntp_handle(String cmd);
  int ntp_state;
  int ntp_ready()
  {
    return ntp_state;
  }
  
  void ntp_begin(int timeZone)
  {
    delay(10);SIM_SERIAL.println("AT+CNTP=\"pool.ntp.org\"," + String(timeZone*4));
    delay(10);SIM_SERIAL.println("AT+CTZU=1");
    delay(10);
    SIM7600_add_handle(ntp_handle);
    ntp_state=1;
  }
  void ntp_getTime(ntp_callback_t _callback)
  {
    if(ntp_state==0)return;
    #ifdef SIM_ENABLE_LOG
    Serial.println("[LOG]Get time ntp");
    #endif
    SIM_SERIAL.println("AT+CCLK?");
    ntp_callback_f = _callback;
  }
  void ntp_handle(String cmd) //tay cầm ntp
  {
    if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'C') && (cmd[3] == 'L') && (cmd[4] == 'K'))
    {
      int8_t time[8];
      time[6] = (cmd[8]-48)*10 + (cmd[9]-48);
      time[5] = (cmd[11]-48)*10 + (cmd[12]-48);
      time[4] = (cmd[14]-48)*10 + (cmd[15]-48);
      time[2] = (cmd[17]-48)*10 + (cmd[18]-48);
      time[1] = (cmd[20]-48)*10 + (cmd[21]-48);
      time[0] = (cmd[23]-48)*10 + (cmd[24]-48);

      time[7] = ((cmd[26]-48)*10 + (cmd[27]-48))/4; //time zone
      if(cmd[25] == '-')time[7]*=-1;
      if(ntp_callback_f != 0) ntp_callback_f(time);
      return;
    }
  }

 #endif