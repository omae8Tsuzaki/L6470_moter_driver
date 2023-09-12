#define MOTOR_SPEED 30000

#include <SPI.h>
#include <MsTimer2.h>
#include <stdio.h>

// ピン定義
#define PIN_SPI_MOSI 51 //11,mega51
#define PIN_SPI_MISO 50 //12,mega50
#define PIN_SPI_SCK 52 //13,mega52
#define PIN_SPI_SS 53 //10,mega53
#define PIN_BUSY 9
//#define PIN_SPI_SS02 4
#define PIN_BUSY2 3
//#define PIN_SPI_SS03 22
#define PIN_BUSY3 24
//#define PIN_SPI_SS04 23
#define PIN_BUSY4 25

void setup() {
  delay(1000);
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_SS, OUTPUT);
  //pinMode(PIN_SPI_SS02, OUTPUT);
  //pinMode(PIN_SPI_SS03, OUTPUT);
  //pinMode(PIN_SPI_SS04, OUTPUT);
  pinMode(PIN_BUSY, INPUT_PULLUP);
  pinMode(PIN_BUSY2, INPUT_PULLUP);
  pinMode(PIN_BUSY3, INPUT_PULLUP);
  pinMode(PIN_BUSY4, INPUT_PULLUP);
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  Serial.begin(38400);
  digitalWrite(PIN_SPI_SS, HIGH);
  //digitalWrite(PIN_SPI_SS02, HIGH);
  //digitalWrite(PIN_SPI_SS03, HIGH);
  //digitalWrite(PIN_SPI_SS04, HIGH);

  L6470_resetdevice(); //1台目のL6470リセット
  L6470_resetdevice2(); //2台目のL6470リセット
  L6470_resetdevice3();
  L6470_resetdevice4();
  L6470_setup();  //1台目のL6470を設定 
  L6470_setup2();  //2台目のL6470を設定
  L6470_setup3();
  L6470_setup4();
  L6470_getstatus(); //1台目のフラグ解放
  L6470_getstatus2();//2台目のフラグ解放
  L6470_getstatus3();
  L6470_getstatus4();
     
 MsTimer2::set(25, fulash);//シリアルモニター用のタイマー割り込み//25msごとに
 MsTimer2::start();
  
delay(2000);
}

//L6470_4_Moter.inoの方が賢い
//北の国から電子工作を４台に変更した。
//L6470_4_commands.inoではなくこちらを使う
//https://www.analog.com/jp/analog-dialogue/articles/introduction-to-spi-interface.html

void loop() {
  L6470_run(1, MOTOR_SPEED);
  L6470_softstop();
  delay(1000);

  L6470_run2(1, MOTOR_SPEED);
  L6470_softstop2();
  delay(1000);

  L6470_run3(1, MOTOR_SPEED);
  L6470_softstop3();
  delay(1000);

  L6470_run4(1, MOTOR_SPEED);
  L6470_softstop4();
  delay(1000);
}


void L6470_setup(){
  L6470_setparam_acc(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  L6470_setparam_kvalhold(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

  L6470_setparam_stepmood(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
}
void L6470_setup2(){
  L6470_setparam_acc2(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec2(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed2(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed2(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd2(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  L6470_setparam_kvalhold2(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun2(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc2(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec2(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

  L6470_setparam_stepmood2(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
}
void L6470_setup3(){
  L6470_setparam_acc3(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec3(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed3(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed3(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd3(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  L6470_setparam_kvalhold3(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun3(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc3(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec3(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

  L6470_setparam_stepmood3(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
}
void L6470_setup4(){
  L6470_setparam_acc4(0x30); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec4(0x30); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed4(0x2a); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed4(0x1200); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd4(0x027); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  L6470_setparam_kvalhold4(0x28); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun4(0x28); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc4(0x28); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec4(0x28); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

  L6470_setparam_stepmood4(0x02); //ステップモードdefault 0x07 (1+3+1+3bit)
}

void fulash(){
  long a=L6470_getparam_abspos();//1
  long b=L6470_getparam_speed();
  long c=L6470_getparam_abspos2();//2
  long d=L6470_getparam_speed2();
  long e=L6470_getparam_abspos3();//3
  long f=L6470_getparam_speed3();
  long g=L6470_getparam_abspos4();//4
  long h=L6470_getparam_speed4();
  char str[15];
  snprintf(str,sizeof(str),"1pos=0x%6.6X ",a);//1
  Serial.print(str);
  snprintf(str,sizeof(str),"1spd=0x%5.5X ",b);
  Serial.print(str);
  snprintf(str,sizeof(str),"2pos=0x%6.6X ",c);//2
  Serial.print(str);
  snprintf(str,sizeof(str),"2spd=0x%5.5X ",d);
  Serial.println(str);
  snprintf(str,sizeof(str),"3pos=0x%6.6X ",e);//3
  Serial.print(str);
  snprintf(str,sizeof(str),"3spd=0x%5.5X ",f);
  Serial.println(str);
  snprintf(str,sizeof(str),"4pos=0x%6.6X ",g);//4
  Serial.print(str);
  snprintf(str,sizeof(str),"4spd=0x%5.5X ",h);
  Serial.println(str);
  
 /* Serial.print("0x");
  Serial.print( L6470_getparam_abspos(),HEX);
  Serial.print(" 0x");
  Serial.print( L6470_getparam_speed(),HEX);
  Serial.print(" 0x");
  Serial.print( L6470_getparam_abspos2(),HEX);
  Serial.print(" 0x");
  Serial.println( L6470_getparam_speed2(),HEX);
  */
}
