# IOT47_ESP_SIM7600
Thư viện LTE SIMCOM cho ESP32
Các giap tiếp hỗ trợ gồm:
- SMS
- CALL
- MQTT(s)
- Firebase
- HTTP(s)
- NTP
- FTP (file tranfer)
- OTA

Sử dụng: Xem ví dụ

------------------------------------------------------------
Cơ chế của thư viện:
Đây là thư viện giao tiếp với module dòng 76XX, nó được thiết kế chủ yếu theo cơ chế bất đồng bộ với các cuộc gọi lại (Callback)
Cơ chế hoạt động:
Thư viện tách biệt hoàn toàn tầng low layer ( cơ chế AT) với tầng ứng dụng thông qua các trình xử lí được add vào thông qua hàm SIM7600_add_handle(user_handle);
Ngoài ra user còn có thể add thêm trình xử lí loop qua hàm SIM7600_add_user_loop(user_loop);

low level sẽ xử lí các tin nhắn AT của module sim gửi ra và gọi các trình xử lí đã được đăng kí tương ứng
Ví dụ: khi 1 tin nhắn kiểu như +HTTPACTION: thì với tiêu đề +HTTPACTION thư viện sẽ gọi trình xử lí http ( tương tự với các chức năng khác)
Bạn có thể thêm các trình xử lí khác vào miễn là bạn biết các tin nhắn AT hoạt động như nào

lowlevel sử dụng 1 ring buffer để lưu trữ các tin nhắn AT để xử lí dần. Tuy nhiên nếu SIM7600_loop(); không được gọi thường xuyên mà bị trễ quá lâu ( do code của bạn dùng delay) thì ringbuffer có thể bị tràn và làm lỗi giao tiếp. Do đó hay hạn chế sử dụng delay trong main loop của bạn nhé !
