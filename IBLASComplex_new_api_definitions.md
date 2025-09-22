# IBLASComplex 新規API定義メモ
## Progress Notes (2025-09-22)
- MSTest (2025-09-22) の結果、`COMBLASLib.BLASClass` に `ZSymmSimple` など複素数 API が公開されておらず、41 件のテストが `RuntimeBinderException` で失敗している。
- `COMBLAS.tlb` を `oleview` で確認したところ、IDL 上で追加済みの `IBLASComplex` メソッドがエクスポートされていない。Type Library の再生成と DLL 再登録が未実施と推測。
- 2025-09-21 に「実装済み」と記録した内容はソース側の進捗メモであり、配布バイナリには反映されていない。以下の「追加対象API一覧」は引き続き対応が必要な項目として扱う。


## 背景
- 2025-09-21 時点で `IBLASComplex` が公開しているのは `ZGemmSimple` / `ZGemvSimple` / `ZAxpy` / `ZDot` / `ZSymmSimple` / `ZHemmSimple` / `ZSyrkSimple` の7件のみで、実数用 `IBLAS` が提供する残りの BLAS Level 1/2/3 に対応する複素数版が欠落している。
- OpenBLAS には該当する `cblas_z*` 系関数が存在し、既に `GatherComplex*` / `ScatterComplex*` といった SAFEARRAY ヘルパーが揃っているため、追加のフォールバック実装は不要。
- SAFEARRAY の次元 1 は列、次元 2 は行に相当するため、`PrepareMatrixView` / `ValidateComplexMatrixPair` で `[row, column]` 形式に正規化し、.NET の `double[,]` と一致させています。

## 設計方針
- 既存の SAFEARRAY 2本 (実部/虚部) を受け取るスタイルと `BlasLayout` / `BlasTranspose` / `BlasSide` / `BlasUplo` / `BlasDiag` 列挙型をそのまま再利用する。
- 係数 `alpha` / `beta` は複素数を `DOUBLE` のペアで受け取り、Hermitian 系 (`zherk` など) のように実数係数のみを受け付ける API では虚部パラメータを省略する。
- 既存ヘルパー (`ValidateComplexMatrixPair` など) を流用し、OpenBLAS の `cblas_z*` ルーチンを直接呼び出すことで余計なフォールバックを設けない。
- IDL 側は `IBLASComplex` にメソッドを追加し、`COMBLAS.idl` / `COMBLAS_i.*` / `BLAS.h` / `BLAS.cpp` / `ReadMe.md` など関連ファイルを同期させる。

## 追加対象API一覧

### Level 3 (行列×行列)
| 既存(実数) | 新規複素API | 対応BLASルーチン | 主なポイント |
| --- | --- | --- | --- |
| SymmSimple | ZSymmSimple | cblas_zsymm | A を複素対称行列として扱う。`AReal/AImag` と `BReal/BImag` に 2 次元 SAFEARRAY ペアを受け取り、`BlasSide` と `BlasUplo` の既定値は実数版に合わせる。 |
| (新設) | ZHemmSimple | cblas_zhemm | Hermitian 行列専用。入力対角の虚部が 0 であることを検証し、`BlasUplo` が指す半分のみを上書きする。`alpha`/`beta` は複素数。 |
| SyrkSimple | ZSyrkSimple | cblas_zsyrk | rank-k 更新。`transA` では `ConjTrans` を許容し、`alpha`/`beta` を複素数で expose。 |
| Syr2kSimple | ZSyr2kSimple | cblas_zsyr2k | rank-2k 更新。A/B の SAFEARRAY ぺアを扱い、`alpha` 複素・`beta` 複素。 |
| (新設) | ZHerkSimple | cblas_zherk | Hermitian rank-k 更新。`alpha` と `beta` は実数のみ (`alphaReal` / `betaReal`)。出力 C の対角虚部を 0 に戻す処理が必要。 |
| (新設) | ZHerk2kSimple | cblas_zher2k | Hermitian rank-2k 更新。`alpha` 複素、`beta` は実数。A/B の SAFEARRAY 形状と `BlasTranspose` の扱いは ZSyr2kSimple と同様。 |
| TrmmSimple | ZTrmmSimple | cblas_ztrmm | 三角行列×行列。`diag`/`side`/`uplo` パラメータをそのまま流用し、結果行列 C を in-place で更新。 |
| TrsmSimple | ZTrsmSimple | cblas_ztrsm | 三角行列方程式の解。B を in-place で上書きし、SAFEARRAY の境界検証は既存関数を再利用。 |

### Level 2 (行列×ベクトル)
| 既存(実数) | 新規複素API | 対応BLASルーチン | 主なポイント |
| --- | --- | --- | --- |
| GerSimple | ZGerSimple | cblas_zgeru / cblas_zgerc | rank-1 更新。`VARIANT_BOOL conjugateX` を追加し、`VARIANT_FALSE` で `zgeru`、`VARIANT_TRUE` で `zgerc` を呼び分ける。`alpha` は複素数。 |
| SymvSimple | ZHemvSimple | cblas_zhemv | Hermitian 行列×ベクトル。`BlasUplo` で半分のみ参照し、対角虚部を検証。 |
| SyrSimple | ZHerSimple | cblas_zher | Hermitian rank-1。`alpha` は実数 (`alphaReal`) のみ。結果行列の対角虚部を 0 に保つ。 |
| Syr2Simple | ZHer2Simple | cblas_zher2 | rank-2。`alpha` 複素、ベクトル x/y の SAFEARRAY ペアを in/out で扱う。 |
| TrmvSimple | ZTrmvSimple | cblas_ztrmv | 三角行列×ベクトル。`incX` を尊重し in-place 更新。 |
| TrsvSimple | ZTrsvSimple | cblas_ztrsv | 三角連立の解。戻り値は `HRESULT` のみで、y ベクトルを in-place に上書き。 |

### Level 1 (ベクトル×ベクトル)
| 既存(実数) | 新規複素API | 対応BLASルーチン | 主なポイント |
| --- | --- | --- | --- |
| Nrm2 | ZNrm2 | cblas_dznrm2 | 戻り値は実数ノルム。IDL では `[out, retval] DOUBLE* norm` を宣言。 |
| Asum | ZAsum | cblas_dzasum | 絶対値和を返す。結果は `DOUBLE`。 |
| Scal | ZScal | cblas_zscal | 複素係数でベクトルを in-place スケール。`alphaReal`/`alphaImag` と `incX` を expose。 |
| (新設) | ZScalReal | cblas_zdscal | 実係数のみでスケールするための薄いラッパー。`alphaReal` のみを受け取る。 |
| Copy | ZCopy | cblas_zcopy | 実部・虚部をそれぞれコピー。長さとインクリメントを検証。 |
| Swap | ZSwap | cblas_zswap | 2 本の複素ベクトルを交換。 |
| Iamax | ZIamax | cblas_izamax | 絶対値最大要素の 1 始まりインデックス (`[out, retval] LONG* index1based`) を返す。 |
| Rot | ZRot | cblas_zdrot | 実数 `c`/`s` を受け取り、複素ベクトルを同時回転。`x`/`y` の SAFEARRAY は in/out。 |
| Rotg | ZRotg | cblas_zrotg | 複素 Givens 係数生成。`a`/`b` を実部・虚部のポインタで `[in, out]` とし、`c` は実数。 |

### 既存APIとの重複・非追加項目
- `ZGemmSimple` / `ZGemvSimple` / `ZAxpy` / `ZDot` は既に IDL 実装済みのため新規作業から除外。
- 実数版に存在する `Rotm` / `Rotmg` には OpenBLAS 側に複素版が存在しないため追加対象外。
- 帯行列 (`zgbmv` など) やパックドストレージ (`ztpmv` など) は実数 API にも無いため今回のスコープから除外。

## IDL シグネチャ共通仕様（案）
- 行列 API は 2 次元 `SAFEARRAY(double)` を実部・虚部で 2 本扱い、戻り値行列は `[in, out] SAFEARRAY(double)*` で受け取る。境界は下限 1 を目安とし、`ValidateComplexMatrixPair` で検証する。
- ベクトル API は 1 次元 SAFEARRAY を使用し、`incX`/`incY` は負値も許容する。内部では既存の `GatherComplexVector` / `ScatterComplexVector` を利用する。
- 既定引数は `alphaReal=1.0`, `alphaImag=0.0`, `betaReal=0.0`, `betaImag=0.0` とし、実数専用 API (`ZHer*`, `ZHerk*`, `ZScalReal`) では必要な実数係数のみ expose する。
- Hermitian 系 API では入力対角の虚部が 0 であることを前提とし、更新後も 0 に再設定する処理を行う。
- エラー時は既存の `SetComError` を使用し、引数不正は `E_INVALIDARG` などの標準 `HRESULT` で通知する。

## 今後のタスクリスト（概要）
- `COM_BLAS/COMBLAS.idl` に上記メソッドを追加し、MIDL の再生成 (`COMBLAS_i.*`, `COMBLAS_p.c`, `COMBLAS.tlb`) を実施。
- `BLAS.cpp` に OpenBLAS 呼び出し実装を追加し、共通検証ヘルパーを流用する。
- マネージドテスト (`COM_BLAS_UnitTest_Managed/Test1.cs`) を拡張し、各メソッドに対するサイズ・境界・エラーケースをカバーする。
- `ReadMe.md` や `complex_blas_implementation_plan.md` を更新し、クライアント向けの利用例とバージョン更新履歴を追記する。

