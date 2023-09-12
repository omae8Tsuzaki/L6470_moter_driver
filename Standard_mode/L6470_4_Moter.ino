//モータドライバ４台制御
//デイジーチェーン接続
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

#define PIN_SPI_SS04 23
#define PIN_MT_BSY04 25

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

  pinMode(PIN_SPI_SS04, OUTPUT);
  pinMode(PIN_MT_BSY04, INPUT);
  
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
  //適宜ここの処理を変える
  dvs_select=1; //右
  L6470_run(1, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

  dvs_select=2;
  L6470_run(1, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

  dvs_select=3;
  L6470_run(0, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

  dvs_select=4;
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
    digitalWrite(PIN_SPI_SS04, HIGH);
  }else if(dvs_select==2){
    digitalWrite(PIN_SPI_SS01, HIGH);    // ~SSディスエーブル。
    digitalWrite(PIN_SPI_SS02, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS03, HIGH);
    digitalWrite(PIN_SPI_SS04, HIGH);
  }else if(dvs_select ==3){
    digitalWrite(PIN_SPI_SS01, HIGH);
    digitalWrite(PIN_SPI_SS02, HIGH);
    digitalWrite(PIN_SPI_SS03, LOW);
    digitalWrite(PIN_SPI_SS04, HIGH);
  }else if(dvs_select==4){
    digitalWrite(PIN_SPI_SS01, HIGH);
    digitalWrite(PIN_SPI_SS02, HIGH);
    digitalWrite(PIN_SPI_SS03, HIGH);
    digitalWrite(PIN_SPI_SS04, LOW);
  }
  else{
    digitalWrite(PIN_SPI_SS01, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS02, LOW);    // ~SSイネーブル。
    digitalWrite(PIN_SPI_SS03, LOW);
    digitalWrite(PIN_SPI_SS04, LOW);
  }
}

void SPI_DISCONNECT(){
  digitalWrite(PIN_SPI_SS01, HIGH);   // ~SSディスエーブル。
  digitalWrite(PIN_SPI_SS02, HIGH);   // ~SSディスエーブル。
  digitalWrite(PIN_SPI_SS03, HIGH);
  digitalWrite(PIN_SPI_SS04, HIGH);
}

// BUSY解除後、指定時間待機
void L6470_wait_not_busy(long time){
  if(dvs_select==1){
    while(!digitalRead(PIN_MT_BSY01)){}
  }else if(dvs_select==2){
    while(!digitalRead(PIN_MT_BSY02)){}
  }else if(dvs_select==3){
    while(!digitalRead(PIN_MT_BSY03)){}
  }else if(dvs_select==4){
    while(!digitalRead(PIN_MT_BSY04)){}
  }else{
    while(!digitalRead(PIN_MT_BSY01) && !digitalRead(PIN_MT_BSY02) && !digitalRead(PIN_MT_BSY03) && !digitalRead(PIN_MT_BSY04)){}     // BUSY解除待機
  }
  delay(time);
}
