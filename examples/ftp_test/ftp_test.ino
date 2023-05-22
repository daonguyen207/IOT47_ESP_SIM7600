#define SIM_SERIAL         Serial2  //khai báo serial cho module sim
#define SIM_ENABLE_LOG              //bật cái này nếu muốn in log thư viện
//#define SIM_LOG_RX_UART           //bật cái này nếu muốn in toàn bộ data nhận được từ uart

#include "sim76xx_api.h"
#include "ftp_at.h"

/*  Note
 *  1.Trong hàm setup gọi SIM7600_on_lte_ready_callback để đăng kí sự kiện LTE sẵn sàng
 *  2.Dùng ftp_downloading_callback; để đăng kí nhận tiến độ tải về
 *  3.Dùng hàm ftp_download để bắt đầu tải xuống, lưu ý: Hàm này là đồng bộ, nó sẽ block toàn bộ chương trình cho tới khi tải xong
 *   Tham số:  ftp_download(ftp_server,ftp_username,ftp_password,ftp_filename.ftp_filesavename);
 *    - Chú ý: ftp_filename là đường dẫn tới file cần tải, nó luôn bắt đầu bằng /
 *             ftp_filesavename là tên file lưu trong snippf của esp, nó cũng luôn bắt đầu bằng /
 *   Trả về: 0 = ok, 1 2 3 4 5 là lỗi
 *  4. Chưa support chức năng tải lên, thông cảm nhé :(
 *  Nếu có bất kì vấn đề gì, vui lòng tạo Issues trên https://github.com/daonguyen207/IOT47_SIM76XX để được hỗ trợ giải quyết
 */

 //các thông số này bạn phải tự điền vào
 #define FTP_SERVER           ""
 #define FTP_USERNAME         ""
 #define FTP_PASSWORD         ""
 #define FTP_FILE_IN_SERVER   "/V100.bin"
 #define FTP_FILE_IN_ESP      "/V100.bin"

bool LTE_OK = false;

void FTP_downloadding(uint32_t current, uint32_t total)
{
  Serial.println(String(current) + "/" + String(total));
  String proces = String((float)current/(float)total*100.0) + "%";
  Serial.println(proces);
}
void SIM_on_lte(void *arg)
{
  LTE_OK = true;
  Serial.println("Đã có mạng");
  ftp_downloading_callback(FTP_downloadding);
  int err = ftp_download(FTP_SERVER,FTP_USERNAME,FTP_PASSWORD,FTP_FILE_IN_SERVER,FTP_FILE_IN_ESP);
  Serial.println("ftp_download: " + String(err));
}
void SIM_on_lte_fail(void *arg)
{
  LTE_OK = false;
  Serial.println("Đã mất mạng");
}
void SIM_on_ready(void *arg)
{
  Serial.println("Modul sim ready");
}

void setup() 
{
  //nên nối thêm chân reset để reset cứng moudle sim khi khởi động ở đây
  
  Serial.begin(115200);
  SIM_SERIAL.setRxBufferSize(2048);
  SIM_SERIAL.begin(115200);
  SIM7600_on_ready_callback(SIM_on_ready);
  SIM7600_on_lte_ready_callback(SIM_on_lte);
  SIM7600_on_lte_fail_callback(SIM_on_lte_fail);
}
void loop() 
{
  SIM7600_loop();
}
