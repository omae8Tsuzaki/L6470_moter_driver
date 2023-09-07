# L6470_moter_driver

Arduino Mega 2560を使用。

https://kats-eye.net/info/2020/03/09/arduino-step-motor-3/
のサイトを参考にしました。  
L6470_3_Moter.inoがモータを３台、L6470_4_Moter.inoがモータを４台動かすプログラム。  
動かすモータ数を増やすにはSPI_DVS_SELECTとSPI_DISCONNECTとL6470_wait_not_busy等を変更する。  

L6470モータドライバはデイジーチェーン(daisy chain)で接続している。

