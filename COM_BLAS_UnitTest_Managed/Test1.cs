using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using System.Numerics;
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


[ComImport]
[Guid("7795391b-e2f5-4f20-943e-14d2aeb5e8b8")]
[InterfaceType(ComInterfaceType.InterfaceIsDual)]
internal partial interface IBLASComplex
{
    void ZGemmSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasTranspose transA,
        BlasTranspose transB);

    void ZGemvSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasTranspose transA);

    void ZAxpy(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        int incY,
        double alphaReal,
        double alphaImag);

    void ZDot(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yImag,
        int incY,
        out double resultReal,
        out double resultImag,
        [MarshalAs(UnmanagedType.VariantBool)] bool conjugate);

    void ZSymmSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasSide side,
        BlasUplo uplo);

    void ZHemmSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasSide side,
        BlasUplo uplo);

    void ZSyrkSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose transA);

    void ZSyr2kSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose trans);

    void ZHerkSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double betaReal,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose transA);

    void ZHerk2kSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] BImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] CImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose transA);

    void ZTrmmSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] BReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] BImag,
        double alphaReal,
        double alphaImag,
        BlasLayout layout,
        BlasSide side,
        BlasUplo uplo,
        BlasTranspose transA,
        BlasDiag diag);

    void ZTrsmSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] BReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] BImag,
        double alphaReal,
        double alphaImag,
        BlasLayout layout,
        BlasSide side,
        BlasUplo uplo,
        BlasTranspose transA,
        BlasDiag diag);

    void ZGerSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AImag,
        double alphaReal,
        double alphaImag,
        BlasLayout layout,
        [MarshalAs(UnmanagedType.VariantBool)] bool conjugateX);

    void ZHemvSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        double alphaReal,
        double alphaImag,
        double betaReal,
        double betaImag,
        BlasLayout layout,
        BlasUplo uplo);

    void ZHerSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AImag,
        double alphaReal,
        BlasLayout layout,
        BlasUplo uplo);

    void ZHer2Simple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] yImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[,] AImag,
        double alphaReal,
        double alphaImag,
        BlasLayout layout,
        BlasUplo uplo);

    void ZTrmvSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose transA,
        BlasDiag diag);

    void ZTrsvSimple(
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[,] AImag,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        BlasLayout layout,
        BlasUplo uplo,
        BlasTranspose transA,
        BlasDiag diag);

    double ZNrm2(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX);

    double ZAsum(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX);

    void ZScal(
        int n,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        int incX,
        double alphaReal,
        double alphaImag);

    void ZScalReal(
        int n,
        double alphaReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        int incX);

    void ZCopy(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        int incY);

    void ZSwap(
        int n,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        int incX,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        int incY);

    int ZIamax(
        int n,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xReal,
        [MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] double[] xImag,
        int incX);

    void ZRot(
        int n,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] xImag,
        int incX,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yReal,
        [In, Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_R8)] ref double[] yImag,
        int incY,
        double c,
        double s);

    void ZRotg(
        [In, Out] ref double aReal,
        [In, Out] ref double aImag,
        [In, Out] ref double bReal,
        [In, Out] ref double bImag,
        out double c,
        out double sReal,
        out double sImag);

    }

    internal static class ComBlasFactory
    {
        private static readonly string DllPath = ResolveDllPath();
        private static readonly IntPtr ModuleHandle = NativeLibrary.Load(DllPath);
        private static readonly DllGetClassObjectDelegate DllGetClassObject = Marshal.GetDelegateForFunctionPointer<DllGetClassObjectDelegate>(NativeLibrary.GetExport(ModuleHandle, "DllGetClassObject"));
        private static readonly Guid CLSID_BLAS = new Guid("e8f3aed3-eec4-48ab-9925-c13253d4c396");
        private static readonly Guid IID_IClassFactory = new Guid("00000001-0000-0000-C000-000000000046");
        private static readonly Guid IID_IBLAS = typeof(IBLAS).GUID;
        private static readonly Guid IID_IBLASComplex = typeof(IBLASComplex).GUID;

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
            return (IBLAS)CreateInternal(IID_IBLAS);
        }

        internal static IBLASComplex CreateComplex()
        {
            return (IBLASComplex)CreateInternal(IID_IBLASComplex);
        }

        private static object CreateInternal(Guid iid)
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
                    Guid requestedIid = iid;
                    factory.CreateInstance(null, ref requestedIid, out object instance);
                    return instance;
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

    internal sealed class ComplexBlasHandle : IDisposable
    {
        public IBLASComplex Instance { get; }

        public ComplexBlasHandle()
        {
            Instance = ComBlasFactory.CreateComplex();
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
        public void GemmSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[,] B = new double[,]
            {
                { 7.0, 8.0 },
                { 9.0, 10.0 },
                { 11.0, 12.0 },
            };
            double[,] C = new double[,]
            {
                { 0.5, 1.0 },
                { -1.5, 2.5 },
            };
            double[,] original = (double[,])C.Clone();

            double alpha = 1.1;
            double beta = -0.3;

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            int rows = C.GetLength(0);
            int cols = C.GetLength(1);
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    double sum = 0.0;
                    for (int k = 0; k < A.GetLength(1); k++)
                    {
                        sum += A[i, k] * B[k, j];
                    }
                    double expected = alpha * sum + beta * original[i, j];
                    AssertScalarEqual(C[i, j], expected);
                }
            }
        }
        [TestMethod]
        public void GemmSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = CreateMatrix(3, 2, (i, j) => 1.0 + 0.5 * i + 0.25 * j);
            double[,] B = CreateMatrix(2, 4, (i, j) => -1.0 + 0.3 * (i + 2 * j));
            double[,] C = CreateMatrix(3, 4, (i, j) => 0.2 * (i - j));
            double[,] original = (double[,])C.Clone();

            double alpha = 0.75;
            double beta = -0.4;

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.ColumnMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            double[,] expected = ComputeGemmExpected(A, B, original, alpha, beta, BlasTranspose.NoTrans, BlasTranspose.NoTrans);
            AssertMatrixNearlyEqual(expected, C);
        }

        [TestMethod]
        public void GemmSimple_RowMajorWithTransposes()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = CreateMatrix(4, 2, (i, j) => 0.5 * (i + 1) + 0.25 * (j + 1));
            double[,] B = CreateMatrix(4, 3, (i, j) => 0.2 * (i + j + 1));
            double[,] C = CreateMatrix(2, 3, (i, j) => -0.3 + 0.05 * (i * 3 + j));
            double[,] original = (double[,])C.Clone();

            double alpha = 1.25;
            double beta = 0.6;

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.RowMajor, BlasTranspose.Trans, BlasTranspose.NoTrans);

            double[,] expected = ComputeGemmExpected(A, B, original, alpha, beta, BlasTranspose.Trans, BlasTranspose.NoTrans);
            AssertMatrixNearlyEqual(expected, C);
        }

        [TestMethod]
        public void GemmSimple_ColumnMajorWithTransposes()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = CreateMatrix(4, 3, (i, j) => 0.15 * (i + j + 1));
            double[,] B = CreateMatrix(5, 4, (i, j) => 0.05 * (2 * i + j + 1));
            double[,] C = CreateMatrix(3, 5, (i, j) => 0.1 * (i - 2 * j));
            double[,] original = (double[,])C.Clone();

            double alpha = -0.9;
            double beta = 0.3;

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.ColumnMajor, BlasTranspose.Trans, BlasTranspose.Trans);

            double[,] expected = ComputeGemmExpected(A, B, original, alpha, beta, BlasTranspose.Trans, BlasTranspose.Trans);
            AssertMatrixNearlyEqual(expected, C);
        }

        [TestMethod]
        public void GemmSimple_SupportsNonZeroLowerBounds()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            int rowsA = 2;
            int colsA = 3;
            int rowsB = 3;
            int colsB = 2;

            var A = (double[,])Array.CreateInstance(typeof(double), new[] { rowsA, colsA }, new[] { 1, -2 });
            var B = (double[,])Array.CreateInstance(typeof(double), new[] { rowsB, colsB }, new[] { -1, 4 });
            var C = (double[,])Array.CreateInstance(typeof(double), new[] { rowsA, colsB }, new[] { 3, 7 });

            FillMatrix(A, (i, j) => 1.0 + 0.25 * i - 0.1 * j);
            FillMatrix(B, (i, j) => -0.5 + 0.2 * (i + j));
            FillMatrix(C, (i, j) => 0.05 * (i - 2 * j));

            double alpha = 1.3;
            double beta = -0.6;

            var aZero = CloneToZeroBased(A);
            var bZero = CloneToZeroBased(B);
            var originalCZero = CloneToZeroBased(C);

            double[,] expected = ComputeGemmExpected(aZero, bZero, originalCZero, alpha, beta, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            var actual = CloneToZeroBased(C);
            AssertMatrixNearlyEqual(expected, actual);
        }


        [TestMethod]
        public void GemmSimple_AllowsEmptyMatrices()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[0, 3];
            double[,] B = CreateMatrix(3, 2, (i, j) => 0.1 * (i + j + 1));
            double[,] C = new double[0, 2];

            blas.GemmSimple(A, B, ref C, 1.0, 1.0, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            Assert.AreEqual(0, C.Length);
        }

        [TestMethod]
        public void GemmSimple_ColumnMajorScalar()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,] { { 2.0 } };
            double[,] B = new double[,] { { -3.0 } };
            double[,] C = new double[,] { { 1.5 } };
            double[,] original = (double[,])C.Clone();

            double alpha = 2.5;
            double beta = -0.4;

            blas.GemmSimple(A, B, ref C, alpha, beta, BlasLayout.ColumnMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            double expected = alpha * A[0, 0] * B[0, 0] + beta * original[0, 0];
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
        public void SyrkSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[,] C = new double[,]
            {
                { 1.0, 0.5 },
                { 0.5, 2.0 },
            };
            double[,] original = (double[,])C.Clone();

            double alpha = 0.75;
            double beta = 0.25;

            blas.SyrkSimple(A, ref C, alpha, beta, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            int n = C.GetLength(0);
            int k = A.GetLength(1);
            for (int i = 0; i < n; i++)
            {
                for (int j = i; j < n; j++)
                {
                    double sum = 0.0;
                    for (int p = 0; p < k; p++)
                    {
                        sum += A[i, p] * A[j, p];
                    }
                    double expected = alpha * sum + beta * original[i, j];
                    AssertScalarEqual(C[i, j], expected);
                    if (i != j)
                    {
                        AssertScalarEqual(C[j, i], original[j, i]);
                    }
                }
            }
        }

        [TestMethod]
        public void SyrkSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[,] C = new double[,]
            {
                { 1.0, 0.5 },
                { 0.5, 2.0 },
            };
            double[,] original = (double[,])C.Clone();

            double alpha = 0.75;
            double beta = 0.25;

            blas.SyrkSimple(A, ref C, alpha, beta, BlasLayout.ColumnMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            int n = C.GetLength(0);
            int k = A.GetLength(1);
            for (int i = 0; i < n; i++)
            {
                for (int j = i; j < n; j++)
                {
                    double sum = 0.0;
                    for (int p = 0; p < k; p++)
                    {
                        sum += A[i, p] * A[j, p];
                    }
                    double expected = alpha * sum + beta * original[i, j];
                    AssertScalarEqual(C[i, j], expected);
                    if (i != j)
                    {
                        AssertScalarEqual(C[j, i], original[j, i]);
                    }
                }
            }
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
        public void Syr2kSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[,] B = new double[,]
            {
                { -1.0, 0.5, 2.0 },
                { 0.0, 1.5, -0.5 },
            };
            double[,] C = new double[,]
            {
                { 2.0, -1.0 },
                { -1.0, 3.0 },
            };
            double[,] original = (double[,])C.Clone();

            double alpha = 0.6;
            double beta = -0.2;

            blas.Syr2kSimple(A, B, ref C, alpha, beta, BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            int n = C.GetLength(0);
            int k = A.GetLength(1);
            for (int i = 0; i < n; i++)
            {
                for (int j = i; j < n; j++)
                {
                    double sum = 0.0;
                    for (int p = 0; p < k; p++)
                    {
                        sum += A[i, p] * B[j, p] + B[i, p] * A[j, p];
                    }
                    double expected = alpha * sum + beta * original[i, j];
                    AssertScalarEqual(C[i, j], expected);
                    if (i != j)
                    {
                        AssertScalarEqual(C[j, i], original[j, i]);
                    }
                }
            }
        }

        [TestMethod]
        public void Syr2kSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[,] B = new double[,]
            {
                { -1.0, 0.5, 2.0 },
                { 0.0, 1.5, -0.5 },
            };
            double[,] C = new double[,]
            {
                { 2.0, -1.0 },
                { -1.0, 3.0 },
            };
            double[,] original = (double[,])C.Clone();

            double alpha = 0.6;
            double beta = -0.2;

            blas.Syr2kSimple(A, B, ref C, alpha, beta, BlasLayout.ColumnMajor, BlasUplo.Upper, BlasTranspose.NoTrans);

            int n = C.GetLength(0);
            int k = A.GetLength(1);
            for (int i = 0; i < n; i++)
            {
                for (int j = i; j < n; j++)
                {
                    double sum = 0.0;
                    for (int p = 0; p < k; p++)
                    {
                        sum += A[i, p] * B[j, p] + B[i, p] * A[j, p];
                    }
                    double expected = alpha * sum + beta * original[i, j];
                    AssertScalarEqual(C[i, j], expected);
                    if (i != j)
                    {
                        AssertScalarEqual(C[j, i], original[j, i]);
                    }
                }
            }
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
        public void TrmmSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0 },
                { 0.0, 3.0 },
            };
            double[,] B = new double[,]
            {
                { 1.0, 0.0 },
                { 2.0, 1.0 },
            };
            double[,] original = (double[,])B.Clone();
            double alpha = 2.0;

            blas.TrmmSimple(A, ref B, alpha, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            int m = B.GetLength(0);
            int n = B.GetLength(1);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    double sum = 0.0;
                    for (int k = 0; k < m; k++)
                    {
                        sum += A[i, k] * original[k, j];
                    }
                    double expected = alpha * sum;
                    AssertScalarEqual(B[i, j], expected);
                }
            }
        }

        [TestMethod]
        public void TrmmSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0 },
                { 0.0, 3.0 },
            };
            double[,] B = new double[,]
            {
                { 1.0, 0.0 },
                { 2.0, 1.0 },
            };
            double[,] original = (double[,])B.Clone();
            double alpha = 2.0;

            blas.TrmmSimple(A, ref B, alpha, BlasLayout.ColumnMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            int m = B.GetLength(0);
            int n = B.GetLength(1);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    double sum = 0.0;
                    for (int k = 0; k < m; k++)
                    {
                        sum += A[i, k] * original[k, j];
                    }
                    double expected = alpha * sum;
                    AssertScalarEqual(B[i, j], expected);
                }
            }
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
        public void TrsmSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0 },
                { 0.0, 3.0 },
            };
            double[,] B = new double[,]
            {
                { 1.0, 0.0 },
                { 2.0, 1.0 },
            };
            double[,] original = (double[,])B.Clone();
            double alpha = 1.5;

            blas.TrsmSimple(A, ref B, alpha, BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            int m = B.GetLength(0);
            int n = B.GetLength(1);
            double[,] rhs = new double[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    rhs[i, j] = alpha * original[i, j];
                }
            }

            double[,] expected = new double[m, n];
            for (int j = 0; j < n; j++)
            {
                for (int i = m - 1; i >= 0; i--)
                {
                    double sum = rhs[i, j];
                    for (int k = i + 1; k < m; k++)
                    {
                        sum -= A[i, k] * expected[k, j];
                    }
                    expected[i, j] = sum / A[i, i];
                }
            }

            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    AssertScalarEqual(B[i, j], expected[i, j]);
                }
            }
        }

        [TestMethod]
        public void TrsmSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0 },
                { 0.0, 3.0 },
            };
            double[,] B = new double[,]
            {
                { 1.0, 0.0 },
                { 2.0, 1.0 },
            };
            double[,] original = (double[,])B.Clone();
            double alpha = 1.5;

            blas.TrsmSimple(A, ref B, alpha, BlasLayout.ColumnMajor, BlasSide.Left, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            int m = B.GetLength(0);
            int n = B.GetLength(1);
            double[,] rhs = new double[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    rhs[i, j] = alpha * original[i, j];
                }
            }

            double[,] expected = new double[m, n];
            for (int j = 0; j < n; j++)
            {
                for (int i = m - 1; i >= 0; i--)
                {
                    double sum = rhs[i, j];
                    for (int k = i + 1; k < m; k++)
                    {
                        sum -= A[i, k] * expected[k, j];
                    }
                    expected[i, j] = sum / A[i, i];
                }
            }

            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    AssertScalarEqual(B[i, j], expected[i, j]);
                }
            }
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
        public void GemvSimple_RowMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[] x = new double[] { 0.5, -1.0, 2.0 };
            double[] y = new double[] { 1.0, -2.0 };
            double[] original = (double[])y.Clone();
            double alpha = 1.2;
            double beta = 0.4;

            blas.GemvSimple(A, x, ref y, alpha, beta, BlasLayout.RowMajor, BlasTranspose.NoTrans);

            int m = y.Length;
            int n = x.Length;
            for (int i = 0; i < m; i++)
            {
                double sum = 0.0;
                for (int j = 0; j < n; j++)
                {
                    sum += A[i, j] * x[j];
                }
                double expected = alpha * sum + beta * original[i];
                AssertScalarEqual(y[i], expected);
            }
        }

        [TestMethod]
        public void GemvSimple_ColumnMajorRectangular()
        {
            using var handle = new BlasHandle();
            var blas = handle.Instance;

            double[,] A = new double[,]
            {
                { 1.0, 2.0, 3.0 },
                { 4.0, 5.0, 6.0 },
            };
            double[] x = new double[] { 0.5, -1.0, 2.0 };
            double[] y = new double[] { 1.0, -2.0 };
            double[] original = (double[])y.Clone();
            double alpha = 1.2;
            double beta = 0.4;

            blas.GemvSimple(A, x, ref y, alpha, beta, BlasLayout.ColumnMajor, BlasTranspose.NoTrans);

            int m = y.Length;
            int n = x.Length;
            for (int i = 0; i < m; i++)
            {
                double sum = 0.0;
                for (int j = 0; j < n; j++)
                {
                    sum += A[i, j] * x[j];
                }
                double expected = alpha * sum + beta * original[i];
                AssertScalarEqual(y[i], expected);
            }
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


        private static double[,] CloneToZeroBased(double[,] source)
        {
            int rows = source.GetLength(0);
            int cols = source.GetLength(1);
            int rowLower = source.GetLowerBound(0);
            int colLower = source.GetLowerBound(1);
            var result = new double[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = source[rowLower + i, colLower + j];
                }
            }
            return result;
        }

        private static void FillMatrix(double[,] matrix, Func<int, int, double> generator)
        {
            int rowLower = matrix.GetLowerBound(0);
            int rowUpper = matrix.GetUpperBound(0);
            int colLower = matrix.GetLowerBound(1);
            int colUpper = matrix.GetUpperBound(1);
            for (int i = rowLower; i <= rowUpper; i++)
            {
                for (int j = colLower; j <= colUpper; j++)
                {
                    matrix[i, j] = generator(i, j);
                }
            }
        }

        private static double[,] CreateMatrix(int rows, int cols, Func<int, int, double> generator)
        {
            var result = new double[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = generator(i, j);
                }
            }
            return result;
        }

        private static void AssertMatrixNearlyEqual(double[,] expected, double[,] actual)
        {
            Assert.AreEqual(expected.GetLength(0), actual.GetLength(0));
            Assert.AreEqual(expected.GetLength(1), actual.GetLength(1));
            for (int i = 0; i < expected.GetLength(0); i++)
            {
                for (int j = 0; j < expected.GetLength(1); j++)
                {
                    AssertScalarEqual(actual[i, j], expected[i, j]);
                }
            }
        }

        private static double[,] ComputeGemmExpected(double[,] a, double[,] b, double[,] cOriginal, double alpha, double beta, BlasTranspose transA, BlasTranspose transB)
        {
            int m = transA == BlasTranspose.NoTrans ? a.GetLength(0) : a.GetLength(1);
            int kA = transA == BlasTranspose.NoTrans ? a.GetLength(1) : a.GetLength(0);
            int n = transB == BlasTranspose.NoTrans ? b.GetLength(1) : b.GetLength(0);
            int kB = transB == BlasTranspose.NoTrans ? b.GetLength(0) : b.GetLength(1);
            Assert.AreEqual(kA, kB, "Inner dimensions must match for expected GEMM calculation.");
            var result = new double[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    double sum = 0.0;
                    for (int kk = 0; kk < kA; kk++)
                    {
                        double aVal = GetMatrixElement(a, i, kk, transA);
                        double bVal = GetMatrixElement(b, kk, j, transB);
                        sum += aVal * bVal;
                    }
                    result[i, j] = alpha * sum + beta * cOriginal[i, j];
                }
            }
            return result;
        }

        private static double GetMatrixElement(double[,] matrix, int row, int col, BlasTranspose transpose)
        {
            return transpose == BlasTranspose.NoTrans
                ? matrix[row, col]
                : matrix[col, row];
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
    [TestClass]
    public sealed partial class ComplexBlasTests
    {
        private const double Tol = 1e-9;
        private const int EInvalidArg = unchecked((int)0x80070057);
        private const int EPointer = unchecked((int)0x80004003);
        private const int EBounds = unchecked((int)0x8000000B);
        private const int ENotImpl = unchecked((int)0x80004001);

        [TestMethod]
        public void ZGemmSimple_ComputesAlphaABPlusBetaC()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, -2.0 }, { 0.5, 3.0 } };
            double[,] aImag = new double[,] { { 1.5, 0.25 }, { -1.0, 2.0 } };
            double[,] bReal = new double[,] { { -1.0, 4.5 }, { 2.0, 0.0 } };
            double[,] bImag = new double[,] { { 0.5, -3.0 }, { 1.0, 2.5 } };
            double[,] cReal = new double[,] { { 0.5, -1.0 }, { 1.0, 0.2 } };
            double[,] cImag = new double[,] { { -0.75, 1.0 }, { 0.0, -1.5 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.8;
            double alphaImag = -0.6;
            double betaReal = -0.3;
            double betaImag = 0.25;

            blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, alphaReal, alphaImag, betaReal, betaImag, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            Complex[,] expected = ComputeZGemmExpected(aReal, aImag, bReal, bImag, originalCReal, originalCImag, new Complex(alphaReal, alphaImag), new Complex(betaReal, betaImag));
            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZGemvSimple_ComputesAlphaAxPlusBetaY()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 2.0 }, { -1.5, 0.5 } };
            double[,] aImag = new double[,] { { 0.5, -0.25 }, { 1.0, 0.0 } };
            double[] xReal = new double[] { 0.5, -1.0 };
            double[] xImag = new double[] { 2.0, 0.0 };
            double[] yReal = new double[] { 1.0, -0.5 };
            double[] yImag = new double[] { 0.25, 1.5 };
            double[] originalYReal = (double[])yReal.Clone();
            double[] originalYImag = (double[])yImag.Clone();

            double alphaReal = 1.2;
            double alphaImag = -0.3;
            double betaReal = -0.4;
            double betaImag = 0.8;

            blas.ZGemvSimple(aReal, aImag, xReal, xImag, ref yReal, ref yImag, alphaReal, alphaImag, betaReal, betaImag, BlasLayout.RowMajor, BlasTranspose.NoTrans);

            Complex[] expected = ComputeZGemvExpected(aReal, aImag, xReal, xImag, originalYReal, originalYImag, new Complex(alphaReal, alphaImag), new Complex(betaReal, betaImag));
            AssertComplexVectorEqual(expected, yReal, yImag);
        }

        [TestMethod]
        public void ZGemvSimple_ColumnMajorMatchesExpected()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = CreateMatrix(3, 2, (i, j) => 0.2 * (i + 1) - 0.1 * (j + 1));
            double[,] aImag = CreateMatrix(3, 2, (i, j) => 0.05 * (2 * i + j + 1));
            double[] xReal = new double[] { -0.3, 0.45 };
            double[] xImag = new double[] { 0.25, -0.1 };
            double[] yReal = new double[] { 0.4, 0.6, -0.2 };
            double[] yImag = new double[] { -0.2, 0.15, 0.35 };
            double[] originalYReal = (double[])yReal.Clone();
            double[] originalYImag = (double[])yImag.Clone();

            double alphaReal = 1.1;
            double alphaImag = -0.6;
            double betaReal = 0.3;
            double betaImag = 0.5;

            double[] yRealWorking = (double[])yReal.Clone();
            double[] yImagWorking = (double[])yImag.Clone();

            Complex[] expected = ComputeZGemvExpected(aReal, aImag, xReal, xImag, originalYReal, originalYImag, new Complex(alphaReal, alphaImag), new Complex(betaReal, betaImag));

            blas.ZGemvSimple(aReal, aImag, xReal, xImag, ref yRealWorking, ref yImagWorking, alphaReal, alphaImag, betaReal, betaImag, BlasLayout.ColumnMajor, BlasTranspose.NoTrans);

            AssertComplexVectorEqual(expected, yRealWorking, yImagWorking);
        }


        [TestMethod]
        public void ZAxpy_AddsScaledVector()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 3;
            double[] xReal = new double[] { 1.0, -0.5, 2.0, 0.0, -1.0 };
            double[] xImag = new double[] { 0.5, 1.0, -1.5, 0.0, 0.0 };
            int incX = 2;
            double[] yReal = new double[] { -1.0, 0.5, 2.0 };
            double[] yImag = new double[] { 0.25, -0.75, 1.5 };
            double[] originalYReal = (double[])yReal.Clone();
            double[] originalYImag = (double[])yImag.Clone();
            int incY = 1;

            double alphaReal = -0.5;
            double alphaImag = 1.0;

            blas.ZAxpy(n, xReal, xImag, incX, ref yReal, ref yImag, incY, alphaReal, alphaImag);

            Complex[] expected = ComputeZAxpyExpected(n, xReal, xImag, incX, originalYReal, originalYImag, incY, new Complex(alphaReal, alphaImag));
            AssertComplexVectorEqual(expected, yReal, yImag);
        }

        [TestMethod]
        public void ZDot_ComputesConjugatedDot()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 3;
            double[] xReal = new double[] { 1.0, -0.5, 2.0, 0.0, -1.0 };
            double[] xImag = new double[] { 0.5, 1.0, -1.5, 0.0, 0.0 };
            int incX = 2;
            double[] yReal = new double[] { -0.5, 1.0, 0.25 };
            double[] yImag = new double[] { 1.5, -0.75, 0.0 };
            int incY = 1;

            blas.ZDot(n, xReal, xImag, incX, yReal, yImag, incY, out double resultReal, out double resultImag, true);

            Complex expected = ComputeZDotExpected(n, xReal, xImag, incX, yReal, yImag, incY, true);
            AssertNearlyEqual(expected.Real, resultReal);
            AssertNearlyEqual(expected.Imaginary, resultImag);
        }

        [TestMethod]
        public void ZDot_ComputesUnconjugatedDot()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 3;
            double[] xReal = new double[] { 1.0, -0.5, 2.0, 0.0, -1.0 };
            double[] xImag = new double[] { 0.5, 1.0, -1.5, 0.0, 0.0 };
            int incX = 2;
            double[] yReal = new double[] { -0.5, 1.0, 0.25 };
            double[] yImag = new double[] { 1.5, -0.75, 0.0 };
            int incY = 1;

            blas.ZDot(n, xReal, xImag, incX, yReal, yImag, incY, out double resultReal, out double resultImag, false);

            Complex expected = ComputeZDotExpected(n, xReal, xImag, incX, yReal, yImag, incY, false);
            AssertNearlyEqual(expected.Real, resultReal);
            AssertNearlyEqual(expected.Imaginary, resultImag);
        }

        [TestMethod]
        public void ZGemmSimple_RowMajorRectangular()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,]
            {
                { 1.0, -2.0, 0.5 },
                { 0.0, 3.0, -1.0 },
            };
            double[,] aImag = new double[,]
            {
                { 0.5, 1.0, -1.5 },
                { 2.0, -0.5, 0.25 },
            };
            double[,] bReal = new double[,]
            {
                { 2.0, -3.0 },
                { 1.5, 0.75 },
                { -0.5, 2.5 },
            };
            double[,] bImag = new double[,]
            {
                { 1.0, 0.0 },
                { -1.0, 1.5 },
                { 0.5, -2.0 },
            };
            double[,] cReal = new double[,]
            {
                { 0.25, -1.0 },
                { 1.5, 0.5 },
            };
            double[,] cImag = new double[,]
            {
                { 0.0, 0.75 },
                { -0.25, -1.5 },
            };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.6;
            double alphaImag = -0.4;
            double betaReal = -0.3;
            double betaImag = 0.2;

            blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, alphaReal, alphaImag, betaReal, betaImag, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            Complex[,] expected = ComputeZGemmExpected(aReal, aImag, bReal, bImag, originalCReal, originalCImag, new Complex(alphaReal, alphaImag), new Complex(betaReal, betaImag));
            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZGemmSimple_ColumnMajorRectangular()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = CreateMatrix(2, 3, (i, j) => 0.2 * (i + 1) + 0.1 * (j + 1));
            double[,] aImag = CreateMatrix(2, 3, (i, j) => -0.15 * (i + j + 1));
            double[,] bReal = CreateMatrix(3, 2, (i, j) => 0.05 * (i + 2 * j + 1));
            double[,] bImag = CreateMatrix(3, 2, (i, j) => 0.08 * (i - j));
            double[,] cReal = CreateMatrix(2, 2, (i, j) => -0.1 * (i + 1) + 0.05 * j);
            double[,] cImag = CreateMatrix(2, 2, (i, j) => 0.07 * (i - j));
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            Complex alpha = new Complex(0.6, -0.4);
            Complex beta = new Complex(0.3, 0.2);

            blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, alpha.Real, alpha.Imaginary, beta.Real, beta.Imaginary, BlasLayout.ColumnMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans);

            Complex[,] expected = ComputeZGemmExpected(aReal, aImag, bReal, bImag, originalCReal, originalCImag, alpha, beta, BlasTranspose.NoTrans, BlasTranspose.NoTrans);
            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZGemmSimple_RowMajorWithTransposes()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = CreateMatrix(4, 2, (i, j) => 0.12 * (i + 1) - 0.05 * (j + 1));
            double[,] aImag = CreateMatrix(4, 2, (i, j) => 0.07 * (i - j));
            double[,] bReal = CreateMatrix(5, 4, (i, j) => 0.09 * (i + j + 1));
            double[,] bImag = CreateMatrix(5, 4, (i, j) => -0.04 * (i - 2 * j));
            double[,] cReal = CreateMatrix(2, 5, (i, j) => 0.05 * (i + j + 1));
            double[,] cImag = CreateMatrix(2, 5, (i, j) => -0.03 * (i + 2 * j));
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            Complex alpha = new Complex(-0.5, 0.75);
            Complex beta = new Complex(0.4, -0.35);

            blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, alpha.Real, alpha.Imaginary, beta.Real, beta.Imaginary, BlasLayout.RowMajor, BlasTranspose.Trans, BlasTranspose.ConjTrans);

            Complex[,] expected = ComputeZGemmExpected(aReal, aImag, bReal, bImag, originalCReal, originalCImag, alpha, beta, BlasTranspose.Trans, BlasTranspose.ConjTrans);
            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZGemmSimple_ReturnsInvalidArgWhenComplexAHasMismatchedDimensions()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 0.0 }, { 0.0, 1.0 } };
            double[,] aImag = new double[,] { { 0.0, 1.0 } };
            double[,] bReal = new double[,] { { 1.0, 2.0 }, { 3.0, 4.0 } };
            double[,] bImag = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };
            double[,] cReal = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };
            double[,] cImag = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };

            AssertComException(EInvalidArg, () => blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, 1.0, 0.0, 0.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void ZGemmSimple_ReturnsInvalidArgWhenOutputShapeMismatch()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0 } };
            double[,] aImag = new double[,] { { 0.0 } };
            double[,] bReal = new double[,] { { 2.0 } };
            double[,] bImag = new double[,] { { 0.0 } };
            double[,] cReal = new double[,] { { 0.0, 0.0 } };
            double[,] cImag = new double[,] { { 0.0, 0.0 } };

            AssertComException(EInvalidArg, () => blas.ZGemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag, 1.0, 0.0, 0.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void ZGemvSimple_ReturnsInvalidArgWhenMatrixPairMismatch()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 0.0 }, { 0.0, 1.0 } };
            double[,] aImag = new double[,] { { 0.0, 0.0 } };
            double[] xReal = new double[] { 1.0, 0.0 };
            double[] xImag = new double[] { 0.0, 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0, 0.0 };

            AssertComException(EInvalidArg, () => blas.ZGemvSimple(aReal, aImag, xReal, xImag, ref yReal, ref yImag, 1.0, 0.0, 0.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void ZGemvSimple_ReturnsBoundsWhenXPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 0.0 }, { 0.0, 1.0 } };
            double[,] aImag = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };
            double[] xReal = new double[] { 1.0, 0.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0, 0.0 };

            AssertComException(EBounds, () => blas.ZGemvSimple(aReal, aImag, xReal, xImag, ref yReal, ref yImag, 1.0, 0.0, 0.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void ZGemvSimple_ReturnsBoundsWhenYPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 0.0 }, { 0.0, 1.0 } };
            double[,] aImag = new double[,] { { 0.0, 0.0 }, { 0.0, 0.0 } };
            double[] xReal = new double[] { 1.0, 0.0 };
            double[] xImag = new double[] { 0.0, 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EBounds, () => blas.ZGemvSimple(aReal, aImag, xReal, xImag, ref yReal, ref yImag, 1.0, 0.0, 0.0, 0.0, BlasLayout.RowMajor, BlasTranspose.NoTrans));
        }

        [TestMethod]
        public void ZAxpy_ReturnsBoundsWhenXPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 2;
            double[] xReal = new double[] { 1.0, 2.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0, 0.0 };

            AssertComException(EBounds, () => blas.ZAxpy(n, xReal, xImag, 1, ref yReal, ref yImag, 1, 1.0, 0.0));
        }

        [TestMethod]
        public void ZAxpy_ReturnsBoundsWhenYPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 2;
            double[] xReal = new double[] { 1.0, 2.0 };
            double[] xImag = new double[] { 0.0, 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EBounds, () => blas.ZAxpy(n, xReal, xImag, 1, ref yReal, ref yImag, 1, 1.0, 0.0));
        }

        [TestMethod]
        public void ZAxpy_ReturnsInvalidArgWhenIncXZero()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EInvalidArg, () => blas.ZAxpy(1, xReal, xImag, 0, ref yReal, ref yImag, 1, 1.0, 0.0));
        }

        [TestMethod]
        public void ZAxpy_ReturnsInvalidArgWhenIncYZero()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EInvalidArg, () => blas.ZAxpy(1, xReal, xImag, 1, ref yReal, ref yImag, 0, 1.0, 0.0));
        }

        [TestMethod]
        public void ZDot_ReturnsBoundsWhenXPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 2;
            double[] xReal = new double[] { 1.0, 2.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0, 0.0 };

            AssertComException(EBounds, () => blas.ZDot(n, xReal, xImag, 1, yReal, yImag, 1, out double _, out double _, true));
        }

        [TestMethod]
        public void ZDot_ReturnsBoundsWhenYPartsTooShort()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            int n = 2;
            double[] xReal = new double[] { 1.0, 2.0 };
            double[] xImag = new double[] { 0.0, 0.0 };
            double[] yReal = new double[] { 0.0, 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EBounds, () => blas.ZDot(n, xReal, xImag, 1, yReal, yImag, 1, out double _, out double _, true));
        }

        [TestMethod]
        public void ZDot_ReturnsInvalidArgWhenIncXZero()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EInvalidArg, () => blas.ZDot(1, xReal, xImag, 0, yReal, yImag, 1, out double _, out double _, true));
        }

        [TestMethod]
        public void ZDot_ReturnsInvalidArgWhenIncYZero()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0 };
            double[] xImag = new double[] { 0.0 };
            double[] yReal = new double[] { 0.0 };
            double[] yImag = new double[] { 0.0 };

            AssertComException(EInvalidArg, () => blas.ZDot(1, xReal, xImag, 1, yReal, yImag, 0, out double _, out double _, true));
        }

        [TestMethod]
        public void ZDot_ReturnsZeroWhenNZero()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = Array.Empty<double>();
            double[] xImag = Array.Empty<double>();
            double[] yReal = Array.Empty<double>();
            double[] yImag = Array.Empty<double>();

            blas.ZDot(0, xReal, xImag, 1, yReal, yImag, 1, out double resultReal, out double resultImag, true);

            AssertNearlyEqual(0.0, resultReal);
            AssertNearlyEqual(0.0, resultImag);
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

        private static Complex[,] ToComplexMatrix(double[,] real, double[,] imag)
        {
            int rows = real.GetLength(0);
            int cols = real.GetLength(1);
            var result = new Complex[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = new Complex(real[i, j], imag[i, j]);
                }
            }
            return result;
        }

        private static Complex[] ToComplexVector(double[] real, double[] imag)
        {
            var result = new Complex[real.Length];
            for (int i = 0; i < real.Length; i++)
            {
                result[i] = new Complex(real[i], imag[i]);
            }
            return result;
        }
        private static Complex GetComplexElement(Complex[,] matrix, int row, int col, BlasTranspose transpose)
        {
            return transpose switch
            {
                BlasTranspose.NoTrans => matrix[row, col],
                BlasTranspose.Trans => matrix[col, row],
                BlasTranspose.ConjTrans => Complex.Conjugate(matrix[col, row]),
                _ => matrix[row, col],
            };
        }

        private static double[,] CreateMatrix(int rows, int cols, Func<int, int, double> generator)
        {
            var result = new double[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = generator(i, j);
                }
            }
            return result;
        }

        private static Complex[,] ComputeZGemmExpected(double[,] aReal, double[,] aImag, double[,] bReal, double[,] bImag, double[,] cReal, double[,] cImag, Complex alpha, Complex beta, BlasTranspose transA = BlasTranspose.NoTrans, BlasTranspose transB = BlasTranspose.NoTrans)
        {
            var a = ToComplexMatrix(aReal, aImag);
            var b = ToComplexMatrix(bReal, bImag);
            var c = ToComplexMatrix(cReal, cImag);
            int m = transA == BlasTranspose.NoTrans ? a.GetLength(0) : a.GetLength(1);
            int kA = transA == BlasTranspose.NoTrans ? a.GetLength(1) : a.GetLength(0);
            int n = transB == BlasTranspose.NoTrans ? b.GetLength(1) : b.GetLength(0);
            int kB = transB == BlasTranspose.NoTrans ? b.GetLength(0) : b.GetLength(1);
            Assert.AreEqual(kA, kB, "Inner dimensions must match for expected ZGEMM calculation.");
            var result = new Complex[m, n];
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    Complex sum = Complex.Zero;
                    for (int kk = 0; kk < kA; kk++)
                    {
                        Complex aVal = GetComplexElement(a, i, kk, transA);
                        Complex bVal = GetComplexElement(b, kk, j, transB);
                        sum += aVal * bVal;
                    }
                    Complex cVal = c[i, j];
                    result[i, j] = alpha * sum + beta * cVal;
                }
            }
            return result;
        }

        private static Complex[] ComputeZGemvExpected(double[,] aReal, double[,] aImag, double[] xReal, double[] xImag, double[] yReal, double[] yImag, Complex alpha, Complex beta)
        {
            var a = ToComplexMatrix(aReal, aImag);
            var x = ToComplexVector(xReal, xImag);
            var y = ToComplexVector(yReal, yImag);
            int rows = a.GetLength(0);
            int cols = a.GetLength(1);
            var result = new Complex[rows];
            for (int i = 0; i < rows; i++)
            {
                Complex sum = Complex.Zero;
                for (int j = 0; j < cols; j++)
                {
                    sum += a[i, j] * x[j];
                }
                result[i] = alpha * sum + beta * y[i];
            }
            return result;
        }

        private static Complex[] ComputeZAxpyExpected(int n, double[] xReal, double[] xImag, int incX, double[] yReal, double[] yImag, int incY, Complex alpha)
        {
            var x = ToComplexVector(xReal, xImag);
            var y = ToComplexVector(yReal, yImag);
            var result = new Complex[y.Length];
            Array.Copy(y, result, y.Length);
            for (int i = 0; i < n; i++)
            {
                int xi = i * incX;
                int yi = i * incY;
                result[yi] = y[yi] + alpha * x[xi];
            }
            return result;
        }

        private static Complex ComputeZDotExpected(int n, double[] xReal, double[] xImag, int incX, double[] yReal, double[] yImag, int incY, bool conjugate)
        {
            var x = ToComplexVector(xReal, xImag);
            var y = ToComplexVector(yReal, yImag);
            Complex sum = Complex.Zero;
            for (int i = 0; i < n; i++)
            {
                Complex xi = x[i * incX];
                if (conjugate)
                {
                    xi = Complex.Conjugate(xi);
                }
                sum += xi * y[i * incY];
            }
            return sum;
        }

        private static void AssertComplexMatrixEqual(Complex[,] expected, double[,] actualReal, double[,] actualImag)
        {
            int rows = expected.GetLength(0);
            int cols = expected.GetLength(1);
            Assert.AreEqual(rows, actualReal.GetLength(0));
            Assert.AreEqual(cols, actualReal.GetLength(1));
            Assert.AreEqual(rows, actualImag.GetLength(0));
            Assert.AreEqual(cols, actualImag.GetLength(1));
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    AssertNearlyEqual(expected[i, j].Real, actualReal[i, j]);
                    AssertNearlyEqual(expected[i, j].Imaginary, actualImag[i, j]);
                }
            }
        }

        private static void AssertComplexVectorEqual(Complex[] expected, double[] actualReal, double[] actualImag)
        {
            Assert.AreEqual(expected.Length, actualReal.Length);
            Assert.AreEqual(expected.Length, actualImag.Length);
            for (int i = 0; i < expected.Length; i++)
            {
                AssertNearlyEqual(expected[i].Real, actualReal[i]);
                AssertNearlyEqual(expected[i].Imaginary, actualImag[i]);
            }
        }

        private static void AssertNearlyEqual(double expected, double actual)
        {
            Assert.AreEqual(expected, actual, Tol);
        }
    }




}
