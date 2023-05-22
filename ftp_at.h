#ifndef __IOT47_SIM7600_FTP_API__
#define __IOT47_SIM7600_FTP_API__

/* Thư viện FTP ( mới hỗ trợ download file )
 * Viết bởi daonguyen IoT47.com
 * Email: daonguyen20798@gmail.com
 * Phone: 0394733311
 * Bạn phải tắt tất cả các kết nối stream như mqtt hoặc firebase trong lúc download
 * Hàm ftp_download sử dụng cache của module sim để download, nó có thể k hoạt động với module sim 7602E -H
 */

#include "LittleFS.h"
uint8_t downLoadBuffer[1024];

typedef void (*ftp_downloading_t)(uint32_t current, uint32_t total);
ftp_downloading_t ftp_downloading_f;
void ftp_downloading_callback(ftp_downloading_t _callback)
{
  ftp_downloading_f = _callback;
}

//sysnc
int ftp_download(String FTP_server,String FTP_usename,String FTP_password,String filePath, String fileSavePath)
{
  startAT_SYNC_mode();
  
  SIM_SERIAL.println("AT+CFTPSABORT");whileReponse(500);
  String rx = simGetRecever("OK","ERROR");
  if(rx == "OK")dumpSimReponse(5000);
  
  SIM_SERIAL.println("AT+CFTPSLOGOUT");whileReponse(500);
  rx = simGetRecever("OK","ERROR");
  if(rx == "OK")dumpSimReponse(5000);
  
  SIM_SERIAL.println("AT+CFTPSSTOP"); whileReponse(500);
  rx = simGetRecever("OK","ERROR");
  if(rx == "OK")dumpSimReponse(5000);
  
  #ifdef SIM_ENABLE_LOG
  Serial.println("[LOG]Connect ftp");
  #endif
  SIM_SERIAL.println("AT+CFTPSSTART");
  if(whileReponseString("+CFTPSSTART: 0",5000) == "")
  {
    #ifdef SIM_ENABLE_LOG
    Serial.println(F("Fail when start ftp"));
    #endif
    return stopAT_SYNC_mode(1);
  }
  String ATCOMMAND = "AT+CFTPSLOGIN=\"" + FTP_server + "\"," + String(21) + ",\"" + FTP_usename + "\",\"" + FTP_password + "\",0";
  SIM_SERIAL.println(ATCOMMAND);
  if(whileReponseString("+CFTPSLOGIN: 0",5000) == "")
  {
    #ifdef SIM_ENABLE_LOG
    Serial.println(F("Fail when login ftp"));
    #endif
    return stopAT_SYNC_mode(1);
  }
  
  SIM_SERIAL.println("AT+CFTPTYPE=I");dumpSimReponse();
  ATCOMMAND = "AT+CFTPSSIZE=\"" + filePath + "\"";
  SIM_SERIAL.println(ATCOMMAND);
  rx = whileReponseString("+CFTPSSIZE:",5000);
  if(rx == "")
  {
    #ifdef SIM_ENABLE_LOG
    Serial.println(F("File not exit"));
    #endif
    return stopAT_SYNC_mode(2);
  }
  //lấy ra file size
  uint32_t FILESIZE =  getNumWithHeader(rx,"+CFTPSSIZE:").toInt();
  #ifdef SIM_ENABLE_LOG
  Serial.print(F("File size: ")); Serial.println(FILESIZE);
  #endif
  
  //ra lệnh tải file
  ATCOMMAND = "AT+CFTPSGET=\"" + filePath + "\",0,1";
  SIM_SERIAL.println(ATCOMMAND);
  if(simCheckRecever_AndWait_CheckRecever("OK",1000,"+CFTPS: RECV EVENT",10000) == 1)
  {
    #ifdef SIM_ENABLE_LOG
    Serial.println(F("Bắt đầu get data"));
    #endif

    //mout phân vùng và forrmat nếu cần
    if(!LittleFS.begin(true))
    {
      #ifdef SIM_ENABLE_LOG
      Serial.println(F("Formating ..."));
      #endif
      LittleFS.format();
      if(!LittleFS.begin(true))
      {
        #ifdef SIM_ENABLE_LOG
        Serial.println(F("Formating error"));
        #endif
        return stopAT_SYNC_mode(4);
      }
    }

    File file;
    if(LittleFS.exists(fileSavePath))LittleFS.remove(fileSavePath);
    file = LittleFS.open(fileSavePath,"w");
    if(!file)
    {
      #ifdef SIM_ENABLE_LOG
      Serial.println(F("Open file error"));
      #endif
      return stopAT_SYNC_mode(4);
    }

    
    int readLength = 0;
    int failCount = 0;
    int demProces=0; 
    while(readLength < FILESIZE)
    {
      ClearSimReponse();//làm sạch hoàn toàn bộ đệm để tránh lỗi linh tinh
      SIM_SERIAL.println("AT+CFTPSCACHERD");
      // lệnh này sẽ phản hồi về +CFTPSGET: DATA,1024\r\n
      // cần lấy được độ dài 1024, data sẽ bắt đầu ngay sau đó

      uint32_t t = millis();
      while(((millis() - t) < 10000) && (SIM_SERIAL.available() == 0)); // chờ cho tới khi có dữ liệu tới hoặc bị timeout
      delay(1);
      rx = "";
      char flag = 0;
      while(SIM_SERIAL.available()) //đọc phần +CFTPSGET: DATA,1024 
      {
         char c = (char)SIM_SERIAL.read();
         if(c == '+')flag=1;
         if(flag==1)
         {
           rx += c;
           if(c == 0x0A)break;
         }
      }
      if(rx.indexOf("+CFTPSGET: DATA,") != -1 )
      {
        int lenData = getNumWithHeader(rx,"+CFTPSGET: DATA,").toInt();
        int dem=0;
        t = millis();
        while(dem<lenData)
        {
          if(SIM_SERIAL.available())
          {
            downLoadBuffer[dem] = (uint8_t)SIM_SERIAL.read();
            dem++; if(dem>=lenData)break;
          }
          if(millis() - t > 1000)
          {
            #ifdef SIM_ENABLE_LOG
            Serial.println(F("Lỗi khi đang download"));
            #endif
            return stopAT_SYNC_mode(5);
          }
        }
        readLength+=dem;
        file.write(downLoadBuffer,dem);
        demProces++;
        if(demProces > 10)
        {
          demProces = 0;
          #ifdef SIM_ENABLE_LOG
          //String proces = String((float)readLength/(float)FILESIZE*100.0) + "%";
          //Serial.println(proces);
          #endif
          if(ftp_downloading_f !=0 )ftp_downloading_f(readLength,FILESIZE);
        }
      }
    }
    #ifdef SIM_ENABLE_LOG
    Serial.print(F("Tải xuống hoàn tất, tổng dung lượng: "));  Serial.println(readLength);
    #endif
    file.close();
    return stopAT_SYNC_mode(0);
//        file = LittleFS.open(nameFileFW,"r");
//        if(file)performUpdate(file, file.size());
  }
    
  
}

#endif