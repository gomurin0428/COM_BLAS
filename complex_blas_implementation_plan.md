# 複素数 BLAS API 実装タスク分解

## ゴール
- `IBLASComplex` で宣言したメソッド (ZGemmSimple, ZGemvSimple, ZAxpy, ZDot) を COM 経由で利用可能にする。
- SAFEARRAY の実部/虚部ペア管理と OpenBLAS の複素数関数呼び出しを正しく橋渡しする。
- マネージ／ネイティブ双方でリグレッションのないテストを備えたリリース体制を整える。

## 1. 仕様・IDL/タイプライブラリ整備
1. `COM_BLAS/COMBLAS.idl` のメソッド定義を最終確定。
   - SAFEARRAY の次元 (行優先/列優先) と lower bound 仕様を文書化。
   - `conjugate` の既定値を `VARIANT_FALSE` (0) に変更し、呼び出し側バリアントと一致させる。
2. 任意の IDL 変更後に MIDL を実行して `COMBLAS_i.h`, `COMBLAS_i.c`, `COMBLAS_p.c`, `COMBLAS.tlb` を再生成。
3. `CBLAS::InterfaceSupportsErrorInfo` を更新し `IID_IBLASComplex` を含める (未対応であれば)。
4. バージョン属性 (`library COMBLASLib` の version, `BLAS.rgs` など) を 1.2 → 1.3 へ更新するか検討。

## 2. ネイティブ実装準備 (COM_BLAS/BLAS.cpp)
1. 複素数向けユーティリティ整備
   - `ValidateComplexMatrixPair`, `ValidateComplexVectorPair` で実部 SAFEARRAY と虚部 SAFEARRAY の整合チェックを実装。
   - `GatherComplexMatrix/Vector`, `ScatterComplexMatrix/Vector` を追加し `std::vector<std::complex<double>>` と SAFEARRAY 間のコピーを実現。
   - 行列／ベクトルの leading dimension 計算を `GetLeadingDimension` で再利用できるよう確認。
2. 共通変換関数
   - `ToLayout`, `ToTranspose` など既存の列挙型変換を流用し、異常値の場合は COM エラーに変換。
   - `ToIntChecked` で BLAS API へ渡す int 変換の境界チェックを整備。
3. 空配列用のダミー複素数 (0+0i) を用意し、ゼロ長 SAFEARRAY でも OpenBLAS 呼び出しがクラッシュしないようにする。

## 3. 個別メソッド実装
1. `CBLAS::ZGemmSimple`
   - `PrepareMatrixView` で A/B/C の実部・虚部 SAFEARRAY を読み込む。
   - `ValidateComplexMatrixPair` でサイズ検証し、M×N×K を算出。
   - `GatherComplexMatrix` で `std::vector<std::complex<double>>` を組み立て。
   - `cblas_zgemm` に `alpha`, `beta` を `std::complex<double>` から `void*` にキャストして渡す。
   - 戻り値を `ScatterComplexMatrix` で CReal/CImag SAFEARRAY に書き戻す。
2. `CBLAS::ZGemvSimple`
   - 行列サイズから期待するベクトル長を計算し、`PrepareVectorView` を使用。
   - `GatherComplexVector` で入力を連続メモリにコピーし `cblas_zgemv` を実行。
   - 出力ベクトルを `ScatterComplexVector` で更新。
3. `CBLAS::ZAxpy`
   - `incX/incY` の範囲チェックを行い、`GatherComplexVector` で一時バッファを作成。
   - `cblas_zaxpy` の結果で yReal/yImag SAFEARRAY を更新。
4. `CBLAS::ZDot`
   - `resultReal/resultImag` の null チェックと初期化を実施。
   - `conjugate` パラメータで `cblas_zdotc_sub` (`VARIANT_TRUE`) と `cblas_zdotu_sub` (`VARIANT_FALSE`) を切り替え。
   - 結果を `DOUBLE*` へ展開。
5. すべてのメソッドで `FAILED(hr)` チェックと `SetComError` を統一し、例外パスを整理。

## 4. ビルド設定・依存関係
1. `COM_BLAS/COM_BLAS.vcxproj`
   - 複素数対応で追加したソースをプロジェクトに含める。
   - `AdditionalDependencies` に `libopenblas.lib` (または `openblas.lib`) が設定されているか確認。
2. x86/x64 双方で `cblas_z*` シンボルが解決するかリンクを検証。
3. `pch.h` に `<complex>` を追加してプリコンパイルヘッダーを更新。

## 5. テスト戦略
1. マネージド単体テスト (`COM_BLAS_UnitTest_Managed/Test1.cs`)
   - 正常系: ZGemmSimple/ZGemvSimple/ZAxpy/ZDot を小サイズのデータで検証。
   - 境界系: n=0、alpha=0、beta=0、incX/incY<0 等の入力を確認。
   - エラー系: SAFEARRAY 次元不一致や null SAFEARRAY を与えた場合の COMException メッセージを確認。
2. ネイティブ統合テスト (任意)
   - `CoCreateInstance` 経由で COM 呼び出しを行う C++ テストを追加し、スレッド アパートメントごとの動作を確認。
3. パフォーマンステスト
   - 大規模行列 (例: 512×512) を用いたベンチマークでリークや例外がないかを確認。
4. CI/ビルドスクリプト
   - 新しいテストを既存のビルド パイプラインへ組み込み、`Debug|x64` と `Release|x64` の両方でパスさせる。

## 6. ドキュメント整備
1. `ReadMe.md` に `IBLASComplex` の API 一覧と C#/VBA からの呼び出し例を追記。
2. `makingInstallerPlan.md` など既存メモへ複素数 API 対応状況を反映。
3. 変更履歴 (CHANGELOG 等) が無い場合は作成し、複素数サポート追加を明記。

## 7. 配布・メンテナンス
1. `COM_BLAS.Setup` プロジェクト
   - 更新後の `COMBLAS.tlb`, `COM_BLAS.dll` をインストーラに取り込む。
   - バージョン番号とアップグレードコードの整合を確認。
2. `Installer/Package.wxs` に新ファイルを反映し、インストーラの検証を行う。
3. 必要に応じて DLL/カタログの再署名を実施。
4. リリースノートを作成し、複素数は double 精度のみ対応である旨など既知の制限を記載。

## 8. 残課題・フォローアップ
- 単精度複素数 (cblas_c*) や追加 BLAS ルーチン (ZScal, ZCopy 等) の対応可否を評価し、必要であれば別タスク化。
- マルチスレッド／MTA 実行時の再入性と SAFEARRAY アクセスの排他要件を確認。
- パフォーマンス改善の余地があれば SAFEARRAY コピー削減などの最適化案を検討。
- 利用者向け FAQ に実部/虚部 SAFEARRAY の下限・上限が一致している必要性などを追記。

---

### 推奨作業順序 (例)
1. 仕様確定と IDL 更新 → MIDL 再生成。
2. `BLAS.cpp` にヘルパーと各メソッド本体を実装。
3. プロジェクト設定を調整し x64 Debug/Release をビルド。
4. マネージド単体テストを追加して CI に統合。
5. ドキュメント更新 → インストーラ更新 → リリース準備。
