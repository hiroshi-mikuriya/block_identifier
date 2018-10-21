# python版 ブロック認識への取り組み

## 背景

Maker Faire Tokyo 2018では、ブロック認識プログラムはC++で実装されたものを使用していた。しかし当日の環境による影響でブロック認識率が悪くなってしまった。  
ひとつの改善策として、カメラをマニュアルモードにして環境による変化を受けないようにすることを検討した。カメラをマニュアルで使用するためにはPython版のカメラライブラリが必要となるため、C++のコードをPythonへ移植することとした。

## 構成

本プログラムは、GOフレームワークと連携して稼働します。  
https://github.com/YGFYHD2018/3d_led_cube_go

## 環境構築

* SSH有効  
sshという名称の空ファイルを作る
* パスワード変更（to 警告消す）  
`sudo raspi-config`
* apt-getアップデート  
`sudo apt-get update`  
`sudo apt-get upgrade`
* vimインストール  
`sudo apt-get install vim`
* FTP有効  
http://yamaryu0508.hatenablog.com/entry/2014/12/02/102648
* リモートデスクトップ有効  
`sudo apt-get install xrdp`
* ホスト名変更  
`sudo raspi-config`  
Network  
Host name  
block-identifier
* 本プログラム取得  
`git clone https://github.com/hiroshi-mikuriya/block_identifier`
* pythonのライブラリをインストール  
`sudo apt-get install libopencv-dev python-opencv python-dev python-picamera`

