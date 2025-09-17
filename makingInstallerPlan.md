# COM_BLAS インストーラ計画 (Visual Studio Installer Projects 版)

## 1. 前提と要件
- 目的: Windows 10/11 (x64) の管理者ユーザーへ COM サーバー `COMBLAS.BLAS` を安定配布する。
- 成果物: `COM_BLAS.dll`、`COMBLAS.tlb`、必要に応じて `ReadMe.md` の PDF や VBA サンプルなど。
- 使用ツール: Visual Studio 2022 +「Microsoft Visual Studio Installer Projects 2022」拡張機能。
- バージョン管理: `COM_BLAS.dll` のファイルバージョンと MSI のプロパティ (ProductVersion) を合わせ、UpgradeCode を固定してマイナーアップグレードに対応。

## 2. 準備作業
- Visual Studio 2022 を最新更新へアップデート。
- VS 内「拡張機能の管理」から「Microsoft Visual Studio Installer Projects 2022」をインストールし、VS を再起動。
- ソリューション構成に `Release|x64` を追加し、`COM_BLAS` プロジェクトを Release/x64 でビルドできるよう確認。
- Release ビルド成果物が `COM_BLAS\x64\Release` に出力されるよう設定。

## 3. セットアップ プロジェクトの作成
- Visual Studio でソリューションを開き、「新しいプロジェクトの追加」→「Setup Project」を選択し、名前を `COM_BLAS.Setup` とする。
- 生成された `.vdproj` がソリューションに追加されることを確認し、ターゲットプラットフォームを x64 に設定。
- 出力フォルダー (Application Folder) の既定名を `COM BLAS` など配布名に変更。

## 4. ファイルの取り込みと配置
- Setup プロジェクトの Application Folder に以下を追加:
  - `COM_BLAS` プロジェクトの「Release/x64 プライマリ出力」(最初に COM DLL をビルドしておく)。
  - `COMBLAS.tlb` など個別ファイル。
  - 付属ドキュメント (例: `ReadMe.pdf`、VBA サンプルなど)。
- 必要に応じて「Program Menu」や「User’s Desktop」にショートカットを追加 (例: ReadMe を開くショートカット)。

## 5. COM 登録設定
- Application Folder 内の `COM_BLAS.dll` を選択し、プロパティウィンドウで `Register` を `vsdrpCOM` に設定。
- `COMBLAS.tlb` も登録が必要なら `Register` を `vsdrpCOM` に設定。
- Self Registration (`vsdrpCOMSelfReg`) は使用せず、インストーラが生成するテーブル登録に依存する。
- COM の CLSID や ProgID を確認する場合はセットアップ プロジェクトの「レジストリ」ビューで確認・編集可能。

## 6. 依存関係と前提条件
- VC++ ランタイムが必要かを `dumpbin /dependents COM_BLAS.dll` で確認。
- 必要なら Setup プロジェクトのプロパティ→`Prerequisites` で「Microsoft Visual C++ 再頒布可能パッケージ (対応バージョン)」を追加。
- .NET ベースの補助ツールがある場合は同様に前提条件を追加するか、Application Folder に含める。

## 7. 64bit 対応の確認
- Setup プロジェクトの「ビルド」プロパティで `TargetPlatform = x64` を再確認。
- プロジェクトの File System ビューで、インストール先が `Program Files 64` 配下 (`[ProgramFiles64Folder]COM BLAS`) になっているか確認。
- 32bit Office 向け配布が必要な場合は、同じ手順で x86 用 Setup プロジェクトを複製し、個別 MSI を生成する。

## 8. ビルドと自動化
- Visual Studio から手動ビルド: `ビルド → ソリューションのビルド` (Release|x64)。
- コマンドライン/CI からは `devenv COM_BLAS.sln /Build "Release|x64" /Project COM_BLAS.Setup` などを利用。
- `msbuild` で `.vdproj` を直接ビルドする場合は Visual Studio がインストールされた環境で実行する (Build Tools 単体では不可)。
- ビルド成果物 (`COM_BLAS.Setup\Release\COM_BLAS.Setup.msi` など) を `Installer\Artifacts` へコピーするスクリプトを作成し、CI で利用。

## 9. 検証手順
- 手動インストール: `msiexec /i COM_BLAS.Setup.msi /l*v install.log` を実行し、完了後 `reg query HKCR\CLSID\{<CLSID>}` や `PowerShell: New-Object -ComObject COMBLAS.BLAS` で登録を確認。
- 単体テスト: `COM_BLAS_UnitTest_Managed` のテストをインストール済み環境で実行し、COM 経由で API が機能するか検証。
- アンインストール: `msiexec /x {ProductCode}` を実行し、DLL やレジストリエントリが削除されることを確認。
- サイレントインストール検証: `/qn`, `/l*v` オプションを使ってログ出力を確認し、企業配布向けの手順を固める。

## 10. ドキュメントとリリース
- `ReadMe.md` に Visual Studio Installer Projects ベースのインストール/アンインストール手順、必要権限、トラブルシュートを追記。
- リリースノートに MSI の SHA256 ハッシュ、サポート OS、依存する再頒布パッケージのバージョン情報を掲載。
- エンタープライズ配布 (Intune/SCCM など) 向けにサイレントスイッチと検証手順を別紙にまとめる。

## 11. 将来拡張
- 32bit 版や多言語セットアップが必要になった場合は追加の Setup プロジェクトを用意し、条件付きインストールを設定。
- Update/Upgrade 対応を円滑にするため、ProductCode をマイナーアップデート時に変更しつつ UpgradeCode を固定。
- Burn など高度なブートストラップが必要になった場合は、WiX Toolset や他の専用ツールへの移行も検討する。
