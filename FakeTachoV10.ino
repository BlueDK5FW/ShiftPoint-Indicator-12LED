//Shift Point Indicator(Fake Tachometer) using a Speedpulse and a Accelerometer
//2016/7/29 BlueDK5FW
#include <MsTimer2.h>
#define P_HI1 170 //Pulse 分岐値
#define GREEN1 2 //GREEN LED 1 の出力ピン番号
#define GREEN2 3 //GREEN LED 2 の出力ピン番号
#define GREEN3 4 //GREEN LED 3 の出力ピン番号
#define GREEN4 5 //GREEN LED 4 の出力ピン番号
#define RED1 6 //RED LED 1 の出力ピン番号
#define RED2 7 //RED LED 2 の出力ピン番号
#define RED3 8 //RED LED 3 の出力ピン番号
#define RED4 9 //RED LED 4 の出力ピン番号
#define RED5 10 //RED LED 5 の出力ピン番号
#define BLUE1 11 //BLUE LED 1 の出力ピン番号
#define BLUE2 12 //BLUE LED 2 の出力ピン番号
#define BLUE3 13 //BLUE LED 3 の出力ピン番号
#define SPEED_PIN 4 //車速Pulseの入力ピン番号
#define ACCX 3 //加速度センサーX軸の入力ピン番号
#define ACCY 5 //加速度センサーY軸の入力ピン番号
#define ACCZ 7 //加速度センサーZ軸の入力ピン番号
#define BAUD 9600 //Serial通信 BAUDレート
#define TIME0 5 //Time0(msec)
#define TIME1 10 //Time1(msec)
#define TIME2 25 //Time1(msec)
#define TIME3 33 //Time3(msec)
#define TIME4 100 //Time4(msec)
#define TIME5 1500 //Time5(msec)
#define TIME6 3000 //Time6(msec)
#define DEAD 3.0 //静止時のY軸デッドゾーン
#define MAX_Y 402 //★1G時のY軸
#define MIN_Y 265 //★-1G時のY軸 402-265=137
#define STOP_Y 323.0000  //★0G時のY軸☆（停止時に値が321-324だったので真ん中）
#define AGGRESSIVE_ACC 343.0000 //★323+20
#define AGGRESSIVE_BRK 303.0000 //★323-20
#define NORMAL_ACC 331.5625 //★323+8.5625 (137/16=8.5625)
#define NORMAL_BRK 314.4375 //★323-8.5625
#define STEP_RPM 411.1111 //rpmSTEP値 ((REV4500回転 - IDLE800回転) / LED9間隔))
#define IDLE_RPM 800 //アイドリング回転数
#define MAX_RPM 4500 //Rev回転数
#define UP_RPM 1600 //ShiftUp回転数
#define SPEED1 17 //1速→2速チェンジ時のスピード（アクセル通常開度）
#define SPEED2 32 //2速→3速チェンジ時のスピード
#define SPEED3 46 //3速→4速チェンジ時のスピード
#define SPEED4 66 //4速→5速チェンジ時のスピード
#define SPEED5 80 //5速→6速チェンジ時のスピード
#define GEAR1 3.552 //1速変速比 3.552 最終減速比 13.54
#define GEAR2 2.022 //2速 2.022  7.708
#define GEAR3 1.452 //3速 1.452  5.535
#define GEAR4 1.000 //4速 1.000  3.812
#define GEAR5 0.708 //5速 0.708  2.699
#define GEAR6 0.599 //6速 0.599  2.283
#define FINAL 3.812 //Final 3.812
#define OFFSET_X 0 //車載時の水平X軸オフセット
#define OFFSET_Y 0 //車載時の水平Y軸オフセット
#define OFFSET_Z 0 //車載時の垂直Z軸オフセット
////////  Global Scope  /////////////////////////////////////////////////////////////////////////////////////
unsigned int i,gear,maxFlg,flagHi,blinkFlg;
unsigned long loopCnt,beforeTime,elapsedTime;
float lastY,last2Y,last3Y,speed,carSpeed,pulse,gearRatio;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initialBlink(){    // like a NightRider
  for (i = 2; i < 15; i++){
   if (i != 14) {
    digitalWrite(i,HIGH);
   }
   if (i != 2) {
    digitalWrite(i-1,LOW);
   }
    delay(TIME3);    //33ms
  }
  for (i = 14; i > 1; i--){
   if (i != 2) {
    digitalWrite(i-1,HIGH);
   }
   if (i != 14) {
    digitalWrite(i,LOW);
   }
    delay(TIME3);
  }
  maxBlink();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void acceleroBlink(){
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
    delay(TIME3);    //33ms
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void maxBlink(){
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME4);    //100ms
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
  }
  delay(TIME4);
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME4);
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
  }
  delay(TIME4);
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME4);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  beforeTime = elapsedTime = 0;
  i = maxFlg = flagHi = loopCnt = blinkFlg = 0;
  lastY = last2Y = last3Y = speed = carSpeed = pulse = 0.0;
  gear = 1 ;
  gearRatio = GEAR1 ;
  Serial.begin(BAUD);
  pinMode(GREEN1,OUTPUT);
  pinMode(GREEN2,OUTPUT);
  pinMode(GREEN3,OUTPUT);
  pinMode(GREEN4,OUTPUT);
  pinMode(RED1,OUTPUT);
  pinMode(RED2,OUTPUT);
  pinMode(RED3,OUTPUT);
  pinMode(RED4,OUTPUT);
  pinMode(RED5,OUTPUT);
  pinMode(BLUE1,OUTPUT);
  pinMode(BLUE2,OUTPUT);
  pinMode(BLUE3,OUTPUT);
  for (i = 2; i < 14; i++) {
    digitalWrite(i,LOW);
  }
  delay(TIME5);    //始動時は少し(1.5秒) 待つ
  initialBlink();
  MsTimer2::set(TIME3,calcSpeed);    //33ms毎に稼働
  MsTimer2::start();
}
void loop(){
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Loop Start Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int rpm;
  unsigned long nowTime;
  float x,y,z;
  rpm = 0;
  nowTime = 0;
  x = y = z = 0.0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               ACC_Y Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for (i = 0; i < 50; i++){         //加速度センサー値安定化（50回読込分を平均）
    y += analogRead(ACCY);
  }
  y /= 50;
  if (loopCnt && abs(y - lastY) > 20) {    //y値がたまに跳ねる時あり
    y = lastY;
  }
/*↓
  if (loopCnt % 30 == 0){ 
    Serial.print("Y:");
    Serial.print("\t");
    Serial.print(y);
    Serial.println("\t");
  }
//↑*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Pulse, Elapssed Time Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//車速信号：JIS規定では1kmごとに637回 * nパルスが発生
//4パルス車は、2548パルス per 1km
//時速1kmで走るとパルス幅は（1時間（3600秒）÷2548パルス=1.41287284144427秒）
//　→　時速は（1.41287284144427÷計測パルス幅）（レシプロカル）
  nowTime = millis();
  pulse = analogRead(SPEED_PIN);
/*↓
  if (loopCnt % 30 == 0){ 
  Serial.print("pulse:");
  Serial.print("\t");
  Serial.print(pulse);
  Serial.println("\t");
  }
//↑*/
  if (pulse > P_HI1 && !flagHi){      //パルスHIGH
    elapsedTime = nowTime - beforeTime; //前回パルスアップからの経過時間
    beforeTime = nowTime;
    flagHi = 1;
  }
  if (pulse <= P_HI1 && flagHi){      //パルスLOW
    flagHi = 0;
  }
  if(nowTime - beforeTime > TIME6){   //前回～今回3秒以上→0.5km以下
    elapsedTime = 0.0;
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Gear Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    減速比 総減速比 速度
//1速 3.552  13.54     0->16
//2速 2.022  7.708    17->31
//3速 1.452  5.535    32->45
//4速 1.000  3.812    46->65
//5速 0.708  2.699    66->79
//6速 0.599  2.283    80->180
//Final 3.812
//車速からギア比設定
  if (carSpeed >= 0 && carSpeed < SPEED1){
    gear = 1;
    gearRatio = GEAR1;
  } else if (carSpeed >= SPEED1 && carSpeed < SPEED2){
    gear = 2;
    gearRatio = GEAR2;
  } else if (carSpeed >= SPEED2 && carSpeed < SPEED3){
    gear = 3;
    gearRatio = GEAR3;
  } else if (carSpeed >= SPEED3 && carSpeed < SPEED4){
    gear = 4;
    gearRatio = GEAR4;
  } else if (carSpeed >= SPEED4 && carSpeed < SPEED5){
    gear = 5;
    gearRatio = GEAR5;
  } else if (carSpeed >= SPEED5){
    gear = 6;
    gearRatio = GEAR6;
  }
///*↓
//加速度からギア比補正
/*↓
  if (loopCnt % 30 == 0){ 
  Serial.print("Y:");
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  }
//↑*/
  if (y  > AGGRESSIVE_ACC){  //急加速時はシフトダウン
    if (gear == 1) {
    } else if (gear == 2) {
      gearRatio = GEAR1;
    } else if (gear == 3) {
      gearRatio = GEAR2;
    } else if (gear == 4) {
      gearRatio = GEAR3;
    } else if (gear == 5) {
      gearRatio = GEAR4;
    } else if (gear == 6){
      gearRatio = GEAR5;
    }
    gear--;
    if (gear < 1){
      gear = 1;
      gearRatio = GEAR1;
    }
  }  //END AGGRESSIVE_ACC
  if (y < AGGRESSIVE_BRK ){  //急減速時はシフトアップ
    if (gear == 1) {
      gearRatio = GEAR2;
    } else if (gear == 2) {
      gearRatio = GEAR3;
    } else if (gear == 3) {
      gearRatio = GEAR4;
    } else if (gear == 4) {
      gearRatio = GEAR5;
    } else if (gear == 5) {
      gearRatio = GEAR6;
    }
    gear++;
    if (gear > 6){
      gear = 6;
      gearRatio = GEAR6;
    }
  }  //END AGGRESSIVE_ACC
//↑*/
/*↓
  if (loopCnt % 30 == 0){ 
    Serial.print("gear:");
    Serial.print("\t");
    Serial.print(gear);
    Serial.print("\t");
  }
//↑*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               RPM Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 速度km/h ＝ 回転数rpm÷変速比÷最終減速比×（タイヤ外径mm×3.14÷1000）×60÷1000
// ex）3000 rpm / 0.599 / 3.812 * ( 685mm * 3.14 / 1000 ) * 60 / 1000 = 169.55km、 ex2）2000 rpm / 0.599 / 3.812 * ( 2.1509 ) * 60 / 1000 = 113.03km
// 回転数rpm ＝ 速度km/h×変速比×最終減速比 ÷ （タイヤ外径mm×3.14÷1000）÷60×1000 
  rpm = carSpeed * gearRatio * FINAL / 2.1509 / 60 * 1000;
  if (rpm < IDLE_RPM) {   //計算値がアイドリング（800回転）未満
    rpm = IDLE_RPM;
  }
  if (rpm > MAX_RPM) {   //計算値が4500回転より大
    rpm = MAX_RPM;
  }
///*↓
  if (rpm > UP_RPM) {  //一旦rpmを下げる（見た目上）
    rpm = IDLE_RPM;
  }
//↑*/
/*↓
  if (loopCnt % 5 == 0){
    Serial.print("rpm:");
    Serial.print("\t");
    Serial.print(rpm);
    Serial.print("\t");
  }
//↑*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               LED Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (y >= (STOP_Y - DEAD) && y <= (STOP_Y + DEAD)  && speed < 1.0) {  //yがデッドゾーンかつスピード1km未満
  } else {    //本当に止まっている時以外（DEAD=3.0）
    for  (i = 2; i < 5; i++){
      digitalWrite(i,HIGH);    //LED1～3をつける
    }
  }
  if (carSpeed >= 5) {  //5km以上
    for  (i = 5; i < 14; i++){  // LED4～
      if (rpm >= IDLE_RPM + ((i-2) * (STEP_RPM * 0.35))){  //点灯補正：右側まで光ってる所がみたいのでSTEP値到達の0.35倍の回転数でつけてる
        if (loopCnt % 7 == 0) {  //ちらつき防止
          digitalWrite(i,HIGH);
        }
        if (i == 13){
          maxFlg++;    //Max到達回数に加算
        }
      } else {    //STEPに満たない時
        if (loopCnt % 7 == 0) {
          digitalWrite(i,LOW);
        }
      }
    }    //END for
    if (maxFlg > 29) {      //30回Max到達でBlink
      maxBlink();
      maxFlg = 0;
    }
///*↓
    //acceleroBlink
    if (y > AGGRESSIVE_ACC && lastY > AGGRESSIVE_ACC && last2Y > AGGRESSIVE_ACC) {
      if (!blinkFlg) {
        acceleroBlink();    //急加速警告表示
        maxBlink();
        maxBlink();
        blinkFlg = 1;
      }
    }
//↑*/
  } else {    //5km未満
    if (y >= (STOP_Y - DEAD) && y <= (STOP_Y + DEAD)  && speed < 1.0) {    //本当に止まっている時（DEAD=3.0）
      digitalWrite(2,HIGH);
      for  (i = 3; i < 14; i++){    //LED2以降をけす
        digitalWrite(i,LOW);
      }
    }
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Loop End Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  last3Y = last2Y;
  last2Y = lastY;
  lastY = y;
  loopCnt++;
  if (loopCnt % 341 == 0) {  //acceleroBlinkは1度なったら5秒間はacceleroBlinkにしない
    blinkFlg == 0;
  }
}  //END loop()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Speed Section  (別スレッド)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calcSpeed(){
  if (elapsedTime) {
    speed = 1412.87284144427 / elapsedTime;    //3600秒÷(637*4パルス) = 1.41287284144427
///*↓
    if (speed <= 20 ){  //スピードメータと比べてspeedをマニュアル補正
      speed = speed / 1.1;
    } else if (speed > 20 && speed <= 30){
      speed = speed / 1.12;
    } else if (speed > 30 && speed <= 40){
      speed = speed / 1.16;
    } else if (speed > 40 && speed <= 50){
      speed = speed / 1.212;
    } else if (speed > 50 && speed <= 60){
      speed = speed / 1.485;
    } else if (speed > 60){  //60km以上は比較未済
      speed = speed / 1.7;
    }
//↑*/
    if (speed > 180){
      speed = 180;
    }
    if (speed < 0){
      speed = 0;
    }
  } else {     //!elapsedTime
    speed = carSpeed = 0;
  }  //END !elapsedTime
/*↓
  Serial.print("carSpeed:");
  Serial.print("\t");
  Serial.print(carSpeed);
  Serial.print("\t");
//↑*/
  carSpeed = speed;    //carSpeed（昔加速度から算出してた）に上書き
/*↓
  Serial.print("speed:");
  Serial.print("\t");
  Serial.print(speed);
  Serial.println("\t");
//↑*/
}  //END calcSpeed()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//以下古LogicBkup
/*↓    //走行中にacceleroBlinkになったらギアを上げる
  if (carSpeed > 1) {
    if (gear == 1){
      gear = 2;
      gearRatio = GEAR2;
    } else if (gear == 2){
      gear = 3;
      gearRatio = GEAR3;
    } else if (gear == 3){
      gear = 4;
      gearRatio = GEAR4;
    } else if (gear == 4){
      gear = 5;
      gearRatio = GEAR5;
    } else if (gear == 5){
      gear = 6;
      gearRatio = GEAR6;
    }
  }
//↑*/
/*↓
    if (rpm > 1900) {
      if (gear == 1){
        gear = 2;
        gearRatio = GEAR2;
        rpm = IDLE_RPM;
      } else if (gear == 2){
        gear = 3;
        gearRatio = GEAR3;
        rpm = IDLE_RPM;
      } else if (gear == 3){
        gear = 4;
        gearRatio = GEAR4;
        rpm = IDLE_RPM;
      } else if (gear == 4){
        gear = 5;
        gearRatio = GEAR5;
        rpm = IDLE_RPM;
      } else if (gear == 5){
        gear = 6;
        gearRatio = GEAR6;
        rpm = IDLE_RPM;
      }
    }
//↑*/
/*↓
    //実車36km ～41km(プルアップ抵抗装着前)対策→スピードを使わずLEDをつける
    if ((y >= NORMAL_ACC && lastY >= NORMAL_ACC && last2Y >= NORMAL_ACC && last3Y >= NORMAL_ACC) ||
       (y <= NORMAL_BRK && lastY <= NORMAL_BRK && last2Y <= NORMAL_BRK && last3Y <= NORMAL_BRK)) {    //過去3ループでYセンサーが通常加減速している時、LED1～LED5点灯
      for  (i = 2; i < 7; i++){  // LED1～5
        digitalWrite(i,HIGH);
      }
    }    //END if
//↑*/
/*↓
  if (y > MAX_Y || y < MIN_Y){  //グローバルスコープで別スレッドではなぜか異常値になってるときあり。
    y = STOP_Y;
  }
  if (loopCnt != 0 && abs(y - lastY) > 20) {    //
    y = lastY;
  }
//↑*/
/*↓
////////////////////     carSpeed（加速度で算出）
  if (y >= STOP_Y + 3.0) {  //加速Gあり
    if (y >= NORMAL_ACC) {
      if (y >= AGGRESSIVE_ACC) {
        carSpeed = lastCarSpeed + 0.8;
      } else {    //通常加速
        carSpeed = lastCarSpeed + 0.6;
      }
    } else {  //緩加速
      carSpeed = lastCarSpeed + 0.4;
    }
  } else if (y >= (STOP_Y - 3.0) && y <= (STOP_Y + 3.0)) {  //0G
      carSpeed = lastCarSpeed ;
  } else if (y < STOP_Y - 3.0) {  //減速Gあり
    if (y <= NORMAL_BRK) {
      if (y <= AGGRESSIVE_BRK) {
        carSpeed = lastCarSpeed - 1.0;
      } else {  //通常減速
        carSpeed = lastCarSpeed - 0.6;
      }
    } else {  //緩減速
      carSpeed = lastCarSpeed - 0.4;
    }
  }
  if (carSpeed > 180) {
    carSpeed = 180;
  } else if (carSpeed < 0) {
    carSpeed = 0;
  }
////////////////////     END carSpeed
//↑*/
/*↓
  if (rpm > UP_RPM) {  //一旦ギアを上げてrpmを下げる（見た目上）
    if (gear == 1){
      gear = 2;
      gearRatio = GEAR2;
    } else if (gear == 2){
      gear = 3;
      gearRatio = GEAR3;
    } else if (gear == 3){
      gear = 4;
      gearRatio = GEAR4;
    } else if (gear == 4){
      gear = 5;
      gearRatio = GEAR5;
    } else if (gear == 5){
      gear = 6;
      gearRatio = GEAR6;
    }
    rpm = IDLE_RPM;
  }
//↑*/
/*↓
if (loopCnt == 341){
    Serial.print("loopCnt == 341:");
    Serial.print("\t");
    Serial.print(millis());
    Serial.print("\t");
}
//↑*/

