#ifndef __IOT47_SIM7600_OTA_API__
#define __IOT47_SIM7600_OTA_API__

/* Thư viện OTA qua HTTP
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Bạn phải tắt tất cả các kết nối stream như mqtt hoặc firebase trong lúc download
 */

static bool muiltil_part_mode = false;
void ota_enable_mutil_part(bool mode) //true nếu bạn muốn chia file để tải nhiều lần, bạn cần chủ động gọi Update.begin()
{
  muiltil_part_mode=mode;
}

typedef void (*ota_downloading_t)(uint32_t current, uint32_t total);
ota_downloading_t ota_downloading_f;
void ota_downloading_callback(ota_downloading_t _callback)
{
  ota_downloading_f = _callback;
}
uint8_t downLoadBuffer[1024];
int ota_start(String ota_link)
{
   SIM_SERIAL.println("AT+HTTPINIT");delay(100);
   SIM_SERIAL.println("AT+HTTPPARA=\"URL\",\"" + ota_link + "\"");delay(100);ClearSimReponse();
   SIM_SERIAL.println("AT+HTTPACTION=0");
   String rx = endline_waitString("OK",2000);
   if(rx != "")
   {
        Serial.println("Download...");
        rx = endline_waitString("+HTTPACTION: 0,200,",100000);
        if(rx != "")
        {
           int index = rx.indexOf("+HTTPACTION: 0,200,");
           int payload_len = getNumByStartIndex(rx,index+19).toInt();   
           Serial.println("payload_len: " + String(payload_len));
           if(!muiltil_part_mode)Update.begin(payload_len); // phía code user tự quản lí hàm này khi ở chế độ chia nhiều phần để tải về
           uint32_t demProces=0;
           uint32_t readLength = 0;
              while(readLength < payload_len)
              {
                uint32_t frame_read = 1024;
                if(readLength + 1024 > payload_len)frame_read=payload_len-readLength;
                SIM_SERIAL.println("AT+HTTPREAD=" + String(frame_read));
                rx = endline_waitString("+HTTPREAD: " + String(frame_read),2000);
                if(rx != "")
                {
                  int dem=0;
                  while(1)
                  {
                    while(SIM_SERIAL.available())
                    {
                      uint8_t c = (uint8_t)SIM_SERIAL.read();
                      downLoadBuffer[dem] = c;
                      dem++; if(dem>=frame_read)
                      {
                        Update.write(downLoadBuffer, dem);
                        goto thoat;
                      }
                    }
                    delay(0);
                  }
                  thoat:;
                  readLength+=dem;
                  demProces++;
                  if(demProces > 10)
                  {
                    demProces = 0;
                    if(ota_downloading_f!=0)ota_downloading_f(readLength,payload_len);
                  }
                  delay(0);
                }
                else 
                {
                  return 2;
                }
              }
              #ifdef SIM_ENABLE_LOG
              Serial.print(F("Tải xuống hoàn tất, tổng dung lượng: "));  Serial.println(readLength);
              #endif
              return 0;
        }
   }
   return 1;
}

#endif
