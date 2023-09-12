# L6470_moter_driver
４台のモータを動かすArduinoのプログラム  

Standard_modeが標準モード、Daisychain_modeがデイジーチェーンモードであるため、ピン配置が異なる。

## ファイル構成  
Daisychain_mode  
　|--L6479_4_commands.ino  
　|--L6470_commands.ino  
　|--L6470_commands2.ino  
　|--L6470_commands3.ino  
　|--L6470_commands4.ino  
Standard_mode  
　|--L6470_3_Moter.ino  
　|--L6470_4_Moter.ino  

## 標準モード

## デイジーチェーンモード

## 実験機材
<img src ="https://github.com/omae8Tsuzaki/L6470_moter_driver/assets/132863781/2e7bcfa3-d5d9-4b2e-9d69-d4bffb394275" width= "640px">


## 機材
- Arduino Mega 2560 Rev3  
- L6470使用モータドライバ (https://akizukidenshi.com/catalog/g/gK-07024/)
- バイポーラステッピングモータ ST-42BYH1004(https://akizukidenshi.com/catalog/g/gP-07600/)
- [200mm リニアステージ 一軸アクチュエータ スクエアリニアレール リニアステージアクチュエータ + 1605 ボールねじ C7 + Nema23ステッピングモータ付き 1.2N.m 2相 4線 3A 56mm 23HS6430 CNC XYZ軸 直動部品 DIY CNCルーター用 (CBX1605-200)](https://www.amazon.co.jp/CBX1605-200mm-%E4%B8%80%E8%BB%B8%E3%82%A2%E3%82%AF%E3%83%81%E3%83%A5%E3%82%A8%E3%83%BC%E3%82%BF-%E3%82%B9%E3%82%AF%E3%82%A8%E3%82%A2%E3%83%AA%E3%83%8B%E3%82%A2%E3%83%AC%E3%83%BC%E3%83%AB-Nema23%E3%82%B9%E3%83%86%E3%83%83%E3%83%94%E3%83%B3%E3%82%B0%E3%83%A2%E3%83%BC%E3%82%BF%E4%BB%98%E3%81%8D-CBX1605-200/dp/B07HNY8GNR/ref=sr_1_5?__mk_ja_JP=%E3%82%AB%E3%82%BF%E3%82%AB%E3%83%8A&crid=3EA89UZ0ZNZ2W&keywords=1%E8%BB%B8%E3%82%A2%E3%82%AF%E3%83%81%E3%83%A5%E3%82%A8%E3%83%BC%E3%82%BF&qid=1673452353&sprefix=1%E8%BB%B8%E3%82%A2%E3%82%AF%E3%83%81%E3%83%A5%E3%82%A8%E3%83%BC%E3%82%BF%2Caps%2C387&sr=8-5)

## 参考
- http://spinelify.blog.fc2.com/blog-entry-41.html
- https://kats-eye.net/info/2020/03/09/arduino-step-motor-3/
- https://www.analog.com/jp/analog-dialogue/articles/introduction-to-spi-interface.html
