# auto_turn_light
tự động bật đèn mà không cần bấm nút với điều kiện phòng không có cửa giống phòng tôi hiện tại

1. Dụng cụ: 
- 2 con hồng ngoại IR MH sensor series Flying fish 3 chân.
- 1 con mh real time clock module. đọc thêm ở đây https://miliohm.com/ds1302-rtc-with-arduino-tutorial/ (mình cũng lấy thư viện ở đây)
- 1 relay
- 1 arduino uno
- 1 công tắc bập bênh

2. Công dụng:
 Hệ thống sẽ tự động bật đèn khi có người đi vào mà không cần bật công tắc.
 Khi người đi vào thì hệ thống cũng đếm, khi số người = 0 thì đèn tắt.
 Cũng có thể dùng công tắc để bật tắt đèn. 
 Trong khoảng thời gian ban ngày từ 6h sáng đến 6h tối, hệ thống vẫn chạy để đếm người trong phòng tuy nhiên cảm biến sẽ không kích hoạt đèn (có thể bật tắt bằng côgn tắc)
 Trong phòng mà có người, thì đến lúc 6h tối hệ thống sẽ tự kích hoạt đèn, còn lúc 6h sáng mà đèn vẫn sáng thì hệ thống sẽ tự tắt.
