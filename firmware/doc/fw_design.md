# FW Design Information
Notes on FW design, etc.

## Task

| Task name | function | Stack size [bytes] | Priority |
| --- | --- | --- | --- |
| loopTask | Arduino loop task | 8192 | 1 |
| drawLoop | Avatar control | 4 * 1024 | 1 |
| facialLoop | Avatar control | 1024 | 2 |
| lipSync | Lip Sync for avatar| 2048 | 2 |
| servo | Servo control synchronized with the avatar | 2048 | 1 |
| battery_check | Battery level check | 2048 | 1 |
| asyncTtsStreamTask | TTS streaming play | 5 * 1024 | 2 |
| webSocketLoopTask | WebSocket processing for LLM Realtime API | 6 * 1024 | 2 |

## Realtime API Function Calling

### Avatar Expression

Realtime API ビルドでは、Function Calling により AI が会話中の感情に合わせて Avatar の表情を変更できる。

- 対象ビルド
  - `REALTIME_API` が定義される PlatformIO 環境。
- 関数名
  - `set_avatar_expression`
- 引数
  - `expression`: `neutral`, `happy`, `angry`, `sad`, `doubt`, `sleepy`
- 実装
  - `src/llm/ChatGPT/FunctionCall.cpp`
  - `m5avatar::Expression` に変換して `Avatar::setExpression()` を呼び出す。
  - `LLMBase.cpp` の `systemRole_realtimeAvatarExpression` で Realtime API の system instructions に利用方針を明示する。
- スコープ
  - `json_Functions` は通常 ChatGPT や Gemini Live からも参照されるため、この関数の schema と実行処理は `#if defined(REALTIME_API)` で限定する。
  - `systemRole_realtimeAvatarExpression` は Realtime 系 LLM の `load_role()` で `systemRole_memory` または `systemRole_noMemory` に追加する。

## Web App
WebAPI.cpp のインラインアセンブラ(マクロ：IMPORT_FILE)で incbinフォルダ内のhtmlファイルやjsファイルをプログラム領域に埋め込む。

### Personalize page
- ファイル構成
  - incbin/personalize.html
  - incbin/personalize.js
- 言語
  - ページやダイアログ内の言語は英語版のみ。
- 画面構成
  - Role (Custom Instructions)

#### Role (Custom Instructions)
- 構成
  - フォーム
    - ロール（カスタム指示）の入出力。
  - 設定ボタン
    - API /role_set をPOSTし、フォームの内容を設定する。
- 画面更新時の動作
  - API /role_get をPOSTし、現在設定されているカスタム指示を取得してフォームに表示する。 

#### Memory
- 構成
  - フォーム
    - 取得した記憶内容を表示。
  - Clearボタン
    - API /memory_clear をPOSTし、記憶内容を消去する。
    - 消去を実行する前に、OK/Cancelのダイアログを表示して本当に消去してよいかを確認する。
- 画面更新時の動作
  - API /memory_get をPOSTし、記憶内容を取得してフォームに表示する。
 

## Gemini Live のTLS検証（岡ゼミ版、2026-10-25）

GeminiLiveはGoogle公式GTS Root R1〜R4をrootCAgoogleGemini.hに保持し、beginSslWithCAで接続する。接続先名と有効期限の検証はWiFiClientSecure/mbedTLSに委ねる。既存の起動時NTP同期とWebSocket再接続を利用し、検証省略へのフォールバックは行わない。PC検査と実機確認の区別・書き込み手順はdoc/20261025_Gemini証明書対策.md（リポジトリ直下）を参照。

## Gemini Liveの表情選択教材（2026-11-18）

GeminiLiveは既存のset_avatar_expressionの宣言を再利用し、公開する表情をhappy / neutralに限定する。他のLLM向けの6種類の定義は変更しない。googleSearchと接続済みMCPツールを維持し、update_memoryは既存の長期記憶設定が有効な場合だけ公開する。その他のローカル関数はこの教材では公開しない。

関数要求は配列の全件を処理し、id/nameを引き継いだtoolResponse.functionResponsesをArduinoJsonで組み立てる。未公開の関数や不正な表情にはエラーを返す。音声とturnCompleteは独立した条件で処理し、同じメッセージ内でも終了処理を落とさない。テキスト・バイナリ両フレームのJSONを処理する。

受信側は表情の選択をatomicで保存し、RealtimeAiModのUI側で反映する。直接Avatarを変更する既存の関数実装は使わず、既定表情による上書きと受信・UI間の二重制御を避ける。次の録音開始・エラー・切断・ツール取消時に選択を解除する。RealtimeLLMBaseの既定フックは何もしないため、他のLLMの表情動作は維持する。厳密な音声同期とAIによる首振りは対象外。学生向け手順はリポジトリ直下のdoc/AI表情_最初の改造例.mdを参照。
