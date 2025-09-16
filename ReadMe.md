# COM_BLAS Error Handling Specification

本ライブラリの COM 実装では、入力検証に失敗した場合は HRESULT を返し、`ISupportErrorInfo` を通じてエラーメッセージを公開します。以下では共通ルールと、メソッドごとの代表的な異常系と HRESULT の対応をまとめます。

## 共通ルール

| 条件 | HRESULT | 備考 |
|------|---------|------|
| `SAFEARRAY*` が `NULL`、または次元数が仕様と異なる | `E_INVALIDARG (0x80070057)` | 1 次元ベクトルは `SAFEARRAY` の次元数=1、行列は 2 次元が必須 |
| `SAFEARRAY**` 引数が `NULL` (例: `ref double[,]` に `null` を渡す) | `E_POINTER (0x80004003)` | `EnsureArrayPointer` により検出 |
| ベクトル長や行列サイズが演算要件を満たさない | `E_INVALIDARG (0x80070057)` | 例: `GemmSimple` の内積次元不一致、非正方行列の指定など |
| インクリメント (`incX`, `incY`) が 0 | `E_INVALIDARG (0x80070057)` | すべての Level-1 ルーチンで共通 |
| 指定した長さと `SAFEARRAY` の実サイズが合わずアクセス範囲外になる | `E_BOUNDS (0x8000000B)` | `PrepareVectorView` が `access out of bounds` を生成 |
| `SafeArrayAccessData` が失敗 (理論上) | `E_FAIL (0x80004005)` | 実際には .NET 配列では発生しませんが、COM 実装内でハンドリング |
| `double*` 引数が `NULL` (`Rotg`, `Rotmg` など) | `E_POINTER (0x80004003)` |

## メソッド別の主な異常系

| メソッド | 代表的な検証 | HRESULT |
|----------|--------------|---------|
| `GemmSimple` | 出力 `C` の行列サイズが `m x n` に一致しない | `E_INVALIDARG` |
| `SymmSimple` | `side=Left` の場合に A が `m x m` でない (`m = rows(C)`) | `E_INVALIDARG` |
| `SyrkSimple` | `C` が正方行列でない | `E_INVALIDARG` |
| `Syr2kSimple` | `A` と `B` の行数が一致しない (RowMajor/NoTrans) | `E_INVALIDARG` |
| `TrmmSimple` | 行列 A が正方でない／`SAFEARRAY** B` が `NULL` | `E_INVALIDARG` / `E_POINTER` |
| `TrsmSimple` | 行列 A が正方でない | `E_INVALIDARG` |
| `GemvSimple` | `y` の長さが期待値より短い | `E_BOUNDS` |
| `GerSimple` | `x` と `A` の行数、`y` と列数が一致しない | `E_BOUNDS` |
| `SymvSimple` | 行列 A が正方でない | `E_INVALIDARG` |
| `SyrSimple` | `x` の長さと `A` の次元不一致 | `E_BOUNDS` |
| `Syr2Simple` | `x` と `y` の長さ不一致 | `E_BOUNDS` |
| `TrmvSimple` | 行列 A が正方でない | `E_INVALIDARG` |
| `TrsvSimple` | 行列 A が正方でない | `E_INVALIDARG` |
| `Axpy` | `incX` または `incY` が 0 | `E_INVALIDARG` |
| `Dot` | `incX` or `incY` が 0 / ベクトル不足で範囲外アクセス | `E_INVALIDARG` / `E_BOUNDS` |
| `Nrm2`, `Asum` | `n` が負 | `E_INVALIDARG` |
| `Scal`, `Copy`, `Swap`, `Iamax`, `Rot` | インクリメントが 0 | `E_INVALIDARG` |
| `Rotm` | `param` 配列の長さが 5 未満 | `E_BOUNDS` |
| `Rotmg` | `param` 出力用 `SAFEARRAY**` が `NULL` | `E_POINTER` |

上記に加え、共通の `SAFEARRAY` 検証 (`EnsureDoubleSafeArray`) により、`VT_R8` 以外の型、要素サイズ不一致、`VARIANT` 型配列なども `E_INVALIDARG` で拒否されます。

## テスト方針

`COM_BLAS_UnitTest_Managed` プロジェクトでは、

- 正常系: 各メソッドに対して 1x1 スカラーケースを利用した成功テストを実装しています。
- 異常系: 上表のチェックに対応する HRESULT を確認する 25 件のテストを追加し、戻り値 (例外の `HResult`) が仕様どおりであることを検証します。

これらのテストは `msbuild COM_BLAS.sln /p:Configuration=Debug /p:Platform=x64` および `vstest.console.exe COM_BLAS_UnitTest_Managed\bin\Debug\net8.0-windows\COM_BLAS_UnitTest_Managed.dll /Platform:x64 /Framework:.NETCoreApp,Version=v8.0` で実行可能です。
