#ifndef __IOT47_SIM7600_SMS_CALL_API__
#define __IOT47_SIM7600_SMS_CALL_API__

/* Thư viện App SMS và Call
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Lưu ý: Chỉ hỗ trợ sdt 10 số
 */

typedef void (*sms_callback_t)(String phone, String sms);
sms_callback_t sms_callback_f;

typedef void (*call_callback_t)();
call_callback_t call_callback_f;
void call_set_callback(call_callback_t __callback)
{
  call_callback_f = __callback;
}
void sms_call_handle(String cmd) //tay cầm sms
{
  if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'M') && (cmd[3] == 'T'))
  {
    if(cmd.indexOf("+CMT: \"+") != -1)
    {
       if(cmd[19] == '"')
       {
        String phone;
        for(int x=8;x<8+11;x++) //lấy sđt
        {
          if((cmd[x]>='0') && (cmd[x]<='9'))phone+=(char)cmd[x];
          else return;
        }
        String mess = read_to_CRLF();
        if(sms_callback_f != 0)sms_callback_f(phone,mess);
        return;
       }
    }
    if(cmd.indexOf("+CMGS: ") != -1)
    {
      #ifdef SIM_ENABLE_LOG 
      int sms_index = getNumByStartIndex(cmd,7).toInt();
      Serial.println("SMS sent:" + String(sms_index));
      #endif
      return;
    }
  }
  else
  {
    if((cmd[0] == 'R') && (cmd[1] == 'I') && (cmd[2] == 'N') && (cmd[3] == 'G') && (cmd[4] == 0x0D) && (cmd[5] == 0x0A))
    {
      if(call_callback_f != 0)call_callback_f();
    }
  }
}
void sms_set_callback(sms_callback_t __callback)
{
  static char flag = 0;
  if(flag == 0) //để đảm bảo đoạn code này chạy 1 lần duy nhất
  {
    flag = 1;
    delay(100);SIM_SERIAL.println("AT+CMGF=1");
    delay(100);SIM_SERIAL.println("AT+CNMI=2,2,0,0,0");
    delay(100);SIM_SERIAL.println("AT+CMGD=,1");
    SIM7600_add_handle(sms_call_handle); //thêm trình xử lí sms vào
  }
  sms_callback_f = __callback;
}
void sms_sent(String phone,String sms)
{
  SIM_SERIAL.print("AT+CMGS=\"" + phone + "\"\r");delay(10);
  SIM_SERIAL.print(sms);
  SIM_SERIAL.write(0x1A);
}
void call_calling(String phone)
{
  SIM_SERIAL.println("ATD"+phone+";");
}
#endif