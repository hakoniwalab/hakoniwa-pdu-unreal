# hakoniwa-pdu-unreal

hakoniwa-pdu-unreal は箱庭プロジェクトで使用される PDU (Protocol Data Unity) 通信を Unreal Engine 上で扱うためのプラグインです。

本リポジトリは Unreal Engine 環境で[箱庭 Web サーバー](https://github.com/toppers/hakoniwa-webserver) と接続し、PDU データの送受信を行うモジュールとなっています。

現時点では WebClient による通信のみサポートしていますが、`CommunicationService` インタフェースを実装することでさまざまな通信方式へ拡張可能です。ROS2 メッセージから生成された PDU 型定義や通信サービスも含まれています。

詳細な利用例として、箱庭ドローンのビジュアライズを行う [hakoniwa-unreal-drone](https://github.com/hakoniwalab/hakoniwa-unreal-drone) プロジェクトがあります。

本プラグインを組み込んだ実践例として参考になります。

## 必要条件
- Unreal Engine 5.6 (動作確認済み)
- git (サブモジュールを取得するため)
- [箱庭 Web サーバー](https://github.com/toppers/hakoniwa-webserver) が起動していること

## クローン方法
```bash
git clone --recursive <このリポジトリの URL>
```
既にクローン済みの場合は、次のコマンドでサブモジュールを取得します。
```bash
git submodule update --init --recursive
```

## Unreal プロジェクトへの導入
1. 本リポジトリを Unreal プロジェクトの `Plugins` ディレクトリに配置します。
2. Unreal Editor を起動し、プラグインが認識されることを確認します。（初回はビルドが走ります）

## PDU 定義の生成 (任意)
`Source/ThirdParty/hakoniwa-ros2pdu` には、ROS2 メッセージから PDU を自動生成するための Docker 環境が用意されています。必要に応じて以下の手順を実行してください。
```bash
cd Source/ThirdParty/hakoniwa-ros2pdu
bash docker/pull_image.bash       # イメージ取得
bash docker/run.bash              # コンテナ起動
bash create_all_pdus.bash          # PDU 型の生成
```
生成された PDU は `pdu/types` 配下に配置されます。

## PDU チャンネル設定
通信に使用する PDU の組み合わせは JSON 形式で指定します。サンプルとして `config/custom.json` が用意されています。独自に設定する場合は `utils/create_pdudef.py` を用いて CSV から生成可能です。
作成した JSON ファイルを Unreal プロジェクトの `Content` 配下に置き、`UPduManager::Initialize()` の第 1 引数に相対パスを指定してください。

## モジュールのロード
プラグインを C++ コードから利用する場合はモジュールをロードします。通常は以下のように記述します。

```cpp
#include "HakoniwaPdu.h"
#include "Modules/ModuleManager.h"

FHakoniwaPduModule& Module = FModuleManager::LoadModuleChecked<FHakoniwaPduModule>("HakoniwaPdu");
```

これにより `HakoniwaPdu` モジュールがロードされ、`UPduManager` などのクラスを使用できるようになります。

## 使用例
```cpp
UPduManager* Manager = NewObject<UPduManager>();
UWebSocketCommunicationService* Service = NewObject<UWebSocketCommunicationService>();
// Content/config/custom.json を読み込み、WebSocket で接続
Manager->Initialize(TEXT("config/custom.json"), Service);
Manager->StartService(TEXT("ws://127.0.0.1:8080"));
```

通信停止時は `StopService()` を呼び出します。プラグイン破棄時には自動で停止処理が行われます。

## PduManager の主な API
`UPduManager` では以下のようなメソッドを利用して PDU を送受信します。

- `DeclarePduForRead(RobotName, PduName)` : 指定した PDU を受信用に宣言します。
- `DeclarePduForWrite(RobotName, PduName)` : 送信用に宣言します。
- `FlushPduRawData(RobotName, PduName, Data)` : バイナリデータを送信します。
- `ReadPduRawData(RobotName, PduName, OutData)` : 受信済みのバイナリデータを取得します。
- `GetPduSize(RobotName, PduName)` : PDU のサイズを取得します。
- `StopService()` : 接続を終了します。

受信したバイナリデータは `Source/ThirdParty/hakoniwa-ros2pdu` に含まれる変換 API を用いて C++ 構造体へ展開できます。以下は Twist メッセージを変換する例です。
```cpp
#include "geometry_msgs/pdu_cpptype_conv_Twist.hpp"
#include "pdu_convertor.hpp"
HakoCpp_Twist pos;
hako::pdu::PduConvertor<HakoCpp_Twist, hako::pdu::msgs::geometry_msgs::Twist> conv;
conv.pdu2cpp((char*)Buffer.GetData(), pos);
```

各 PDU のチャンネル ID は `GetPduChannelId` で確認できます。サービスの開始後に宣言を行うことで、通信相手と PDU のやり取りができるようになります。

