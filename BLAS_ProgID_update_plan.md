# BLAS ProgID を `Ckt.Com.Blas.BlasCore` に更新する計画

## 背景と目的
- 当初は COM 登録スクリプト (`COM_BLAS/BLAS.rgs`) が `MYBLAS` 系列の ProgID を持ち、ドキュメントやインストーラ計画も `COMBLAS.BLAS` を前提としていたが、2025-09-23 の更新で `Ckt.Com.Blas.BlasCore` 系へ統一した。
- 目標は、COM coclass `BLAS` の ProgID を製品命名規則に合わせて `Ckt.Com.Blas.BlasCore` へ統一し、ビルド・テスト・ドキュメント・インストーラのすべてで整合性を取ること。
- 変更後も `CLSID_BLAS` や TypeLib GUID を変えずに既存クライアントとの互換性を保ちつつ、新 ProgID を優先する運用へ切り替える。

## 現状整理（2025-09-23 時点）
- `COM_BLAS/BLAS.rgs` は 2025-09-23 更新で VersionIndependentProgID/ProgID ともに `Ckt.Com.Blas.BlasCore` 系へ切り替え済み (旧 `MYBLAS` 系の定義を解消)。
- 開発ドキュメント (`ReadMe.md`, `TROUBLESHOOTING.md`, `makingInstallerPlan.md`, `残作業.md`) は 2025-09-23 時点ですべて `Ckt.Com.Blas.BlasCore` を案内する最新記述へ差し替え済み。
- TypeLib (`COMBLAS.idl`) には `progid` 属性がなく、`BLASClass` という CoClass 名のみが露出している。Interop アセンブリ経由 (`new BLASClass()`) のテストは ProgID 変更の影響を受けない。
- インストーラ関連 (WiX / VS Installer Projects) では具体的な ProgID をまだ配布物に書き込んでいないが、登録手順の記述は旧値を参照している。

## 対応方針と作業手順
1. **仕様確認と名称定義**
   - ProgID を `Ckt.Com.Blas.BlasCore`（バージョン非依存）に固定し、必要ならバージョン付き ProgID `Ckt.Com.Blas.BlasCore.1` を定義する。
   - 既存 ProgID を残すリダイレクトは不要という前提だが、互換要求があれば明示的に関係者へ確認する。->互換の必要なし。
2. **レジストリスクリプト更新**
   - `COM_BLAS/BLAS.rgs` の `ProgID` / `VersionIndependentProgID` / ルートキー名をすべて `Ckt.Com.Blas.BlasCore` 系へ差し替える。
   - `CurVer` の値を最新のバージョン付き ProgID (`...BlasCore.1` など) に合わせて更新。
   - 旧 `COMBLASLib` / `COMBLAS.BLAS` 系の ProgID と `HKCR\TypeLib\{5650...}\1.*` を `ForceRemove` し、新しい `CktComBlas.tlb` のみが登録されるようにする。
   - 変更後に ATL のリソース ID (`DECLARE_REGISTRY_RESOURCEID(106)`) が指す `.rgs` に齟齬がないかを確認。
3. **ATL プロジェクト資産の追随確認**
   - `COM_BLAS.rc` と `resource.h` に ProgID 文字列が埋め込まれていないか確認し、存在すれば更新。
   - `COM_BLAS.vcxproj` の `REGISTRY_RESOURCE_FILE` 設定に変更が不要か確認。
4. **ドキュメント整備**
   - `ReadMe.md` の C#/VB サンプルを `Activator.CreateInstance(Type.GetTypeFromProgID("Ckt.Com.Blas.BlasCore"))` に更新。
   - `TROUBLESHOOTING.md`, `makingInstallerPlan.md`, `残作業.md`, `Agents.md`（該当する場合）で旧 ProgID の記述を更新。
   - 併せて、ProgID 変更後の再登録手順 (`regsvr32`) を追記し、古い記述が残らないよう棚卸し。
5. **インストーラ関連の確認**
   - VS Installer プロジェクト (`COM_BLAS.Setup`) や WiX 定義 (`Installer/Package.wxs`) で ProgID を明示的に扱う箇所があるか確認し、必要なら新しい ProgID を前提とした説明またはカスタム動作を追加。
   - インストール後の COM 登録検証手順を `Ckt.Com.Blas.BlasCore` を使って書き換える。
6. **ビルドとテスト**
   - `msbuild COM_BLAS.sln /p:Configuration=Debug /p:Platform=x64` でビルドが通ることを確認。
   - `dotnet test COM_BLAS_UnitTest_Managed` を実行し、既存テストが通ることを再確認。
   - 追加で `regsvr32 /u`→再登録 (`regsvr32 COM_BLAS.dll`) を行い、新 ProgID で `powershell -Command New-Object -ComObject Ckt.Com.Blas.BlasCore` が成功することまで検証する。
7. **後処理**
   - `history.md` に作業記録を追記。
   - `TEXT_FILE_OVERVIEW.md` の `COM_BLAS/BLAS.rgs` 項の説明を新 ProgID に合わせて更新。
   - 落とし穴が判明した場合は `TROUBLESHOOTING.md` に追記し、古い情報を整理する。

## 検証項目
- 変更後の DLL を登録すると、レジストリ `HKCR\Ckt.Com.Blas.BlasCore` と `HKCR\Ckt.Com.Blas.BlasCore.1` が生成される。
- PowerShell / VBScript などから新 ProgID で COM インスタンスが生成できる。
- Interop アセンブリを再生成すると `Ckt.Com.Blas.BlasCore` CoClass が公開されるため、旧 `BLASClass` を参照しているプロジェクトは参照の再解決が必要 (GUID は従来どおり)。

## リスクとオープン事項
- 現行利用者が `COMBLAS.BLAS` や `MYBLAS` を使っている場合、互換性が途切れるため移行ガイドが必要になる。必要に応じて旧 ProgID をサポートするか要確認。
- リリース済み MSI で旧 ProgID が登録されている場合、アップグレード時にレジストリ掃除を行わないとゴミが残る恐れがある。インストーラのカスタムアクション有無を確認する。
- ProgID 変更に伴う QA フロー（特に他チームの自動テスト）がある場合、スケジュール調整を事前に共有する。
