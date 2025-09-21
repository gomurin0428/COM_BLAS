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