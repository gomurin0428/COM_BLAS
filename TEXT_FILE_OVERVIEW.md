# TEXT_FILE_OVERVIEW

## ルート
- `.gitattributes`: Git の属性設定。改行コードやマージ戦略を制御。
- `.gitignore`: ビルド生成物・VS のキャッシュ・TestResults などを無視する設定。
- `Agents.md`: リポジトリ運用時の注意事項とガイドライン。
- `COM_BLAS.sln`: Visual Studio ソリューション。COM 本体、proxy/stub、MSTest、Installer を束ねる。
- `COMBLASComplex_recovery_plan.md`: 複素数 BLAS API がテストで欠落している事象に対する対応計画と検証チェックリスト。フェーズ0 (TypeLib ギャップ洗い出し) 完了ログに加えて、2025-09-22 の COM マップ修正 (IID_IDispatch→IBLASComplex) を対応方針へ反映。
- `BLAS_ProgID_update_plan.md`: COM coclass `BLAS` の ProgID を `Ckt.Com.Blas.BlasCore` に切り替えるための作業計画と検証項目。
- `IBLASComplex_new_api_definitions.md`: 複素数 BLAS API 拡張の進捗メモと仕様整理。2025-09-22 に TypeLib 再生成状況と COM マップ更新ログ (IDispatch が 27 メソッドを返すことの検証結果) を追記。
- `PrepareMatrixView_fix_plan.md`: SAFEARRAY の行列ビュー周りの改善計画と検証ログ。
- `CktComBlas_BlasCore_plan.md`: CoClass `BLAS` を .NET 側で `Ckt.Com.Blas.BlasCore` として公開するための調査・実装・検証計画を記載。
- `ReadMe.md`: COM_BLAS の概要、公開 API、ビルドとセットアップ手順。2025-09-22 時点の `IBLASComplex` 既定 IDispatch 対応に加え、2025-09-23 版で TypeLib `CktComBlasLib` と `BlasCore` CoClass への改名手順を記載。
- `manual.md`: ComponentSource で配布する利用者向けマニュアル。MSI インストーラ経由の導入手順と公開 API の完全一覧を記載。
- `TEXT_FILE_OVERVIEW.md`: 本ファイル。テキスト資産の一覧。
- `TROUBLESHOOTING.md`: ビルド・テスト・依存 DLL の既知の落とし穴と対処法。TypeLib 再登録や複素数 API 列挙問題 (2025-09-22〜23) に加え、`regsvr32` が `ExitCode=5` を返すアクセス権不足時の対処 (管理者実行 / per-user 登録 / RegisterOutput の無効化) を追記。
- `makingInstallerPlan.md`: Visual Studio Installer Projects を使った MSI 作成手順の詳細プラン。
- `残作業.md`: リリース完了までに必要な TODO リスト。

## COM_BLAS ディレクトリ
- `COM_BLAS/BLAS.cpp`: COM 実装の本体。SAFEARRAY マーシャリングと OpenBLAS 呼び出しに加え、2025-09-22 版で `EnsureDoubleSafeArray` の `NULL` 入力を `E_POINTER` に統一し、`CBLAS::Invoke` が `DISPPARAMS` を都度コピーして `IBLASComplex` → `IBLAS` の順で解決するようになった。2025-09-23 の修正では TypeLib 改名に伴い `IDispatchImpl` の `LIBID_CktComBlasLib` 参照へ更新しつつ、`TrmmSimple` の `SAFEARRAY** B` ガードと `Rotmg` の Automation 向けディスパッチ改善を継続適用。
- `COM_BLAS/BLAS.h`: `CBLAS` クラス宣言。2025-09-22 の `COM_INTERFACE_ENTRY2(IDispatch, IBLASComplex)` 切り替えに加え、2025-09-23 で `CComCoClass`/`IDispatchImpl` を `CLSID_BlasCore` & `LIBID_CktComBlasLib` に差し替え、`OBJECT_ENTRY_AUTO(__uuidof(BlasCore), CBLAS)` とした。
- `COM_BLAS/BLAS.rgs`: BLAS クラスのレジストリ スクリプト。2025-09-23 更新で ProgID/VersionIndependentProgID を `Ckt.Com.Blas.BlasCore` 系へ統一し、`CurVer` も `...BlasCore.1` へ揃えたうえで、旧 `COMBLASLib` / `COMBLAS.BLAS` の ProgID と `HKCR\TypeLib\{5650...}` (1.x) を ForceRemove してから `CktComBlas.tlb` を再登録する。
- `COM_BLAS/COMBLAS.idl`: 公開インターフェース (IBLAS / IBLASComplex) と列挙体を定義する MIDL。2025-09-23 に `library CktComBlasLib` / `coclass BlasCore` へ改名し、`custom(… "Ckt.Com.Blas")` で .NET 名前空間を固定した。
- `COM_BLAS/COMBLAS_i.h`: MIDL 自動生成のインターフェース定義ヘッダー。手動編集不可。
- `COM_BLAS/COM_BLAS.cpp`: DLL エントリーポイントなど ATL のブートストラップ コード。
- `COM_BLAS/COM_BLAS.def`: エクスポートシンボルの定義。
- `COM_BLAS/COM_BLAS.rc`: リソーススクリプト。バージョン情報・レジストリスクリプトに加え、`CktComBlas.tlb` の TYPELIB リソースを埋め込み、`TEXTINCLUDE` からの再同期に利用する。
- `COM_BLAS/COM_BLAS.rgs`: COM DLL 全体に対する追加レジストリ設定 (現状プレースホルダー)。
- `COM_BLAS/COM_BLAS.vcxproj`: COM DLL プロジェクト設定。OpenBLAS を同梱の lib/include から参照し、vcpkg 自動統合を無効化。
- `COM_BLAS/COM_BLAS.vcxproj.filters`: プロジェクト内のフィルター構成。
- `COM_BLAS/COM_BLASps.def`: proxy/stub DLL 用 DEF。
- `COM_BLAS/dllmain.cpp`: DLL のエントリーポイント。
- `COM_BLAS/dllmain.h`: `DllMain` の宣言と ATL モジュールのラッパー。2025-09-23 に `DECLARE_LIBID` を `LIBID_CktComBlasLib` へ更新。
- `COM_BLAS/framework.h`: Windows ヘッダーの集約。
- `COM_BLAS/pch.cpp`: プリコンパイル済みヘッダーのソース。
- `COM_BLAS/pch.h`: PCH のヘッダー。
- `COM_BLAS/resource.h`: リソース識別子定義。
- `COM_BLAS/targetver.h`: 対応 Windows バージョンの定義。

## COM_BLASPS ディレクトリ
- `COM_BLASPS/COM_BLASPS.vcxproj`: proxy/stub DLL プロジェクト設定。vcpkg 自動統合を無効化し、2025-09-23 に `dlldata.c`/`COM_BLASps.def` を `..\COM_BLAS` から直接参照するよう PreBuild と `.def` パスを修正。
- `COM_BLASPS/COM_BLASPS.vcxproj.filters`: proxy/stub プロジェクトのフィルター設定。

## COM_BLAS_UnitTest_Managed ディレクトリ
- `COM_BLAS_UnitTest_Managed/COM_BLAS_UnitTest_Managed.csproj`: MSTest プロジェクト設定 (net8.0-windows x64)。COM 参照は `CktComBlasLib` を指定し、`tlbimp` で生成した Interop を埋め込み参照する。
- `COM_BLAS_UnitTest_Managed/ComplexBlasTests.Additional.cs`: 複素数 BLAS API 用の追加テスト。`Ckt.Com.Blas` の複素 CoClass を `ComplexBlasHandle` 経由で利用。
- `COM_BLAS_UnitTest_Managed/MSTestSettings.cs`: テスト実行時の共通設定ヘルパー。
- `COM_BLAS_UnitTest_Managed/Test1.cs`: `Ckt.Com.Blas` 名前空間の `IBLAS`/`IBLASComplex` を exercise する MSTest ケース。`BlasHandle`/`ComplexBlasHandle` が `new BlasCore()` で COM インスタンスを生成し管理。

## Installer ディレクトリ
- `Installer/COM_BLAS.Installer.wixproj`: WiX v4 用 MSBuild プロジェクト。Package.wxs をビルドする設定。
- `Installer/Package.wxs`: WiX 定義ファイル (現状はプレースホルダー構成)。

## COM_BLAS.Setup ディレクトリ
- `COM_BLAS.Setup/COM_BLAS.Setup.vdproj`: Visual Studio Installer Projects による MSI 定義。Release 出力の `CktComBlas.tlb` を取り込み、COM 登録に `vsdrpCOM` を設定する。

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
