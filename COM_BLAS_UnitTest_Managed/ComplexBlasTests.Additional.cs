using System;
using System.Numerics;
using System.Runtime.InteropServices;
using Ckt.Com.Blas;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace COM_BLAS_UnitTest_Managed
{
    public sealed partial class ComplexBlasTests
    {
        [TestMethod]
        public void ZSymmSimple_ComputesAlphaABPlusBetaC_LeftSide()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 2.0, -1.5 }, { -1.5, 3.0 } };
            double[,] aImag = new double[,] { { 0.5, 0.75 }, { 0.75, -0.25 } };
            double[,] bReal = new double[,] { { 1.0, -0.5 }, { 2.0, 0.75 } };
            double[,] bImag = new double[,] { { -0.25, 1.25 }, { 0.5, -1.5 } };
            double[,] cReal = new double[,] { { 0.25, 1.0 }, { -0.5, 0.75 } };
            double[,] cImag = new double[,] { { -0.10, 0.40 }, { 0.30, -0.60 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.8;
            double alphaImag = -0.4;
            double betaReal = -0.3;
            double betaImag = 0.2;

            ExecuteOrInconclusive("ZSymmSimple", () =>
                blas.ZSymmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag,
                                 alphaReal, alphaImag, betaReal, betaImag,
                                 BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper));

            Complex[,] expected = CombineComplexMatrices(
                new Complex(alphaReal, alphaImag),
                MatrixMultiply(ToComplexMatrix(aReal, aImag), ToComplexMatrix(bReal, bImag)),
                new Complex(betaReal, betaImag),
                ToComplexMatrix(originalCReal, originalCImag));

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZHemmSimple_ComputesAlphaABPlusBetaC_LeftSide()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 3.0, -1.2 }, { -1.2, 1.5 } };
            double[,] aImag = new double[,] { { 0.0, 0.9 }, { -0.9, 0.0 } };
            double[,] bReal = new double[,] { { 1.0, 2.0 }, { -0.5, 1.5 } };
            double[,] bImag = new double[,] { { -0.25, 0.75 }, { 1.0, -1.25 } };
            double[,] cReal = new double[,] { { -0.2, 0.5 }, { 0.6, -1.1 } };
            double[,] cImag = new double[,] { { 0.3, -0.8 }, { 0.2, 0.9 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 1.1;
            double alphaImag = 0.3;
            double betaReal = -0.6;
            double betaImag = 0.5;

            ExecuteOrInconclusive("ZHemmSimple", () =>
                blas.ZHemmSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag,
                                 alphaReal, alphaImag, betaReal, betaImag,
                                 BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper));

            Complex[,] a = MakeHermitian(ToComplexMatrix(aReal, aImag));
            Complex[,] expected = CombineComplexMatrices(
                new Complex(alphaReal, alphaImag),
                MatrixMultiply(a, ToComplexMatrix(bReal, bImag)),
                new Complex(betaReal, betaImag),
                ToComplexMatrix(originalCReal, originalCImag));

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }
        [TestMethod]
        public void ZSyrkSimple_PerformsRankKUpdate()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 0.5, -1.0, 1.5 }, { 1.0, 0.25, -0.75 } };
            double[,] aImag = new double[,] { { 1.2, -0.2, 0.4 }, { -0.5, 0.8, 0.6 } };
            double[,] cReal = new double[,] { { 0.75, -0.5 }, { -0.5, 1.2 } };
            double[,] cImag = new double[,] { { 0.0, 0.35 }, { -0.35, -0.1 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.7;
            double alphaImag = -0.3;
            double betaReal = -0.5;
            double betaImag = 0.2;

            ExecuteOrInconclusive("ZSyrkSimple", () =>
                blas.ZSyrkSimple(aReal, aImag, ref cReal, ref cImag,
                                 alphaReal, alphaImag, betaReal, betaImag,
                                 BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));

            Complex[,] opA = ApplyTranspose(ToComplexMatrix(aReal, aImag), BlasTranspose.NoTrans);
            Complex[,] product = MatrixMultiply(opA, TransposeMatrix(opA));
            Complex[,] expected = CombineComplexMatrices(
                new Complex(alphaReal, alphaImag),
                product,
                new Complex(betaReal, betaImag),
                ToComplexMatrix(originalCReal, originalCImag));
            expected = MakeSymmetric(expected);

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZSyr2kSimple_PerformsRank2kUpdate()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 0.5, -1.0, 1.5 }, { 1.0, 0.25, -0.75 } };
            double[,] aImag = new double[,] { { 1.2, -0.2, 0.4 }, { -0.5, 0.8, 0.6 } };
            double[,] bReal = new double[,] { { -0.25, 0.9, 1.1 }, { 0.6, -1.3, 0.4 } };
            double[,] bImag = new double[,] { { 0.2, 0.5, -0.8 }, { -0.7, 0.1, 1.25 } };
            double[,] cReal = new double[,] { { 0.75, -0.5 }, { -0.5, 1.2 } };
            double[,] cImag = new double[,] { { 0.0, 0.35 }, { -0.35, -0.1 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 1.0;
            double alphaImag = 0.5;
            double betaReal = -0.2;
            double betaImag = 0.1;

            ExecuteOrInconclusive("ZSyr2kSimple", () =>
                blas.ZSyr2kSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag,
                                   alphaReal, alphaImag, betaReal, betaImag,
                                   BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));

            Complex[,] opA = ApplyTranspose(ToComplexMatrix(aReal, aImag), BlasTranspose.NoTrans);
            Complex[,] opB = ApplyTranspose(ToComplexMatrix(bReal, bImag), BlasTranspose.NoTrans);
            Complex alpha = new Complex(alphaReal, alphaImag);
            Complex[,] term1 = MatrixMultiply(opA, TransposeMatrix(opB));
            Complex[,] term2 = MatrixMultiply(opB, TransposeMatrix(opA));
            Complex[,] combined = CombineComplexMatrices(alpha, term1, Complex.Conjugate(alpha), term2);
            Complex[,] expected = CombineComplexMatrices(Complex.One, combined, new Complex(betaReal, betaImag), ToComplexMatrix(originalCReal, originalCImag));
            expected = MakeSymmetric(expected);

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }
        [TestMethod]
        public void ZHerkSimple_PerformsHermitianRankKUpdate()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 0.8, -1.2, 0.5 }, { 1.1, 0.4, -0.6 } };
            double[,] aImag = new double[,] { { -0.3, 0.9, 0.2 }, { 0.7, -0.5, 1.0 } };
            double[,] cReal = new double[,] { { 1.0, -0.6 }, { -0.6, 0.75 } };
            double[,] cImag = new double[,] { { 0.0, 0.45 }, { -0.45, 0.0 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.8;
            double betaReal = -0.3;

            ExecuteOrInconclusive("ZHerkSimple", () =>
                blas.ZHerkSimple(aReal, aImag, ref cReal, ref cImag,
                                  alphaReal, betaReal,
                                  BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));

            Complex[,] opA = ApplyTranspose(ToComplexMatrix(aReal, aImag), BlasTranspose.NoTrans);
            Complex[,] product = MatrixMultiply(opA, ConjugateTransposeMatrix(opA));
            Complex[,] expected = CombineComplexMatrices(new Complex(alphaReal, 0.0), product, new Complex(betaReal, 0.0), ToComplexMatrix(originalCReal, originalCImag));
            expected = MakeHermitian(expected);

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }

        [TestMethod]
        public void ZHerk2kSimple_PerformsHermitianRank2kUpdate()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 0.2, -1.0, 0.75 }, { 1.3, 0.5, -0.4 } };
            double[,] aImag = new double[,] { { 0.6, -0.7, 0.3 }, { -0.2, 0.9, 0.8 } };
            double[,] bReal = new double[,] { { -0.5, 0.4, 1.2 }, { 0.3, -1.1, 0.6 } };
            double[,] bImag = new double[,] { { 0.9, -0.3, 0.1 }, { -0.6, 0.2, -0.8 } };
            double[,] cReal = new double[,] { { 0.9, -0.35 }, { -0.35, 1.1 } };
            double[,] cImag = new double[,] { { 0.0, 0.2 }, { -0.2, 0.0 } };
            double[,] originalCReal = (double[,])cReal.Clone();
            double[,] originalCImag = (double[,])cImag.Clone();

            double alphaReal = 0.6;
            double alphaImag = -0.45;
            double betaReal = -0.4;

            ExecuteOrInconclusive("ZHerk2kSimple", () =>
                blas.ZHerk2kSimple(aReal, aImag, bReal, bImag, ref cReal, ref cImag,
                                    alphaReal, alphaImag, betaReal,
                                    BlasLayout.RowMajor, BlasUplo.Upper, BlasTranspose.NoTrans));

            Complex[,] opA = ApplyTranspose(ToComplexMatrix(aReal, aImag), BlasTranspose.NoTrans);
            Complex[,] opB = ApplyTranspose(ToComplexMatrix(bReal, bImag), BlasTranspose.NoTrans);
            Complex alpha = new Complex(alphaReal, alphaImag);
            Complex[,] term1 = MatrixMultiply(opA, ConjugateTransposeMatrix(opB));
            Complex[,] term2 = MatrixMultiply(opB, ConjugateTransposeMatrix(opA));
            Complex[,] combined = CombineComplexMatrices(alpha, term1, Complex.Conjugate(alpha), term2);
            Complex[,] expected = CombineComplexMatrices(Complex.One, combined, new Complex(betaReal, 0.0), ToComplexMatrix(originalCReal, originalCImag));
            expected = MakeHermitian(expected);

            AssertComplexMatrixEqual(expected, cReal, cImag);
        }
        [TestMethod]
        public void ZTrmmSimple_LeftSideTriangularMultiply()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, -0.5, 0.25 }, { 0.0, 1.5, -0.8 }, { 0.0, 0.0, 0.75 } };
            double[,] aImag = new double[,] { { 0.0, 0.4, -0.6 }, { 0.0, 0.0, 0.5 }, { 0.0, 0.0, 0.0 } };
            double[,] bReal = new double[,] { { 0.5, -1.0 }, { 1.2, 0.3 }, { -0.4, 0.8 } };
            double[,] bImag = new double[,] { { 0.2, 0.9 }, { -0.6, 0.1 }, { 0.7, -0.3 } };
            double[,] originalBReal = (double[,])bReal.Clone();
            double[,] originalBImag = (double[,])bImag.Clone();

            double alphaReal = 0.7;
            double alphaImag = 0.2;

            ExecuteOrInconclusive("ZTrmmSimple", () =>
                blas.ZTrmmSimple(aReal, aImag, ref bReal, ref bImag,
                                  alphaReal, alphaImag,
                                  BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper,
                                  BlasTranspose.NoTrans, BlasDiag.NonUnit));

            Complex[,] opA = PrepareTriangularMatrix(ToComplexMatrix(aReal, aImag), BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);
            Complex[,] expected = ScaleMatrix(MatrixMultiply(opA, ToComplexMatrix(originalBReal, originalBImag)), new Complex(alphaReal, alphaImag));

            AssertComplexMatrixEqual(expected, bReal, bImag);
        }

        [TestMethod]
        public void ZTrsmSimple_SolvesTriangularSystem()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 2.0, -0.5, 0.25 }, { 0.0, 1.5, 0.75 }, { 0.0, 0.0, -1.2 } };
            double[,] aImag = new double[,] { { 0.0, 0.6, -0.4 }, { 0.0, 0.0, 0.5 }, { 0.0, 0.0, 0.0 } };
            double[,] bReal = new double[,] { { 1.0, -0.5 }, { 0.8, 1.2 }, { -0.6, 0.9 } };
            double[,] bImag = new double[,] { { -0.3, 0.4 }, { 0.5, -0.2 }, { 1.1, 0.7 } };
            double[,] originalBReal = (double[,])bReal.Clone();
            double[,] originalBImag = (double[,])bImag.Clone();

            double alphaReal = -0.4;
            double alphaImag = 0.9;

            ExecuteOrInconclusive("ZTrsmSimple", () =>
                blas.ZTrsmSimple(aReal, aImag, ref bReal, ref bImag,
                                  alphaReal, alphaImag,
                                  BlasLayout.RowMajor, BlasSide.Left, BlasUplo.Upper,
                                  BlasTranspose.NoTrans, BlasDiag.NonUnit));

            Complex[,] opA = PrepareTriangularMatrix(ToComplexMatrix(aReal, aImag), BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);
            Complex[,] scaledB = ScaleMatrix(ToComplexMatrix(originalBReal, originalBImag), new Complex(alphaReal, alphaImag));
            Complex[,] expected = SolveTriangularMatrix(opA, scaledB, BlasUplo.Upper, BlasTranspose.NoTrans, BlasDiag.NonUnit);

            AssertComplexMatrixEqual(expected, bReal, bImag);
        }
        [TestMethod]
        public void ZGerSimple_PerformsRank1Update()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.5, -1.0, 1.5 };
            double[] xImag = new double[] { 1.2, 0.0, -0.7 };
            double[] yReal = new double[] { -0.25, 0.8 };
            double[] yImag = new double[] { 0.6, -0.4 };
            double[,] aReal = new double[,] { { 0.3, -0.2 }, { 0.1, 0.5 }, { -0.7, 0.9 } };
            double[,] aImag = new double[,] { { 0.4, 0.3 }, { -0.5, 0.2 }, { 0.6, -0.8 } };
            double[,] originalAReal = (double[,])aReal.Clone();
            double[,] originalAImag = (double[,])aImag.Clone();

            double alphaReal = 0.75;
            double alphaImag = -0.5;

            ExecuteOrInconclusive("ZGerSimple", () =>
                blas.ZGerSimple(xReal, xImag, yReal, yImag, ref aReal, ref aImag,
                                 alphaReal, alphaImag, BlasLayout.RowMajor, conjugateX: false));

            Complex[,] update = OuterProduct(ToComplexVector(xReal, xImag), ToComplexVector(yReal, yImag), conjugateY: false);
            Complex[,] expected = CombineComplexMatrices(new Complex(alphaReal, alphaImag), update, Complex.One, ToComplexMatrix(originalAReal, originalAImag));

            AssertComplexMatrixEqual(expected, aReal, aImag);
        }

        [TestMethod]
        public void ZHemvSimple_ComputesAlphaAxPlusBetaY()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 2.0, -0.5, 0.75 }, { -0.5, 1.5, -1.2 }, { 0.75, -1.2, 3.0 } };
            double[,] aImag = new double[,] { { 0.0, 1.0, -0.6 }, { -1.0, 0.0, 0.2 }, { 0.6, -0.2, 0.0 } };
            double[] xReal = new double[] { 0.4, -1.1, 0.9 };
            double[] xImag = new double[] { 1.0, 0.3, -0.7 };
            double[] yReal = new double[] { -0.6, 0.8, 1.2 };
            double[] yImag = new double[] { 0.5, -0.9, 0.4 };
            double[] originalYReal = (double[])yReal.Clone();
            double[] originalYImag = (double[])yImag.Clone();

            double alphaReal = 0.9;
            double alphaImag = 0.25;
            double betaReal = -0.3;
            double betaImag = 0.5;

            ExecuteOrInconclusive("ZHemvSimple", () =>
                blas.ZHemvSimple(aReal, aImag, xReal, xImag, ref yReal, ref yImag,
                                  alphaReal, alphaImag, betaReal, betaImag,
                                  BlasLayout.RowMajor, BlasUplo.Upper));

            Complex[,] a = MakeHermitian(ToComplexMatrix(aReal, aImag));
            Complex[] expected = CombineComplexVectors(new Complex(alphaReal, alphaImag),
                MatrixVectorMultiply(a, ToComplexVector(xReal, xImag)),
                new Complex(betaReal, betaImag),
                ToComplexVector(originalYReal, originalYImag));

            AssertComplexVectorEqual(expected, yReal, yImag);
        }

        [TestMethod]
        public void ZHerSimple_PerformsHermitianRank1Update()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.5, -1.2, 0.8 };
            double[] xImag = new double[] { -0.4, 0.9, 0.6 };
            double[,] aReal = new double[,] { { 1.5, -0.4, 0.6 }, { -0.4, 2.0, -1.1 }, { 0.6, -1.1, 0.8 } };
            double[,] aImag = new double[,] { { 0.0, 0.7, -0.3 }, { -0.7, 0.0, 0.9 }, { 0.3, -0.9, 0.0 } };
            double[,] originalAReal = (double[,])aReal.Clone();
            double[,] originalAImag = (double[,])aImag.Clone();

            double alphaReal = 1.1;

            ExecuteOrInconclusive("ZHerSimple", () =>
                blas.ZHerSimple(xReal, xImag, ref aReal, ref aImag,
                                alphaReal, BlasLayout.RowMajor, BlasUplo.Upper));

            Complex[,] update = OuterProduct(ToComplexVector(xReal, xImag), ToComplexVector(xReal, xImag), conjugateY: true);
            Complex[,] expected = CombineComplexMatrices(new Complex(alphaReal, 0.0), update, Complex.One, ToComplexMatrix(originalAReal, originalAImag));
            expected = MakeHermitian(expected);

            AssertComplexMatrixEqual(expected, aReal, aImag);
        }

        [TestMethod]
        public void ZHer2Simple_PerformsHermitianRank2Update()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.3, -0.8, 1.4 };
            double[] xImag = new double[] { -0.6, 0.2, 0.5 };
            double[] yReal = new double[] { 1.0, -0.3, 0.7 };
            double[] yImag = new double[] { 0.8, 0.4, -1.0 };
            double[,] aReal = new double[,] { { 0.7, -0.2, 0.9 }, { -0.2, 1.6, -0.5 }, { 0.9, -0.5, 1.1 } };
            double[,] aImag = new double[,] { { 0.0, 0.6, -0.4 }, { -0.6, 0.0, 0.8 }, { 0.4, -0.8, 0.0 } };
            double[,] originalAReal = (double[,])aReal.Clone();
            double[,] originalAImag = (double[,])aImag.Clone();

            double alphaReal = 0.6;
            double alphaImag = -0.45;

            ExecuteOrInconclusive("ZHer2Simple", () =>
                blas.ZHer2Simple(xReal, xImag, yReal, yImag, ref aReal, ref aImag,
                                 alphaReal, alphaImag,
                                 BlasLayout.RowMajor, BlasUplo.Upper));

            Complex alpha = new Complex(alphaReal, alphaImag);
            Complex[] x = ToComplexVector(xReal, xImag);
            Complex[] y = ToComplexVector(yReal, yImag);
            Complex[,] term1 = OuterProduct(x, y, conjugateY: true);
            Complex[,] term2 = OuterProduct(y, x, conjugateY: true);
            Complex[,] update = CombineComplexMatrices(alpha, term1, Complex.Conjugate(alpha), term2);
            Complex[,] expected = CombineComplexMatrices(Complex.One, update, Complex.One, ToComplexMatrix(originalAReal, originalAImag));
            expected = MakeHermitian(expected);

            AssertComplexMatrixEqual(expected, aReal, aImag);
        }
        [TestMethod]
        public void ZTrmvSimple_AppliesTriangularMatrixToVector()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 1.0, 0.2, -0.4 }, { 0.0, 1.5, 0.8 }, { 0.0, 0.0, -0.9 } };
            double[,] aImag = new double[,] { { 0.0, -0.5, 0.3 }, { 0.0, 0.0, 0.6 }, { 0.0, 0.0, 0.0 } };
            double[] xReal = new double[] { 0.7, -1.1, 0.5 };
            double[] xImag = new double[] { -0.2, 0.9, -0.6 };
            double[] originalXReal = (double[])xReal.Clone();
            double[] originalXImag = (double[])xImag.Clone();

            ExecuteOrInconclusive("ZTrmvSimple", () =>
                blas.ZTrmvSimple(aReal, aImag, ref xReal, ref xImag,
                                 BlasLayout.RowMajor, BlasUplo.Upper,
                                 BlasTranspose.NoTrans, BlasDiag.NonUnit));

            Complex[] expected = ApplyTriangularMatrixVector(
                ToComplexMatrix(aReal, aImag),
                ToComplexVector(originalXReal, originalXImag),
                BlasUplo.Upper,
                BlasTranspose.NoTrans,
                BlasDiag.NonUnit);

            AssertComplexVectorEqual(expected, xReal, xImag);
        }

        [TestMethod]
        public void ZTrsvSimple_SolvesTriangularSystemForVector()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[,] aReal = new double[,] { { 2.0, -0.5, 0.25 }, { 0.0, 1.5, 0.75 }, { 0.0, 0.0, -1.2 } };
            double[,] aImag = new double[,] { { 0.0, 0.6, -0.4 }, { 0.0, 0.0, 0.5 }, { 0.0, 0.0, 0.0 } };
            double[] xReal = new double[] { 0.9, -0.4, 1.2 };
            double[] xImag = new double[] { 0.5, 1.1, -0.3 };
            double[] originalXReal = (double[])xReal.Clone();
            double[] originalXImag = (double[])xImag.Clone();

            ExecuteOrInconclusive("ZTrsvSimple", () =>
                blas.ZTrsvSimple(aReal, aImag, ref xReal, ref xImag,
                                 BlasLayout.RowMajor, BlasUplo.Upper,
                                 BlasTranspose.NoTrans, BlasDiag.NonUnit));

            Complex[] expected = SolveTriangularVector(
                ToComplexMatrix(aReal, aImag),
                ToComplexVector(originalXReal, originalXImag),
                BlasUplo.Upper,
                BlasTranspose.NoTrans,
                BlasDiag.NonUnit);

            AssertComplexVectorEqual(expected, xReal, xImag);
        }
        [TestMethod]
        public void ZNrm2_ComputesEuclideanNorm()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.8, -1.2, 0.4, -0.9 };
            double[] xImag = new double[] { -0.5, 0.6, 1.3, -0.7 };

            ExecuteOrInconclusive("ZNrm2", () =>
            {
                double result = blas.ZNrm2(4, xReal, xImag, 1);
                double expected = ComplexNorm2(ToComplexVector(xReal, xImag), 4, 1);
                AssertNearlyEqual(expected, result);
            });
        }

        [TestMethod]
        public void ZAsum_ComputesSumOfAbsoluteValues()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.8, -1.2, 0.4, -0.9 };
            double[] xImag = new double[] { -0.5, 0.6, 1.3, -0.7 };

            ExecuteOrInconclusive("ZAsum", () =>
            {
                double result = blas.ZAsum(4, xReal, xImag, 1);
                double expected = ComplexAsum(ToComplexVector(xReal, xImag), 4, 1);
                AssertNearlyEqual(expected, result);
            });
        }

        [TestMethod]
        public void ZScal_ScalesVectorByComplexAlpha()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.8, -1.2, 0.4, -0.9 };
            double[] xImag = new double[] { -0.5, 0.6, 1.3, -0.7 };
            double[] originalXReal = (double[])xReal.Clone();
            double[] originalXImag = (double[])xImag.Clone();

            double alphaReal = 1.1;
            double alphaImag = -0.6;

            ExecuteOrInconclusive("ZScal", () =>
                blas.ZScal(4, ref xReal, ref xImag, 1, alphaReal, alphaImag));

            Complex[] expected = ScaleVector(ToComplexVector(originalXReal, originalXImag), new Complex(alphaReal, alphaImag));

            AssertComplexVectorEqual(expected, xReal, xImag);
        }

        [TestMethod]
        public void ZScalReal_ScalesVectorByRealAlpha()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.3, -0.8, 1.2 };
            double[] xImag = new double[] { -0.6, 0.5, 0.9 };
            double[] originalXReal = (double[])xReal.Clone();
            double[] originalXImag = (double[])xImag.Clone();

            double alphaReal = -1.5;

            ExecuteOrInconclusive("ZScalReal", () =>
                blas.ZScalReal(3, alphaReal, ref xReal, ref xImag, 1));

            Complex[] expected = ScaleVector(ToComplexVector(originalXReal, originalXImag), new Complex(alphaReal, 0.0));

            AssertComplexVectorEqual(expected, xReal, xImag);
        }
        [TestMethod]
        public void ZCopy_CopiesVector()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0, -0.5, 0.75 };
            double[] xImag = new double[] { -0.25, 0.6, 1.2 };
            double[] yReal = new double[] { 0.0, 0.0, 0.0 };
            double[] yImag = new double[] { 0.0, 0.0, 0.0 };

            ExecuteOrInconclusive("ZCopy", () =>
                blas.ZCopy(3, xReal, xImag, 1, ref yReal, ref yImag, 1));

            AssertComplexVectorEqual(ToComplexVector(xReal, xImag), yReal, yImag);
        }

        [TestMethod]
        public void ZSwap_ExchangesVectors()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 1.0, -0.5, 0.75 };
            double[] xImag = new double[] { -0.25, 0.6, 1.2 };
            double[] yReal = new double[] { -1.5, 0.8, 0.4 };
            double[] yImag = new double[] { 0.9, -0.3, -1.1 };
            Complex[] originalX = ToComplexVector(xReal, xImag);
            Complex[] originalY = ToComplexVector(yReal, yImag);

            ExecuteOrInconclusive("ZSwap", () =>
                blas.ZSwap(3, ref xReal, ref xImag, 1, ref yReal, ref yImag, 1));

            AssertComplexVectorEqual(originalY, xReal, xImag);
            AssertComplexVectorEqual(originalX, yReal, yImag);
        }

        [TestMethod]
        public void ZIamax_ReturnsIndexOfLargestMagnitude()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.5, -2.0, 1.0, 0.2 };
            double[] xImag = new double[] { -0.4, 1.5, -3.0, 0.1 };

            ExecuteOrInconclusive("ZIamax", () =>
            {
                int result = blas.ZIamax(4, xReal, xImag, 1);
                int expected = ComplexIamax(ToComplexVector(xReal, xImag), 4, 1);
                Assert.AreEqual(expected, result);
            });
        }
        [TestMethod]
        public void ZRot_AppliesPlaneRotation()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double[] xReal = new double[] { 0.5, -1.0, 1.5 };
            double[] xImag = new double[] { 0.2, 0.8, -1.1 };
            double[] yReal = new double[] { -0.75, 0.4, 1.2 };
            double[] yImag = new double[] { 1.0, -0.5, 0.3 };
            double[] originalXReal = (double[])xReal.Clone();
            double[] originalXImag = (double[])xImag.Clone();
            double[] originalYReal = (double[])yReal.Clone();
            double[] originalYImag = (double[])yImag.Clone();

            double c = Math.Cos(0.6);
            double s = Math.Sin(0.6);

            ExecuteOrInconclusive("ZRot", () =>
                blas.ZRot(3, ref xReal, ref xImag, 1, ref yReal, ref yImag, 1, c, s));

            Complex[] originalX = ToComplexVector(originalXReal, originalXImag);
            Complex[] originalY = ToComplexVector(originalYReal, originalYImag);
            var (expectedX, expectedY) = ApplyZRotExpected(originalX, originalY, c, s);

            AssertComplexVectorEqual(expectedX, xReal, xImag);
            AssertComplexVectorEqual(expectedY, yReal, yImag);
        }

        [TestMethod]
        public void ZRotg_ComputesComplexGivensRotation()
        {
            using var handle = new ComplexBlasHandle();
            var blas = handle.Instance;

            double aReal = 3.0;
            double aImag = 4.0;
            double bReal = -2.0;
            double bImag = 5.0;

            Complex originalA = new Complex(aReal, aImag);
            Complex originalB = new Complex(bReal, bImag);

            ExecuteOrInconclusive("ZRotg", () =>
            {
                blas.ZRotg(ref aReal, ref aImag, ref bReal, ref bImag, out double c, out double sReal, out double sImag);

                Complex rotatedA = new Complex(aReal, aImag);
                Complex rotatedB = new Complex(bReal, bImag);
                Complex s = new Complex(sReal, sImag);

                AssertNearlyEqual(0.0, rotatedB.Real);
                AssertNearlyEqual(0.0, rotatedB.Imaginary);
                AssertNearlyEqual(1.0, c * c + s.Magnitude * s.Magnitude);

                Complex lhs = c * originalA + s * originalB;
                AssertComplexEqual(lhs, rotatedA);

                Complex rhs = c * originalB - Complex.Conjugate(s) * originalA;
                AssertNearlyEqual(0.0, rhs.Real);
                AssertNearlyEqual(0.0, rhs.Imaginary);
            });
        }
        private static void ExecuteOrInconclusive(string methodName, Action action)
        {
            try
            {
                action();
            }
            catch (COMException ex) when (ex.HResult == ENotImpl)
            {
                Assert.Inconclusive($"{methodName} is not yet implemented in COM_BLAS.dll (E_NOTIMPL).");
            }
            catch (NotImplementedException)
            {
                Assert.Inconclusive($"{methodName} is not yet implemented in COM_BLAS.dll (NotImplementedException).");
            }
        }

        private static Complex[,] MatrixMultiply(Complex[,] left, Complex[,] right)
        {
            int rows = left.GetLength(0);
            int shared = left.GetLength(1);
            int cols = right.GetLength(1);
            var result = new Complex[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    Complex sum = Complex.Zero;
                    for (int k = 0; k < shared; k++)
                    {
                        sum += left[i, k] * right[k, j];
                    }
                    result[i, j] = sum;
                }
            }
            return result;
        }

        private static Complex[,] CombineComplexMatrices(Complex alpha, Complex[,] left, Complex beta, Complex[,] right)
        {
            int rows = left.GetLength(0);
            int cols = left.GetLength(1);
            var result = new Complex[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = alpha * left[i, j] + beta * right[i, j];
                }
            }
            return result;
        }

        private static Complex[,] ScaleMatrix(Complex[,] matrix, Complex scalar)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            var result = new Complex[rows, cols];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i, j] = scalar * matrix[i, j];
                }
            }
            return result;
        }

        private static Complex[,] ApplyTranspose(Complex[,] matrix, BlasTranspose trans)
        {
            return trans switch
            {
                BlasTranspose.NoTrans => (Complex[,])matrix.Clone(),
                BlasTranspose.Trans => TransposeMatrix(matrix),
                BlasTranspose.ConjTrans => ConjugateTransposeMatrix(matrix),
                _ => throw new ArgumentOutOfRangeException(nameof(trans))
            };
        }

        private static Complex[,] TransposeMatrix(Complex[,] matrix)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            var result = new Complex[cols, rows];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[j, i] = matrix[i, j];
                }
            }
            return result;
        }

        private static Complex[,] ConjugateTransposeMatrix(Complex[,] matrix)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            var result = new Complex[cols, rows];
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[j, i] = Complex.Conjugate(matrix[i, j]);
                }
            }
            return result;
        }

        private static Complex[,] MakeSymmetric(Complex[,] matrix)
        {
            int size = matrix.GetLength(0);
            var result = (Complex[,])matrix.Clone();
            for (int i = 0; i < size; i++)
            {
                for (int j = i + 1; j < size; j++)
                {
                    result[j, i] = result[i, j];
                }
            }
            return result;
        }

        private static Complex[,] MakeHermitian(Complex[,] matrix)
        {
            int size = matrix.GetLength(0);
            var result = (Complex[,])matrix.Clone();
            for (int i = 0; i < size; i++)
            {
                result[i, i] = new Complex(result[i, i].Real, 0.0);
                for (int j = i + 1; j < size; j++)
                {
                    result[j, i] = Complex.Conjugate(result[i, j]);
                }
            }
            return result;
        }

        private static Complex[] MatrixVectorMultiply(Complex[,] matrix, Complex[] vector)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            var result = new Complex[rows];
            for (int i = 0; i < rows; i++)
            {
                Complex sum = Complex.Zero;
                for (int j = 0; j < cols; j++)
                {
                    sum += matrix[i, j] * vector[j];
                }
                result[i] = sum;
            }
            return result;
        }

        private static Complex[] CombineComplexVectors(Complex alpha, Complex[] left, Complex beta, Complex[] right)
        {
            var result = new Complex[left.Length];
            for (int i = 0; i < left.Length; i++)
            {
                result[i] = alpha * left[i] + beta * right[i];
            }
            return result;
        }

        private static Complex[] ScaleVector(Complex[] vector, Complex scalar)
        {
            var result = new Complex[vector.Length];
            for (int i = 0; i < vector.Length; i++)
            {
                result[i] = scalar * vector[i];
            }
            return result;
        }

        private static Complex[,] OuterProduct(Complex[] x, Complex[] y, bool conjugateY)
        {
            var result = new Complex[x.Length, y.Length];
            for (int i = 0; i < x.Length; i++)
            {
                for (int j = 0; j < y.Length; j++)
                {
                    Complex rhs = conjugateY ? Complex.Conjugate(y[j]) : y[j];
                    result[i, j] = x[i] * rhs;
                }
            }
            return result;
        }

        private static Complex[,] PrepareTriangularMatrix(Complex[,] matrix, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            var opMatrix = ApplyTranspose(matrix, trans);
            if (diag == BlasDiag.Unit)
            {
                int size = opMatrix.GetLength(0);
                for (int i = 0; i < size; i++)
                {
                    opMatrix[i, i] = Complex.One;
                }
            }
            return opMatrix;
        }

        private static Complex[] ApplyTriangularMatrixVector(Complex[,] matrix, Complex[] vector, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            var opMatrix = PrepareTriangularMatrix(matrix, uplo, trans, diag);
            int n = vector.Length;
            var result = new Complex[n];
            for (int i = 0; i < n; i++)
            {
                Complex sum = Complex.Zero;
                for (int j = 0; j < n; j++)
                {
                    bool use = trans == BlasTranspose.NoTrans
                        ? (uplo == BlasUplo.Upper ? j >= i : j <= i)
                        : (uplo == BlasUplo.Upper ? i >= j : i <= j);
                    if (!use)
                    {
                        continue;
                    }
                    sum += opMatrix[i, j] * vector[j];
                }
                result[i] = sum;
            }
            return result;
        }
        private static Complex[] SolveTriangularVector(Complex[,] matrix, Complex[] rhs, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            var opMatrix = PrepareTriangularMatrix(matrix, uplo, trans, diag);
            BlasUplo effectiveUplo = trans == BlasTranspose.NoTrans ? uplo : (uplo == BlasUplo.Upper ? BlasUplo.Lower : BlasUplo.Upper);
            int n = rhs.Length;
            var result = new Complex[n];
            rhs.CopyTo(result, 0);

            if (effectiveUplo == BlasUplo.Upper)
            {
                for (int row = n - 1; row >= 0; row--)
                {
                    Complex sum = result[row];
                    for (int col = row + 1; col < n; col++)
                    {
                        sum -= opMatrix[row, col] * result[col];
                    }
                    if (diag != BlasDiag.Unit)
                    {
                        sum /= opMatrix[row, row];
                    }
                    result[row] = sum;
                }
            }
            else
            {
                for (int row = 0; row < n; row++)
                {
                    Complex sum = result[row];
                    for (int col = 0; col < row; col++)
                    {
                        sum -= opMatrix[row, col] * result[col];
                    }
                    if (diag != BlasDiag.Unit)
                    {
                        sum /= opMatrix[row, row];
                    }
                    result[row] = sum;
                }
            }

            return result;
        }

        private static Complex[,] SolveTriangularMatrix(Complex[,] matrix, Complex[,] rhs, BlasUplo uplo, BlasTranspose trans, BlasDiag diag)
        {
            int rows = rhs.GetLength(0);
            int cols = rhs.GetLength(1);
            var result = new Complex[rows, cols];
            for (int col = 0; col < cols; col++)
            {
                var vector = new Complex[rows];
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

        private static double ComplexNorm2(Complex[] vector, int n, int inc)
        {
            double sum = 0.0;
            for (int i = 0; i < n; i++)
            {
                Complex value = vector[i * inc];
                sum += value.Magnitude * value.Magnitude;
            }
            return Math.Sqrt(sum);
        }

        private static double ComplexAsum(Complex[] vector, int n, int inc)
        {
            double sum = 0.0;
            for (int i = 0; i < n; i++)
            {
                Complex value = vector[i * inc];
                sum += Math.Abs(value.Real) + Math.Abs(value.Imaginary);
            }
            return sum;
        }

        private static int ComplexIamax(Complex[] vector, int n, int inc)
        {
            double max = -1.0;
            int index = 1;
            for (int i = 0; i < n; i++)
            {
                Complex value = vector[i * inc];
                double magnitude = Math.Abs(value.Real) + Math.Abs(value.Imaginary);
                if (magnitude > max)
                {
                    max = magnitude;
                    index = i + 1;
                }
            }
            return index;
        }

        private static (Complex[] X, Complex[] Y) ApplyZRotExpected(Complex[] x, Complex[] y, double c, double s)
        {
            var rotatedX = new Complex[x.Length];
            var rotatedY = new Complex[y.Length];
            for (int i = 0; i < x.Length; i++)
            {
                rotatedX[i] = c * x[i] + s * y[i];
                rotatedY[i] = c * y[i] - s * x[i];
            }
            return (rotatedX, rotatedY);
        }

        private static void AssertComplexEqual(Complex expected, Complex actual)
        {
            AssertNearlyEqual(expected.Real, actual.Real);
            AssertNearlyEqual(expected.Imaginary, actual.Imaginary);
        }
    }
}
