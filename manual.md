# COM_BLAS ユーザーマニュアル
最終更新日: 2025-09-27

## 1. 製品概要
COM_BLAS は、OpenBLAS をバックエンドに利用した数値線形代数ライブラリを COM コンポーネントとして提供します。ProgID は `Ckt.Com.Blas.BlasCore` で公開されており、.NET、VBA、C++ など COM Automation を扱える言語から倍精度実数および複素数の BLAS ルーチンを利用できます。

## 2. システム要件
- 対応 OS: Windows 10 / Windows 11 (64bit 版)
- ユーザー権限: インストール時に管理者権限（企業配布でサイレントインストールを行う場合も同様）
- ランタイム: Microsoft Visual C++ 2022 再頒布可能パッケージ (x64)。インストーラ前提条件で配布可能。
- 依存ライブラリ: `libopenblas.dll` (インストーラが本体 DLL と同じフォルダーに配置)
- 開発環境: COM Automation を利用できる言語/フレームワーク (.NET 6 以降、.NET Framework 4.8、VBA、C++/ATL など)

## 3. 提供物
ComponentSource で配布されるファイル一式には以下が含まれます。
- `COM_BLAS.Setup.msi`: 64bit Windows インストーラ。本インストーラのみで DLL/Type Library の配置と COM 登録が完了します。
- `manual.md`: 本ドキュメント。
- （オプション）VBA サンプル、利用許諾契約書、製品リリースノートなど。

## 4. インストール手順
### 4.1 GUI インストール
1. 管理者として Windows にサインインします。
2. `COM_BLAS.Setup.msi` をダブルクリックし、ウィザードの指示に従います。
3. インストール先は既定で `C:\Program Files\COM BLAS`（`[ProgramFiles64Folder]COM BLAS`）です。必要に応じて変更します。
4. セットアップが完了すると、`COM_BLAS.dll`、`CktComBlas.tlb`、`libopenblas.dll` がインストール先に配置され、COM レジストリ (`HKLM\Software\Classes`) が更新されます。

### 4.2 サイレントインストール
企業配布や自動化では `msiexec` のコマンドラインオプションを利用します。
```powershell
msiexec /i "COM_BLAS.Setup.msi" /qn INSTALLDIR="C:\Program Files\COM BLAS" /l*v install.log
```
- `/qn` で UI を抑止、`/l*v` で詳細ログを出力します。
- 既存バージョンがインストールされている場合、MSI の ProductCode が新バージョンに置き換わり、旧ファイルやレジストリは自動的にアンインストールされます。

### 4.3 アンインストール
- GUI: 「設定」→「アプリ」→「インストールされたアプリ」で **COM_BLAS** を選択し、アンインストール。
- コマンドライン: `msiexec /x {ProductCode} /qn` または 
  ```powershell
  msiexec /x "COM_BLAS.Setup.msi" /l*v uninstall.log
  ```
  を実行します（ProductCode は MSI のプロパティで確認できます）。

### 4.4 動作確認
インストール後、管理者権限は不要です。以下のコマンドで COM 登録を確認できます。
```powershell
$blas = New-Object -ComObject Ckt.Com.Blas.BlasCore
$blas.GetType().FullName  # IDispatch が正しく解決されれば成功
```
エラーが発生する場合は `TROUBLESHOOTING.md` を参照し、必要に応じて再インストールを実施してください。

## 5. アップグレードとメンテナンス
- 同一 UpgradeCode を利用しているため、新しい MSI を実行すると旧バージョンが自動置換されます。
- バージョンアップ時は `ReadMe.md` 記載のリリースノートで変更点を確認し、必要であれば `msiexec /i` に `/qb` を付与して進捗のみ表示することも可能です。
- インストール後に DLL を手動で差し替える運用は推奨されません。常に MSI を経由してください。

## 6. クイックスタート
### 6.1 C# (.NET 6 以降)
```csharp
dynamic blas = Activator.CreateInstance(Type.GetTypeFromProgID("Ckt.Com.Blas.BlasCore"));

double[,] a = { { 1, 2, 3 }, { 4, 5, 6 } };
double[,] b = { { 1, 0 }, { 0, 1 }, { 1, 1 } };
double[,] result = (double[,])blas.GemmSimple(a, b, null, 1.0, 0.0, 101, 111, 111);
```
- `tlbimp CktComBlas.tlb /namespace:Ckt.Com.Blas /out:Interop.CktComBlas.dll` を実行すると早期バインディングによる IntelliSense が利用できます。
- 生成した相互運用アセンブリはプロジェクトの参照に追加してください。

### 6.2 VBA (Excel)
```vb
Sub MultiplySample()
    Dim blas As Object
    Set blas = CreateObject("Ckt.Com.Blas.BlasCore")

    Dim left(1 To 2, 1 To 2) As Double
    Dim right(1 To 2, 1 To 2) As Double

    left(1, 1) = 1#: left(1, 2) = 2#
    left(2, 1) = 3#: left(2, 2) = 4#
    right(1, 1) = 1#: right(1, 2) = 0#
    right(2, 1) = 0#: right(2, 2) = 1#

    Dim product As Variant
    product = blas.GemmSimple(left, right)

    Debug.Print product(1, 1), product(1, 2)
End Sub
```
- VBA で列挙値を利用する場合は Type Library を参照設定し、`BlasLayout.RowMajor` などの定数を使用できます。

## 7. 公開 API 一覧
`BlasCore` は 3 系統の API を公開しています。すべてのメソッドは SAFEARRAY(double) を利用し、`BlasLayout` などの列挙値は Type Library に含まれます。

### 7.1 シンプル API (行列・ベクトル操作)
| API 名 | 概要 | 内部で利用する BLAS ルーチン |
|--------|------|-----------------------------|
| MultiplyMatrix | 2 行列の乗算。転置フラグで行列を転置してから乗算します。 | BLAS_GemmSimple |
| MultiplyMatrixAdd | `alpha * A * B + beta * C` を計算し結果を返却します。 | BLAS_GemmSimple |
| AddMatrices | 同じサイズの 2 行列を要素ごとに加算します。 | BLAS_Axpy |
| MultiplySymmetricMatrix | 対称行列と一般行列の乗算。左右どちらを対称とみなすかを選択。 | BLAS_SymmSimple |
| SolveTriangularSystem | 三角行列と右辺行列から線形方程式を解きます。 | BLAS_TrsmSimple |
| MultiplyMatrixVector | 行列と列ベクトルの積を計算します。 | BLAS_GemvSimple |
| OuterProduct | 2 ベクトルの外積を行列として生成します。 | BLAS_GerSimple |
| AddVectors | `alpha * x + beta * y` を計算して返却します。 | BLAS_Axpy + BLAS_Scal |
| ScaleVector | ベクトルをスカラー倍します。 | BLAS_Scal |
| DotProduct | 2 ベクトルの内積を返却します。 | BLAS_Dot |
| NormalizeVector | ベクトルを L2 ノルムで正規化します。 | BLAS_Nrm2 + BLAS_Scal |

### 7.2 IBLAS (倍精度実数 BLAS)
| レベル | メソッド | 概要 |
|--------|----------|------|
| Level 3 | GemmSimple | `C = alpha * op(A) * op(B) + beta * C` |
| Level 3 | SymmSimple | 対称行列と一般行列の積 (`side` で位置指定) |
| Level 3 | SyrkSimple | `C = alpha * op(A) * op(A)^T + beta * C`、C は対称行列 |
| Level 3 | Syr2kSimple | 対称 rank-2k 更新 `C = alpha * A * B^T + alpha * B * A^T + beta * C` |
| Level 3 | TrmmSimple | 三角行列による行列乗算 (左/右、副対角設定を選択) |
| Level 3 | TrsmSimple | 三角行列を用いた連立一次方程式の解法 |
| Level 2 | GemvSimple | `y = alpha * op(A) * x + beta * y` |
| Level 2 | GerSimple | Rank-1 更新 `A = A + alpha * x * y^T` |
| Level 2 | SymvSimple | 対称行列とのベクトル積 |
| Level 2 | SyrSimple | 対称行列の rank-1 更新 |
| Level 2 | Syr2Simple | 対称行列の rank-2 更新 |
| Level 2 | TrmvSimple | 三角行列とベクトルの積 |
| Level 2 | TrsvSimple | 三角行列を用いたベクトル方程式の解法 |
| Level 1 | Axpy | `y = alpha * x + y` |
| Level 1 | Dot | ベクトル内積。戻り値は `DOUBLE` |
| Level 1 | Nrm2 | ベクトルの 2-ノルム |
| Level 1 | Asum | 絶対値の総和 |
| Level 1 | Scal | ベクトルをスカラー倍 |
| Level 1 | Copy | ベクトルをコピー |
| Level 1 | Swap | 2 ベクトルの要素を交換 |
| Level 1 | Iamax | 絶対値が最大の要素インデックス (1 ベース) |
| Level 1 | Rot | Givens 回転を適用 |
| Level 1 | Rotg | Givens 回転パラメータの生成 |
| Level 1 | Rotm | 変形 Givens 回転 (param 長さ 5) |
| Level 1 | Rotmg | 変形 Givens 回転パラメータの生成 |

### 7.3 IBLASComplex (複素倍精度 BLAS)
| レベル | メソッド | 概要 |
|--------|----------|------|
| Level 3 | ZGemmSimple | `C = alpha * op(A) * op(B) + beta * C` |
| Level 3 | ZSymmSimple | 対称行列×一般行列 (実装は複素対称 = Hermitian) |
| Level 3 | ZHemmSimple | Hermitian 行列×一般行列 |
| Level 3 | ZSyrkSimple | 対称 rank-k 更新 |
| Level 3 | ZSyr2kSimple | 対称 rank-2k 更新 |
| Level 3 | ZHerkSimple | Hermitian rank-k 更新 (alpha, beta は実数) |
| Level 3 | ZHerk2kSimple | Hermitian rank-2k 更新 |
| Level 3 | ZTrmmSimple | 三角行列を用いた行列乗算 |
| Level 3 | ZTrsmSimple | 三角行列を用いた線形方程式の解法 |
| Level 2 | ZGemvSimple | `y = alpha * op(A) * x + beta * y` |
| Level 2 | ZGerSimple | Rank-1 更新 `A = A + alpha * x * y^H` |
| Level 2 | ZHemvSimple | Hermitian 行列×ベクトル |
| Level 2 | ZHerSimple | Hermitian rank-1 更新 |
| Level 2 | ZHer2Simple | Hermitian rank-2 更新 |
| Level 2 | ZTrmvSimple | 三角行列とベクトルの積 |
| Level 2 | ZTrsvSimple | 三角行列を用いたベクトル方程式の解法 |
| Level 1 | ZAxpy | `y = y + alpha * x` |
| Level 1 | ZDot | 複素内積 (`conjugate` 引数で共役有無を選択) |
| Level 1 | ZNrm2 | 複素ベクトルの 2-ノルム |
| Level 1 | ZAsum | 絶対値の総和 |
| Level 1 | ZScal | 複素スカラー倍 |
| Level 1 | ZScalReal | 実数スカラー倍 |
| Level 1 | ZCopy | ベクトルコピー |
| Level 1 | ZSwap | ベクトル交換 |
| Level 1 | ZIamax | 絶対値が最大の要素インデックス |
| Level 1 | ZRot | 実数 Givens 回転を複素ベクトルへ適用 |
| Level 1 | ZRotg | 複素 Givens 回転パラメータの生成 |

### 7.4 列挙体と ProgID
- 列挙体: `BlasLayout` (RowMajor/ColumnMajor), `BlasTranspose` (NoTrans/Trans/ConjTrans), `BlasUplo` (Upper/Lower), `BlasDiag` (NonUnit/Unit), `BlasSide` (Left/Right)
- ProgID: `Ckt.Com.Blas.BlasCore`
- Type Library: `CktComBlas.tlb` (`library CktComBlasLib`, version 1.5)

## 8. トラブルシューティング (Q&A)
- **Q:** インストーラ実行時に「アクセスが拒否されました」やエラーコード 5 が表示されます。  
  **A:** 管理者権限のあるアカウントでサインインし、`COM_BLAS.Setup.msi` を右クリックして「管理者として実行」を選択してください。
- **Q:** インストール後に `New-Object -ComObject Ckt.Com.Blas.BlasCore` が 0x0000007E で失敗します。  
  **A:** `libopenblas.dll` が COM DLL と同じフォルダーにあるか、PATH へ追加されているかを確認してください。標準インストール先 (`C:\Program Files\COM BLAS`) の DLL を誤って削除した場合は、MSI を再実行して修復してください。
- **Q:** 旧バージョンからアップグレードしたら VBA や `dynamic` から複素数 API が表示されません。  
  **A:** いったんアンインストール後に最新 MSI を再インストールし、Office や .NET プロセスを再起動してください。Type Library が更新されていない場合は、管理者 PowerShell で `regtlib.exe "C:\Program Files\COM BLAS\CktComBlas.tlb"` を実行して再登録します。
- **Q:** MSI 実行途中で 0x80070643 や Visual C++ ランタイムに関するメッセージが表示されます。  
  **A:** Microsoft Visual C++ 2022 再頒布可能パッケージ (x64) を先にインストールしてから再実行してください。

## 9. サポート窓口
- 技術的な問い合わせ: 製品同梱のサポート連絡先 (メールまたは専用ポータル)。
- 不具合報告: サポート窓口に再現手順、環境情報 (OS、バージョン、ログ) を添えて連絡してください。

## 10. ライセンスと再配布
- ご購入いただいたライセンスに従い、エンドユーザーへ再配布する際はインストーラ (`COM_BLAS.Setup.msi`) ごと配布してください。
- MSI を改変する場合は事前にライセンス条項をご確認のうえ、必要に応じてサポート窓口と協議してください。

## 11. 変更履歴
- 2025-09-27: インストーラ手順と公開 API 全件を反映した改訂版を公開。
- 2025-09-27: 初版公開。
