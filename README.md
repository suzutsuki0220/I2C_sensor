# IoT_environment 

I2C接続の温湿度・気圧センサーモジュールからデータを取得するためのサンプルプログラムです
センサーでデータを保存していくデーモンプログラムと、それをWeb形式で表示するためのCGIプログラムが含まれています

# Contents

* driver: Bosch BME280 からI2C経由で気温・湿度・気圧を取得するためのドライバです
* get_sensor: リアルタイムのセンサー値を取得するためのCGIプログラムです
* htdocs: Webサーバーで公開するためのファイル群が入っています
* environment_daemon: 定期的にセンサーの値を取得するデーモンです

# インストール

```
$ make install
```

※インストールには C++ の開発環境が必要です

## Webサーバーの設定

httpd.conf に以下の設定を入れてください

```
ScriptAlias /environment/cgi-bin/ "/srv/environment/cgi-bin/"
<Directory "/srv/environment/cgi-bin">
    AllowOverride None
    Options None
    Require all granted
</Directory>

Alias /environment/ "/srv/environment/htdocs/"
<Directory "/srv/environment/htdocs">
    Options None
    AllowOverride None
    Require all granted
</Directory>
```

Aliasで指定しているディレクトリは適宜変更しても構いません。上の設定では `http://%サーバーのアドレス%/environment/` でアクセス可能とするケースです

# driver

## Driverの選択

センサーは Bosch の BME280 に対応したドライバーと動作確認用の stub ドライバーが含まれています

* BME280 を使用する場合
```
make driver=bme280
```
* stub ドライバを使用する場合
```
make driver=stub
```

単にmakeを実行した場合は BME280 のドライバが使用されます

# daemon

センサーの値を記録するデーモンを起動するには、以下のコマンドを実行します

```
$ sudo -u apache /srv/environment/bin/environment_daemon -l /srv/environment/htdocs/log &
```

sudo コマンドでログ保存場所に書き込めるユーザーを指定(ここではWebサーバーデーモン実行ユーザーの apache) `-l` オプションでログの書き込み先を指定してください
デーモンのオプションは次の通りです

```
Usage: ./environment_daemon [OPTIONS]
OPTIONS
  -l: output log directory
  -s: check span second
```

記録頻度は `-s` オプションで指定できます。指定がなければ15分間隔で記録されます
