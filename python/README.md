# python版 ブロック認識への取り組み

## 背景

Maker Faire Tokyo 2018では、ブロック認識プログラムはC++で実装されたものを使用していた。しかし当日の環境による影響でブロック認識率が悪くなってしまった。  
ひとつの改善策として、カメラをマニュアルモードにして環境による変化を受けないようにすることを検討した。カメラをマニュアルで使用するためにはPython版のカメラライブラリが必要となるため、C++のコードをPythonへ移植することとした。

## 開発環境構築

pythonのライブラリをインストールする。

```
sudo apt-get install libopencv-dev python-opencv python-dev python-picamera
```

golangをインストールする。

```
$ wget https://storage.googleapis.com/golang/go1.11.1.linux-armv6l.tar.gz 
$ sudo tar -C /usr/local -xzf go1.11.1.linux-armv6l.tar.gz
```

/usr/local/go/binにPATHを通す。
~/.bashrcファイルの末尾に

`export PATH=$PATH:/usr/local/go/bin`

を追加します。

/usr/local/go/bin/goにSUIDを付与しておきます。これによりgoは常にroot権限で実行されます。  
`$ chmod +s /usr/local/go/bin/go`
