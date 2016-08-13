//Shift Point Indicator(Fake Tachometer) with GearDisplay(7SegmentLED) which uses a Speedpulse and a Accelerometer
//2016/8/12 BlueDK5FW
#include <MsTimer2.h>
#include <pt.h>  //仮想スレッドライブラリprotothreads（MsTimer2は複数設置不可）
unsigned const int P_HI = 170; //Pulse 分岐値
unsigned const int GREEN1 = 2; //GREEN LED 1 の出力ピン番号
unsigned const int GREEN2 = 3; //GREEN LED 2 の出力ピン番号
unsigned const int GREEN3 = 4; //GREEN LED 3 の出力ピン番号
unsigned const int GREEN4 = 5; //GREEN LED 4 の出力ピン番号
unsigned const int RED1 = 6; //RED LED 1 の出力ピン番号
unsigned const int RED2 = 7; //RED LED 2 の出力ピン番号
unsigned const int RED3 = 8; //RED LED 3 の出力ピン番号
unsigned const int RED4 = 9; //RED LED 4 の出力ピン番号
unsigned const int RED5 = 10; //RED LED 5 の出力ピン番号
unsigned const int BLUE1 = 11; //BLUE LED 1 の出力ピン番号
unsigned const int BLUE2 = 12; //BLUE LED 2 の出力ピン番号
unsigned const int BLUE3 = 13; //BLUE LED 3 の出力ピン番号
unsigned const int SER = 14; //SHIFT REGISTER SER(SI) の出力ピン番号（A0をDjigitalとして使用）
unsigned const int RCLK = 15; //SHIFT REGISTER RCLK(RCK) の出力ピン番号（A1を、同上）
unsigned const int SRCLK = 16; //SHIFT REGISTER SRCLK(SCK) の出力ピン番号（A2を、同上）
unsigned const int ACCY = 3; //加速度センサーY軸のanalog入力ピン番号
unsigned const int SPEED_PIN = 4; //車速Pulseのanalog入力ピン番号
unsigned const int BAUD = 9600; //Serial通信 BAUDレート
unsigned const int TIME0 = 5; //Time0(msec)
unsigned const int TIME1 = 10; //Time1(msec)
unsigned const int TIME2 = 25; //Time2(msec)
unsigned const int TIME3 = 33; //Time3(msec)
unsigned const int TIME4 = 90; //Time4(msec)
unsigned const int TIME5 = 100; //Time5(msec)
unsigned const int TIME6 = 210; //Time6(msec)
unsigned const int TIME7 = 400; //Time7(msec)
unsigned const int TIME8 = 500; //Time8(msec)
unsigned const int TIME9 = 1000; //Time9(msec)
unsigned const int TIME10 = 3000; //Time10(msec)
unsigned const int TIME11 = 5000; //Time11(msec)
const float DEAD = 7.0; //静止時のY軸デッドゾーン
const float MAX_Y = 404.0000; //★1G時のY軸
const float MIN_Y = 241.0000; //★-1G時のY軸 404-241=163
const float STOP_Y = 329.5000;  //★0G時のY軸☆（停止時に値が327-332だったので真ん中）
const float MORE_ACC = 339.6875; //★329.5+10.1875 (163/16=10.1875)
const float MORE_BRK = 319.3125; //★329.5-10.1875
const float AGGRESSIVE_ACC = 349.5000; //★329.5+20
const float AGGRESSIVE_BRK = 309.5000; //★329.5-20
const float STEP_RPM = 411.1111; //rpmSTEP値 ((REV4500回転 - IDLE800回転) / GREEN3以降のLED9間隔))
unsigned const int IDLE_RPM = 800; //アイドリング回転数
unsigned const int MAX_RPM = 4500; //Rev回転数
const float SPEED1 = 18.0; //1速→2速チェンジ時のスピード（アクセル通常開度）
const float SPEED2 = 36.0; //2速→3速チェンジ時のスピード
const float SPEED3 = 44.5; //3速→4速チェンジ時のスピード
const float SPEED4 = 65.0; //4速→5速チェンジ時のスピード
const float SPEED5 = 79.0; //5速→6速チェンジ時のスピード
const float GEAR1 = 3.552; //1速変速比 3.552 最終減速比 13.54
const float GEAR2 = 2.022; //2速 2.022  7.708
const float GEAR3 = 1.452; //3速 1.452  5.535
const float GEAR4 = 1.000; //4速 1.000  3.812
const float GEAR5 = 0.708; //5速 0.708  2.699
const float GEAR6 = 0.599; //6速 0.599  2.283
const float FINAL = 3.812; //Final 3.812
////////  Global Scope  /////////////////////////////////////////////////////////////////////////////////////
unsigned int i,j,gear,flagHi,blinkFlg;
unsigned long loopCnt,beforeTime,elapsedTime,acceleroTime,aggrStartTime,aggrStartTime2;
float lastY,last2Y,last3Y,carSpeed,pulse;
static struct pt pt;  //protothreads用struct
//unsigned int sDwnFlg;  //★★★★★★★temp(for Demonstration)
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
  for (i = 1;i < 7; i++) {    //i = 数字（配列）番号
    shiftRegisterDispNum(i);
    delay(TIME6);    //210ms
  }
  maxBlink();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void acceleroBlink(){
  for (i = 4; i < 14; i++){  //GREEN3以降を消す
    digitalWrite(i,LOW);
  }
  delay(TIME5);    //100ms
  for (i = 4; i < 14; i++){  //GREEN3以降
    digitalWrite(i,HIGH);
    delay(TIME3+(i-4)*20);    //33ms～213ms
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void maxBlink(){    //2回フラッシュ
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME5);    //100ms
  shiftRegisterDispNum(10);  //"H"
  for (i = 2; i < 14; i++){
    digitalWrite(i,HIGH);
  }
  delay(TIME5);
  for (i = 2; i < 14; i++){
    digitalWrite(i,LOW);
  }
  delay(TIME5);
  shiftRegisterDispNum(10);  //"H"
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
//               ギア表示（仮想スレッド）  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int dispGearThread(struct pt *pt) {
  static unsigned long ptTime = 0;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - ptTime > TIME7);    //400ms毎に稼働
    ptTime = millis();
    shiftRegisterDispNum(gear);  //現在のギアを表示
  }
  PT_END(pt);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               シフトレジスタで７セグLED制御（arduinoピン3本）
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void shiftRegisterDispNum(unsigned int val) {
  //7Seg(anode common)Pin     ↓1, 2, 3, 4, 5      ↑6, 7, 8, 9, 10　　
  //7Seg Segment              　e, d, +, c, (d.p)  　b, a, +, f, g  →左下segから反時計回りでedcbafgの順
  //ShiftRegisterQ-Pin          A, B, +, C, -        D, E, -, F, G
  // ex 0：0000001（edcbafg順、bitを渡す時は下位ビットから、先頭ビットは未使用、1（消灯）固定）
  // ex 1：1100111（0は点灯、1は消灯）
  boolean digit[12][8] = {{1,1,0,0,0,0,0,0},   //0
                         {1,1,1,1,0,0,1,1},    //1
                         {1,0,1,0,0,1,0,0},    //2
                         {1,0,1,0,0,0,0,1},    //3
                         {1,0,0,1,0,0,1,1},    //4
                         {1,0,0,0,1,0,0,1},    //5
                         {1,0,0,0,1,0,0,0},    //6
                         {1,1,0,0,0,0,1,1},    //7
                         {1,0,0,0,0,0,0,0},    //8
                         {1,0,0,0,0,0,0,1},    //9
                         {1,0,0,1,0,0,1,0},    //H
                         {1,0,1,1,1,1,1,1}};   //-
  digitalWrite(RCLK, LOW);     // データ格納前：RCLKをLOWに
  digitalWrite(SRCLK, LOW);    //格納アドレス移動フラグクリア
  for(j = 0; j < 8; j++){      //j = bit番号
    digitalWrite(SER, digit[val][j]);    //bit値格納（HIGH or LOW）
    digitalWrite(SRCLK, HIGH);    //格納アドレス移動
    digitalWrite(SRCLK, LOW);     //格納アドレス移動フラグクリア
  }
  digitalWrite(RCLK, HIGH);   // 8ビット（1文字）分のデータをparallel出力
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  beforeTime = elapsedTime = acceleroTime = aggrStartTime = aggrStartTime2 = 0;
  i = j = gear = flagHi = loopCnt = blinkFlg = 0;
  lastY = last2Y = last3Y = carSpeed = pulse = 0.0;
  Serial.begin(BAUD);
  for(i = 2; i < 14; i++){    //IndicatorLEDピン
    pinMode(i,OUTPUT);
  }
  for(i = 14; i < 17; i++){    //ShiftRegister制御ピン
    pinMode(i,OUTPUT);
  }
  for (i = 2; i < 14; i++) {
    digitalWrite(i,LOW);
  }
  shiftRegisterDispNum(11);  //"-"
  delay(TIME9);    //始動時は少し(1秒) 待つ
  initialBlink();
  MsTimer2::set(TIME3,calcSpeed);    //33ms毎に稼働
  MsTimer2::start();
  PT_INIT(&pt);    //ギア表示スレッド初期化（400ms毎に稼働）
//sDwnFlg = 0;  //★★★★★★★temp(for Demonstration)
}
void loop(){
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Loop Start Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int rpm;
  unsigned long nowTime,milliSec;
  float y,gearRatio;
  rpm = 0;
  nowTime = milliSec = 0;
  y = gearRatio = 0.0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               ACC_Y Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  y = analogRead(ACCY);
/*↓  //for debug
  if (loopCnt % 30 == 0){ 
    Serial.print("Y:");
    Serial.print("\t");
    Serial.print(y);
    Serial.println("\t");
  }
↑*/
  if (y > MAX_Y || y < MIN_Y) {   //y値が異常値の場合
    y = STOP_Y;    //0Gにする
  }
  if (loopCnt && abs(y - lastY) > 20) {    //y値がたまに跳ねる時あり
    y = lastY;
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Pulse, Elapssed Time Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//車速信号：JIS規定では1kmごとに637回 * nパルスが発生
//4パルス車は、2548パルス per 1km
//時速1kmで走るとパルス幅は（1時間（3600秒）÷2548パルス=1.41287284144427秒）
//　→　時速は（1.41287284144427÷計測パルス幅）（レシプロカル）
  nowTime = millis();
  pulse = analogRead(SPEED_PIN);  //analogRead値はarduino電源電圧で変化する→ex)VIN+USB構成から、USBを抜くと値が変わる
/*↓  //for debug
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
  if(nowTime - beforeTime > TIME10){   //前回～今回3秒以上→0.5km以下
    elapsedTime = 0.0;
  }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//               Gear Section  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    減速比 総減速比 速度
//1速 3.552  13.54     0->16
//2速 2.022  7.708    17->34
//3速 1.452  5.535    35->44.4
//4速 1.000  3.812    44.5->64
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
  if (y  > AGGRESSIVE_ACC || y < AGGRESSIVE_BRK){    //急加・減速時はシフトダウン
    milliSec = millis();
    if (!aggrStartTime) {  //初めてaggressive検知 or （前回）aggressive検知してから0.5秒以上（0.5秒含まず）経過
      aggrStartTime = milliSec;  //検知開始時間を記録
    } else {
      if (aggrStartTime + TIME8 <  milliSec) {  //検知開始から0.5秒以上（0.5秒含まず）経過
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
        aggrStartTime = 0;  //処理がおわったのでクリア
      }  //END 検知開始から0.5秒以上（0.5秒含まず）経過
    }  //END aggStartTime != 0 段差乗上等、0.5秒未満のy値大変動では反応させない
  }  //END AGGRESSIVE_ACC or AGGRESSIVE_BRK
  dispGearThread(&pt);    //ギア表示スレッドコール
/*↓  //for debug
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
/*↓  //for debug
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
      if (aggrStartTime2 + TIME8 <  milliSec) {  //検知開始から0.5秒以上（0.5秒含まず）経過
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
  if (acceleroTime && (acceleroTime + TIME11 <  milliSec)) {  //acceleroBlinkは1度なったら加速度が続いていても
    acceleroTime = 0;                                        //終了後5秒間(5000ms)はacceleroBlinkにしない
    blinkFlg = 0;
  }
  if (aggrStartTime && (aggrStartTime + TIME8 <  milliSec)) {  //0.5秒（500ms）以内のy値大変動は無視
    aggrStartTime = 0;
  }
  if (aggrStartTime2 && (aggrStartTime2 + TIME8 <  milliSec)) {  //0.5秒（500ms）以内のy値大変動は無視
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
  carSpeed = speed;  //carSpeedで受け渡し
/*↓  //★★★★★★★temp(for Demonstration)
if (carSpeed <= 90 && !sDwnFlg) {
  carSpeed += 0.7;
  if (carSpeed >= 90){
    sDwnFlg = 1;
  }
}
if (sDwnFlg) {
  carSpeed -= 0.7;
  if (carSpeed <= 1){
    sDwnFlg = 0;
  }
}
//↑*/
/*↓  //for debug
  Serial.print("carSpeed = speed:");
  Serial.print("\t");
  Serial.print(speed);
  Serial.println("\t");
//↑*/
}  //END calcSpeed()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

