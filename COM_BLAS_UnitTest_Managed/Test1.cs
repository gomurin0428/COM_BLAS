using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace COM_BLAS_UnitTest_Managed
{
    internal enum BlasLayout
    {
        RowMajor = 101,
        ColumnMajor = 102,
    }

    internal enum BlasTranspose
    {
        NoTrans = 111,
        Trans = 112,
        ConjTrans = 113,
    }

    internal enum BlasUplo
    {
        Upper = 121,
        Lower = 122,
    }

    internal enum BlasDiag
    {
        NonUnit = 131,
        Unit = 132,
    }

    internal enum BlasSide
    {
        Left = 141,
        Right = 142,
    }

    [ComImport]
    [Guid("19ef5ee4-5e52-47fa-ba23-c9f70bef7faa")]
    [InterfaceType(ComInterfaceType.InterfaceIsDual)]
    internal interface IBLAS
    {
        void GemmSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] B,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] C,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasTranspose transA,
            BlasTranspose transB);

        void SymmSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] B,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] C,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasSide side,
            BlasUplo uplo);

        void SyrkSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] C,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasUplo uplo,
            BlasTranspose transA);

        void Syr2kSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] B,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] C,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasUplo uplo,
            BlasTranspose trans);

        void TrmmSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] B,
            double alpha,
            BlasLayout layout,
            BlasSide side,
            BlasUplo uplo,
            BlasTranspose transA,
            BlasDiag diag);

        void TrsmSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] B,
            double alpha,
            BlasLayout layout,
            BlasSide side,
            BlasUplo uplo,
            BlasTranspose transA,
            BlasDiag diag);

        void GemvSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasTranspose transA);

        void GerSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] y,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] A,
            double alpha,
            BlasLayout layout);

        void SymvSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            double alpha,
            double beta,
            BlasLayout layout,
            BlasUplo uplo);

        void SyrSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] A,
            double alpha,
            BlasLayout layout,
            BlasUplo uplo);

        void Syr2Simple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] y,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] A,
            double alpha,
            BlasLayout layout,
            BlasUplo uplo);

        void TrmvSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            BlasLayout layout,
            BlasUplo uplo,
            BlasTranspose transA,
            BlasDiag diag);

        void TrsvSimple(
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] A,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            BlasLayout layout,
            BlasUplo uplo,
            BlasTranspose transA,
            BlasDiag diag);

        void Axpy(
            int n,
            double alpha,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            int incY);

        double Dot(
            int n,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] y,
            int incY);

        double Nrm2(
            int n,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX);

        double Asum(
            int n,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX);

        void Scal(
            int n,
            double alpha,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            int incX);

        void Copy(
            int n,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            int incY);

        void Swap(
            int n,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            int incX,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            int incY);

        int Iamax(
            int n,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] x,
            int incX);

        void Rot(
            int n,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            int incX,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            int incY,
            double c,
            double s);

        void Rotg(
            ref double a,
            ref double b,
            out double c,
            out double s);

        void Rotm(
            int n,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] x,
            int incX,
            [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] y,
            int incY,
            [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] param);

        void Rotmg(
            ref double d1,
            ref double d2,
            ref double x1,
            double y1,
            [Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] out double[] param);
    }
    internal static class ComBlasFactory
    {
        private static readonly string DllPath = ResolveDllPath();
        private static readonly IntPtr ModuleHandle = NativeLibrary.Load(DllPath);
        private static readonly DllGetClassObjectDelegate DllGetClassObject = Marshal.GetDelegateForFunctionPointer<DllGetClassObjectDelegate>(NativeLibrary.GetExport(ModuleHandle, "DllGetClassObject"));
        private static readonly Guid CLSID_BLAS = new Guid("e8f3aed3-eec4-48ab-9925-c13253d4c396");
        private static readonly Guid IID_IClassFactory = new Guid("00000001-0000-0000-C000-000000000046");
        private static readonly Guid IID_IBLAS = typeof(IBLAS).GUID;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private delegate int DllGetClassObjectDelegate(ref Guid clsid, ref Guid iid, out IntPtr ppv);

        [ComImport]
        [Guid("00000001-0000-0000-C000-000000000046")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IClassFactoryInternal
        {
            void CreateInstance([MarshalAs(UnmanagedType.IUnknown)] object? pUnkOuter, ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppvObject);
            void LockServer(bool fLock);
        }

        internal static IBLAS Create()
        {
            IntPtr factoryPtr = IntPtr.Zero;
            Guid clsid = CLSID_BLAS;
            Guid iidFactory = IID_IClassFactory;
            int hr = DllGetClassObject(ref clsid, ref iidFactory, out factoryPtr);
            if (hr < 0)
            {
                Marshal.ThrowExceptionForHR(hr);
            }

            try
            {
                var factory = (IClassFactoryInternal)Marshal.GetObjectForIUnknown(factoryPtr);
                try
                {
                    Guid iid = IID_IBLAS;
                    factory.CreateInstance(null, ref iid, out object instance);
                    return (IBLAS)instance;
                }
                finally
                {
                    Marshal.ReleaseComObject(factory);
                }
            }
            finally
            {
                if (factoryPtr != IntPtr.Zero)
                {
                    Marshal.Release(factoryPtr);
                }
            }
        }

        private static string ResolveDllPath()
        {
            string baseDir = AppContext.BaseDirectory;
            string root = Path.GetFullPath(Path.Combine(baseDir, "..", "..", "..", ".."));
            string[] candidates = new[]
            {
                Path.Combine(root, "x64", "Debug", "COM_BLAS.dll"),
                Path.Combine(root, "COM_BLAS", "x64", "Debug", "COM_BLAS.dll"),
                Path.Combine(root, "COM_BLAS", "Debug", "COM_BLAS.dll"),
                Path.Combine(root, "x64", "Release", "COM_BLAS.dll"),
                Path.Combine(root, "COM_BLAS", "x64", "Release", "COM_BLAS.dll"),
                Path.Combine(root, "COM_BLAS", "Release", "COM_BLAS.dll"),
            };

            foreach (var candidate in candidates)
            {
                if (File.Exists(candidate))
                {
                    return candidate;
                }
            }

            throw new FileNotFoundException("Could not locate COM_BLAS.dll.", string.Join(Environment.NewLine, candidates));
        }
    }

    internal sealed class BlasHandle : IDisposable
    {
        public IBLAS Instance { get; }

        public BlasHandle()
        {
            Instance = ComBlasFactory.Create();
        }

        public void Dispose()
        {
            if (Instance != null)
            {
                Marshal.ReleaseComObject(Instance);
            }
        }
    }
    [TestClass]
    public sealed class BlasTests
    {
        private const double Tol = 1e-9;
        private const int EInvalidArg = unchecked((int)0x80070057);
        private const int EFail = unchecked((int)0x80004005);
        private const int EPointer = unchecked((int)0x80004003);
        private const int EBounds = unchecked((int)0x8000000B);

        [TestMethod]
        public void GemmSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 2.0 } };
            double[,] B = new double[,] { { 3.0 } };
            double[,] C = new double[,] { { 4.0 } };
            double[,] original = (double[,])C.Clone();

            blas.GemmSimple(A, B, ref C, 1.5, 0.5, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            double expected = 1.5 * 2.0 * 3.0 + 0.5 * original[0, 0];
            AssertScalarEqual(C[0, 0], expected);
        }

        [TestMethod]
        public void SymmSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 5.0 } };
            double[,] B = new double[,] { { 7.0 } };
            double[,] C = new double[,] { { 2.0 } };
            double[,] original = (double[,])C.Clone();

            blas.SymmSimple(A, B, ref C, 1.2, 0.3, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper);

            double expected = 1.2 * 5.0 * 7.0 + 0.3 * original[0, 0];
            AssertScalarEqual(C[0, 0], expected);
        }

        [TestMethod]
        public void SyrkSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 4.0 } };
            double[,] C = new double[,] { { 1.0 } };
            double[,] original = (double[,])C.Clone();

            blas.SyrkSimple(A, ref C, 0.8, 0.2, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            double expected = 0.8 * 4.0 * 4.0 + 0.2 * original[0, 0];
            AssertScalarEqual(C[0, 0], expected);
        }

        [TestMethod]
        public void Syr2kSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 2.0 } };
            double[,] B = new double[,] { { 3.0 } };
            double[,] C = new double[,] { { 0.5 } };
            double[,] original = (double[,])C.Clone();

            blas.Syr2kSimple(A, B, ref C, 1.0, 0.1, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            double expectedUpdate = 1.0 * (2.0 * 3.0 + 3.0 * 2.0);
            double expected = expectedUpdate + 0.1 * original[0, 0];
            AssertScalarEqual(C[0, 0], expected);
        }

        [TestMethod]
        public void TrmmSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 2.0 } };
            double[,] B = new double[,] { { 5.0 } };

            blas.TrmmSimple(A, ref B, 1.5, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            double expected = 1.5 * 2.0 * 5.0;
            AssertScalarEqual(B[0, 0], expected);
        }

        [TestMethod]
        public void TrsmSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 4.0 } };
            double[,] B = new double[,] { { 8.0 } };

            blas.TrsmSimple(A, ref B, 1.0, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            double expected = 8.0 / 4.0;
            AssertScalarEqual(B[0, 0], expected);
        }

        [TestMethod]
        public void GemvSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 3.0 } };
            double[] x = new double[] { 4.0 };
            double[] y = new double[] { 2.0 };

            double original = y[0];
            blas.GemvSimple(A, x, ref y, 1.2, 0.4, BlasLayout.RowMajor, BlasTranspose.NoTrans);

            double expected = 1.2 * 3.0 * 4.0 + 0.4 * original;
            AssertScalarEqual(y[0], expected);
        }

        [TestMethod]
        public void GerSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 2.0 };
            double[] y = new double[] { 3.0 };
            double[,] A = new double[,] { { 0.0 } };

            blas.GerSimple(x, y, ref A, 4.0, BlasLayout.RowMajor);

            double expected = 4.0 * 2.0 * 3.0;
            AssertScalarEqual(A[0, 0], expected);
        }

        [TestMethod]
        public void SymvSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 6.0 } };
            double[] x = new double[] { 5.0 };
            double[] y = new double[] { 1.0 };
            double original = y[0];

            blas.SymvSimple(A, x, ref y, 0.5, 0.3, BlasLayout.RowMajor, BlasUplo.Upper);

            double expected = 0.5 * 6.0 * 5.0 + 0.3 * original;
            AssertScalarEqual(y[0], expected);
        }

        [TestMethod]
        public void SyrSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 3.0 };
            double[,] A = new double[,] { { 0.0 } };

            blas.SyrSimple(x, ref A, 2.0, BlasLayout.RowMajor, BlasUplo.Upper);

            double expected = 2.0 * 3.0 * 3.0;
            AssertScalarEqual(A[0, 0], expected);
        }

        [TestMethod]
        public void Syr2Simple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 2.0 };
            double[] y = new double[] { 5.0 };
            double[,] A = new double[,] { { 1.0 } };

            blas.Syr2Simple(x, y, ref A, 1.0, BlasLayout.RowMajor, BlasUplo.Upper);

            double expected = 1.0 + (2.0 * 5.0 + 5.0 * 2.0);
            AssertScalarEqual(A[0, 0], expected);
        }

        [TestMethod]
        public void TrmvSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 4.0 } };
            double[] x = new double[] { 3.0 };

            blas.TrmvSimple(A, ref x, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            double expected = 4.0 * 3.0;
            AssertScalarEqual(x[0], expected);
        }

        [TestMethod]
        public void TrsvSimple_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 5.0 } };
            double[] x = new double[] { 10.0 };

            blas.TrsvSimple(A, ref x, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            double expected = 10.0 / 5.0;
            AssertScalarEqual(x[0], expected);
        }

        [TestMethod]
        public void Axpy_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 3.0 };
            double[] y = new double[] { 2.0 };

            blas.Axpy(1, 4.0, x, 1, ref y, 1);

            double expected = 2.0 + 4.0 * 3.0;
            AssertScalarEqual(y[0], expected);
        }

        [TestMethod]
        public void Dot_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 2.0 };
            double[] y = new double[] { 5.0 };

            double result = blas.Dot(1, x, 1, y, 1);

            AssertScalarEqual(result, 10.0);
        }

        [TestMethod]
        public void Nrm2_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { -4.0 };

            double result = blas.Nrm2(1, x, 1);
            AssertScalarEqual(result, 4.0);
        }

        [TestMethod]
        public void Asum_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { -7.0 };

            double result = blas.Asum(1, x, 1);
            AssertScalarEqual(result, 7.0);
        }

        [TestMethod]
        public void Scal_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 3.0 };
            blas.Scal(1, -2.0, ref x, 1);
            AssertScalarEqual(x[0], -6.0);
        }

        [TestMethod]
        public void Copy_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 9.0 };
            double[] y = new double[] { 0.0 };
            blas.Copy(1, x, 1, ref y, 1);
            AssertScalarEqual(y[0], 9.0);
        }

        [TestMethod]
        public void Swap_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };
            blas.Swap(1, ref x, 1, ref y, 1);
            AssertScalarEqual(x[0], 2.0);
            AssertScalarEqual(y[0], 1.0);
        }

        [TestMethod]
        public void Iamax_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { -3.0 };
            int index = blas.Iamax(1, x, 1);
            Assert.AreEqual(1, index);
        }

        [TestMethod]
        public void Rot_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 0.0 };
            blas.Rot(1, ref x, 1, ref y, 1, Math.Cos(Math.PI / 2), Math.Sin(Math.PI / 2));
            AssertScalarEqual(x[0], 0.0);
            AssertScalarEqual(y[0], -1.0);
        }

        [TestMethod]
        public void Rotg_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double a = 3.0;
            double b = 4.0;
            (double expectedA, double expectedB, double expectedC, double expectedS) = ComputeRotgExpected(a, b);

            blas.Rotg(ref a, ref b, out double c, out double s);

            AssertScalarEqual(a, expectedA);
            AssertScalarEqual(b, expectedB);
            AssertScalarEqual(c, expectedC);
            AssertScalarEqual(s, expectedS);
        }

        [TestMethod]
        public void Rotm_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };
            double[] param = new double[] { -1.0, 1.0, 0.0, 0.0, 1.0 };

            double[] expectedX = (double[])x.Clone();
            double[] expectedY = (double[])y.Clone();
            ApplyRotm(expectedX, expectedY, param);

            blas.Rotm(1, ref x, 1, ref y, 1, param);

            AssertScalarEqual(x[0], expectedX[0]);
            AssertScalarEqual(y[0], expectedY[0]);
        }

        [TestMethod]
        public void Rotmg_ScalarCase()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double d1 = 4.0;
            double d2 = 9.0;
            double x1 = 2.0;
            double y1 = 3.0;

            blas.Rotmg(ref d1, ref d2, ref x1, y1, out double[] param);

            Assert.AreEqual(5, param.Length);
            Assert.IsTrue(d1 >= 0);
            Assert.IsTrue(d2 >= 0);
            Assert.IsTrue(!double.IsNaN(x1));

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { -1.0 };
            double[] expectedX = (double[])x.Clone();
            double[] expectedY = (double[])y.Clone();
            ApplyRotm(expectedX, expectedY, param);
            blas.Rotm(1, ref x, 1, ref y, 1, param);
            AssertScalarEqual(x[0], expectedX[0]);
            AssertScalarEqual(y[0], expectedY[0]);
        }
        [TestMethod]
        public void GemmSimple_ReturnsInvalidArgWhenOutputSizeMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0 } };
            double[,] B = new double[,] { { 2.0 } };
            double[,] C = new double[,] { { 0.0, 0.0 } };

            AssertComException(EInvalidArg, () => blas.GemmSimple(A, B, ref C, 1.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void SymmSimple_ReturnsInvalidArgWhenAMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[,] B = new double[,] { { 1.0 } };
            double[,] C = new double[,] { { 0.0 } };

            AssertComException(EInvalidArg, () => blas.SymmSimple(A, B, ref C, 1.0, 0.0, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper));
        }

        [TestMethod]
        public void SyrkSimple_ReturnsInvalidArgWhenCNotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0 } };
            double[,] C = new double[,] { { 1.0, 2.0 } };

            AssertComException(EInvalidArg, () => blas.SyrkSimple(A, ref C, 1.0, 0.0, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void Syr2kSimple_ReturnsInvalidArgWhenRowMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0 } };
            double[,] B = new double[,] { { 2.0 }, { 3.0 } };
            double[,] C = new double[,] { { 0.0 } };

            AssertComException(EInvalidArg, () => blas.Syr2kSimple(A, B, ref C, 1.0, 0.0, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void TrmmSimple_ReturnsPointerWhenBNull()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0 } };
            double[,]? B = null;

            AssertComException(EPointer, () => blas.TrmmSimple(A, ref B!, 1.0, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit));
        }

        [TestMethod]
        public void TrmmSimple_ReturnsInvalidArgWhenANotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[,] B = new double[,] { { 1.0 } };

            AssertComException(EInvalidArg, () => blas.TrmmSimple(A, ref B, 1.0, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit));
        }

        [TestMethod]
        public void TrsmSimple_ReturnsInvalidArgWhenANotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[,] B = new double[,] { { 1.0 } };

            AssertComException(EInvalidArg, () => blas.TrsmSimple(A, ref B, 1.0, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit));
        }

        [TestMethod]
        public void GemvSimple_ReturnsBoundsWhenVectorShort()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0 } };
            double[] x = new double[] { 1.0 };
            double[] y = Array.Empty<double>();

            AssertComException(EBounds, () => blas.GemvSimple(A, x, ref y, 1.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void GerSimple_ReturnsBoundsWhenVectorLengthsMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0, 3.0 };
            double[,] A = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };

            AssertComException(EBounds, () => blas.GerSimple(x, y, ref A, 1.0, BlasLayout.RowMajor));
        }

        [TestMethod]
        public void SymvSimple_ReturnsInvalidArgWhenANotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[] x = new double[] { 1.0, 2.0 };
            double[] y = new double[] { 0.0, 0.0 };

            AssertComException(EInvalidArg, () => blas.SymvSimple(A, x, ref y, 1.0, 0.0, BlasLayout.RowMajor, BlasUplo.Upper));
        }

        [TestMethod]
        public void SyrSimple_ReturnsBoundsWhenVectorMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[,] A = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };

            AssertComException(EBounds, () => blas.SyrSimple(x, ref A, 1.0, BlasLayout.RowMajor, BlasUplo.Upper));
        }

        [TestMethod]
        public void Syr2Simple_ReturnsBoundsWhenVectorMismatch()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0, 2.0 };
            double[] y = new double[] { 3.0 };
            double[,] A = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };

            AssertComException(EBounds, () => blas.Syr2Simple(x, y, ref A, 1.0, BlasLayout.RowMajor, BlasUplo.Upper));
        }

        [TestMethod]
        public void TrmvSimple_ReturnsInvalidArgWhenANotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[] x = new double[] { 1.0, 2.0 };

            AssertComException(EInvalidArg, () => blas.TrmvSimple(A, ref x, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit));
        }

        [TestMethod]
        public void TrsvSimple_ReturnsInvalidArgWhenANotSquare()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 1.0, 2.0 } };
            double[] x = new double[] { 1.0, 2.0 };

            AssertComException(EInvalidArg, () => blas.TrsvSimple(A, ref x, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit));
        }

        [TestMethod]
        public void Axpy_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };

            AssertComException(EInvalidArg, () => blas.Axpy(1, 1.0, x, 0, ref y, 1));
        }

        [TestMethod]
        public void Dot_ReturnsInvalidArgWhenIncZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };

            AssertComException(EInvalidArg, () => blas.Dot(1, x, 0, y, 1));
        }

        [TestMethod]
        public void Dot_ReturnsBoundsWhenVectorTooShort()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = Array.Empty<double>();
            double[] y = new double[] { 1.0 };

            AssertComException(EBounds, () => blas.Dot(1, x, 1, y, 1));
        }

        [TestMethod]
        public void Nrm2_ReturnsInvalidArgWhenNegativeN()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = Array.Empty<double>();

            AssertComException(EInvalidArg, () => blas.Nrm2(-1, x, 1));
        }

        [TestMethod]
        public void Asum_ReturnsInvalidArgWhenNegativeN()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = Array.Empty<double>();

            AssertComException(EInvalidArg, () => blas.Asum(-1, x, 1));
        }

        [TestMethod]
        public void Scal_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };

            AssertComException(EInvalidArg, () => blas.Scal(1, 1.0, ref x, 0));
        }

        [TestMethod]
        public void Copy_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 0.0 };

            AssertComException(EInvalidArg, () => blas.Copy(1, x, 0, ref y, 1));
        }

        [TestMethod]
        public void Swap_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };

            AssertComException(EInvalidArg, () => blas.Swap(1, ref x, 0, ref y, 1));
        }

        [TestMethod]
        public void Iamax_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };

            AssertComException(EInvalidArg, () => blas.Iamax(1, x, 0));
        }

        [TestMethod]
        public void Rot_ReturnsInvalidArgWhenIncrementZero()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };

            AssertComException(EInvalidArg, () => blas.Rot(1, ref x, 0, ref y, 1, 1.0, 0.0));
        }

        [TestMethod]
        public void Rotm_ReturnsBoundsWhenParamTooShort()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[] x = new double[] { 1.0 };
            double[] y = new double[] { 2.0 };
            double[] param = new double[] { 0.0, 1.0, 2.0, 3.0 };

            AssertComException(EBounds, () => blas.Rotm(1, ref x, 1, ref y, 1, param));
        }
        private static void AssertScalarEqual(double actual, double expected)
        {
            Assert.AreEqual(expected, actual, Tol);
        }

        private static void AssertComException(int expectedHResult, Action action)
        {
            try
            {
                action();
                Assert.Fail("Expected COM failure was not thrown.");
            }
            catch (Exception ex)
            {
                if (ex is not COMException && ex is not ArgumentException && ex is not NullReferenceException)
                {
                    throw;
                }

                Assert.AreEqual(expectedHResult, ex.HResult);
            }
        }

        private static double[,] Outer(double[] x, double[] y)
        {
            var result = new double[x.Length, y.Length];
            for (int i = 0; i < x.Length; i++)
            {
                for (int j = 0; j < y.Length; j++)
                {
                    result[i, j] = x[i] * y[j];
                }
            }
            return result;
        }

        private static double[,] Multiply(double[,] left, double[,] right)
        {
            int m = left.GetLength(0);
            int k = left.GetLength(1);
            int n = right.GetLength(1);
            var result = new double[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    double sum = 0.0;
                    for (int p = 0; p < k; p++)
                    {
                        sum += left[i, p] * right[p, j];
                    }
                    result[i, j] = sum;
                }
            }
            return result;
        }

        private static double[,] MultiplyScalar(double[,] matrix, double scalar)
        {
            var result = (double[,])matrix.Clone();
            int rows = result.GetLength(0);
            int cols = result.GetLength(1);
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] *= scalar;
                }
            }
            return result;
        }

        private static double[,] CombineMatrices(double alpha, double[,] left, double beta, double[,] right)
        {
            int rows = left.GetLength(0);
            int cols = left.GetLength(1);
            var result = new double[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = alpha * left[i, j] + beta * right[i, j];
                }
            }
            return result;
        }

        private static double[] CombineVectors(double alpha, double[] left, double beta, double[] right)
        {
            var result = new double[left.Length];
            for (int i = 0; i < result.Length; i++)
            {
                result[i] = alpha * left[i] + beta * right[i];
            }
            return result;
        }

        private static double[,] Transpose(double[,] matrix)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            var result = new double[cols, rows];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[j, i] = matrix[i, j];
                }
            }
            return result;
        }

        private static double[] SolveTriangularVector(double[,] matrix, double[] rhs, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            int n = rhs.Length;
            var result = new double[n];
            if (trans == BlasTranspose.NoTrans)
            {
                if (uplo == BlasUplo.Upper)
                {
                    for (int row = n - 1; row >= 0; row--)
                    {
                        double sum = rhs[row];
                        for (int col = row + 1; col < n; col++)
                        {
                            sum -= matrix[row, col] * result[col];
                        }
                        double diagValue = (diag == BlasDiag.Unit) ? 1.0 : matrix[row, row];
                        result[row] = sum / diagValue;
                    }
                }
                else
                {
                    for (int row = 0; row < n; row++)
                    {
                        double sum = rhs[row];
                        for (int col = 0; col < row; col++)
                        {
                            sum -= matrix[row, col] * result[col];
                        }
                        double diagValue = (diag == BlasDiag.Unit) ? 1.0 : matrix[row, row];
                        result[row] = sum / diagValue;
                    }
                }
            }
            else
            {
                var transposed = Transpose(matrix);
                BlasUplo transformedUplo = uplo == BlasUplo.Upper ? BlasUplo.Lower : BlasUplo.Upper;
                result = SolveTriangularVector(transposed, rhs, transformedUplo, BlasTranspose.NoTrans, diag);
            }
            return result;
        }

        private static double[,] SolveTriangularMatrix(double[,] matrix, double[,] rhs, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            int rows = rhs.GetLength(0);
            int cols = rhs.GetLength(1);
            var result = new double[rows, cols];
            for (int col = 0; col < cols; col++)
            {
                var vector = new double[rows];
                for (int row = 0; row < rows; row++)
                {
                    vector[row] = rhs[row, col];
                }
                var solved = SolveTriangularVector(matrix, vector, uplo, trans, diag);
                for (int row = 0; row < rows; row++)
                {
                    result[row, col] = solved[row];
                }
            }
            return result;
        }

        private static double[] TriangularMatrixVector(double[,] matrix, double[] vector, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            int n = vector.Length;
            var result = new double[n];
            for (int row = 0; row < n; row++)
            {
                double sum = 0.0;
                for (int col = 0; col < n; col++)
                {
                    bool use = trans == BlasTranspose.NoTrans
                        ? (uplo == BlasUplo.Upper ? col >= row : col <= row)
                        : (uplo == BlasUplo.Upper ? row >= col : row <= col);
                    if (!use)
                    {
                        continue;
                    }

                    int sourceRow = trans == BlasTranspose.NoTrans ? row : col;
                    int sourceCol = trans == BlasTranspose.NoTrans ? col : row;
                    double entry = matrix[sourceRow, sourceCol];
                    if (diag == BlasDiag.Unit && sourceRow == sourceCol)
                    {
                        entry = 1.0;
                    }
                    sum += entry * vector[col];
                }
                result[row] = sum;
            }
            return result;
        }

        private static void ApplyRotm(double[] x, double[] y, double[] param)
        {
            double flag = param[0];
            double h11 = param[1];
            double h21 = param[2];
            double h12 = param[3];
            double h22 = param[4];

            for (int i = 0; i < x.Length; i++)
            {
                double xi = x[i];
                double yi = y[i];
                if (flag == -2.0)
                {
                }
                else if (flag == -1.0)
                {
                    x[i] = xi * h11 + yi * h12;
                    y[i] = xi * h21 + yi * h22;
                }
                else if (flag == 0.0)
                {
                    x[i] = xi + yi * h12;
                    y[i] = xi * h21 + yi;
                }
                else
                {
                    x[i] = xi * h11 + yi;
                    y[i] = -xi + yi * h22;
                }
            }
        }

        private static (double r, double z, double c, double s) ComputeRotgExpected(double a, double b)
        {
            double roe = Math.Abs(a) > Math.Abs(b) ? a : b;
            double scale = Math.Abs(a) + Math.Abs(b);
            double r, c, s, z;
            if (scale == 0.0)
            {
                c = 1.0;
                s = 0.0;
                r = 0.0;
                z = 0.0;
            }
            else
            {
                double sa = a / scale;
                double sb = b / scale;
                r = scale * Math.Sqrt(sa * sa + sb * sb);
                if (roe < 0.0)
                {
                    r = -r;
                }
                c = a / r;
                s = b / r;
                if (Math.Abs(a) > Math.Abs(b))
                {
                    z = s;
                }
                else if (c != 0.0)
                {
                    z = 1.0 / c;
                }
                else
                {
                    z = 1.0;
                }
            }
            return (r, z, c, s);
        }
    }
}
