#ifndef __IOT47_SIM7600_SMS_CALL_API__
#define __IOT47_SIM7600_SMS_CALL_API__

/* Thư viện App SMS và Call
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Lưu ý: Chỉ hỗ trợ sdt 10 số
 */

int sms_sent_index; 

int find_char(String s,char c,int count_c)
{
  int count = 0;
  for(int i=0;i<s.length();i++)
  {
    if(s[i] == '\n')return -1;
    if(s[i] == c)
    {
      count++;
      if(count == count_c)
      {
        return i;
      }
    }
  }
  return -1;
}

typedef void (*sms_callback_t)(String phone, String sms, String time_sms);
sms_callback_t sms_callback_f;

typedef void (*call_callback_t)();
call_callback_t call_callback_f;
void call_set_callback(call_callback_t __callback)
{
  call_callback_f = __callback;
}

String ucs2_hex_to_ascii_simple(const char *ucs2_hex) {
  String out;
  while (*ucs2_hex) {
    // Đọc 4 ký tự hex = 2 byte UCS2 BE
    if (!ucs2_hex[0] || !ucs2_hex[1] || !ucs2_hex[2] || !ucs2_hex[3]) break;

    // Lấy byte thấp
    char lo_byte_str[3] = {ucs2_hex[2], ucs2_hex[3], 0};
    int lo_val = strtol(lo_byte_str, NULL, 16);

    if (lo_val >= 0x20 && lo_val <= 0x7E) {
      out += (char)lo_val;  // ASCII in được
    } else if (lo_val == 0x0A) {
      out += '\n'; // xuống dòng
    } else if (lo_val == 0x0D) {
      out += '\r'; // CR
    } else if (lo_val == 0x09) {
      out += '\t'; // TAB
    } else {
      out += '?';  // ngoài ASCII -> ?
    }

    ucs2_hex += 4; // sang cặp UCS2 kế tiếp
  }
  return out;
}

void sms_call_handle(String cmd) //tay cầm sms
{
  if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'M') && (cmd[3] == 'T'))
  {
    if(cmd.indexOf("+CMT: \"") != -1)
    {
       int last = find_char(cmd,'"',2);
       String phone = cmd.substring(7,last);
       int firt_c = find_char(cmd,'"',5);
       int last_c = find_char(cmd,'"',6);
       String time_sms = cmd.substring(firt_c+1,last_c);
       delay(5);get_data_to_buffer();
       String mess = read_to_CRLF();
       if(sms_callback_f != 0)
        {
          String out_phone,out_sms;
          out_phone = ucs2_hex_to_ascii_simple(phone.c_str());
          out_sms = ucs2_hex_to_ascii_simple(mess.c_str());
          sms_callback_f(out_phone,out_sms,time_sms);
        }
       return;
    }
  }
  else if((cmd[0] == 'R') && (cmd[1] == 'I') && (cmd[2] == 'N') && (cmd[3] == 'G') && (cmd[4] == 0x0D) && (cmd[5] == 0x0A))
  {
    if(call_callback_f != 0)call_callback_f();
  }
  else if((cmd[0] == '+') && (cmd[1] == 'C') && (cmd[2] == 'M') && (cmd[3] == 'G') && (cmd[4] == 'S'))
  {
    sms_sent_index = getNumByStartIndex(cmd,7).toInt();
    #ifdef SIM_ENABLE_LOG 
    Serial.println("SMS sent:" + String(sms_sent_index));
    #endif
  }
}
void sms_set_callback(sms_callback_t __callback)
{
  static char flag = 0;
  if(flag == 0) //để đảm bảo đoạn code này chạy 1 lần duy nhất
  {
    flag = 1;
    delay(10);SIM_SERIAL.println("AT+CMGF=1");
    delay(100);SIM_SERIAL.println("AT+CNMI=2,2,0,0,0");
    delay(100);SIM_SERIAL.println("AT+CMGD=,1");
    delay(100);SIM_SERIAL.println("AT+CSCS=\"UCS2\"");
    delay(100);SIM_SERIAL.println("AT+CSMP=17,167,0,8");
    SIM7600_add_handle(sms_call_handle); //thêm trình xử lí sms vào
  }
  sms_callback_f = __callback;
}
bool utf8_to_ucs2_be_hex(const char* utf8, String &outHex) {
  outHex = "";  // Xóa dữ liệu cũ
  bool ok = true;

  const uint8_t *p = (const uint8_t*)utf8;
  while (*p) {
    uint32_t cp;
    uint8_t c = *p++;

    if (c < 0x80) {
      // 1-byte ASCII
      cp = c;
    } else if ((c >> 5) == 0x6) {
      // 2-byte UTF-8
      if (!(*p & 0x80)) { cp = 0x003F; ok = false; continue; }
      cp = ((c & 0x1F) << 6) | (*p & 0x3F);
      p++;
    } else if ((c >> 4) == 0xE) {
      // 3-byte UTF-8
      if (!(*p & 0x80) || !(p[1] & 0x80)) { cp = 0x003F; ok = false; p++; continue; }
      cp = ((c & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F);
      p += 2;
    } else if ((c >> 3) == 0x1E) {
      // 4-byte UTF-8 (ngoài UCS2)
      if (!(*p & 0x80) || !(p[1] & 0x80) || !(p[2] & 0x80)) { cp = 0x003F; ok = false; p++; continue; }
      cp = 0x003F; // thay bằng '?'
      p += 3;
      ok = false;
    } else {
      cp = 0x003F;
      ok = false;
    }

    // Loại trừ vùng surrogate
    if (cp >= 0xD800 && cp <= 0xDFFF) { cp = 0x003F; ok = false; }
    if (cp > 0xFFFF) { cp = 0x003F; ok = false; }

    // Ghi dạng HEX big-endian vào chuỗi
    char buf[5];
    sprintf(buf, "%04X", (unsigned int)cp);
    outHex += buf;
  }
  return ok;
}

void sms_sent(String phone,String sms)
{
  String ucs2;
  utf8_to_ucs2_be_hex(phone.c_str(),ucs2);
  SIM_SERIAL.print("AT+CMGS=\"" + ucs2 + "\"\r");delay(50);  Serial.println(ucs2);
  utf8_to_ucs2_be_hex(sms.c_str(),ucs2);                      Serial.println(ucs2);
  SIM_SERIAL.print(ucs2);
  SIM_SERIAL.write(0x1A);
}
int sms_sent_andWait(String phone,String sms,int timeout)
{
  sms_sent_index=-1;
  sms_sent(phone,sms);
  uint32_t t = millis();
  while(1)
  {
    if((millis() - t ) > timeout)
    {
      return sms_sent_index;
    }
    delay(10);
    SIM7600_loop();
    if(sms_sent_index != -1)
    {
      return sms_sent_index;
    }
  }
}
void call_calling(String phone)
{
  SIM_SERIAL.println("ATD"+phone+";");
}
#endif

