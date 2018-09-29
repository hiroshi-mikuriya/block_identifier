# python版 ブロック認識への取り組み

## 背景

Maker Faire Tokyo 2018では、ブロック認識プログラムはC++で実装されたものを使用していた。しかし当日の環境による影響でブロック認識率が悪くなってしまった。  
ひとつの改善策として、カメラをマニュアルモードにして環境による変化を受けないようにすることを検討した。カメラをマニュアルで使用するためにはPython版のカメラライブラリが必要となるため、C++のコードをPythonへ移植することとした。

## 開発環境構築

pythonのライブラリをインストールする。

```
sudo apt-get install libopencv-dev python-opencv python-dev python-picamera
```

