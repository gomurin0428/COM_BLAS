# History

## 2025-09-23
- `COM_BLAS/BLAS.cpp` にて `TrmmSimple` の `SAFEARRAY** B` が `null` だった場合に確実に `E_POINTER (0x80004003)` を返すガードを追加し、`EnsureArrayPointer` で捕捉できない COM Automation 経由の `VT_BYREF|VT_VARIANT` ケースも捕捉するよう修正。
- 同ファイルの `CBLAS::Invoke` に `Rotmg` 専用のディスパッチを追加し、`dynamic` 呼び出し時に 5 番目の `param` が `VT_BYREF|VT_VARIANT` で渡されても SAFEARRAY を生成して返せるようにして `Rotmg_ScalarCase` の失敗を解消。
- x64 構成で `COM_BLAS.vcxproj` を再ビルドし、`vstest.console` で `COM_BLAS_UnitTest_Managed` の全 113 テストが成功することを確認。
- `BLAS_ProgID_update_plan.md` を作成し、ProgID を `Ckt.Com.Blas.BlasCore` へ切り替える際の手順・検証項目・リスクを整理。`TEXT_FILE_OVERVIEW.md` にも同計画を追記。
- `COM_BLAS/BLAS.rgs` の ProgID/CurVer を `Ckt.Com.Blas.BlasCore` 系に更新し、関連ドキュメント (`ReadMe.md`, `makingInstallerPlan.md`, `TROUBLESHOOTING.md`, `残作業.md`, `TEXT_FILE_OVERVIEW.md`) を新 ProgID 前提へ統一。`msbuild COM_BLAS.sln /p:Configuration=Debug /p:Platform=x64` が成功することを確認。
- `CktComBlas_BlasCore_plan.md` を作成し、CoClass `BLAS` を .NET 側で `Ckt.Com.Blas.BlasCore` として公開するための調査・実装・検証の段取りを整理。`TEXT_FILE_OVERVIEW.md` に追記して共有。
- 同計画の「現状調査」セクションを執筆し、`COMBLAS.idl`・`BLAS.rgs`・MSTest から得た現状と、TypeLib/ATL 側をどう改修するかの具体方針（TypeLib 名空間の `custom` 付与や `coclass` 改名等）を明文化。
- `COM_BLAS/COMBLAS.idl` を `library CktComBlasLib` / `coclass BlasCore` へ改名し、`custom(0F21F359-AB84-41E8-9A78-36D110E6D2F9, "Ckt.Com.Blas")` を追加して .NET 相互運用アセンブリの名前空間を固定。MIDL 再生成で `COMBLAS_i.*` を更新。
- ATL 実装 (`BLAS.h` / `BLAS.cpp` / `dllmain.h`) を `LIBID_CktComBlasLib` と `CLSID_BlasCore` に合わせて修正し、`OBJECT_ENTRY_AUTO(__uuidof(BlasCore), CBLAS)` に切り替え。
- COM 参照を利用する MSTest プロジェクトを `CktComBlasLib` 参照＋`Ckt.Com.Blas.BlasCore` 生成に移行し、関連ドキュメント (`ReadMe.md`, `IBLASComplex_new_api_definitions.md`, `COMBLASComplex_recovery_plan.md`, `BLAS_ProgID_update_plan.md`, `TEXT_FILE_OVERVIEW.md`, `TROUBLESHOOTING.md`) を新命名規則で更新。
- `COM_BLASPS/COM_BLASPS.vcxproj` の PreBuild と ModuleDefinitionFile を `..\COM_BLAS` 配下の `dlldata.c` / `COM_BLASps.def` を直接参照するよう調整し、Debug|x64 構成で単体ビルドが通ることを確認。
- `msbuild COM_BLAS.sln /t:COM_BLAS /p:Configuration=Debug /p:Platform=x64` および `msbuild COM_BLASPS/COM_BLASPS.vcxproj /p:Configuration=Debug /p:Platform=x64` が警告のみで成功することを確認 (COM_BLAS 側は既存の C4819/C4267 警告のみ)。
- Release|x64 のビルド後に `regsvr32` が `ExitCode=5` で失敗して登録できない事象を調査。`COM_BLAS\COM_BLAS\x64\Release\COM_BLAS.log` の `warning MSB3075`/`error MSB8011` と `Start-Process regsvr32` で再現し、管理者権限不足が原因と判明したため、対処法を `TROUBLESHOOTING.md`・`TEXT_FILE_OVERVIEW.md` に追記。
- 管理者操作の要否を再確認するため、非昇格 PowerShell から `regsvr32 /s (x64\Release\COM_BLAS.dll)` を実行し、再度 `ExitCode=5` (アクセス拒否) を取得したことを確認。
- 別の管理者権限操作として `New-Item HKLM:\SOFTWARE\CodexPrivilegeTest` を実行したところ成功し、当該キーが作成できることを確認 (作業後に `Remove-Item` で削除)。
