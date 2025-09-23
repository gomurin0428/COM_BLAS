# TEXT_FILE_OVERVIEW

## ルート
- `.gitattributes`: Git の属性設定。改行コードやマージ戦略を制御。
- `.gitignore`: ビルド生成物・VS のキャッシュ・TestResults などを無視する設定。
- `Agents.md`: リポジトリ運用時の注意事項とガイドライン。
- `COM_BLAS.sln`: Visual Studio ソリューション。COM 本体、proxy/stub、MSTest、Installer を束ねる。
- `COMBLASComplex_recovery_plan.md`: 複素数 BLAS API がテストで欠落している事象に対する対応計画と検証チェックリスト。フェーズ0 (TypeLib ギャップ洗い出し) 完了ログに加えて、2025-09-22 の COM マップ修正 (IID_IDispatch→IBLASComplex) を対応方針へ反映。
- `IBLASComplex_new_api_definitions.md`: 複素数 BLAS API 拡張の進捗メモと仕様整理。2025-09-22 に TypeLib 再生成状況と COM マップ更新ログ (IDispatch が 27 メソッドを返すことの検証結果) を追記。
- `PrepareMatrixView_fix_plan.md`: SAFEARRAY の行列ビュー周りの改善計画と検証ログ。
- `ReadMe.md`: COM_BLAS の概要、公開 API、ビルドとセットアップ手順。最新 TypeLib の再登録手順と、2025-09-22 時点で COM マップが `IBLASComplex` を既定 IDispatch に切り替えつつ `IBLAS` へフォールバックする旨を記載。
- `TEXT_FILE_OVERVIEW.md`: 本ファイル。テキスト資産の一覧。
- `TROUBLESHOOTING.md`: ビルド・テスト・依存 DLL の既知の落とし穴と対処法。TypeLib 再登録に加え、COM マップ更新後も複素数 API が列挙されない場合の対策を 2025-09-22 に追記。
- `makingInstallerPlan.md`: Visual Studio Installer Projects を使った MSI 作成手順の詳細プラン。
- `残作業.md`: リリース完了までに必要な TODO リスト。

## COM_BLAS ディレクトリ
- `COM_BLAS/BLAS.cpp`: COM 実装の本体。SAFEARRAY マーシャリングと OpenBLAS 呼び出しに加え、2025-09-22 版で `EnsureDoubleSafeArray` の `NULL` 入力を `E_POINTER` に統一し、`CBLAS::Invoke` が `DISPPARAMS` を都度コピーして `IBLASComplex` → `IBLAS` の順で解決するようになった。2025-09-23 の修正では `TrmmSimple` へ `SAFEARRAY** B` が `NULL` のまま渡された場合に `E_POINTER` を返し直すガードと、`Rotmg` を COM Automation から呼び出した際に `VT_BYREF|VT_VARIANT` 経由の SAFEARRAY を正しく束ねる専用ディスパッチを追加。
- `COM_BLAS/BLAS.h`: `CBLAS` クラス宣言。`DECLARE_REGISTRY_RESOURCEID` 等の ATL マクロに加え、2025-09-22 の `COM_INTERFACE_ENTRY2(IDispatch, IBLASComplex)` 変更を含む。
- `COM_BLAS/BLAS.rgs`: BLAS クラスのレジストリ スクリプト (ProgID、CLSID、TypeLib)。
- `COM_BLAS/COMBLAS.idl`: 公開インターフェース (IBLAS / IBLASComplex) と列挙体を定義する MIDL。2025-09-22 版では `midl` 再実行済みで TypeLib (27 メソッド) と同期済み。
- `COM_BLAS/COMBLAS_i.h`: MIDL 自動生成のインターフェース定義ヘッダー。手動編集不可。
- `COM_BLAS/COM_BLAS.cpp`: DLL エントリーポイントなど ATL のブートストラップ コード。
- `COM_BLAS/COM_BLAS.def`: エクスポートシンボルの定義。
- `COM_BLAS/COM_BLAS.rc`: リソーススクリプト。バージョン情報・レジストリスクリプト関連を保持。
- `COM_BLAS/COM_BLAS.rgs`: COM DLL 全体に対する追加レジストリ設定 (現状プレースホルダー)。
- `COM_BLAS/COM_BLAS.vcxproj`: COM DLL プロジェクト設定。OpenBLAS を同梱の lib/include から参照し、vcpkg 自動統合を無効化。
- `COM_BLAS/COM_BLAS.vcxproj.filters`: プロジェクト内のフィルター構成。
- `COM_BLAS/COM_BLASps.def`: proxy/stub DLL 用 DEF。
- `COM_BLAS/dllmain.cpp`: DLL のエントリーポイント。
- `COM_BLAS/dllmain.h`: `DllMain` の宣言と ATL モジュールのラッパー。
- `COM_BLAS/framework.h`: Windows ヘッダーの集約。
- `COM_BLAS/pch.cpp`: プリコンパイル済みヘッダーのソース。
- `COM_BLAS/pch.h`: PCH のヘッダー。
- `COM_BLAS/resource.h`: リソース識別子定義。
- `COM_BLAS/targetver.h`: 対応 Windows バージョンの定義。

## COM_BLASPS ディレクトリ
- `COM_BLASPS/COM_BLASPS.vcxproj`: proxy/stub DLL プロジェクト設定。vcpkg 自動統合を無効化。
- `COM_BLASPS/COM_BLASPS.vcxproj.filters`: proxy/stub プロジェクトのフィルター設定。

## COM_BLAS_UnitTest_Managed ディレクトリ
- `COM_BLAS_UnitTest_Managed/COM_BLAS_UnitTest_Managed.csproj`: MSTest プロジェクト設定 (net8.0-windows x64)。COMBLASLib を COM 参照として解決し、Interop アセンブリを生成する。 
- `COM_BLAS_UnitTest_Managed/ComplexBlasTests.Additional.cs`: 複素数 BLAS API 用の追加テスト。
- `COM_BLAS_UnitTest_Managed/MSTestSettings.cs`: テスト実行時の共通設定ヘルパー。
- `COM_BLAS_UnitTest_Managed/Test1.cs`: COMBLASLib の IBLAS/IBLASComplex を直接 exercise する MSTest ケース。`BlasHandle`/`ComplexBlasHandle` で COM インスタンスの生成と解放を管理。

## Installer ディレクトリ
- `Installer/COM_BLAS.Installer.wixproj`: WiX v4 用 MSBuild プロジェクト。Package.wxs をビルドする設定。
- `Installer/Package.wxs`: WiX 定義ファイル (現状はプレースホルダー構成)。

## COM_BLAS.Setup ディレクトリ
- `COM_BLAS.Setup/COM_BLAS.Setup.vdproj`: Visual Studio Installer Projects による MSI 定義。

## include ディレクトリ (OpenBLAS / LAPACKE ヘッダー)
- `include/cblas.h`: CBLAS API ヘッダー。
- `include/f77blas.h`: Fortran BLAS API ヘッダー。
- `include/lapack.h`: LAPACK 基本ヘッダー。
- `include/lapacke.h`: C インターフェース (LAPACKE) のメインヘッダー。
- `include/lapacke_config.h`: LAPACKE のビルド設定。
- `include/lapacke_mangling.h`: LAPACKE シンボルの名前マングリング定義。
- `include/lapacke_utils.h`: LAPACKE のユーティリティ関数群。
- `include/openblas_config.h`: OpenBLAS の構成マクロ。

## lib/cmake/openblas
- `lib/cmake/openblas/OpenBLASConfig.cmake`: CMake 向け OpenBLAS 設定モジュール。
- `lib/cmake/openblas/OpenBLASConfigVersion.cmake`: CMake 用のバージョン情報。

## lib ディレクトリその他
- `lib/libopenblas.def`: OpenBLAS のエクスポート定義。
- `lib/pkgconfig/openblas.pc`: pkg-config 用の OpenBLAS 設定ファイル。
