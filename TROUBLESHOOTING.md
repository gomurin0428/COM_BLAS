# TROUBLESHOOTING

## msbuild で COM_BLAS.Setup をビルドすると MSB4078 が出る
- 事象: `msbuild COM_BLAS.sln /p:Configuration=Release /p:Platform=x64` で `COM_BLAS.Setup.vdproj` が未サポートとして警告 (MSB4078) が表示される。
- 対処: Visual Studio 2022 + Installer Projects 拡張で `devenv COM_BLAS.sln /Build "Release|x64" /Project COM_BLAS.Setup` を実行するか、WiX v4 など別形式へ移行する。CI で `vdproj` を扱う場合は Visual Studio のフルインストールを使う。

## Release|x64 で COM_BLASPS が生成されない
- 事象: ソリューション構成 `Release|x64` に `COM_BLASPS` の Build フラグがなく、`msbuild` から 64bit proxy/stub DLL が出力されない。
- 対処: Visual Studio の構成マネージャーで `COM_BLASPS` の Build チェックを有効化し、`.sln` に `Release|x64.Build.0` エントリを追加して再保存する。修正前は Visual Studio GUI で個別ビルドするか、`devenv /build` を利用する。

## libopenblas.dll が見つからず COM 登録に失敗する
- 事象: `regsvr32 COM_BLAS.dll` 実行時に `libopenblas.dll` が見つからず `0x0000007E` (指定されたモジュールが見つかりません) で失敗する。
- 対処: `COM_BLAS.dll` と同じフォルダー、または PATH に `libopenblas.dll` (x64) を配置する。インストーラーでは `libopenblas.dll` を payload に含め、VC++ 再頒布可能パッケージ (MSVCP140 など) を前提条件に設定する。

## Hermitian / Symmetric 系 API で行列がミラーリングされない
- 事象: `zher*` / `zsyr*` / `zhemv` などは片側三角のみを更新するため、SAFEARRAY に書き戻す前に対角線や反対側三角が未更新のまま残り、マネージド テストで失敗する。
- 対処: `CompleteHermitianMatrix` / `CompleteSymmetricMatrix` を必ず呼び出して両側を同期し、複素対角をゼロ化してから `ScatterComplexMatrix` で SAFEARRAY へ戻す。新規 API 追加時も同じパターンを踏襲する。

## 32bit 構成を選ぶとリンク エラーになる
- 事象: `Platform=Win32` でビルドすると OpenBLAS の x64 向けライブラリしか存在しないため `cblas_*` が未解決になる。
- 対処: 64bit のみサポートする。32bit を必要とする場合は OpenBLAS x86 ビルドを追加し、`.vcxproj` のライブラリ ディレクトリを切り替えるスクリプトを用意する。

## Managed テストが大量に失敗した場合
- 事象: `dotnet test COM_BLAS_UnitTest_Managed` で複数の `AssertNearlyEqual` が落ちる。
- 切り分け手順:
  1. `dotnet test COM_BLAS_UnitTest_Managed --filter FullyQualifiedName~Z` など対象 API を絞って再実行する。
  2. `TestResults/*/` 内の `.trx` を確認し、失敗している SAFEARRAY の下限・行列レイアウトを調査する。
  3. `COM_BLAS/BLAS.cpp` の `PrepareMatrixView` / `Gather*` / `Scatter*` にブレークポイントを置き、RowMajor/ColumnMajor や LBound の扱いをトレースする。
- 備考: 2025-09-21 時点では 113 テストすべて成功しているため、直近の変更で退行していないかをまず疑うこと。
