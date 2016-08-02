//Shift Point Indicator(Fake Tachometer) using a Speedpulse and a Accelerometer
//2016/8/2 BlueDK5FW
#include <MsTimer2.h>
#define P_HI 170 //Pulse 分岐値
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
#define TIME2 25 //Time2(msec)
#define TIME3 33 //Time3(msec)
#define TIME4 90 //Time4(msec)
#define TIME5 100 //Time5(msec)
#define TIME6 500 //Time6(msec)
#define TIME7 1500 //Time7(msec)
#define TIME8 3000 //Time8(msec)
#define TIME9 5000 //Time9(msec)
#define DEAD 7.0 //静止時のY軸デッドゾーン
#define MAX_Y 402 //★1G時のY軸
#define MIN_Y 265 //★-1G時のY軸 402-265=137
#define STOP_Y 323.0000  //★0G時のY軸☆（停止時に値が321-324だったので真ん中）
#define AGGRESSIVE_ACC 343.0000 //★323+20
#define AGGRESSIVE_BRK 303.0000 //★323-20
#define NORMAL_ACC 331.5625 //★323+8.5625 (137/16=8.5625)
#define NORMAL_BRK 314.4375 //★323-8.5625
#define STEP_RPM 411.1111 //rpmSTEP値 ((REV4500回転 - IDLE800回転) / GREEN3以降のLED9間隔))
#define IDLE_RPM 800 //アイドリング回転数
#define MAX_RPM 4500 //Rev回転数
#define SPEED1 18 //1速→2速チェンジ時のスピード（アクセル通常開度）
#define SPEED2 36 //2速→3速チェンジ時のスピード
#define SPEED3 45 //3速→4速チェンジ時のスピード
#define SPEED4 65 //4速→5速チェンジ時のスピード
#define SPEED5 79 //5速→6速チェンジ時のスピード
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
unsigned int i,flagHi,blinkFlg;
unsigned long loopCnt,beforeTime,elapsedTime,acceleroTime,aggrStartTime,aggrStartTime2;
float lastY,last2Y,last3Y,carSpeed,pulse;
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
  for (i = 4; i < 14; i++){  //GREEN3以降を消す
    digitalWrite(i,LOW);
  }
  delay(TIME5);    //100ms
  for (i = 4; i < 14; i++){  //GREEN3以降を90ms間隔でつける
    digitalWrite(i,HIGH);
    delay(TIME4);    //90ms
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void maxBlink(){    //2回フラッシュ
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME5);    //100ms
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
  }
  delay(TIME5);
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME5);
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
  }
  delay(TIME5);
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME5);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  beforeTime = elapsedTime = acceleroTime = aggrStartTime = aggrStartTime2 = 0;
  i = flagHi = loopCnt = blinkFlg = 0;
  lastY = last2Y = last3Y = carSpeed = pulse = 0.0;
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
  delay(TIME7);    //始動時は少し(1.5秒) 待つ
  initialBlink();
  MsTimer2::set(TIME3,calcSpeed);    //33ms毎に稼働
  MsTimer2::start();
}
void loop(){
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Loop Start Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int rpm,gear;
  unsigned long nowTime,milliSec;
  float x,y,z,gearRatio;
  rpm = gear = 0;
  nowTime = milliSec = 0;
  x = y = z = gearRatio = 0.0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               ACC_Y Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  y = analogRead(ACCY);
  if (y > MAX_Y || y < MIN_Y) {   //y値が異常値の場合
    y = STOP_Y;    //0Gにする
  }
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
  pulse = analogRead(SPEED_PIN);  //analogRead値はarduino電源電圧で変化する→ex)VIN+USB構成から、USBを抜くと値が変わる
/*↓
  if (loopCnt % 30 == 0){ 
  Serial.print("pulse:");
  Serial.print("\t");
  Serial.print(pulse);
  Serial.println("\t");
  }
//↑*/
  if (pulse > P_HI && !flagHi){      //パルスHIGH
    elapsedTime = nowTime - beforeTime; //前回パルスアップからの経過時間
    beforeTime = nowTime;
    flagHi = 1;
  }
  if (pulse <= P_HI && flagHi){      //パルスLOW
    flagHi = 0;
  }
  if(nowTime - beforeTime > TIME8){   //前回～今回3秒以上→0.5km以下
    elapsedTime = 0.0;
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Gear Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    減速比 総減速比 速度
//1速 3.552  13.54     0->16
//2速 2.022  7.708    17->34
//3速 1.452  5.535    35->44
//4速 1.000  3.812    45->64
//5速 0.708  2.699    65->78
//6速 0.599  2.283    79->180
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
//加速度からギア比補正
  if (y  > AGGRESSIVE_ACC || y < AGGRESSIVE_BRK){
    milliSec = millis();
    if (!aggrStartTime) {  //初めてaggressive検知 or （前回）aggressive検知してから0.5秒以上（0.5秒含まず）経過
      aggrStartTime = milliSec;  //検知開始時間を記録
    } else {
      if (aggrStartTime + TIME6 <  milliSec) {  //検知開始から0.5秒以上（0.5秒含まず）経過
        if (y  > AGGRESSIVE_ACC) {    //急加速時はシフトダウン
          if (gear == 1) {
            gearRatio = GEAR1;
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
          }
        } else {    //急減速時はシフトアップ
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
          } else if (gear == 6) {
            gearRatio = GEAR6;
          }
          gear++;
          if (gear > 6){
            gear = 6;
          }
        }
        aggrStartTime = 0;  //処理がおわったのでクリア
      }  //END 検知開始から0.5秒以上（0.5秒含まず）経過
    }  //END aggStartTime != 0 段差乗上等、0.5秒未満のy値大変動では反応させない
  }  //END AGGRESSIVE_ACC or AGGRESSIVE_BRK
/*↓
//  if (loopCnt % 5 == 0){ 
    Serial.print("gear:");
    Serial.print("\t");
    Serial.print(gear);
    Serial.print("\t");
//  }
//↑*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               RPM Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 速度km/h ＝ 回転数rpm÷変速比÷最終減速比×（タイヤ外径mm×3.14÷1000）×60÷1000
// ex）3000 rpm / 6速0.599 / 3.812 * ( 685mm * 3.14 / 1000 ) * 60 / 1000 = 169.55km 
// ex2）2000 rpm / 6速0.599 / 3.812 * ( 2.1509 ) * 60 / 1000 = 113.03km
// 回転数rpm ＝ 速度km/h×変速比×最終減速比 ÷ （タイヤ外径mm×3.14÷1000）÷60×1000 
  rpm = carSpeed * gearRatio * FINAL / 2.1509 / 60 * 1000;
  if (rpm < IDLE_RPM) {   //計算値がアイドリング（800回転）未満
    rpm = IDLE_RPM;
  }
  if (rpm > MAX_RPM) {   //計算値が4500回転より大
    rpm = MAX_RPM;
  }
/*↓
  if (rpm > 800) {
    Serial.print("rpm:");
    Serial.print("\t");
    Serial.print(rpm);
    Serial.println("\t");
  }
//↑*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               LED Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (y >= (STOP_Y - DEAD) && y <= (STOP_Y + DEAD)  && carSpeed < 1.0) {  //yがデッドゾーンかつスピード1km未満
  } else {    //本当に止まっている時以外（DEAD=7.0）
    for  (i = 2; i < 5; i++){
      digitalWrite(i,HIGH);    //GREEN1～3をつける
    }
  }
  if (carSpeed >= 5) {  //5km以上
    for  (i = 5; i < 14; i++){  // GREEN4以降
      if (rpm > IDLE_RPM) {  //800回転は無視  
        if (rpm >= IDLE_RPM + ((i-4) * (STEP_RPM * 0.6))) {  //点灯補正(VIN電圧9V＋USB電源電圧5V≒基盤供給電圧6V時にrpm0.6倍が適正)
          digitalWrite(i,HIGH);
        } else {    //STEPに満たない時
          if (loopCnt % 10 == 0) {   //ちらつき防止
            digitalWrite(i,LOW);
          }
        }  //END else
      }  //END 801回転以上
    }    //END for
  } else {    //5km未満
    if (y >= (STOP_Y - DEAD) && y <= (STOP_Y + DEAD)  && carSpeed < 1.0) {    //本当に止まっている時（DEAD=7.0）
      digitalWrite(2,HIGH);
      for  (i = 3; i < 14; i++){    //GREEN2以降をけす
        digitalWrite(i,LOW);
      }
    }
  }
  //acceleroBlink
  if (carSpeed >= 1.0 && y > AGGRESSIVE_ACC && lastY > AGGRESSIVE_ACC && last2Y > AGGRESSIVE_ACC && last3Y > AGGRESSIVE_ACC) {
     milliSec = millis();
    if (!aggrStartTime2) {  //初めてaggressive検知 or （前回）aggressive検知してから0.5秒以上（0.5秒含まず）経過
      aggrStartTime2 = milliSec;  //検知開始時間を記録
    } else {
      if (aggrStartTime2 + TIME6 <  milliSec) {  //検知開始から0.5秒以上（0.5秒含まず）経過
        if (!acceleroTime) {    //初めてacceleroになった or 前回accelero終了から5秒以上（5秒含まず）経過
          acceleroBlink();    //急加速警告表示
          acceleroTime = millis();  //終了時間を記録
          blinkFlg = 1;
        }
        if (blinkFlg) {  //直前にacceleroBlinkになっててaggressive加速度が続いてる時
          maxBlink();  //2回フラッシュ
        }
        aggrStartTime2 = 0;  //処理がおわったのでクリア
      }  //END 検知開始から0.5秒以上（0.5秒含まず）経過
    }  //END aggStartTime2 != 0 段差乗上げ等、0.5秒未満のy大変動ではblinkさせない 
  }  //END carSpeed >= 1.0 && y > AGGRESSIVE～
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Loop End Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  last3Y = last2Y;
  last2Y = lastY;
  lastY = y;
  loopCnt++;
  milliSec = millis();
  if (acceleroTime && (acceleroTime + TIME9 <  milliSec)) {  //acceleroBlinkは1度なったら加速度が続いていても
    acceleroTime = 0;                                        //終了後5秒間(5000ms)はacceleroBlinkにしない
    blinkFlg = 0;
  }
  if (aggrStartTime && (aggrStartTime + TIME6 <  milliSec)) {  //0.5秒（500ms）以内のy値大変動は無視
    aggrStartTime = 0;
  }
  if (aggrStartTime2 && (aggrStartTime2 + TIME6 <  milliSec)) {  //0.5秒（500ms）以内のy値大変動は無視
    aggrStartTime2 = 0;
  }
}  //END loop()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Speed Section  (別スレッド)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calcSpeed(){
  float speed = 0.0;
  if (elapsedTime) {
    speed = 1412.87284144427 / elapsedTime;    //3600秒÷(637*4パルス) = 1.41287284144427
    if (speed <= 5 ){        //スピードメータと比べてspeedをマニュアル補正
      speed = speed * 1.0;
    } else if (speed > 5 && speed <= 20){
      speed = speed * 1.105;
    } else if (speed > 20 && speed <= 35){
      speed = speed * 1.100;
    } else if (speed > 35 && speed <= 40){
      speed = speed * 1.052;
    } else if (speed > 40 && speed <= 45){
      speed = speed * 1.0;
    } else if (speed > 45 && speed <= 50){
      speed = speed * 1.041;
    } else if (speed > 50 && speed <= 55){
      speed = speed * 1.052;
    } else if (speed > 55 && speed <= 60){
      speed = speed * 1.071;
    } else if (speed > 60){  //60km以上は危ないので比較未済
      speed = speed * 1.071;
    }
    if (speed > 180){
      speed = 180;
    }
    if (speed < 0){
      speed = 0;
    }
  } else {     //END elapsedTime
    speed = 0;
  }  //END !elapsedTime
  carSpeed = speed;    //carSpeedで受け渡し
/*↓
  Serial.print("carSpeed = speed:");
  Serial.print("\t");
  Serial.print(speed);
  Serial.println("\t");
//↑*/
}  //END calcSpeed()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//以下は古LogicBkup
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
  if (loopCnt && abs(y - lastY) > 20) {    //
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
  if (rpm > UP_RPM) {  //一旦rpmを下げる（見た目上）
    rpm = IDLE_RPM + 1;  //800回転とは区別する
  }
//↑*/
/*↓
//loop速度確認
if (loopCnt == 400){    //約3秒
    Serial.print("loopCnt == 400:");
    Serial.print("\t");
    Serial.print(millis());
    Serial.print("\t");
}
}
//↑*/
/*↓
  for (i = 0; i < 50; i++){         //加速度センサー値安定化（50回読込分を平均）
    y += analogRead(ACCY);
  }
  y /= 50;
//↑*/
/*↓
      if (rpm > IDLE_RPM) {  //800回転は無視
        if (rpm >= IDLE_RPM + ((i-4) * (STEP_RPM * 0.6))) {  //点灯補正：右側まで光ってる所がみたいのでSTEP値到達の0.6倍の回転数でつけてる
//↑*/
/*↓
  //acceleroBlink
carSpeed = 5;
if (loopCnt < 40 ) {
  y = AGGRESSIVE_ACC+1;
} else {
  y = STOP_Y;
}
  if (carSpeed >= 1.0 && y > AGGRESSIVE_ACC && lastY > AGGRESSIVE_ACC && last2Y > AGGRESSIVE_ACC && last3Y > AGGRESSIVE_ACC) {
    if (!aggrStartTime2) {
      aggrStartTime2 = millis();
    }
Serial.print("aggrStartTime2:");
Serial.print("\t");
Serial.print(aggrStartTime2/1000);
Serial.println("\t");
    if (aggrStartTime2 + TIME6 <  millis()) {  //段差乗上等0.5秒未満のy大変動ではblinkさせない

      if (!blinkFlg) {
        acceleroBlink();    //急加速警告表示
        blinkFlg = 1;
        acceleroTime = millis();
Serial.print("blink !! Time:");
Serial.print("\t");
Serial.print(acceleroTime/1000);
Serial.println("\t");
        maxBlink();
        maxBlink();
      }  //END !blinkFlg
      aggrStartTime2 = 0;
    }
  }  //END carSpeed >= 1.0 && y > AGG～
//↑*/


