# これは何？

Unreal Engineでプラグインプロジェクトを作成するための手順を説明します。

# 環境

- Unreal Engine 5.6以降
- Visual Studio 2022以降
- Windows 11
- C++開発環境

# 前提条件
- Unreal Engineがインストールされていること
- Visual Studioがインストールされていること
- C++開発環境がセットアップされていること

# プラグインプロジェクトの作成手順

まずは、プラグインを開発するためのプロジェクトを作成します。


1. **Unreal Engineを起動**: Unreal Engineを起動します。

![image](/docs/images/tips/plugin/activate.png)

2. **新規プロジェクトの作成**: 「新規プロジェクト」を作成します。

![image](/docs/images/tips/plugin/project-create.png)

必ず、`C++`プロジェクトを選択してください。Blueprintプロジェクトではプラグインの作成ができません。

この例では、プロジェクトの場所は、`C:\project\work` とし、プロジェクト名は、`HakoniwaPduDev`としています。

設定完了後、`作成`ボタンをクリックします。


成功すると、`Visual Studio` と　`Unreal Engine` が起動し、プロジェクトが作成されます。

Visual Studio:
![image](/docs/images/tips/plugin/visual-studio-activation.png)

Unreal Engine:
![image](/docs/images/tips/plugin/unreal-engine-activation.png)

# プラグインの作成

Unreal Engineが起動し、プロジェクトが作成されたら、次にプラグインを作成します。

1. **編集メニューからプラグインを選択**: Unreal Engineのメインメニューから「編集」を選択し、「プラグイン」をクリックします。

![image](/docs/images/tips/plugin/plugin-menu.png)

2. **新規プラグインの作成**: 「新規プラグイン」をクリックします。

![image](/docs/images/tips/plugin/plugin-create.png)

今回は、`空白` のプラグインを作成します。プラグイン名は、`HakoniwaPdu`とします。

`プラグインを作成` ボタンをクリックします。

成功すると、`Visual Studio` から、`Plugins` フォルダが見えます。

![image](/docs/images/tips/plugin/visual-studio-plugin.png)

ただ、このままだと、エラーが残ります。次に続く。

# uprojectの設定

1. Visual Studio と Unreal Engine を終了します。
2. **uprojectファイルの編集**: プロジェクトの`uproject`ファイルを開きます。

3. **プラグインの追加**: `Plugins`セクションに以下のように記述します。
   ```json
   "Plugins": [
        :
       {
           "Name": "HakoniwaPdu",
           "Enabled": true
       }
   ]
   ```
4. **保存**: `uproject`ファイルを保存します。
5. `uproject`ファイルを右クリックし、「Generate Visual Studio project files」を選択します。
6. **Visual Studioでプロジェクトを開く**: Visual Studioでプロジェクトを開きます。
7. **ビルド**: Visual Studioでプロジェクトをビルドします。

ビルド→ソリューションのビルドを選択します。うまくいけば成功するはず。

8. **Unreal Engineを起動**: Unreal Engineを起動し、プロジェクトを開きます。

