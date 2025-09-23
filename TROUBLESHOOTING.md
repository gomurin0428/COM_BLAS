# TROUBLESHOOTING

## msbuild で COM_BLAS.Setup をビルドすると MSB4078 が出る
- 事象: `msbuild COM_BLAS.sln /p:Configuration=Release /p:Platform=x64` で `COM_BLAS.Setup.vdproj` が未サポートとして警告 (MSB4078) が表示される。
- 対処: Visual Studio 2022 + Installer Projects 拡張で `devenv COM_BLAS.sln /Build "Release|x64" /Project COM_BLAS.Setup` を実行するか、WiX v4 など別形式へ移行する。CI で `vdproj` を扱う場合は Visual Studio のフルインストールを使う。

## ビルドログに `COM_BigDecimal\vcpkg` が表示される
- 原因: 過去に vcpkg をグローバル統合した環境のキャッシュが残っていると、古い cpkg.targets が呼び出されてログにパスが残ります。
- 対処: プロジェクト側では VcpkgEnabled=false を設定しているので、msbuild /t:Clean 後に再ビルドしてください。あわせてユーザー プロファイル配下の旧 COM_BigDecimal\vcpkg ディレクトリや VCPKG_ROOT 環境変数を整理するとログから参照が消えます。
## Release|x64 で COM_BLASPS が生成されない
- 事象: ソリューション構成 `Release|x64` に `COM_BLASPS` の Build フラグがなく、`msbuild` から 64bit proxy/stub DLL が出力されない。
- 対処: Visual Studio の構成マネージャーで `COM_BLASPS` の Build チェックを有効化し、`.sln` に `Release|x64.Build.0` エントリを追加して再保存する。修正前は Visual Studio GUI で個別ビルドするか、`devenv /build` を利用する。

## libopenblas.dll が見つからず COM 登録に失敗する
- 事象: `regsvr32 COM_BLAS.dll` 実行時に `libopenblas.dll` が見つからず `0x0000007E` (指定されたモジュールが見つかりません) で失敗する。
- 対処: `COM_BLAS.dll` と同じフォルダー、または PATH に `libopenblas.dll` (x64) を配置する。インストーラーでは `libopenblas.dll` を payload に含め、VC++ 再頒布可能パッケージ (MSVCP140 など) を前提条件に設定する。

## COM Automation で `Z*` 系メソッドが列挙されない
- 事象: `dynamic blas = new COMBLASLib.BLASClass();` や VBA で `CreateObject("COMBLAS.BLAS")` した際、`ZGemmSimple` など 4 件しか表示されず他の複素数 API が見つからない。
- 原因: 旧 DLL では `IID_IDispatch` が `IBLAS` の型情報を返しており、`IBLASComplex` 27 メソッドが列挙されなかった。
- 対処: 2025-09-22 21:15 (JST) 適用のビルド以降では `CBLAS` の COM マップが `IBLASComplex` を返しつつ、`GetIDsOfNames`/`Invoke` で実数 API (IBLAS) にフォールバックするよう修正済み。`COM_BLAS.dll` と `COMBLAS.tlb` を最新に置き換え、`regsvr32 /s COM_BLAS.dll` で再登録する。既存の .NET プロジェクトは `tlbimp` 再実行や参照の再解決を行う。

## PowerShell で `Internal Windows PowerShell error. Loading managed Windows PowerShell failed with error 8009001d.` が表示されビルドが開始しない
- 事象: `vcvarsall.bat` 実行後に `MSBuild.exe COM_BLAS.sln`、`devenv.com /Build`、`vstest.console.exe` などを PowerShell から起動すると、DPAPI の初期化が失敗してコマンドが即終了しビルド/テストが走らない。
- 原因: Windows 10/11 の一部環境で PowerShell 実行プロファイルが壊れており、CertificatePrivateKey の復号に失敗するケースがある。`vcvars64.bat` などバッチをネストすると再現しやすい。
- 対処: `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat -arch=x64` を `cmd.exe` から起動して VS Dev Command Prompt 上で `MSBuild.exe` / `vstest.console.exe` を実行するか、`cmd.exe` セッションで `dotnet` コマンドを呼び出す。また PowerShell を使う場合は `setx DOTNET_CLI_HOME %USERPROFILE%\.dotnet` で DPAPI が参照するプロファイルを明示し、`Remove-Item "$env:APPDATA\Microsoft\PowerShell\PSReadLine\ConsoleHost_history.txt" -Force` など壊れたファイルを再作成する。Windows Terminal を経由している場合は `powershell -NoProfile` で再実行し、必要に応じて `Enter-VsDevShell` を利用する。

## Hermitian / Symmetric 系 API で行列がミラーリングされない
- 事象: `zher*` / `zsyr*` / `zhemv` などは片側三角のみを更新するため、SAFEARRAY に書き戻す前に対角線や反対側三角が未更新のまま残り、マネージド テストで失敗する。
- 対処: `CompleteHermitianMatrix` / `CompleteSymmetricMatrix` を必ず呼び出して両側を同期し、複素対角をゼロ化してから `ScatterComplexMatrix` で SAFEARRAY へ戻す。新規 API 追加時も同じパターンを踏襲する。

## 32bit 構成を選ぶとリンク エラーになる
- 事象: `Platform=Win32` でビルドすると OpenBLAS の x64 向けライブラリしか存在しないため `cblas_*` が未解決になる。
- 対処: 64bit のみサポートする。32bit を必要とする場合は OpenBLAS x86 ビルドを追加し、`.vcxproj` のライブラリ ディレクトリを切り替えるスクリプトを用意する。

## COM 参照を含む MSTest プロジェクトを dotnet CLI でビルドできない
- 事象: `dotnet test COM_BLAS_UnitTest_Managed` を実行すると `ResolveComReference` タスクが .NET Core 版 MSBuild で未対応のため MSB4803 で失敗する。
- 対処: `msbuild COM_BLAS_UnitTest_Managed\COM_BLAS_UnitTest_Managed.csproj /restore /t:Build` のようにフル MSBuild (Visual Studio 付属の .NET Framework 版) を使ってビルド・テストを行う。CI でも `vswhere` で Visual Studio インストールを検出して `vsdevcmd` 環境で実行する。
## Managed テストが大量に失敗した場合
- 事象: `dotnet test COM_BLAS_UnitTest_Managed` で複数の `AssertNearlyEqual` が落ちる。
- 切り分け手順:
  1. `dotnet test COM_BLAS_UnitTest_Managed --filter FullyQualifiedName~Z` など対象 API を絞って再実行する。
  2. `TestResults/*/` 内の `.trx` を確認し、失敗している SAFEARRAY の下限・行列レイアウトを調査する。
  3. `COM_BLAS/BLAS.cpp` の `PrepareMatrixView` / `Gather*` / `Scatter*` にブレークポイントを置き、RowMajor/ColumnMajor や LBound の扱いをトレースする。
  4. `oleview` などで `COMBLAS.tlb` 内の `IBLASComplex` を開き、期待するメソッドがエクスポートされているか確認し、もし欠けていれば `midl` 再実行と DLL 再登録を行う。
- 備考: 2025-09-22 18:50 (JST) に `COM_BLAS/COMBLAS.idl` を再 MIDL したタイプライブラリでは `IBLASComplex` の 27 メソッドが公開される。旧 DLL/`COMBLAS.tlb` を参照している環境では `regsvr32 COM_BLAS.dll` で再登録し、タイムスタンプ 2025-09-22 18:50 以降のファイルに置き換えること。
