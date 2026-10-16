# DxLib（共通ライブラリ）

各作品の `Source` フォルダから `../../DxLib/file` を参照しています。
リポジトリをそのまま clone（または ZIP でダウンロード）すれば、DXライブラリを別途インストールしなくてもビルドできます。

- DXライブラリ Ver 3.24f（Effekseer For DXLib 同梱）
- 対象: Visual Studio 2022 / x64 / ランタイム MT・MTd

## file フォルダの中身

公式配布物「プロジェクトに追加すべきファイル_VC用」から、4作品が実際にリンクするものだけを残しています。

| 種類 | 内容 |
|---|---|
| ヘッダー | `DxLib.h` などの `.h` と `Effekseer*/`, `LLGI/` フォルダ（すべて） |
| lib（Release） | `*_vs2015_x64_MT.lib`, `Effekseer*_vs2022_x64.lib` |
| lib（Debug） | `*_vs2015_x64_MTd.lib`, `Effekseer*_vs2022_x64_d.lib` |
| その他 | `DxLib.txt`（公式の説明書） |

x86 用・VS2012〜2019 用・MD ランタイム用の lib は含めていません。
他の構成でビルドする場合は、公式サイトから同じバージョンをダウンロードして `file` に追加してください。

## ライセンス

DX Library Copyright (C) 2001-2026 Takumi Yamada.
https://dxlib.xsrv.jp/
