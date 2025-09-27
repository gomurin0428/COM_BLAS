# COM_BLAS User Manual
Last Updated: 2025-09-27

## 1. Product Overview
COM_BLAS exposes a numerical linear algebra library backed by OpenBLAS as a COM component. The ProgID is `Ckt.Com.Blas.BlasCore`, so any COM Automation capable environment (.NET, VBA, C++/ATL, etc.) can call the double-precision real and complex BLAS routines that the component provides. This guide targets the installer package distributed through ComponentSource.

## 2. System Requirements
- Supported OS: Windows 10 / Windows 11 (64-bit)
- Privileges: Local administrator privileges are required during installation, including silent deployments in enterprise environments
- Runtime: Microsoft Visual C++ 2022 Redistributable (x64); can be delivered as an installer prerequisite
- Dependency: `libopenblas.dll` (deployed by the installer into the same directory as the COM DLL)
- Development environments: Any framework that can use COM Automation (.NET 6+, .NET Framework 4.8, VBA, native C++/ATL, etc.)

## 3. Package Contents
A standard delivery obtained from ComponentSource contains:
- `COM_BLAS.Setup.msi`: 64-bit Windows Installer package. Running this MSI handles DLL/Type Library placement and COM registration.
- `manual.md`: Japanese product manual.
- `manual_en.md`: This English product manual.
- Optional artifacts such as VBA samples, license agreements, and release notes.

## 4. Installation Instructions
### 4.1 GUI Installation
1. Sign in to Windows with an administrator account.
2. Double-click `COM_BLAS.Setup.msi` and follow the wizard.
3. The default installation path is `C:\Program Files\COM BLAS` (internally `[ProgramFiles64Folder]COM BLAS`); adjust if needed.
4. After completion, `COM_BLAS.dll`, `CktComBlas.tlb`, and `libopenblas.dll` are copied to the installation directory and the COM registry (HKLM\Software\Classes) is updated.

### 4.2 Silent Installation
For enterprise deployment or automation, use `msiexec` command-line switches.
```powershell
msiexec /i "COM_BLAS.Setup.msi" /qn INSTALLDIR="C:\Program Files\COM BLAS" /l*v install.log
```
- `/qn` suppresses the UI; `/l*v` writes a verbose log.
- When an older version is present, the MSI replaces it (same UpgradeCode) and removes outdated files and registry entries automatically.

### 4.3 Uninstall
- GUI: Settings → Apps → Installed Apps, select **COM_BLAS**, choose uninstall.
- Command line: `msiexec /x {ProductCode} /qn` or
  ```powershell
  msiexec /x "COM_BLAS.Setup.msi" /l*v uninstall.log
  ```
  (`ProductCode` can be retrieved from the MSI properties or the original installation logs.)

### 4.4 Post-install Verification
Administrator privileges are no longer required once installation completes. Validate COM registration with:
```powershell
$blas = New-Object -ComObject Ckt.Com.Blas.BlasCore
$blas.GetType().FullName  # If IDispatch resolves, registration succeeded
```
If an error occurs, reinstall using the MSI or follow the troubleshooting section below.

## 5. Upgrades and Maintenance
- The MSI reuses the same UpgradeCode; running a newer MSI upgrades the existing installation in place.
- Review the release notes in `ReadMe.md` before upgrading. If you want progress UI during deployment, run `msiexec /i ... /qb`.
- Avoid manually swapping DLLs after installation. Always redeploy via the MSI to keep COM registration consistent.

## 6. Quick Start
### 6.1 C# (.NET 6 or later)
```csharp
dynamic blas = Activator.CreateInstance(Type.GetTypeFromProgID("Ckt.Com.Blas.BlasCore"));

double[,] a = { { 1, 2, 3 }, { 4, 5, 6 } };
double[,] b = { { 1, 0 }, { 0, 1 }, { 1, 1 } };
double[,] result = (double[,])blas.GemmSimple(a, b, null, 1.0, 0.0, 101, 111, 111);
```
- For early binding and IntelliSense, run `tlbimp CktComBlas.tlb /namespace:Ckt.Com.Blas /out:Interop.CktComBlas.dll` and add the generated assembly as a project reference.

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
- When you reference the Type Library in VBA, you can use constants such as `BlasLayout.RowMajor` directly.

## 7. Published APIs
`BlasCore` exposes three API families. All methods accept SAFEARRAY(double), and enumerations such as `BlasLayout` are defined in the Type Library.

### 7.1 Simple APIs (matrix/vector helpers)
| API | Description | Backing BLAS routine |
|-----|-------------|-----------------------|
| MultiplyMatrix | Matrix multiplication with optional transpose flags. | BLAS_GemmSimple |
| MultiplyMatrixAdd | Computes `alpha * A * B + beta * C`. | BLAS_GemmSimple |
| AddMatrices | Element-wise addition for matrices of equal size. | BLAS_Axpy |
| MultiplySymmetricMatrix | Multiply a symmetric matrix with a general matrix; choose which side is symmetric. | BLAS_SymmSimple |
| SolveTriangularSystem | Solve linear systems using a triangular matrix and right-hand side. | BLAS_TrsmSimple |
| MultiplyMatrixVector | Matrix-vector product. | BLAS_GemvSimple |
| OuterProduct | Builds a matrix from the outer product of two vectors. | BLAS_GerSimple |
| AddVectors | Computes `alpha * x + beta * y`. | BLAS_Axpy + BLAS_Scal |
| ScaleVector | Scales a vector by a scalar. | BLAS_Scal |
| DotProduct | Returns the dot product of two vectors. | BLAS_Dot |
| NormalizeVector | Normalizes a vector using its L2 norm. | BLAS_Nrm2 + BLAS_Scal |

### 7.2 IBLAS (double-precision real BLAS)
| Level | Method | Summary |
|-------|--------|---------|
| Level 3 | GemmSimple | `C = alpha * op(A) * op(B) + beta * C` |
| Level 3 | SymmSimple | Symmetric/general matrix multiply (select side). |
| Level 3 | SyrkSimple | Symmetric rank-k update `C = alpha * op(A) * op(A)^T + beta * C`. |
| Level 3 | Syr2kSimple | Symmetric rank-2k update `C = alpha * A * B^T + alpha * B * A^T + beta * C`. |
| Level 3 | TrmmSimple | Triangular matrix multiply (left/right, diag flags). |
| Level 3 | TrsmSimple | Solve triangular systems. |
| Level 2 | GemvSimple | `y = alpha * op(A) * x + beta * y`. |
| Level 2 | GerSimple | Rank-1 update `A = A + alpha * x * y^T`. |
| Level 2 | SymvSimple | Symmetric matrix-vector multiply. |
| Level 2 | SyrSimple | Symmetric rank-1 update. |
| Level 2 | Syr2Simple | Symmetric rank-2 update. |
| Level 2 | TrmvSimple | Triangular matrix-vector multiply. |
| Level 2 | TrsvSimple | Solve triangular vector equations. |
| Level 1 | Axpy | `y = alpha * x + y`. |
| Level 1 | Dot | Dot product (returns `DOUBLE`). |
| Level 1 | Nrm2 | Vector L2 norm. |
| Level 1 | Asum | Sum of absolute values. |
| Level 1 | Scal | Scale a vector by a scalar. |
| Level 1 | Copy | Copy one vector to another. |
| Level 1 | Swap | Swap two vectors. |
| Level 1 | Iamax | Index (1-based) of the maximum absolute value. |
| Level 1 | Rot | Apply a Givens rotation. |
| Level 1 | Rotg | Generate Givens rotation parameters. |
| Level 1 | Rotm | Apply modified Givens rotation (param length 5). |
| Level 1 | Rotmg | Generate modified Givens rotation parameters. |

### 7.3 IBLASComplex (double-precision complex BLAS)
| Level | Method | Summary |
|-------|--------|---------|
| Level 3 | ZGemmSimple | `C = alpha * op(A) * op(B) + beta * C`. |
| Level 3 | ZSymmSimple | Symmetric/general matrix multiply (implemented as Hermitian). |
| Level 3 | ZHemmSimple | Hermitian/general matrix multiply. |
| Level 3 | ZSyrkSimple | Symmetric rank-k update. |
| Level 3 | ZSyr2kSimple | Symmetric rank-2k update. |
| Level 3 | ZHerkSimple | Hermitian rank-k update (alpha, beta real). |
| Level 3 | ZHerk2kSimple | Hermitian rank-2k update. |
| Level 3 | ZTrmmSimple | Triangular matrix multiply. |
| Level 3 | ZTrsmSimple | Solve triangular systems. |
| Level 2 | ZGemvSimple | `y = alpha * op(A) * x + beta * y`. |
| Level 2 | ZGerSimple | Rank-1 update `A = A + alpha * x * y^H`. |
| Level 2 | ZHemvSimple | Hermitian matrix-vector multiply. |
| Level 2 | ZHerSimple | Hermitian rank-1 update. |
| Level 2 | ZHer2Simple | Hermitian rank-2 update. |
| Level 2 | ZTrmvSimple | Triangular matrix-vector multiply. |
| Level 2 | ZTrsvSimple | Solve triangular vector equations. |
| Level 1 | ZAxpy | `y = y + alpha * x`. |
| Level 1 | ZDot | Complex dot product (with optional conjugation). |
| Level 1 | ZNrm2 | Complex vector L2 norm. |
| Level 1 | ZAsum | Sum of absolute values. |
| Level 1 | ZScal | Complex scaling. |
| Level 1 | ZScalReal | Real scaling. |
| Level 1 | ZCopy | Copy a vector. |
| Level 1 | ZSwap | Swap two vectors. |
| Level 1 | ZIamax | Index (1-based) of the maximum magnitude. |
| Level 1 | ZRot | Apply a real Givens rotation to complex vectors. |
| Level 1 | ZRotg | Generate complex Givens rotation parameters. |

### 7.4 Enumerations and ProgID
- Enumerations: `BlasLayout` (RowMajor/ColumnMajor), `BlasTranspose` (NoTrans/Trans/ConjTrans), `BlasUplo` (Upper/Lower), `BlasDiag` (NonUnit/Unit), `BlasSide` (Left/Right)
- ProgID: `Ckt.Com.Blas.BlasCore`
- Type Library: `CktComBlas.tlb` (`library CktComBlasLib`, version 1.5)

## 8. Troubleshooting (Q&A)
- **Q:** The installer shows "Access is denied" or error code 5.  
  **A:** Sign in with an administrator account and run `COM_BLAS.Setup.msi` using "Run as administrator". In managed environments, deploy with elevated privileges (for example, `msiexec /i ... /qn` executed by IT).
- **Q:** After installation, `New-Object -ComObject Ckt.Com.Blas.BlasCore` fails with 0x0000007E.  
  **A:** Ensure `libopenblas.dll` is present in the same directory as `COM_BLAS.dll` or on the system PATH. If it was removed, rerun the MSI to repair the installation.
- **Q:** After upgrading from an older version, VBA or `dynamic` clients cannot see the complex APIs.  
  **A:** Uninstall the previous version, install the latest MSI, and restart Office/.NET processes. If the Type Library did not refresh, run `regtlib.exe "C:\Program Files\COM BLAS\CktComBlas.tlb"` in an elevated PowerShell session.
- **Q:** The MSI aborts with 0x80070643 or messages about the Visual C++ runtime.  
  **A:** Install the Microsoft Visual C++ 2022 Redistributable (x64) first, then rerun the MSI. For enterprise deployment, include the redistributable as a prerequisite.

## 9. Support
- Technical questions: Use the support contact bundled with the product (email or customer portal).
- Bug reports: Provide reproduction steps and environment details (OS version, logs) when contacting support.

## 10. Licensing and Redistribution
- Follow your purchased license when redistributing COM_BLAS; ship the MSI (`COM_BLAS.Setup.msi`) as part of the payload.
- If you need to modify the MSI, review the license terms and consult support beforehand.

## 11. Revision History
- 2025-09-27: First English edition aligning with the Japanese manual and the installer-based workflow.
