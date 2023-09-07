//モータドライバ４台制御
//デイジーチェーン接続
//書いてる途中
#include <SPI.h>
#define PIN_SPI_SCLK 52         // クロック（SPI CLK、SCLK）
#define PIN_SPI_MISO 50         // マスタ入力／スレーブ出力（MISO）
#define PIN_SPI_MOSI 51
#define PIN_SPI_SS01 53         // スレーブ選択（Slave Select） 
#define PIN_MT_BSY01 9          // BUSY
                                         
#define PIN_SPI_SS02 4         // スレーブ選択（Slave Select） 
#define PIN_MT_BSY02 3

#define PIN_SPI_SS03 22
#define PIN_MT_BSY03 24

#define PIN_5 5                 // ﾘﾐｯﾄSW(原点側(左端))
#define PIN_6 6                 // ﾘﾐｯﾄSW(右端)
#define PIN_7 7                 // LED出力
#define PIN_8 8 

// 対象デバイス初期値
int dvs_select=0;

// 入力値初期化
int sw_sts_5 = LOW;
int sw_sts_6 = LOW;
#define MOTOR_SPEED 30000

void setup(){
  // 各PIN入出力定義
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_SCLK, OUTPUT);
  
  pinMode(PIN_SPI_SS01, OUTPUT);
  pinMode(PIN_MT_BSY01, INPUT);
  
  pinMode(PIN_SPI_SS02, OUTPUT);
  pinMode(PIN_MT_BSY02, INPUT);

  pinMode(PIN_SPI_SS03, OUTPUT);
  pinMode(PIN_MT_BSY03, INPUT);
  
  pinMode(PIN_5, INPUT );
  pinMode(PIN_6, INPUT );
  pinMode(PIN_7, OUTPUT );
  pinMode(PIN_8, OUTPUT );
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  
  dvs_select=0;
  SPI_DISCONNECT();

  L6470_rst_device();                             // L6470リセット
  L6470_set_parameter();                          // L6470パラメータ設定
  delay(500);
}

// シリアルデータ送信
void snd_str(String snd_dat){
  String snd_msg = "          " + snd_dat;
  Serial.println(snd_msg);
  return;
}

void loop(){
  dvs_select=1; //右
  L6470_run(0, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

  dvs_select=2;
  L6470_run(0, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);
  dvs_select=3;
  L6470_run(0, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

}


/////////////////////////////////関数////////////////
//回転急停止、保持トルクあり[n]
void L6470_hardstop(){
  L6470_data_transfer(0xb8,0,0);
}

//指定方向に連続回転 [n]
void L6470_run(int dia,long spd){
  if(dia==1)
    L6470_data_transfer(0x51,3,spd);
  else
    L6470_data_transfer(0x50,3,spd);
}

//指定方向に指定数ステップする
// dia 1:正転 0:逆転,n_step (22bit)
void L6470_move(int dia,long n_step){
  if(dia==1)
    L6470_data_transfer(0x41,3,n_step);
  else
    L6470_data_transfer(0x40,3,n_step);
}

//回転停止、保持トルクあり[B]
void L6470_softstop(){
  L6470_data_transfer(0xb0,0,0);
}
////////////////////////////////////////////////////

// 原点復帰
void get_origin(){
  
  digitalWrite( PIN_7 , LOW );
  digitalWrite( PIN_8 , LOW );

  // 反-原点方向（右）移動
  if(digitalRead( PIN_5 )){
    L6470_data_transfer(0x51,3,8000);             // 速度指定移動（中速）
    delay(50);
    while(digitalRead( PIN_5 )){ delay(500); }
    L6470_data_transfer(0xb0,0,0);                // 回転停止、保持トルク有
    L6470_wait_not_busy(500);
  }
  
  // 原点方向（左）移動
  L6470_data_transfer(0x50,3,8000);               // 速度指定移動（中速）
  while(!digitalRead( PIN_5 )){} 
  L6470_data_transfer(0xb0,0,0);                  // 回転停止、保持トルク有
  L6470_wait_not_busy(500);

  // 反-原点方向（右）移動
  L6470_data_transfer(0x51,3,500);                // 速度指定移動（低速）
  delay(50);
  while(digitalRead( PIN_5 )){}
  L6470_data_transfer(0xb0,0,0);                  // 回転停止、保持トルク有（機械原点）
  L6470_wait_not_busy(500);

  // 反-原点方向（右）移動（移動量指定）          // 600X20.8/16000=0.78(cm)
  L6470_data_transfer(0x41,3,600);                // 移動量・方向指定移動
  L6470_wait_not_busy(100);

  digitalWrite( PIN_7 , HIGH );
  digitalWrite( PIN_8 , HIGH );
}


// 初期設定
void L6470_set_parameter(){//
  L6470_data_transfer(0x05,2,0x40);
  L6470_data_transfer(0x06,2,0x40);
  L6470_data_transfer(0x07,2,0x40);
  L6470_data_transfer(0x08,2,0x01);
  L6470_data_transfer(0x15,2,0x3ff);
  L6470_data_transfer(0x09,1,0x50);
  L6470_data_transfer(0x0a,1,0x50);
  L6470_data_transfer(0x0b,1,0x50);
  L6470_data_transfer(0x0c,1,0x50);
  L6470_data_transfer(0x16,1,0x03);
}

// デバイスリセット
void L6470_rst_device(){
  L6470_data_send_u(0x00);                        //nop命令
  L6470_data_send_u(0x00);
  L6470_data_send_u(0x00);
  L6470_data_send_u(0x00);
  L6470_data_send_u(0xc0);
}

// 送信データ加工
void L6470_data_transfer(int add,int bytes,long val){
  int data[3];
  L6470_data_send(add);
  for(int i=0;i<=bytes-1;i++){
    data[i] = val & 0xff;  
    val = val >> 8;
  }
  if(bytes==3){
    L6470_data_send(data[2]);
  }
  if(bytes>=2){
    L6470_data_send(data[1]);
  }
  if(bytes>=1){
    L6470_data_send(data[0]);
  }  
}

// データ送信（BUSY待機）
void L6470_data_send(unsigned char add_or_val){
  L6470_wait_not_busy(10);
  SPI_DVS_SELECT();
  SPI.transfer(add_or_val);           // アドレスもしくはデータ送信。
  SPI_DISCONNECT();
}

// データ送信（直ぐ送信）
void L6470_data_send_u(unsigned char add_or_val){
  SPI_DVS_SELECT();
  SPI.transfer(add_or_val);           // アドレスもしくはデータ送信。
  SPI_DISCONNECT();
}

void SPI_DVS_SELECT(){
  if(dvs_select==1){
    digitalWrite(PIN_SPI_SS01, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS02, HIGH);    // ~SSディスエーブル。
    digitalWrite(PIN_SPI_SS03, HIGH);
  }else if(dvs_select==2){
    digitalWrite(PIN_SPI_SS01, HIGH);    // ~SSディスエーブル。
    digitalWrite(PIN_SPI_SS02, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS03, HIGH);
  }else if(dvs_select ==3){
    digitalWrite(PIN_SPI_SS01, HIGH);
    digitalWrite(PIN_SPI_SS02, HIGH);
    digitalWrite(PIN_SPI_SS03, LOW);
  }else{
    digitalWrite(PIN_SPI_SS01, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS02, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS03, LOW);
  }
}

void SPI_DISCONNECT(){
  digitalWrite(PIN_SPI_SS01, HIGH);   // ~SSディスエーブル。
  digitalWrite(PIN_SPI_SS02, HIGH);   // ~SSディスエーブル。
  digitalWrite(PIN_SPI_SS03, HIGH);
}

// BUSY解除後、指定時間待機
void L6470_wait_not_busy(long time){
  if(dvs_select==1){
    while(!digitalRead(PIN_MT_BSY01)){}
  }else if(dvs_select==2){
    while(!digitalRead(PIN_MT_BSY02)){}
  }else if(dvs_select==3){
    while(!digitalRead(PIN_MT_BSY03)){}
  }
  else{
    while(!digitalRead(PIN_MT_BSY01) && !digitalRead(PIN_MT_BSY02) && !digitalRead(PIN_MT_BSY03)){}     // BUSY解除待機
  }
  delay(time);
}
//#define MOTOR_SPEED 30000

// #include <SPI.h>
// #include <MsTimer2.h>
// #include <stdio.h>

// // ピン定義
// #define PIN_SPI_MOSI 51 //11,mega51
// #define PIN_SPI_MISO 50 //12,mega50
// #define PIN_SPI_SCK 52 //13,mega52
// #define PIN_SPI_SS 53 //10,mega53
// #define PIN_BUSY 9
// #define PIN_BUSY2 8
// #define PIN_BUSY3 7
// #define PIN_BUSY4 6

// void setup() {
//   delay(1000);
//   pinMode(PIN_SPI_MOSI, OUTPUT);
//   pinMode(PIN_SPI_MISO, INPUT);
//   pinMode(PIN_SPI_SCK, OUTPUT);
//   pinMode(PIN_SPI_SS, OUTPUT);
//   pinMode(PIN_BUSY, INPUT_PULLUP);
//   pinMode(PIN_BUSY2, INPUT_PULLUP);
//   pinMode(PIN_BUSY3, INPUT_PULLUP);
//   pinMode(PIN_BUSY4, INPUT_PULLUP);
//   SPI.begin();
//   SPI.setDataMode(SPI_MODE3);
//   SPI.setBitOrder(MSBFIRST);
//   Serial.begin(38400);
//   digitalWrite(PIN_SPI_SS, HIGH);
//   // put your setup code here, to run once:

//   L6470_resetdevice(); //1台目のL6470リセット
//   L6470_resetdevice2(); //2台目のL6470リセット
//   L6470_resetdevice3();
//   L6470_resetdevice4();
//   L6470_setup();  //1台目のL6470を設定 
//   L6470_setup2();  //2台目のL6470を設定
//   L6470_setup3();
//   L6470_setup4();
//   L6470_getstatus(); //1台目のフラグ解放
//   L6470_getstatus2();//2台目のフラグ解放
//   L6470_getstatus3();
//   L6470_getstatus4();
     
//  MsTimer2::set(25, fulash);//シリアルモニター用のタイマー割り込み
//  MsTimer2::start();
  
// delay(2000);
// }

// void loop() {
//   // put your main code here, to run repeatedly:

// }


// void L6470_setup(){
//   L6470_setparam_acc(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_dec(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_maxspeed(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
//   L6470_setparam_minspeed(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
//   L6470_setparam_fsspd(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
//   L6470_setparam_kvalhold(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalrun(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalacc(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvaldec(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

//   L6470_setparam_stepmood(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
// }
// void L6470_setup2(){
//   L6470_setparam_acc2(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_dec2(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_maxspeed2(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
//   L6470_setparam_minspeed2(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
//   L6470_setparam_fsspd2(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
//   L6470_setparam_kvalhold2(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalrun2(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalacc2(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvaldec2(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

//   L6470_setparam_stepmood2(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
// }
// void L6470_setup3(){
//   L6470_setparam_acc3(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_dec3(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_maxspeed3(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
//   L6470_setparam_minspeed3(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
//   L6470_setparam_fsspd3(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
//   L6470_setparam_kvalhold3(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalrun3(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalacc3(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvaldec3(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

//   L6470_setparam_stepmood3(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
// }
// void L6470_setup4(){
//   L6470_setparam_acc4(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_dec4(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
//   L6470_setparam_maxspeed4(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
//   L6470_setparam_minspeed4(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
//   L6470_setparam_fsspd4(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
//   L6470_setparam_kvalhold4(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalrun4(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvalacc4(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
//   L6470_setparam_kvaldec4(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

//   L6470_setparam_stepmood4(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
// }

// void fulash(){
//   long a=L6470_getparam_abspos();//1
//   long b=L6470_getparam_speed();
//   long c=L6470_getparam_abspos2();//2
//   long d=L6470_getparam_speed2();
//   long e=L6470_getparam_abspos3();//3
//   long f=L6470_getparam_speed3();
//   long g=L6470_getparam_abspos4();//4
//   long h=L6470_getparam_speed4();
//   char str[15];
//   snprintf(str,sizeof(str),"1pos=0x%6.6X ",a);//1
//   Serial.print(str);
//   snprintf(str,sizeof(str),"1spd=0x%5.5X ",b);
//   Serial.print(str);
//   snprintf(str,sizeof(str),"2pos=0x%6.6X ",c);//2
//   Serial.print(str);
//   snprintf(str,sizeof(str),"2spd=0x%5.5X ",d);
//   Serial.println(str);
//   snprintf(str,sizeof(str),"3pos=0x%6.6X ",e);//3
//   Serial.print(str);
//   snprintf(str,sizeof(str),"3spd=0x%5.5X ",f);
//   Serial.println(str);
//   snprintf(str,sizeof(str),"4pos=0x%6.6X ",g);//4
//   Serial.print(str);
//   snprintf(str,sizeof(str),"4spd=0x%5.5X ",h);
//   Serial.println(str);
  
//  /* Serial.print("0x");
//   Serial.print( L6470_getparam_abspos(),HEX);
//   Serial.print(" 0x");
//   Serial.print( L6470_getparam_speed(),HEX);
//   Serial.print(" 0x");
//   Serial.print( L6470_getparam_abspos2(),HEX);
//   Serial.print(" 0x");
//   Serial.println( L6470_getparam_speed2(),HEX);
//   */
// }