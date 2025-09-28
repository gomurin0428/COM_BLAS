# History

## 2025-09-27
- リポジトリ直下に `manual.md` を追加し、ComponentSource で配布する利用者向け手順・クイックスタート・トラブル事例への導線を整理。
- 同マニュアルを MSI インストール前提の内容へ改訂し、公開 API を全件掲載した表とユーザー向け Q&A を整備。
- `manual_en.md` を新規追加し、英語圏向けに同一内容を翻訳して公開。
- `TEXT_FILE_OVERVIEW.md` に `manual.md` を追記し、新規ドキュメントの位置付けと内容概要を反映。
- `msbuild COM_BLAS.sln /p:Configuration=Debug /p:Platform=x64` を実行し、警告 C4267 のみでビルドと `regsvr32` の自動登録が成功することを確認。

## 2025-09-23
- `COM_BLAS/BLAS.cpp` にて `TrmmSimple` の `SAFEARRAY** B` が `null` だった場合に確実に `E_POINTER (0x80004003)` を返すガードを追加し、`EnsureArrayPointer` で捕捉できない COM Automation 経由の `VT_BYREF|VT_VARIANT` ケースも捕捉するよう修正。
- 同ファイルの `CBLAS::Invoke` に `Rotmg` 専用のディスパッチを追加し、`dynamic` 呼び出し時に 5 番目の `param` が `VT_BYREF|VT_VARIANT` で渡されても SAFEARRAY を生成して返せるようにして `Rotmg_ScalarCase` の失敗を解消。
- x64 構成で `COM_BLAS.vcxproj` を再ビルドし、`vstest.console` で `COM_BLAS_UnitTest_Managed` の全 113 テストが成功することを確認。
- `COM_BLAS/BLAS.rgs` の ProgID/CurVer を `Ckt.Com.Blas.BlasCore` 系に更新し、関連ドキュメント (`ReadMe.md`, `makingInstallerPlan.md`, `TROUBLESHOOTING.md`, `残作業.md`, `TEXT_FILE_OVERVIEW.md`) を新 ProgID 前提へ統一。`msbuild COM_BLAS.sln /p:Configuration=Debug /p:Platform=x64` が成功することを確認。
- `COM_BLAS/COMBLAS.idl` を `library CktComBlasLib` / `coclass BlasCore` へ改名し、`custom(0F21F359-AB84-41E8-9A78-36D110E6D2F9, "Ckt.Com.Blas")` を追加して .NET 相互運用アセンブリの名前空間を固定。MIDL 再生成で `COMBLAS_i.*` を更新。
- ATL 実装 (`BLAS.h` / `BLAS.cpp` / `dllmain.h`) を `LIBID_CktComBlasLib` と `CLSID_BlasCore` に合わせて修正し、`OBJECT_ENTRY_AUTO(__uuidof(BlasCore), CBLAS)` に切り替え。
- `COM_BLASPS/COM_BLASPS.vcxproj` の PreBuild と ModuleDefinitionFile を `..\COM_BLAS` 配下の `dlldata.c` / `COM_BLASps.def` を直接参照するよう調整し、Debug|x64 構成で単体ビルドが通ることを確認。
- `msbuild COM_BLAS.sln /t:COM_BLAS /p:Configuration=Debug /p:Platform=x64` および `msbuild COM_BLASPS/COM_BLASPS.vcxproj /p:Configuration=Debug /p:Platform=x64` が警告のみで成功することを確認 (COM_BLAS 側は既存の C4819/C4267 警告のみ)。
- Release|x64 のビルド後に `regsvr32` が `ExitCode=5` で失敗して登録できない事象を調査。`COM_BLAS\COM_BLAS\x64\Release\COM_BLAS.log` の `warning MSB3075`/`error MSB8011` と `Start-Process regsvr32` で再現し、管理者権限不足が原因と判明したため、対処法を `TROUBLESHOOTING.md`・`TEXT_FILE_OVERVIEW.md` に追記。
- 管理者操作の要否を再確認するため、非昇格 PowerShell から `regsvr32 /s (x64\Release\COM_BLAS.dll)` を実行し、再度 `ExitCode=5` (アクセス拒否) を取得したことを確認。
- 別の管理者権限操作として `New-Item HKLM:\SOFTWARE\CodexPrivilegeTest` を実行したところ成功し、当該キーが作成できることを確認 (作業後に `Remove-Item` で削除)。
- `x64/Release/COM_BLAS.dll` の `DllRegisterServer` が `0x80020009` を返す現象を調査し、Release 用の `BLAS.rgs` リソースに UTF-8 BOM (`0xEF,0xBB,0xBF`) が含まれており ATL Registrar が冒頭トークンを解釈できず例外を投げていることを確認。`ResourceBytes` で先頭 3 バイトを確認し、BOM を除去すれば登録が通る見込みである点を記録。
- 2025-09-28: docs cleanup
  - Removed obsolete WIP/plan docs: BLAS_ProgID_update_plan.md, COMBLASComplex_recovery_plan.md, PrepareMatrixView_fix_plan.md, IBLASComplex_new_api_definitions.md, CktComBlas_BlasCore_plan.md, makingInstallerPlan.md, 残作業.md
  - Added Documentation section to ReadMe.md
  - Updated TEXT_FILE_OVERVIEW.md and removed references to deleted files
