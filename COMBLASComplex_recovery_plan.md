# COMBLAS Complex API ギャップ対応計画 (2025-09-22)

## 背景と現状整理
- 2025-09-22 の MSTest 実行では 113 件中 72 件成功、41 件失敗。失敗ログは `COM_BLAS_UnitTest_Managed/bin/Any CPU/Debug/net8.0-windows/TestResults/Codex.trx` に記録済み。
- 失敗の大半が `RuntimeBinderException` に起因し、`COMBLASLib.BLASClass` に `ZSymmSimple` など複素数 API が存在しない、もしくは署名不一致であることが示唆される。
- 一部テストでは HRESULT が仕様と異なる (`TrmmSimple_ReturnsPointerWhenBNull` で `E_POINTER` を期待するが `E_INVALIDARG` が返却)。また `Rotmg_ScalarCase` では COM 側が 0 値 VARIANT の変換に失敗している。
- `IBLASComplex_new_api_definitions.md` などのドキュメントは実装済みと記載しているが、提供された DLL / TypeLib が未更新である可能性が高く、情報が乖離している。

## 失敗カテゴリと原因仮説
- **Missing Method 定義**: Type Library (`COMBLAS.tlb`) が v1.3 仕様に追従しておらず、Interop 生成時に `ZGemmSimple` 以外の複素数 API が公開されていない。
- **署名不一致**: テストは実部・虚部の SAFEARRAY を別々に渡す前提だが、COM 側が複合構造体や異なる順序を期待している。IDL と実装の間で `SAFEARRAY` 属性や `[in, out]` 修飾が揃っていない可能性。
- **エラーコード運用不整合**: 既存実装が `E_INVALIDARG` を返すところをテストでは `E_POINTER` を期待しており、仕様合意が取れていない。
- **引数変換エラー**: `Rotmg_ScalarCase` では `VARIANT` → ネイティブ変換が想定通りになっておらず、IDispatch 層の `DISPID` 対応や `SAFEARRAY` 下限が影響している恐れ。

## 対応方針
1. **Type Library と IDL の同期**: 現行配布物の `COMBLAS.idl` と生成された `COMBLAS.tlb` を照合し、漏れている `IBLASComplex` メソッドを洗い出す。IDL 更新後は必ず MIDL を再実行し、`COMBLASLib` の登録を更新する。
2. **COM マップの既定 IDispatch を `IBLASComplex` に切り替え**: `CBLAS` の `COM_INTERFACE_ENTRY2(IDispatch, ...)` を見直し、`IID_IDispatch` から `IBLASComplex` の型情報を返すよう ATL マップを修正する。これにより `tlbimp` 生成クラスや VBA などの Automation クライアントでも 27 メソッドが列挙可能になる。
3. **IDL/実装/テストのインターフェース整合**: SAFEARRAY の次元・下限・`[in, out]` 指定、係数パラメータの並びをテスト仕様と突き合わせ、COM 実装 (`BLAS.cpp`) の marshaling ヘルパーを調整する。
4. **エラーコード仕様の明文化**: HRESULT の返却条件を `ReadMe.md` / テストコメントで明記し、`E_POINTER` を期待するケースについて実装側と合意を取る。合意結果に合わせて実装またはテストを修正。
5. **ドキュメント整備**: `IBLASComplex_new_api_definitions.md` と `ReadMe.md` を現状に合わせて更新し、進捗と既知の制限を明確化。
6. **検証とリグレッション防止**: フル MSBuild で COM DLL とテストプロジェクトをビルドし、`COM_BLAS_UnitTest_Managed.exe` のフル実行で全 113 テストの成功を確認する。必要に応じて `dotnet test` 用の代替シナリオを `TROUBLESHOOTING.md` に追記。

## 手順詳細
### フェーズ 0: 現状の Type Library 解析
- `oleview` もしくは `midl /tlb` を用いて現行 `COMBLAS.tlb` をダンプし、`IBLASComplex` のメソッド一覧を抽出。
- `git` 上の `COM_BLAS/COMBLAS.idl` と差分比較し、未公開メソッドと署名差異を表にまとめる。
- `IBLASComplex_new_api_definitions.md` に現状のギャップを追記し、進捗ログを刷新。
- 2025-09-22 18:55 (JST): `midl COMBLAS.idl` → `tlbimp` の結果、`IBLASComplex` の公開メソッドが 4 件→27 件になったことを確認。旧バイナリ配布物との差分リストは `IBLASComplex_new_api_definitions.md` に反映済み。

### フェーズ 1: IDL/実装の同期
- `COM_BLAS/COMBLAS.idl` を v1.3 仕様に更新し、必要な `[propget]` / `[propput]` 属性・`SAFEARRAY` 宣言を追加。
- `midl.exe COMBLAS.idl` を実行し、`COMBLAS_i.c` / `.h` / `.tlb` / プロキシ スタブを再生成。
- `COMBLAS.vcxproj` のビルド生成物 (`COMBLASLib.tlb`) を登録し直し、`regsvr32` で DLL を再登録。

### フェーズ 2: 実装と marshaling 調整
- `BLAS.cpp` に欠落している `IBLASComplex` 実装を追加 (既存の `GatherComplexMatrix` 等を流用)。
- `BLAS.h` の COM マップを修正し、`IID_IDispatch` に対して `IBLASComplex` の `IDispatchImpl` を返すよう `COM_INTERFACE_ENTRY2` を更新。必要に応じて QueryInterface の拡張を検討。
- Hermitian/Symmetric 系は `CompleteHermitianMatrix` などのヘルパーで反対側の三角を同期し、テスト期待値と一致させる。
- `Rotmg` 系は `VARIANT` 変換ヘルパーを精査し、`0.0` を渡した際の `ChangeType` 失敗を解消。

### フェーズ 3: エラーコードと例外処理
- `TrmmSimple_ReturnsPointerWhenBNull` などテストで `E_POINTER` を期待するケースを一覧化し、`BLAS.cpp` 側のガード条件を `E_POINTER` で返すよう修正。
- 他 API のガード条件 (`E_INVALIDARG`, `E_BOUNDS`) も仕様化し、`TROUBLESHOOTING.md` に既知のエラーコードを追記。

### フェーズ 4: 検証
- Visual Studio 付属 `MSBuild.exe` で `COM_BLAS.sln` を `Debug|x64` ビルド。
- `COM_BLAS_UnitTest_Managed.exe` を再実行し、41 件の失敗が解消されることを確認。
- 成功後は `TestResults` の最新 `.trx` を保存し、コミットメッセージにテスト結果概要を含める。

## リソースと依存関係
- **外部**: COM DLL と TypeLib の更新を行うビルド環境 (Visual Studio 2022 + C++/ATL + WiX)、OpenBLAS ランタイム (`libopenblas.dll`)。
- **内部**: SAFEARRAY ヘルパー、Hermitian ミラーリングユーティリティ、`ComplexBlasHandle`。
- 既存ビルドスクリプトの `ResolveComReference` は .NET Core 版では使用不可のため、CI ではフル MSBuild に統一。

## リスクとフォールバック (非推奨)
- TypeLib 更新を怠ると `RuntimeBinderException` が再発するため、DLL 配布時に `tlb` とセットで登録する手順を必須化する。
- 一時的にテストを `NotImplemented` マークで抑制する案は品質低下につながるため採用しない。

## 次のアクションチェックリスト
- [x] `COMBLAS.tlb` の実体をダンプし、欠落メソッド一覧を作成（2025-09-22 18:55 JST 時点で 27 メソッド公開を確認済み）
- [x] COM マップの `IID_IDispatch` 対応を `IBLASComplex` に切り替え（2025-09-22 21:15 JST 適用）
- [ ] IDL とドキュメントを v1.3 仕様に同期
- [ ] `BLAS.cpp` の実装補完と HRESULT 整理
- [ ] フル MSBuild + MSTest 実行で 113 件成功を確認
- [ ] ドキュメント更新 (`ReadMe.md`, `IBLASComplex_new_api_definitions.md`, `TROUBLESHOOTING.md`)
