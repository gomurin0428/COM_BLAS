# History

## 2025-09-23
- `COM_BLAS/BLAS.cpp` にて `TrmmSimple` の `SAFEARRAY** B` が `null` だった場合に確実に `E_POINTER (0x80004003)` を返すガードを追加し、`EnsureArrayPointer` で捕捉できない COM Automation 経由の `VT_BYREF|VT_VARIANT` ケースも捕捉するよう修正。
- 同ファイルの `CBLAS::Invoke` に `Rotmg` 専用のディスパッチを追加し、`dynamic` 呼び出し時に 5 番目の `param` が `VT_BYREF|VT_VARIANT` で渡されても SAFEARRAY を生成して返せるようにして `Rotmg_ScalarCase` の失敗を解消。
- x64 構成で `COM_BLAS.vcxproj` を再ビルドし、`vstest.console` で `COM_BLAS_UnitTest_Managed` の全 113 テストが成功することを確認。
