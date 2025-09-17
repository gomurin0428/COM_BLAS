# COM_BLAS シンプル API

COM_BLAS は BLAS の各ルーチンを COM として公開していますが、`GemmSimple` や `Axpy` のような命名とパラメーター構成は数値線形代数に不慣れな利用者には理解しづらいままです。`BLAS_` 接頭辞で元の BLAS API を区別しつつ、日常的な行列・ベクトル操作に直感的なシンプル API を以下のとおり確定版として公開します。

## 命名ポリシー
- 既存の BLAS 相当メソッドは `BLAS_GemmSimple`, `BLAS_Axpy` のように `BLAS_` 接頭辞を付与し、BLAS 仕様との 1:1 対応を明確にする。
- 新しく導入したシンプル API は、アルゴリズム名ではなく処理内容をそのまま表す英語フレーズ (`MultiplyMatrix`, `AddVectors` など) を採用する。
- 共通の列挙値 (`BlasLayout` など) は内部的に `BLAS_` 呼び出しへマッピングし、シンプル API からは基本的に露出させない。必要な分岐は `VARIANT_BOOL` など少数のフラグで表現する。
- 既存のエラーハンドリングとテスト資産は `BLAS_` 系で維持し、シンプル API 側は内部で同じバリデーション関数を呼び出すことで仕様の一貫性を確保する。

## シンプル API の設計原則
- 2 次元 `SAFEARRAY(double)` は既定で RowMajor とみなし、.NET の `double[,]` と 1:1 で往復できる形を標準とする。
- 戻り値で結果配列を返し、呼び出し側でサイズを事前確保しなくても良いようにする (必要に応じてインプレース版を追加)。
- スカラー係数は 0/1 を既定値とし、指定しない限り単純な演算に限定する。
- 失敗時の HRESULT は既存の `Ensure*` 系ユーティリティを再利用して既存仕様と揃える。
- 実装はできるだけ単一の `BLAS_` 呼び出しで完結させ、追加のループ実装が必要な場合はサイズが小さいケースに限定する。

## シンプル API 一覧
以下が COM_BLAS が提供するシンプル API の確定版です。括弧内は IDL 上のシグネチャ例と内部で利用する `BLAS_` メソッドです。

### 行列演算
- **MultiplyMatrix**  
  `SAFEARRAY(double) MultiplyMatrix(SAFEARRAY(double) left, SAFEARRAY(double) right, [optional, defaultvalue(VARIANT_FALSE)] VARIANT_BOOL transposeLeft, [optional, defaultvalue(VARIANT_FALSE)] VARIANT_BOOL transposeRight);`  
  行列乗算 (既定は RowMajor/非転置)。転置指定は TRUE のときのみ `BLAS_GemmSimple` の `BlasTranspose.Trans` を設定する。
- **MultiplyMatrixAdd**  
  `SAFEARRAY(double) MultiplyMatrixAdd(SAFEARRAY(double) left, SAFEARRAY(double) right, SAFEARRAY(double) addend, [optional, defaultvalue(1.0)] DOUBLE alpha, [optional, defaultvalue(1.0)] DOUBLE beta);`  
  `result = alpha * left * right + beta * addend` を返す。`addend` が `NULL` の場合は `beta` を 0 扱いにする。内部では `BLAS_GemmSimple` を利用。
- **AddMatrices**  
  `SAFEARRAY(double) AddMatrices(SAFEARRAY(double) left, SAFEARRAY(double) right);`  
  同じサイズの 2 行列を単純加算。内部で配列を 1 次元ビューとして `BLAS_Axpy` を呼び出す。
- **MultiplySymmetricMatrix**  
  `SAFEARRAY(double) MultiplySymmetricMatrix(SAFEARRAY(double) symmetric, SAFEARRAY(double) other, [optional, defaultvalue(VARIANT_TRUE)] VARIANT_BOOL symmetricOnLeft);`  
  対称行列と一般行列の乗算。`symmetricOnLeft = TRUE` で `symmetric * other`、FALSE で `other * symmetric`。内部では `BLAS_SymmSimple` を利用。
- **SolveTriangularSystem**  
  `SAFEARRAY(double) SolveTriangularSystem(SAFEARRAY(double) triangular, SAFEARRAY(double) rhs, [optional, defaultvalue(VARIANT_TRUE)] VARIANT_BOOL solveLeft, [optional, defaultvalue(VARIANT_TRUE)] VARIANT_BOOL isUpper, [optional, defaultvalue(VARIANT_FALSE)] VARIANT_BOOL unitDiagonal);`  
  三角行列と右辺行列 (複数列ベクトル含む) から解を返す。内部では `BLAS_TrsmSimple` を呼び出し、フラグを `BlasSide`, `BlasUplo`, `BlasDiag` にマッピングする。

### 行列とベクトルの組み合わせ
- **MultiplyMatrixVector**  
  `SAFEARRAY(double) MultiplyMatrixVector(SAFEARRAY(double) matrix, SAFEARRAY(double) vector, [optional, defaultvalue(VARIANT_FALSE)] VARIANT_BOOL transposeMatrix);`  
  行列と列ベクトルの積を返す。内部は `BLAS_GemvSimple`。
- **OuterProduct**  
  `SAFEARRAY(double) OuterProduct(SAFEARRAY(double) leftVector, SAFEARRAY(double) rightVector);`  
  `leftVector` と `rightVector` の外積行列を生成。`BLAS_GerSimple` を利用。

### ベクトル演算
- **AddVectors**  
  `SAFEARRAY(double) AddVectors(SAFEARRAY(double) left, SAFEARRAY(double) right, [optional, defaultvalue(1.0)] DOUBLE alpha, [optional, defaultvalue(1.0)] DOUBLE beta);`  
  `alpha * left + beta * right` を返す。単純加算としても利用可能で、内部では `BLAS_Axpy` ならびに必要に応じて `BLAS_Scal` を組み合わせる。
- **ScaleVector**  
  `SAFEARRAY(double) ScaleVector(SAFEARRAY(double) vector, DOUBLE alpha);`  
  ベクトルをスカラー倍した結果を返す。`BLAS_Scal` を利用。
- **DotProduct**  
  `DOUBLE DotProduct(SAFEARRAY(double) left, SAFEARRAY(double) right);`  
  2 ベクトルの内積を返す。`BLAS_Dot` をそのまま呼び出す。
- **NormalizeVector**  
  `SAFEARRAY(double) NormalizeVector(SAFEARRAY(double) vector);`  
  `vector / ||vector||_2` を返す。`BLAS_Nrm2` でノルムを取得し、`BLAS_Scal` でスケーリングする。ゼロベクトル時は `E_INVALIDARG` を返す方針。

## 代表的な利用イメージ
```csharp
// C# から COM_BLAS を late binding で呼び出す例
dynamic blas = Activator.CreateInstance(Type.GetTypeFromProgID("COMBLAS.BLAS"));

// 2x3 と 3x2 の行列乗算
double[,] a = { { 1, 2, 3 }, { 4, 5, 6 } };
double[,] b = { { 1, 0 }, { 0, 1 }, { 1, 1 } };
double[,] product = (double[,])blas.MultiplyMatrix(a, b);

// ベクトル正規化と外積
double[] x = { 2, 0, 0 };
double[] y = { 1, 2, 3 };
double[] unitX = (double[])blas.NormalizeVector(x);
double[,] outer = (double[,])blas.OuterProduct(unitX, y);
```

## 実装およびテストのメモ
- シンプル API から生成する `SAFEARRAY` は `SafeArrayCreateVector` を利用し、内部的に `PrepareVectorView`/`PrepareMatrixView` と同様の検証を施したヘルパーを共用する。
- `MultiplyMatrix*` などの配列サイズ検証は既存の `PrepareMatrixView` / `PrepareVectorView` / `EnsureDoubleSafeArray` を内部で使い回し、下記表のエラーテーブルと同じ HRESULT を返す。
- ユニットテストでは `COM_BLAS_UnitTest_Managed` の高水準シナリオ (行列積、ベクトル加算、正規化の異常系など) を通じて、`BLAS_` API を直接呼び出したケースとの比較で正確性を検証する。
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

## VBA サンプルコード

以下は VBA (Excel) から COM_BLAS コンポーネントの動作を確認するための例です。64bit Office を想定しています。

`b
Sub TestComBlas()
    ' COM コンポーネントを生成
    Dim blas As Object
    Set blas = CreateObject(""COMBLAS.BLAS"")

    ' 1x1 スカラーの GEMM: C = 1.5 * A*B + 0.2 * C
    Dim A(1 To 1, 1 To 1) As Double
    Dim B(1 To 1, 1 To 1) As Double
    Dim C(1 To 1, 1 To 1) As Double

    A(1, 1) = 2#
    B(1, 1) = 3#
    C(1, 1) = 4#

    blas.GemmSimple A, B, C, 1.5, 0.2, 101, 111, 111 ' RowMajor/NoTrans

    Debug.Print ""GemmSimple result:""; C(1, 1)

    ' Level-1: y = alpha * x + y (AXPY)
    Dim x(0 To 2) As Double
    Dim y(0 To 2) As Double
    x(0) = 1#: x(1) = 2#: x(2) = 3#
    y(0) = 4#: y(1) = 5#: y(2) = 6#

    blas.Axpy 3, 0.5, x, 1, y, 1
    Debug.Print ""Axpy result:""; y(0); y(1); y(2)

    ' 異常系: incX = 0 にすると E_INVALIDARG (HRESULT 0x80070057)
    On Error Resume Next
    blas.Axpy 3, 1#, x, 0, y, 1
    If Err.Number <> 0 Then
        Debug.Print ""Expected failure:""; Hex(Err.Number)
        Err.Clear
    End If

    Set blas = Nothing
End Sub
`

> **補足:** VBA から列挙値を指定する場合は、IDL で定義した定数値を直接指定しています (BlasLayout.RowMajor=101 など)。COM を参照設定して型ライブラリをインポートすれば、定数名をそのまま利用することも可能です。
