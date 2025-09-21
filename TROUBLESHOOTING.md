# TROUBLESHOOTING

## COM_BLAS.sln のビルドで `MIDL will not generate DLLDATA.C` が発生する
PowerShell スクリプトを含む `COM_BLASPS` プロジェクトは、IDL 内に少なくとも 1 つのインターフェイスが無いとプリビルドで失敗します。現状このプロジェクトはテスト対象外のため、`msbuild COM_BLAS.sln` をそのまま実行するとエラーになります。

### 回避策
- 中核の COM 実装を確認したい場合は、`msbuild COM_BLAS\COM_BLAS.vcxproj /p:Configuration=Debug /p:Platform=x64` など、必要な vcxproj を直接ビルドしてください。
- もしくはソリューションを開いて `COM_BLASPS` をビルド対象から外した構成を作成します。

## 32bit 構成でのリンクエラー (`cblas_*` が未解決)
同梱の OpenBLAS バイナリは x64 のみ配置されています。`/p:Platform=Win32` でビルドすると `cblas_*` 関数が解決できずリンクに失敗します。

### 回避策
- x64 構成でのビルド (例: `/p:Platform=x64`) を使用してください。
- 32bit 版をビルドしたい場合は、`lib` 配下に x86 用の OpenBLAS を追加し、`Additional Library Directories` を更新する必要があります。

## Hermitian complex APIs leave unsynchronized halves
OpenBLAS hermitian kernels (zher, zher2, zhemv, etc.) only touch the triangle you ask for, so the mirrored half inside the SAFEARRAY stays stale and managed tests compare stale values.
### Actions
- Call CompleteHermitianMatrix after the BLAS operation to mirror the updated triangle and zero the imaginary diagonal before scattering back.
- When adding new Hermitian routines, double-check that the output SAFEARRAY carries both triangles and real-valued diagonal entries.


## Symmetric complex rank-k updates need mirroring

OpenBLAS zsyrk/zsyr2k only touches the triangle indicated by CBLAS_UPLO, so the other half of the SAFEARRAY keeps pre-update values and managed tests fail.

### Actions

- Call CompleteSymmetricMatrix immediately after the BLAS call so both triangles stay in sync before scattering back to the SAFEARRAY.
- Ensure the output matrix is converted back to column-major order before scatter to match how C# double[,] data is laid out.

## Managed test suite baseline failures

Several MSTest cases (GemmSimple/ZGemmSimple families) currently fail with dimension mismatch assertions because the legacy simple wrappers have not been updated for non-square SAFEARRAY inputs yet. They run before the new complex routines execute, so `dotnet test` shows 18 failures even when recently added APIs are correct.

### Actions

- Filter to the specific complex tests you changed when smoke-testing (for example via `--filter ZHer2Simple_`), or review `TestResults/*.log` to confirm only the known Gemm variants failed.
- Avoid using the aggregate test result as a deployment gate until the GemmSimple backlog is resolved.


## ZRotg output keeps original b value

OpenBLAS の cblas_zrotg は第 2 成分 (b) をゼロ化せず入力値を維持するため、COM_BLAS ラッパーがそのまま値を返すと管理側テストで回転結果が不整合になります。

### Actions

- cblas_zrotg 呼び出し後に元のベクトルと返却された c / s を用いて b' = c * b - conj(s) * a を再計算し、OUT パラメーターへ書き戻してから SAFEARRAY へ scatter してください。
