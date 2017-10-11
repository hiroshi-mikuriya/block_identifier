# 横浜ガジェット祭り2017に出展する<br>3D LEDのブロック認識プログラム


## 本プログラム

カメラから入力した画像の中からLEGO Duploを探し出して、ブロックの色を認識するプログラム。  
認識した色の情報は後段のモジュールにTCP/IPで渡し、色ごとに決まったルールで3D LEDを点灯させる。

## 最新バージョン

- 0.9.0

## 言語

- c++11(Visual Studio 2013で開発)

## ライブラリ

ビルドでは必要だが実行には不要
- opencv
- boost
- picojson

## 動作環境

- windows  
開発はMacでも行っているので、XCODEプロジェクトもコミットしている。

## 使い方

|exe名|説明|
|---|---|
|Debug/block_identifier.exe|カメラと非接続<br>擬似カメラ画像を表示<br>TCP送信可|
|Release/block_identifier.exe|カメラと接続<br>TCP送信可|

- ローカルホスト（ポート80）にTCP送信するモード  
`block_identifier -a ::1 -p 80`
- TCP送信はしないモード  
`block_identifier`
- WEBカメラではないほうのカメラ（インカメラなど）が表示されてしまうとき  
`block_identifier -d 1`
- 色数や命令を変更する  
`block_identifier -g`  
block_identifier.xmlが出力される。  
colorタグ、instuctionタグの中身を修正する。  
`block_identifier -o block_identifier.xml`

## コマンドラインオプション

  -h [ --help ]            show help  
  -v [ --version ]         print sarry lib version  
  -g [ --generate ]        Generate option file  
  -o [ --option ] arg      Option file path  
  -d [ --device ] arg (=0) Camera device number if PC has multiple camera devices  
  -a [ --address ] arg     Python process IP address  
  -p [ --port ] arg (=80)  Python process port number

## 未実装項目

- ブロック幅を命令に反映させていない。  
ただし幅の測定までは実施済み
- ブロック色と命令の紐付けがテキトーすぎる（暫定のつもり）
- キャリブレーションモード未実装  
ただし、XMLファイルを手で修正すれば校正は可能（手動であり自動でないだけ）
