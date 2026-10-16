# KaminagaAyumu_GameData

今までに制作したゲーム（C++ / DXライブラリ）のデータをまとめています。

| フォルダ | 作品 | 制作時期 |
|---|---|---|
| `0_TheKnightOfFUGEDA` | The Knight of FUGEDA | 2026年6月〜9月 |
| `1_BarrierCounter` | Barrier Counter | 2026年3月〜4月 |
| `2_JumpHero` | Jump Hero | 2025年11月〜2026年2月 |
| `3_BlockDestroyer` | Block Destroyer | 2025年7月〜8月 |

各作品の詳細（開発期間・動作確認環境・動画）は、それぞれのフォルダ内の README を参照してください。

## フォルダ構成

```
<作品名>/
├─ Release/   実行ファイルとゲームデータ（ダウンロードしてそのまま遊べます）
└─ Source/    Visual Studio のプロジェクトとソースコード
DxLib/
└─ file/      共通の DXライブラリ（ヘッダーと lib）
```

## ビルド方法

1. このリポジトリを clone（または ZIP でダウンロード）します。
2. `<作品名>/Source/*.sln` を Visual Studio 2022 で開きます。
3. 構成を `Release | x64` にしてビルドします。

DXライブラリは `DxLib/file` に同梱しているので、別途インストールは不要です。
対応している構成は **x64（Debug / Release）** のみです。詳細は [DxLib/README.md](DxLib/README.md) を参照してください。

## 遊び方

`<作品名>/Release/` 内の `.exe` を実行してください。`Data`（または `data`）フォルダは `.exe` と同じ場所に置いたままにしてください。
