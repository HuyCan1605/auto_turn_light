#include <virtuabotixRTC.h>
virtuabotixRTC myRTC(13, 12, 11);
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 16, 2); có thể thêm LCD để theo dõi
unsigned long timeToResetIR = 0;
// unsigned long timeToTurnLCD = 0;
short lIR = 8; // hồng ngoại bên trái
short rIR = 7; // hồng ngoại bên phải
short led = 5; // rơ le điều khiển đèn
short button = 2; 
short countPeople = 0; 
short turnLight = 2; 
bool l, r; // tín hiệu của hồng ngoại trái và phải. 0 là có vật cản, 1 là bình thường
bool leftMotion1, leftMotion2, leftMotion3, rightMotion1, rightMotion2, rightMotion3; // thứ tự di chuyển
bool lightStatus = LOW;
bool buttonStatus = 1;
bool lastButtonStatus = 1;
bool allowTurnLight = 0; // biến để nhận biết có được bật đèn hay không
// bool LCDstatus = 0;


void setup() {
  pinMode(lIR, INPUT);
  pinMode(rIR, INPUT);
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP); 
  // lcd.begin(16,2);
  // lcd.backlight();
  // attachInterrupt(digitalPinToInterrupt(button), button_ISR, CHANGE); 
  // Serial.begin(9600);
  myRTC.setDS1302Time(30, 6, 14, 4, 26, 2, 2022); 
}

void condition_in_room(){ // số người trong phòng chỉ có thể bằng 0
  if(countPeople < 0)
  countPeople = 0;
}

void detect_motion(){
  l = digitalRead(lIR); // đọc tín hiệu hồng ngoại trái
  r = digitalRead(rIR); // đọc tín hiệu hồng ngoại phải

  /* đi ra ngoài, trái > phải
    theo thứ tự là:
    0. l = 0, r = 1 (bắt đi vào chạm hồng ngoại trái)
    1. l = 0, r = 0 (đi vào chạm cả 2)
    2. l = 1, r = 0 (chạm hồng ngoại phải)
    3. l = 0, r = 0 (đã đi ra ngoài)

    vì khi đi vào hồng ngoại, tín hiệu bắt được sẽ được đưa 
    vào vòng lặp để kiểm tra cho thứ tự đi tiếp theo nên các 
    công việc khác sẽ không được thực hiện. Vì vậy nên có 
    "millis() - timeToResetIR >= 2000" để kiểm tra, nếu sau 2 giây
    mà không bắt được tín hiệu tiếp thì quay về ban đầu.
  */
  if (l < r){
    // Serial.println("leftMotion");
    timeToResetIR = millis();   // khởi tạo thời gian bắt đầu đi vào hồng ngoại trái 
    while(true){
      if (digitalRead(lIR) == 0 && digitalRead(rIR) == 0){
        leftMotion1 = 1;
        // Serial.println("leftMotion1");
        break;
      }
      if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
    }
    if (leftMotion1 == 1){
      timeToResetIR = millis();
      while(true){
        if (digitalRead(lIR) > digitalRead(rIR)){
        leftMotion2 = 1;
        // Serial.println("leftMotion2");
        break;
        }
        if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
      }
    }
    if (leftMotion2 == 1){
      timeToResetIR = millis();
      while(true){
        if (digitalRead(lIR) == 1 && digitalRead(rIR) == 1){
        // Serial.println("leftMotion3");
        leftMotion3 = 1;
        break;
        }
        if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
      }
    }
    if (leftMotion3 == 1){
      --countPeople; // số người trong phòng giảm
      condition_in_room();
      if(lightStatus == HIGH){ // nếu đèn sáng thì tắt, và số ng trong phòng = 0 thì tắt đèn 
        switch_light(); 
        // LCDstatus = 1; 
        // timeToTurnLCD = millis();
        }
      // Serial.println("left motion detect");
      leftMotion1 = 0; // reset thứ tự chuyển động
      leftMotion2 = 0;
      leftMotion3 = 0;
    }
  }

  /* đi ra ngoài, phải -> trái
    theo thứ tự là:
    0. r = 0, l = 1 (bắt đi vào chạm hồng ngoại phải)
    1. r = 0, l = 0 (đi vào chạm cả 2)
    2. r = 1, l = 0 (chạm hồng ngoại trái)
    3. r = 0, l = 0 (đã đi ra ngoài)
  */
  if (r < l){
    Serial.println("rightMotion");
    timeToResetIR = millis();
    while(true){
      if (digitalRead(lIR) == 0 && digitalRead(rIR) == 0){
        // Serial.println("rightMotion1");
        rightMotion1 = 1;
        break;
      }
      if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
    }
    if(rightMotion1 == 1){
      timeToResetIR = millis();
      while(true){
        if (digitalRead(lIR) < digitalRead(rIR)){
        // Serial.println("rightMotion2");
        rightMotion2 = 1;
        break;
        }
        if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
      }
    }
    if (rightMotion2 == 1){
      timeToResetIR = millis();
      while(true){
        if (digitalRead(lIR) == 1 && digitalRead(rIR) == 1){
        rightMotion3 = 1;
        // Serial.println("rightMotion3");
        break;
        }
        if((unsigned long)(millis() - timeToResetIR >= 2000)) break;
      }
    }
    if (rightMotion3 == 1){
      ++countPeople; // tăng số lượng người trong phòng
      if(allowTurnLight == 1) { // nếu trong khoảng thời gian được bật đèn thì sẽ bật
        switch_light();
        // LCDstatus = 1; 
        // timeToTurnLCD = millis();
        }
      // Serial.println("right motion detect");
      rightMotion1 = 0;
      rightMotion2 = 0;
      rightMotion3 = 0;
    }
  }
  delay(50); // debound
}

void switch_light(){ //hàm xét điều kiện bật đèn
  if(countPeople > 0){ 
    lightStatus = HIGH;
  }else{
    lightStatus = LOW;
  }
  digitalWrite(led, lightStatus);
}

// void button_ISR(){
//   buttonStatus = 1;
// }

// void switch_by_button(){
//   if(buttonStatus == 1){
//     lightStatus = !lightStatus;
//     digitalWrite(led, lightStatus);
//     buttonStatus = 0;
//   }
// }

void switch_by_button(){ // hàm dùng nút bấm bật đèn
  // dùng công tắc bập bênh
  buttonStatus = digitalRead(2);
    if(buttonStatus != lastButtonStatus){ // nếu trạng thái nút bấm thay đổi
    lightStatus = !lightStatus; // thay đổi trạng thái đèn
    digitalWrite(led, lightStatus);
    lastButtonStatus = buttonStatus; //cập nhật trạng thái nút bấm
    delay(50);
    }
}




void set_time_turn_light(){ // hàm xét điều kiện thời gian để bật đèn
  if(myRTC.seconds == 0 && myRTC.minutes == 0 &&  myRTC.hours == 18){ // nếu lúc 6h tối mà trong phòng có người sẽ tự bật đèn
    turnLight = 1; 
  }else if(myRTC.seconds == 0 && myRTC.minutes == 0 &&  myRTC.hours == 6){// nếu lúc 6h sáng mà đèn sáng thì sẽ được cho tắt
    turnLight = 0;
  }else{
    turnLight = 2;
  }

  if(myRTC.hours > 6 && myRTC.hours < 18){ // nếu trong khoảng thời gian từ 6h sáng đến 6h tối tức là ban ngày không được bật đèn
    allowTurnLight = 0;
  }else{
    allowTurnLight = 1;
  }
}


// void LCD_display(){
//   lcd.setCursor(0, 0);              
//   lcd.print(myRTC.hours); 
//   lcd.print(":");

//   lcd.setCursor(3, 0);        
//   lcd.print(myRTC.minutes);
//   lcd.print(":");

//   lcd.setCursor(6, 0);         
//   lcd.print(myRTC.seconds);
//   lcd.print("|");

//   lcd.setCursor(10, 0);         
//   lcd.print(countPeople);  


//   lcd.setCursor(0, 1);          
//   lcd.print("T");        
//   lcd.setCursor(1, 1);        
//   lcd.print(myRTC.dayofweek);
//   lcd.print("/");

//   lcd.setCursor(3, 1);     
//   lcd.print(myRTC.dayofmonth);
//   lcd.print("/");

//   lcd.setCursor(6, 1);     
//   lcd.print(myRTC.month);
//   lcd.print("/");

//   lcd.setCursor(9, 1);     
//   lcd.print(myRTC.year);
// }

void loop() {
  // put your main code here, to run repeatedly:
  set_time_turn_light();
  detect_motion();
  if (turnLight == 1){
    switch_light();
  } else if(turnLight == 0){
    digitalWrite(led, LOW);
  }
  myRTC.updateTime();
  switch_by_button();
  // if(LCDstatus = 1){
  //   LCD_display();
  //   // if(millis() - timeToTurnLCD >= 15000){
  //   //   LCDstatus = 0;
  //   //   lcd.clear();
  //   //   lcd.noBacklight();
  //   //   lcd.noDisplay();
  //   // }
  // }
  
}
