# blink_zq

Vivado/Vitis 2023.2（特にVitis）は、2020年版までの「Vitis Classic」から「Vitis Unified IDE」へとUIが劇的に刷新されました。そのため、書籍の手順とは画面構成や用語が全く異なります。

2023.2で「Hoge」をプリントするアプリケーションを作成する正しい手順を整理しました。

---

### Vitis 2023.2 でのアプリケーション作成手順

Vitis Unified IDEを起動した後の手順は以下の通りです。

#### 1. ワークスペースの作成とプラットフォームの確認

すでにプラットフォーム（`.xsa`ベースのStandalone）は作成済みとのことですので、以下の状態になっているはずです。

* 左側の **Component** ビューに、作成したプラットフォームが表示されている。

#### 2. アプリケーションコンポーネントの作成

2023.2からは「Application Project」ではなく **「Application Component」** と呼びます。

1. **Welcomeページ** または **File > New > Component > Application** を選択します。
2. **Name and Location**: アプリ名（例: `hello_world`）を入力します。
3. **Select Platform**: すでに作成済みのプラットフォームを選択します。
4. **Select Domain**: `standalone_ps7_cortexa9_0`（Zynqの場合）など、対応するドメインを選択します。
5. **Next** を押し、テンプレートの選択画面へ進みます。

#### 3. テンプレート（Hello World）の選択

1. テンプレート一覧から **「Hello World」** を選択します。
2. **Finish** を押すと、プロジェクト（Component）が生成されます。

#### 4. ソースコードの編集

1. 左側の Component ビューで `hello_world` を展開し、`Sources > src > helloworld.c` をダブルクリックして開きます。
2. `print("Hello World\n\r");` を `print("hoge\n\r");` に書き換えて保存（Ctrl+S）します。

#### 5. ビルド

1. Component ビューで `hello_world` を選択した状態にします。
2. 下の **FLOW** パネル（または右クリックメニュー）にある **Build** をクリックします。
3. コンソールに `Build Finished Successfully` と出れば成功です。

---

### 2020年版（Classic）と2023.2（Unified）の主な違い

書籍を読み進める上で混乱しやすいポイントをまとめました。

| 項目 | 2020.x (Classic) | 2023.2 (Unified) |
| --- | --- | --- |
| **UIのベース** | Eclipse | VS Code ベース |
| **プロジェクト単位** | Application Project | Application Component |
| **設定ファイル** | `lscript.ld` (Linker Script) | `vitis-comp.json` 内で管理 |
| **ビルド設定** | 右クリック > Build Project | FLOWパネルの Build ボタン |
| **デバッグ/実行** | Debug As > Launch on Hardware | FLOWパネルの Debug または Run |

---

### 実行・デバッグ時の注意点

ビルドが終わった後、実際にボードで動かす際の手順です。

1. **シリアルターミナルの準備**: Vitis 2023.2 には標準で「Serial Terminal」タブがあります。これを開き、UARTの設定（通常は 115200 baud）を行います。
2. **実行**: FLOWパネルの **Run** を押します。
* 初回実行時、`Launch Configuration` の作成を求められることがありますが、基本的にはデフォルトのままで「Standalone on Single Core」を選べばOKです。


3. **FPGAのプログラミング**: Unified IDEでは、Run/Debugボタンを押すと、必要に応じて `.bit` ファイル（XSAに含まれているもの）を自動的にダウンロードしてからソフトを走らせてくれます。

### うまくいかない場合のチェックポイント

* **UARTの接続**: Vivadoのブロックデザインで、Zynqの `UART1`（またはUART0）が有効になっていて、ボード上のUSB-UARTブリッジに繋がる設定になっていますか？
* **Stdoutの設定**: プラットフォームの `Board Support Package (BSP)` 設定を開き、`stdin` と `stdout` が `ps7_uart_1` など、実際のハードウェアに割り当てられているか確認してください。

もし特定の画面でエラーが出る場合は、そのメッセージを教えていただければさらに詳しく解説できます。

---

テンプレートが選べないですが、どうしたらいいですか？

---
Vitis 2023.2（Unified IDE）でテンプレート（Hello Worldなど）が表示されない、あるいは選択できない場合、いくつかの原因が考えられます。

主に、「プラットフォームのビルドが終わっていない」**か、**「ドメイン（OS/プロセッサの設定）が正しく認識されていない」ことが原因であることが多いです。以下の手順で確認・修正してみてください。

---

### 1. プラットフォームコンポーネントをビルドする

Vitis 2023.2では、プラットフォームを作成しただけでは不十分で、一度**ビルド**してライブラリを生成しないと、アプリケーション側でテンプレートが表示されないことがあります。

1. 左側の **Component** ビューで、作成したプラットフォーム（例：`design_1_wrapper`）を選択します。
2. 画面下の **FLOW** パネルにある **Build**（トンカチのアイコン）をクリックします。
3. ビルドが完了（Success）するまで待ちます。

### 2. Domain（ドメイン）の設定を確認する

テンプレートは「Standalone」や「Linux」といったドメインに紐付いています。プラットフォーム作成時にドメインが正しく設定されていないと、テンプレートが出てきません。

1. プラットフォームコンポーネントの中にある `vitis-comp.json` を開きます。
2. **User Domains** のリストに `standalone_ps7_cortexa9_0`（Zynqの場合）などが存在し、ステータスが正常か確認してください。
3. もし空欄であれば、`+` ボタンから新しく Standalone ドメインを追加してください。

### 3. Application Component 作成時の「Select Domain」

アプリケーション作成ウィザードを進める際、以下の点に注意してください。

* **Select Platform**: 作成してビルドしたプラットフォームを確実に選択します。
* **Select Domain**: ここでプラットフォーム内のドメイン（例：`ps7_cortexa9_0`）を選択しないと、次の画面でテンプレートが空になります。

---

### それでも解決しない場合の「手動」対処法

もしウィザードがバグ等で動かない場合は、以下の方法を試してください。

#### A. Vitisを再起動して「Examples」から作る

1. Vitis Unified IDE のトップ画面（Welcomeページ）にある **Examples** をクリックします。
2. `Embedded Software Examples` の中にある **Hello World** を探します。
3. **Create Application Component from Template** をクリックし、作成済みのプラットフォームを指定します。

#### B. Workspace を作り直す（最終手段）

Vitis 2023.2 は、以前のプロジェクト設定がワークスペースに残っていると挙動が不安定になることがあります。

1. 一度 Vitis を閉じ、デスクトップなどに**新しい空のフォルダ**を作成します。
2. Vitis を起動し、その新しいフォルダを Workspace として指定します。
3. もう一度 `.xsa` のインポート（Platform作成）からやり直してみてください。

---

### 2023.2 での画面イメージ

### 補足：XSAファイルにCPU情報が含まれているか

Vivadoで `Export Hardware` を行う際、**"Include bitstream"** にチェックを入れ、なおかつブロックデザイン内に **Processing System 7 (Zynq)** が正しく配置・配線されている必要があります。これがないと、Vitisは「どのCPU向けのソフトを作ればいいか」がわからず、テンプレートを出せません。

まずは **「1. プラットフォームのビルド」** が終わっているか、今一度チェックしてみてください！