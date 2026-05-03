# PolyWiFiPortal

このライブラリは、
ESP32でWiFiを簡単に利用すること
ESP32を非同期Webサーバとして簡単に利用すること
を目的に作成しています。非同期Webサーバ（ESPAsyncWebServer）とそれに必要な（AsyncTCP）は、事前にインストールしてください。

ESPAsyncWebServer
	https://github.com/ESP32Async/AsyncTCP

AsyncTCP
	https://github.com/ESP32Async/ESPAsyncWebServer

WiFiネットワークを利用してWebサービス提供する場合、そのIPアドレスが分からなければWebサーバとして
利用できません。このWiFiPortalは、既存のWiFiネットワークに接続したESP32のWebサーバに、SoftAPを
使って接続し、既存のWiFiネットワークから割り当てられたIPに切り替えるWebPortalの機能を提供します。

使い方
(1) ESPAsyncWebServerの使用に従って/index.html を実装し、次の様にportalを開始してください。

	portal.begin(WiFi_SSID, WiFi_PASS, 番号);

		WiFi_SSID, WiFi_PASS は既存の接続したいWiFiネットワークの接続情報を設定します。
		SoftAPのSSIDは、「ESP32-SETUP-番号」（番号は0～99まで、beginで指定）です。
		パスワードは「12345678」です。

注：ESP32のボードマネージャのVre3.3以降では、接続するWiFiネットワークのチャンネル番号を指定しなけ
　　れば接続できなくなったそうです。ライブラリ内にある「ScanWiFiChannel」でチャンネルを確認し、
　　ライブラリ内のPolyWiFiPortal_config.hに指定します。
		constexpr uint32_t WIFI_CHANNEL_NUM = 11;	// 11 番指定の場合

(2) Androidは、既存の接続したいWiFiネットワークに接続し、インターネット接続を確認しておきます。

(3) ESP32を起動します。Androidで、SoftAP（SSIDは"ESP32-SETUP-番号"/passwordは"12345678"）に
　　接続します。

(4) AndroidのWebクライアントでSoftAPのデフォルトIPアドレス192.168.4.1にアクセスします。
　　http://192.168.4.1/  Webページが表示されれば20秒ほどで、既存のWiFiに切り替えられます。

注：http://192.168.4.1/ にアクセスするとportalはSoftAPを停止します。
　　Androidは接続中のWiFi（SoftAP）が停止すると接続実績のある既存WiFiに接続し直します。
　　http://192.168.4.1/ ページは、自動で http://WiFiのIP/index.html に接続を切り替えます。
　　AndroidのSSID切り替えのために、Webページの切り替えは15秒待ってから行います。

注：http://192.168.4.1/ 以外のpathにアクセスしてもSoftAPは停止しません。
　　この場合、http://WiFiのIP/index.html と http://192.168.4.1/index.html が利用可能です。 

参考：
　　ESP32をWiFi Webサーバとして利用する場合、通常mDNSを利用してホストを参照します。
　　しかし、mDNSは、同一セグメント内での利用に限定され多くの場合使えません。
　　（セグメントの切り分けを行っていない自宅での利用以外はほとんど使い物にならないと思います）


AndroidのWebクライアントを利用することを想定しています。
　〇Androidは同時に複数のSSIDに接続できないことを前提としています。

PCを利用しての開発では、USBシリアル接続でIPを確認してください。
　〇portalを使うことでWiFi接続が安定します。
　　（これはVer3.3以降のシステムが安定したためと考えられます）
　〇一度WiFi_SSID側に接続すればIPアドレスは通常同じアドレスが割り当てられます。
　〇ネットワーク接続ができていれば、PCから直接IP指定でESP32のWebサーバに接続できます。
　〇「ESP32-SETUP-番号」を経由せずに直接ESP32側のSTA側のアドレスにアクセスして利用できます。
　　WiFi_SSID側に割り当てられるIPアドレスが変更された場合にのみSoftApに接続すれば良いでしょう。


このライブラリの何が嬉しいかと言うと、ESP32側に何も表示システムがなくてもWiFiを用いてデフォルト
（192.168.4.1）にアクセスすることで既存WiFiに割り当てられたIPのWebサーバへの接続ができることです。
多分、多くの方がこれをいろいろと工夫して実装していると思います。
それを授業で一斉に利用するためにまとめました。
因みに、私の使っている環境（ポリテクカレッジ九州）では、WiFiと各教室が同じセグメントではないため、ｍDNSは利用できません。
ただ、同じセグメントで数十台のmDNSがうまく動作するかという問題もあります。数十台の動作確認では、必ずhost名の重なり出ますよね。

PolyWiFiPortalの設定値は PolyWiFiPortal_config.h にまとめていますが、問題なければデフォルトのまま使ってください。
WiFiのチェンネル番号は指定しないと繋がらないと思いますので、これは変更してください。
（WiFiのチェンネル番号はライブラリ内にある「ScanWiFiChannel」で確認できます。ステルスSSIDは確認できません。）

WiFiのチェンネル番号が違っているとWiFi接続できません。
接続したいWiFiのチェンネル番号を確認し、指定してください。

ESP32のSoftAPとステーション側のWiFi接続は異なるチェンネル番号で通信はできないようです。
ESP32用ボードマネージャの3.3以降は、チェンネルを切り替えて接続試行はしてくれないようです。
ですから、WiFiのチェンネル番号を把握しておかなければWiFiに接続できません。

まだ、githubの使い方、Arduinoのライブラリ等テスト段階です。

2026年4月16日 kpc.ac.jp@gmail.com Ver 0.1
2026年4月27日 kpc.ac.jp@gmail.com Ver 0.2
2026年5月02日 kpc.ac.jp@gmail.com Ver 0.3