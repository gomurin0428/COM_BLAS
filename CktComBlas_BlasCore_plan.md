# Ckt.Com.Blas.BlasCore 対応計画

## 目的
CoClass `BLAS` を COM 参照した際に、Visual Studio が生成する相互運用アセンブリ上で `Ckt.Com.Blas.BlasCore` クラスとして早期バインディング利用できるようにする。

## 作業方針
- COM 型ライブラリ定義とレジストリスクリプトを確認し、CoClass 名と .NET 側の公開名の関連付けを把握する。
- 必要に応じて IDL・RGS・AssemblyInfo 等を更新し、適切な `Guid` と `ClassInterface` 指定を反映する。
- ビルド成功と単体テスト合格をもって完了とし、関連ドキュメントと履歴を更新する。

## タスク
1. 現状調査（2025-09-23 完了）
   - 旧構成では `COM_BLAS/COMBLAS.idl` が `library COMBLASLib` 配下で `coclass BLAS` を公開しており、TLBIMP で生成される相互運用アセンブリは `COMBLASLib.BLAS` / `BLASClass` として露出していた。
   - 本対応で `library CktComBlasLib` と `coclass BlasCore` へ改名し、`custom(0F21F359-AB84-41E8-9A78-36D110E6D2F9, "Ckt.Com.Blas")` を付与して .NET 側名前空間を固定した。CLSIDs は従来どおり `e8f3aed3-eec4-48ab-9925-c13253d4c396` を継続利用。
   - `COM_BLAS/BLAS.rgs` は ProgID を既に `Ckt.Com.Blas.BlasCore(.1)` へ揃えていたため流用。マネージド テストは `using Ckt.Com.Blas;` / `new BlasCore()` へ更新済み。
2. 実装調整（2025-09-23 完了）
   - `COMBLAS.idl` の変更に合わせて `BLAS.h` / `BLAS.cpp` / `dllmain.h` で `LIBID_CktComBlasLib` と `CLSID_BlasCore` を参照するよう更新し、`OBJECT_ENTRY_AUTO(__uuidof(BlasCore), CBLAS)` へ切り替えた。
   - `COM_BLAS_UnitTest_Managed` の COM 参照は `CktComBlasLib` に差し替え、生成される Interop アセンブリで `Ckt.Com.Blas.BlasCore` CoClass を期待する形に調整。
3. 検証とドキュメント（進行中）
   - COM_BLAS / COM_BLASPS のビルド確認、ならびに関連ドキュメント (`TEXT_FILE_OVERVIEW.md`, `TROUBLESHOOTING.md`, `history.md`) の更新を実施する。
   - Interop アセンブリの確認と MSTest 側の再実行もこのフェーズで追跡する。
