# PrepareMatrixView SAFEARRAY 軸整合 修正計画

## 背景と目的
- 現状の `PrepareMatrixView` は SAFEARRAY の第1軸を列、第2軸を行として扱っており、.NET `double[,]` の [行, 列] 順とずれている。
- 軸解釈のズレがあると、行列サイズ検証・leading dimension 計算・行列レイアウト変換の前提が崩れ、呼び出し元で行列の行列数指定に齟齬が生じる。
- 本修正では SAFEARRAY を .NET 側と同じ [行, 列] = [第1軸, 第2軸] として扱い直し、すべての BLAS ルーチン／補助関数／ドキュメントを同じ前提で統一する。

## 現状整理（確認済みポイント）
- `COM_BLAS/BLAS.cpp:168-203` の `PrepareMatrixView` が第1軸→`cols`、第2軸→`rows` として値を格納している。
- `MatrixView` 構造体（`COM_BLAS/BLAS.cpp:178-199`）と `GetLeadingDimension`（`COM_BLAS/BLAS.cpp:374-377`）は `rows`/`cols` をそのまま前提にしている。
- BLAS ラッパー各種（`GemmSimple` ほか）では `rows`/`cols` を使って M/N/K を組み立てているため、軸の入れ替えはサイズ検証・引数変換に波及する。
- 複素数 API では `GatherComplexMatrix`/`ScatterComplexMatrix` とレイアウト変換関数を通じて SAFEARRAY データを CBLAS 呼び出し向け buffer に詰め替えており、行・列数の扱いを修正する必要がある。

## 対応方針と作業手順
1. **軸解釈の正規化**
   - `PrepareMatrixView` を [第1軸=行, 第2軸=列] の前提で再計算する。
   - `MatrixView` の `lboundRow`/`lboundCol` の格納順を入れ替え、名称やコメントがあれば整合（コメントは必要最小限）。
   - 空配列／下限値付き SAFEARRAY の挙動が変わらないか確認。
2. **依存ロジックの追随**
   - `GetLeadingDimension` を新しい軸解釈に合わせて検証し、必要なら実装を調整（RowMajor→列数、ColumnMajor→行数の関係を維持）。
   - `ValidateComplexMatrixPair` ほか、`rows`/`cols` に直接依存している関数を総点検。
   - 変換関数（`GatherComplexMatrix`/`ScatterComplexMatrix`/`ConvertColumnMajorToRowMajor`/`ConvertRowMajorToColumnMajor`）が新しい軸解釈でも正しく動くよう、ループインデックスやサイズ指定を見直す。
3. **BLAS ルーチンの整合性確認**
   - 実数 BLAS ラッパー全体（`GemmSimple` から `TrsvSimple` まで）で M/N/K、leading dimension、行列サイズ検証の計算を横断的に点検し、必要に応じて修正する。
   - 複素数 BLAS ラッパー（`ZGem*` 系等）でも同様に軸解釈を適用し、`Gather/Scatter` 後のレイアウト変換が想定どおりになることを確認。
   - 変換後の `Convert*` 呼び出し条件（RowMajor⇔ColumnMajor）が妥当かを実データで検証し、不要な変換の削除や条件分岐調整が必要なら盛り込む。
4. **テスト追加・更新**
   - 行列の SAFEARRAY を .NET 側で [行, 列] 指定して生成し、BLAS ラッパーが期待通りに動く MSTest ケースを追加する。
   - RowMajor / ColumnMajor 双方のパス、転置指定、空行列／1行1列など境界ケースを含める。
   - 既存テストが軸解釈変更で失敗する場合は期待値を見直し、実装後にすべてパスする形に調整する。
5. **ドキュメント整備**
   - `ReadMe.md`・`IBLASComplex_new_api_definitions.md`・`Agents.md` などで SAFEARRAY 軸の説明があれば更新し、古い記述が残らないようにする。
   - コードコメントが事実と異なる場合は簡潔に補正。
6. **ビルドと回帰確認**
   - `msbuild COM_BLAS.sln /p:Configuration=Debug` などでネイティブ DLL を再ビルド。
   - `dotnet test COM_BLAS_UnitTest_Managed` を実行し、追加テストを含めて成功することを確認。
   - 必要に応じて Release ビルドや 64bit 構成もスポット確認する。

## リスクと観点
- SAFEARRAY の内部メモリ順序（列優先）と .NET の `double[,]` の取り扱いが混同されないよう、テストで実データの並びを検証する。
- 軸入れ替えに伴う leading dimension の変化が、OpenBLAS 呼び出しパラメータに影響するため、CBLAS 呼び出し境界での引数の整合性を重点確認する。
- 既存ユーザーの期待値が変わらないか（行列の実データをそのまま渡して動いているか）をヒアリング／テストで確認し、必要ならリリースノート項目を作成する。

## 成果物
- 修正済みコード一式と追加ユニットテスト。
- 更新済みドキュメント。
- 上記を反映したビルド／テスト結果ログ（`build_last.log` など）。
