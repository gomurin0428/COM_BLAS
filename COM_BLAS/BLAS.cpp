// BLAS.cpp : CBLAS の実装

#include "pch.h"
#include "BLAS.h"
#include "cblas.h"
#include <limits>
#include <string>
#include <sstream>
#include <algorithm>


namespace {

    // IErrorInfo を設定して hr を返す
    HRESULT SetComError(const std::wstring& msg, HRESULT hr = E_INVALIDARG) noexcept {
        CComPtr<ICreateErrorInfo> cei;
        if (SUCCEEDED(CreateErrorInfo(&cei)) && cei) {
            cei->SetDescription(const_cast<LPOLESTR>(msg.c_str()));
            CComPtr<IErrorInfo> ei;
            if (SUCCEEDED(cei->QueryInterface(&ei)) && ei) {
                SetErrorInfo(0, ei);
            }
        }
        return hr;
    }

    // 1 次元 SAFEARRAY の長さを取得（VT は厳密チェックせず、IDL に従って double 前提で扱う）
    HRESULT Get1DLength(SAFEARRAY* sa, size_t& len) noexcept {
        if (!sa) return SetComError(L"SAFEARRAY が NULL です。", E_INVALIDARG);
        if (sa->cDims != 1) return SetComError(L"1 次元の SAFEARRAY を渡してください。", E_INVALIDARG);

        LONG lbound = 0, ubound = -1;
        HRESULT hr = SafeArrayGetLBound(sa, 1, &lbound);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetUBound(sa, 1, &ubound);
        if (FAILED(hr)) return hr;

        if (ubound < lbound) {
            len = 0;
            return S_OK;
        }
        len = static_cast<size_t>(static_cast<long long>(ubound) - static_cast<long long>(lbound) + 1);
        return S_OK;
    }

    // RAII: SafeArrayAccessData / UnaccessData
    struct SafeArrayAccessor {
        SAFEARRAY* sa = nullptr;
        double* ptr = nullptr;
        SafeArrayAccessor() = default;
        explicit SafeArrayAccessor(SAFEARRAY* a) {
            Attach(a);
        }
        ~SafeArrayAccessor() {
            Release();
        }
        HRESULT Attach(SAFEARRAY* a) noexcept {
            if (sa == a && ptr) {
                return S_OK;
            }
            Release();
            sa = a;
            ptr = nullptr;
            if (sa) {
                HRESULT hr = SafeArrayAccessData(sa, reinterpret_cast<void**>(&ptr));
                if (FAILED(hr)) {
                    sa = nullptr;
                    ptr = nullptr;
                    return hr;
                }
            }
            return S_OK;
        }
        void Release() noexcept {
            if (sa && ptr) {
                SafeArrayUnaccessData(sa);
            }
            sa = nullptr;
            ptr = nullptr;
        }
        SafeArrayAccessor(SafeArrayAccessor&& other) noexcept {
            sa = other.sa;
            ptr = other.ptr;
            other.sa = nullptr;
            other.ptr = nullptr;
        }
        SafeArrayAccessor& operator=(SafeArrayAccessor&& other) noexcept {
            if (this != &other) {
                Release();
                sa = other.sa;
                ptr = other.ptr;
                other.sa = nullptr;
                other.ptr = nullptr;
            }
            return *this;
        }
        SafeArrayAccessor(const SafeArrayAccessor&) = delete;
        SafeArrayAccessor& operator=(const SafeArrayAccessor&) = delete;
    };

    bool FitsInInt64(size_t v) noexcept {
        return v <= static_cast<size_t>((std::numeric_limits<long long>::max)());
    }

    HRESULT ParameterError(const wchar_t* name, const wchar_t* message, HRESULT hr = E_INVALIDARG) noexcept {
        std::wostringstream oss;
        oss << L"[" << name << L"] " << message;
        return SetComError(oss.str(), hr);
    }

    HRESULT ParameterError(const wchar_t* name, const std::wstring& message, HRESULT hr = E_INVALIDARG) noexcept {
        std::wostringstream oss;
        oss << L"[" << name << L"] " << message;
        return SetComError(oss.str(), hr);
    }

    HRESULT EnsureArrayPointer(SAFEARRAY** sa, const wchar_t* name) noexcept {
        if (!sa) {
            return ParameterError(name, L"SAFEARRAY** is NULL.", E_POINTER);
        }
        if (!*sa) {
            return ParameterError(name, L"SAFEARRAY pointer is NULL.", E_POINTER);
        }
        return S_OK;
    }

    HRESULT EnsureDoubleSafeArray(SAFEARRAY* sa, const wchar_t* name) noexcept {
        if (!sa) {
            return ParameterError(name, L"SAFEARRAY is NULL.");
        }
        if ((sa->fFeatures & FADF_VARIANT) != 0) {
            return ParameterError(name, L"SAFEARRAY of VARIANT is not supported.", DISP_E_BADVARTYPE);
        }
        VARTYPE vt = VT_EMPTY;
        HRESULT hr = SafeArrayGetVartype(sa, &vt);
        if (FAILED(hr)) {
            if (hr != E_INVALIDARG) {
                return hr;
            }
        } else if (vt != VT_R8 && vt != VT_EMPTY) {
            return ParameterError(name, L"SAFEARRAY must contain doubles (VT_R8).", DISP_E_BADVARTYPE);
        }
        if (sa->cbElements != sizeof(double)) {
            return ParameterError(name, L"SAFEARRAY element size does not match double.", DISP_E_BADVARTYPE);
        }
        return S_OK;
    }

    HRESULT ToIntChecked(size_t value, const wchar_t* name, int& out) noexcept {
        if (value > static_cast<size_t>((std::numeric_limits<int>::max)())) {
            std::wostringstream oss;
            oss << L"Value for " << name << L" is too large: " << value;
            return SetComError(oss.str(), E_INVALIDARG);
        }
        out = static_cast<int>(value);
        return S_OK;
    }

    struct MatrixView {
        SafeArrayAccessor accessor;
        size_t rows = 0;
        size_t cols = 0;
        LONG lboundRow = 0;
        LONG lboundCol = 0;
    };

    HRESULT PrepareMatrixView(SAFEARRAY* sa, const wchar_t* name, MatrixView& view) noexcept {
        HRESULT hr = EnsureDoubleSafeArray(sa, name);
        if (FAILED(hr)) return hr;
        if (sa->cDims != 2) {
            return ParameterError(name, L"SAFEARRAY must be two-dimensional.");
        }
        LONG lb1 = 0, ub1 = -1, lb2 = 0, ub2 = -1;
        hr = SafeArrayGetLBound(sa, 1, &lb1);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetUBound(sa, 1, &ub1);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetLBound(sa, 2, &lb2);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetUBound(sa, 2, &ub2);
        if (FAILED(hr)) return hr;
        if (ub1 < lb1) {
            view.cols = 0;
        } else {
            view.cols = static_cast<size_t>(static_cast<long long>(ub1) - static_cast<long long>(lb1) + 1);
        }
        if (ub2 < lb2) {
            view.rows = 0;
        } else {
            view.rows = static_cast<size_t>(static_cast<long long>(ub2) - static_cast<long long>(lb2) + 1);
        }
        view.lboundCol = lb1;
        view.lboundRow = lb2;
        hr = view.accessor.Attach(sa);
        if (FAILED(hr)) return hr;
        if ((view.rows * view.cols) != 0 && !view.accessor.ptr) {
            return ParameterError(name, L"Failed to access SAFEARRAY data.", E_FAIL);
        }
        return S_OK;
    }

    struct VectorView {
        SafeArrayAccessor accessor;
        size_t length = 0;
        size_t start = 0;
        LONG n = 0;
        LONG inc = 0;
        double* data = nullptr;
    };

    HRESULT PrepareVectorView(SAFEARRAY* sa, LONG n, LONG inc, const wchar_t* name, VectorView& view) noexcept {
        HRESULT hr = EnsureDoubleSafeArray(sa, name);
        if (FAILED(hr)) return hr;
        hr = Get1DLength(sa, view.length);
        if (FAILED(hr)) return hr;
        view.n = n;
        view.inc = inc;
        hr = view.accessor.Attach(sa);
        if (FAILED(hr)) return hr;
        if (!view.accessor.ptr && view.length != 0) {
            return ParameterError(name, L"Failed to access SAFEARRAY data.", E_FAIL);
        }
        if (n < 0) {
            return ParameterError(name, L"n must be >= 0.");
        }
        if (n == 0) {
            view.start = 0;
            view.data = view.accessor.ptr;
            return S_OK;
        }
        if (inc == 0) {
            return ParameterError(name, L"inc must not be zero.");
        }
        if (!FitsInInt64(view.length)) {
            return ParameterError(name, L"SAFEARRAY is too large.");
        }
        long long nll = static_cast<long long>(n);
        long long incAbs = static_cast<long long>(inc > 0 ? inc : -inc);
        long long start = (inc > 0) ? 0 : (nll - 1) * incAbs;
        long long last = start + (nll - 1) * incAbs;
        long long lenll = static_cast<long long>(view.length);
        if (start < 0 || last < 0 || start >= lenll || last >= lenll) {
            std::wostringstream oss;
            oss << L"access out of bounds (length=" << lenll << L", n=" << nll << L", inc=" << inc << L")";
            return ParameterError(name, oss.str(), E_BOUNDS);
        }
        view.start = static_cast<size_t>(start);
        view.data = view.accessor.ptr ? view.accessor.ptr + view.start : nullptr;
        return S_OK;
    }

    size_t GetLeadingDimension(CBLAS_LAYOUT layout, const MatrixView& view) noexcept {
        size_t ld = (layout == CblasRowMajor) ? view.cols : view.rows;
        return ld == 0 ? 1 : ld;
    }

    HRESULT ToLayout(const wchar_t* name, BlasLayout value, CBLAS_LAYOUT& out) noexcept {
        switch (value) {
        case BlasLayout::RowMajor:
            out = CblasRowMajor;
            return S_OK;
        case BlasLayout::ColumnMajor:
            out = CblasColMajor;
            return S_OK;
        default:
            return ParameterError(name, L"Invalid layout value.");
        }
    }

    HRESULT ToTranspose(const wchar_t* name, BlasTranspose value, CBLAS_TRANSPOSE& out) noexcept {
        switch (value) {
        case BlasTranspose::NoTrans:
            out = CblasNoTrans;
            return S_OK;
        case BlasTranspose::Trans:
            out = CblasTrans;
            return S_OK;
        case BlasTranspose::ConjTrans:
            out = CblasConjTrans;
            return S_OK;
        default:
            return ParameterError(name, L"Invalid transpose value.");
        }
    }

    HRESULT ToUplo(const wchar_t* name, BlasUplo value, CBLAS_UPLO& out) noexcept {
        switch (value) {
        case BlasUplo::Upper:
            out = CblasUpper;
            return S_OK;
        case BlasUplo::Lower:
            out = CblasLower;
            return S_OK;
        default:
            return ParameterError(name, L"Invalid uplo value.");
        }
    }

    HRESULT ToDiag(const wchar_t* name, BlasDiag value, CBLAS_DIAG& out) noexcept {
        switch (value) {
        case BlasDiag::NonUnit:
            out = CblasNonUnit;
            return S_OK;
        case BlasDiag::Unit:
            out = CblasUnit;
            return S_OK;
        default:
            return ParameterError(name, L"Invalid diag value.");
        }
    }

    HRESULT ToSide(const wchar_t* name, BlasSide value, CBLAS_SIDE& out) noexcept {
        switch (value) {
        case BlasSide::Left:
            out = CblasLeft;
            return S_OK;
        case BlasSide::Right:
            out = CblasRight;
            return S_OK;
        default:
            return ParameterError(name, L"Invalid side value.");
        }
    }

} // namespace


// CBLAS

STDMETHODIMP CBLAS::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IBLAS,
        &IID_IBLASComplex
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT __stdcall CBLAS::GemmSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasTranspose transA, BlasTranspose transB)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    MatrixView b;
    hr = PrepareMatrixView(B, L"B", b);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(C, L"C");
    if (FAILED(hr)) return hr;
    MatrixView c;
    hr = PrepareMatrixView(*C, L"C", c);
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transAFlag;
    hr = ToTranspose(L"transA", transA, transAFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transBFlag;
    hr = ToTranspose(L"transB", transB, transBFlag);
    if (FAILED(hr)) return hr;

    size_t m = (transAFlag == CblasNoTrans) ? a.rows : a.cols;
    size_t kA = (transAFlag == CblasNoTrans) ? a.cols : a.rows;
    size_t kB = (transBFlag == CblasNoTrans) ? b.rows : b.cols;
    size_t n = (transBFlag == CblasNoTrans) ? b.cols : b.rows;

    if (kA != kB) {
        std::wostringstream oss;
        oss << L"Incompatible inner dimensions: " << kA << L" vs " << kB << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    if (c.rows != m || c.cols != n) {
        std::wostringstream oss;
        oss << L"Output matrix has size " << c.rows << L"x" << c.cols
            << L" but expected " << m << L"x" << n << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    int M, N, K, lda, ldb, ldc;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(kA, L"K", K);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, b), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, c), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !b.accessor.ptr || !c.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dgemm(order, transAFlag, transBFlag, M, N, K, alpha,
                a.accessor.ptr, lda,
                b.accessor.ptr, ldb,
                beta,
                c.accessor.ptr, ldc);

    return S_OK;
}

HRESULT __stdcall CBLAS::SymmSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasSide side, BlasUplo uplo)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    MatrixView b;
    hr = PrepareMatrixView(B, L"B", b);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(C, L"C");
    if (FAILED(hr)) return hr;
    MatrixView c;
    hr = PrepareMatrixView(*C, L"C", c);
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_SIDE sideFlag;
    hr = ToSide(L"side", side, sideFlag);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;

    size_t m = c.rows;
    size_t n = c.cols;
    if (b.rows != m || b.cols != n) {
        std::wostringstream oss;
        oss << L"Matrix B has size " << b.rows << L"x" << b.cols
            << L" but expected " << m << L"x" << n << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    if (sideFlag == CblasLeft) {
        if (a.rows != m || a.cols != m) {
            std::wostringstream oss;
            oss << L"Matrix A must be square of size " << m << L" for side=Left.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    } else {
        if (a.rows != n || a.cols != n) {
            std::wostringstream oss;
            oss << L"Matrix A must be square of size " << n << L" for side=Right.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    }

    int M, N, lda, ldb, ldc;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, b), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, c), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !b.accessor.ptr || !c.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dsymm(order, sideFlag, uploFlag, M, N, alpha,
                a.accessor.ptr, lda,
                b.accessor.ptr, ldb,
                beta,
                c.accessor.ptr, ldc);

    return S_OK;
}

HRESULT __stdcall CBLAS::SyrkSimple(SAFEARRAY* A, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose transA)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(C, L"C");
    if (FAILED(hr)) return hr;
    MatrixView c;
    hr = PrepareMatrixView(*C, L"C", c);
    if (FAILED(hr)) return hr;

    if (c.rows != c.cols) {
        std::wostringstream oss;
        oss << L"Matrix C must be square, but got " << c.rows << L"x" << c.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;

    size_t n = c.rows;
    size_t k = 0;
    if (transFlag == CblasNoTrans) {
        if (a.rows != n) {
            std::wostringstream oss;
            oss << L"Matrix A must have " << n << L" rows.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
        k = a.cols;
    } else {
        if (a.cols != n) {
            std::wostringstream oss;
            oss << L"Matrix A must have " << n << L" columns when transposed.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
        k = a.rows;
    }

    int N, K, lda, ldc;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(k, L"K", K);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, c), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !c.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dsyrk(order, uploFlag, transFlag, N, K, alpha,
                a.accessor.ptr, lda,
                beta,
                c.accessor.ptr, ldc);

    return S_OK;
}

HRESULT __stdcall CBLAS::Syr2kSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose trans)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    MatrixView b;
    hr = PrepareMatrixView(B, L"B", b);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(C, L"C");
    if (FAILED(hr)) return hr;
    MatrixView c;
    hr = PrepareMatrixView(*C, L"C", c);
    if (FAILED(hr)) return hr;

    if (c.rows != c.cols) {
        std::wostringstream oss;
        oss << L"Matrix C must be square, but got " << c.rows << L"x" << c.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"trans", trans, transFlag);
    if (FAILED(hr)) return hr;

    size_t n = c.rows;
    size_t k = 0;
    if (transFlag == CblasNoTrans) {
        if (a.rows != n || b.rows != n) {
            return SetComError(L"Matrices A and B must have N rows.", E_INVALIDARG);
        }
        k = a.cols;
        if (b.cols != k) {
            return SetComError(L"Matrices A and B must have the same K dimension.", E_INVALIDARG);
        }
    } else {
        if (a.cols != n || b.cols != n) {
            return SetComError(L"Matrices A and B must have N columns when transposed.", E_INVALIDARG);
        }
        k = a.rows;
        if (b.rows != k) {
            return SetComError(L"Matrices A and B must share the same K dimension.", E_INVALIDARG);
        }
    }

    int N, K, lda, ldb, ldc;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(k, L"K", K);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, b), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, c), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !b.accessor.ptr || !c.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dsyr2k(order, uploFlag, transFlag, N, K, alpha,
                 a.accessor.ptr, lda,
                 b.accessor.ptr, ldb,
                 beta,
                 c.accessor.ptr, ldc);

    return S_OK;
}

HRESULT __stdcall CBLAS::TrmmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(B, L"B");
    if (FAILED(hr)) return hr;
    MatrixView b;
    hr = PrepareMatrixView(*B, L"B", b);
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_SIDE sideFlag;
    hr = ToSide(L"side", side, sideFlag);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;
    CBLAS_DIAG diagFlag;
    hr = ToDiag(L"diag", diag, diagFlag);
    if (FAILED(hr)) return hr;

    size_t m = b.rows;
    size_t n = b.cols;
    if (sideFlag == CblasLeft) {
        if (a.rows != m || a.cols != m) {
            return SetComError(L"Matrix A must be square with dimension matching rows of B.", E_INVALIDARG);
        }
    } else {
        if (a.rows != n || a.cols != n) {
            return SetComError(L"Matrix A must be square with dimension matching columns of B.", E_INVALIDARG);
        }
    }

    int M, N, lda, ldb;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, b), L"ldb", ldb);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !b.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dtrmm(order, sideFlag, uploFlag, transFlag, diagFlag, M, N, alpha,
                a.accessor.ptr, lda,
                b.accessor.ptr, ldb);

    return S_OK;
}

HRESULT __stdcall CBLAS::TrsmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(B, L"B");
    if (FAILED(hr)) return hr;
    MatrixView b;
    hr = PrepareMatrixView(*B, L"B", b);
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_SIDE sideFlag;
    hr = ToSide(L"side", side, sideFlag);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;
    CBLAS_DIAG diagFlag;
    hr = ToDiag(L"diag", diag, diagFlag);
    if (FAILED(hr)) return hr;

    size_t m = b.rows;
    size_t n = b.cols;
    if (sideFlag == CblasLeft) {
        if (a.rows != m || a.cols != m) {
            return SetComError(L"Matrix A must be square with dimension matching rows of B.", E_INVALIDARG);
        }
    } else {
        if (a.rows != n || a.cols != n) {
            return SetComError(L"Matrix A must be square with dimension matching columns of B.", E_INVALIDARG);
        }
    }

    int M, N, lda, ldb;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, b), L"ldb", ldb);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || !b.accessor.ptr) {
        return SetComError(L"Failed to access matrix data.", E_FAIL);
    }

    cblas_dtrsm(order, sideFlag, uploFlag, transFlag, diagFlag, M, N, alpha,
                a.accessor.ptr, lda,
                b.accessor.ptr, ldb);

    return S_OK;
}

HRESULT __stdcall CBLAS::GemvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasTranspose transA)
{
    MatrixView a;
    HRESULT hr = PrepareMatrixView(A, L"A", a);
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;

    size_t m = a.rows;
    size_t n = a.cols;
    size_t expectedX = (transFlag == CblasNoTrans) ? n : m;
    size_t expectedY = (transFlag == CblasNoTrans) ? m : n;

    if (expectedX > static_cast<size_t>((std::numeric_limits<LONG>::max)()) ||
        expectedY > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(x, static_cast<LONG>(expectedX), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, static_cast<LONG>(expectedY), 1, L"y", yView);
    if (FAILED(hr)) return hr;

    if (!a.accessor.ptr || (expectedX > 0 && !xView.data) || (expectedY > 0 && !yView.data)) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    int M, N, lda;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, a), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (expectedX == 0) ? &dummy : xView.data;
    double* yPtr = (expectedY == 0) ? &dummy : yView.data;

    cblas_dgemv(order, transFlag, M, N, alpha,
                a.accessor.ptr, lda,
                xPtr, 1,
                beta,
                yPtr, 1);

    return S_OK;
}

HRESULT __stdcall CBLAS::GerSimple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout)
{
    HRESULT hr = EnsureArrayPointer(A, L"A");
    if (FAILED(hr)) return hr;
    MatrixView mat;
    hr = PrepareMatrixView(*A, L"A", mat);
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;

    size_t m = mat.rows;
    size_t n = mat.cols;
    if (m > static_cast<size_t>((std::numeric_limits<LONG>::max)()) ||
        n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Matrix dimensions are too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(x, static_cast<LONG>(m), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(y, static_cast<LONG>(n), 1, L"y", yView);
    if (FAILED(hr)) return hr;

    if (!mat.accessor.ptr || (m > 0 && !xView.data) || (n > 0 && !yView.data)) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    int M, N, lda;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (m == 0) ? &dummy : xView.data;
    double* yPtr = (n == 0) ? &dummy : yView.data;

    cblas_dger(order, M, N, alpha, xPtr, 1, yPtr, 1, mat.accessor.ptr, lda);

    return S_OK;
}

HRESULT __stdcall CBLAS::SymvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo)
{
    MatrixView mat;
    HRESULT hr = PrepareMatrixView(A, L"A", mat);
    if (FAILED(hr)) return hr;
    if (mat.rows != mat.cols) {
        std::wostringstream oss;
        oss << L"Matrix A must be square, but got " << mat.rows << L"x" << mat.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    size_t n = mat.rows;
    if (n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }
    hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    VectorView xView;
    hr = PrepareVectorView(x, static_cast<LONG>(n), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, static_cast<LONG>(n), 1, L"y", yView);
    if (FAILED(hr)) return hr;

    if (!mat.accessor.ptr || (n > 0 && (!xView.data || !yView.data))) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;

    int N, lda;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;
    double* yPtr = (n == 0) ? &dummy : yView.data;

    cblas_dsymv(order, uploFlag, N, alpha, mat.accessor.ptr, lda, xPtr, 1, beta, yPtr, 1);

    return S_OK;
}

HRESULT __stdcall CBLAS::SyrSimple(SAFEARRAY* x, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo)
{
    HRESULT hr = EnsureArrayPointer(A, L"A");
    if (FAILED(hr)) return hr;
    MatrixView mat;
    hr = PrepareMatrixView(*A, L"A", mat);
    if (FAILED(hr)) return hr;
    if (mat.rows != mat.cols) {
        std::wostringstream oss;
        oss << L"Matrix A must be square, but got " << mat.rows << L"x" << mat.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    size_t n = mat.rows;
    if (n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(x, static_cast<LONG>(n), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!mat.accessor.ptr || (n > 0 && !xView.data)) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;

    int N, lda;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dsyr(order, uploFlag, N, alpha, xPtr, 1, mat.accessor.ptr, lda);

    return S_OK;
}

HRESULT __stdcall CBLAS::Syr2Simple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo)
{
    HRESULT hr = EnsureArrayPointer(A, L"A");
    if (FAILED(hr)) return hr;
    MatrixView mat;
    hr = PrepareMatrixView(*A, L"A", mat);
    if (FAILED(hr)) return hr;
    if (mat.rows != mat.cols) {
        std::wostringstream oss;
        oss << L"Matrix A must be square, but got " << mat.rows << L"x" << mat.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    size_t n = mat.rows;
    if (n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(x, static_cast<LONG>(n), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(y, static_cast<LONG>(n), 1, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!mat.accessor.ptr || (n > 0 && (!xView.data || !yView.data))) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;

    int N, lda;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;
    double* yPtr = (n == 0) ? &dummy : yView.data;

    cblas_dsyr2(order, uploFlag, N, alpha, xPtr, 1, yPtr, 1, mat.accessor.ptr, lda);

    return S_OK;
}

HRESULT __stdcall CBLAS::TrmvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    MatrixView mat;
    hr = PrepareMatrixView(A, L"A", mat);
    if (FAILED(hr)) return hr;
    if (mat.rows != mat.cols) {
        std::wostringstream oss;
        oss << L"Matrix A must be square, but got " << mat.rows << L"x" << mat.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    size_t n = mat.rows;
    if (n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(*x, static_cast<LONG>(n), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!mat.accessor.ptr || (n > 0 && !xView.data)) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;
    CBLAS_DIAG diagFlag;
    hr = ToDiag(L"diag", diag, diagFlag);
    if (FAILED(hr)) return hr;

    int N, lda;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dtrmv(order, uploFlag, transFlag, diagFlag, N, mat.accessor.ptr, lda, xPtr, 1);

    return S_OK;
}

HRESULT __stdcall CBLAS::TrsvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    MatrixView mat;
    hr = PrepareMatrixView(A, L"A", mat);
    if (FAILED(hr)) return hr;
    if (mat.rows != mat.cols) {
        std::wostringstream oss;
        oss << L"Matrix A must be square, but got " << mat.rows << L"x" << mat.cols << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    size_t n = mat.rows;
    if (n > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    VectorView xView;
    hr = PrepareVectorView(*x, static_cast<LONG>(n), 1, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!mat.accessor.ptr || (n > 0 && !xView.data)) {
        return SetComError(L"Failed to access vector or matrix data.", E_FAIL);
    }

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_UPLO uploFlag;
    hr = ToUplo(L"uplo", uplo, uploFlag);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;
    CBLAS_DIAG diagFlag;
    hr = ToDiag(L"diag", diag, diagFlag);
    if (FAILED(hr)) return hr;

    int N, lda;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, mat), L"lda", lda);
    if (FAILED(hr)) return hr;

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dtrsv(order, uploFlag, transFlag, diagFlag, N, mat.accessor.ptr, lda, xPtr, 1);

    return S_OK;
}

HRESULT __stdcall CBLAS::Axpy(LONG n, DOUBLE alpha, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    hr = PrepareVectorView(x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, n, incY, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!xView.data || !yView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }
    cblas_daxpy(static_cast<int>(n), alpha, xView.data, static_cast<int>(incX), yView.data, static_cast<int>(incY));
    return S_OK;
}

HRESULT __stdcall CBLAS::Dot(LONG n, SAFEARRAY* x, LONG incX, SAFEARRAY* y, LONG incY, DOUBLE* result)
{
    // ポインタ検証
    if (!result) {
        return SetComError(L"result が NULL です。", E_POINTER);
    }
    *result = 0.0;

    // n 検証
    if (n < 0) {
        return SetComError(L"n は 0 以上である必要があります。", E_INVALIDARG);
    }
    if (n == 0) {
        // BLAS 規約：n==0 のとき結果は 0
        *result = 0.0;
        return S_OK;
    }

    // 増分検証
    if (incX == 0 || incY == 0) {
        return SetComError(L"incX と incY は 0 以外である必要があります。", E_INVALIDARG);
    }

    // SAFEARRAY の長さ取得
    size_t lenX = 0, lenY = 0;
    HRESULT hr = Get1DLength(x, lenX);
    if (FAILED(hr)) return hr;
    hr = Get1DLength(y, lenY);
    if (FAILED(hr)) return hr;

    // アクセス
    SafeArrayAccessor ax(x);
    SafeArrayAccessor ay(y);
    if (!ax.ptr || !ay.ptr) {
        return SetComError(L"SAFEARRAY のデータアクセスに失敗しました。", E_FAIL);
    }

    // 負の増分に対応：開始インデックスを最後の要素側に寄せる
    const long long nll = static_cast<long long>(n);
    const long long incXabs = static_cast<long long>(incX > 0 ? incX : -incX);
    const long long incYabs = static_cast<long long>(incY > 0 ? incY : -incY);

    // 使用する最大オフセット（0-based）
    // start = (inc<0) ? (n-1)*abs(inc) : 0
    const long long startX = (incX > 0) ? 0 : (nll - 1) * incXabs;
    const long long startY = (incY > 0) ? 0 : (nll - 1) * incYabs;

    // 最終アクセス位置 = start + (n-1)*abs(inc)
    const long long lastX = startX + (nll - 1) * incXabs;
    const long long lastY = startY + (nll - 1) * incYabs;

    // 境界検証（len は size_t、計算は long long）
    if (!FitsInInt64(lenX) || !FitsInInt64(lenY)) {
        return SetComError(L"SAFEARRAY が大きすぎます。", E_INVALIDARG);
    }
    const long long lenXll = static_cast<long long>(lenX);
    const long long lenYll = static_cast<long long>(lenY);

    if (startX < 0 || lastX < 0 || startX >= lenXll || lastX >= lenXll) {
        std::wostringstream oss;
        oss << L"x の長さ " << lenXll
            << L" に対して (n=" << nll << L", incX=" << incX
            << L") のアクセスが範囲外です。";
        return SetComError(oss.str(), E_BOUNDS);
    }
    if (startY < 0 || lastY < 0 || startY >= lenYll || lastY >= lenYll) {
        std::wostringstream oss;
        oss << L"y の長さ " << lenYll
            << L" に対して (n=" << nll << L", incY=" << incY
            << L") のアクセスが範囲外です。";
        return SetComError(oss.str(), E_BOUNDS);
    }

    // CBLAS 呼び出し（int への安全キャスト確認）
    if (nll > (std::numeric_limits<int>::max)()
        || incXabs > (std::numeric_limits<int>::max)()
        || incYabs > (std::numeric_limits<int>::max)()) {
        return SetComError(L"n または inc が大きすぎます（int に収まりません）。", E_INVALIDARG);
    }

    const double* px = ax.ptr + static_cast<size_t>(startX);
    const double* py = ay.ptr + static_cast<size_t>(startY);

    const int N = static_cast<int>(nll);
    const int iX = static_cast<int>(incX);
    const int iY = static_cast<int>(incY);

    const double val = cblas_ddot(N, px, iX, py, iY);
    *result = val;
    return S_OK;

}

HRESULT __stdcall CBLAS::Nrm2(LONG n, SAFEARRAY* x, LONG incX, DOUBLE* result)
{
    if (!result) {
        return ParameterError(L"result", L"must not be null.", E_POINTER);
    }
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        *result = 0.0;
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    HRESULT hr = PrepareVectorView(x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!xView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    *result = cblas_dnrm2(static_cast<int>(n), xView.data, static_cast<int>(incX));
    return S_OK;
}

HRESULT __stdcall CBLAS::Asum(LONG n, SAFEARRAY* x, LONG incX, DOUBLE* result)
{
    if (!result) {
        return ParameterError(L"result", L"must not be null.", E_POINTER);
    }
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        *result = 0.0;
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    HRESULT hr = PrepareVectorView(x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!xView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    *result = cblas_dasum(static_cast<int>(n), xView.data, static_cast<int>(incX));
    return S_OK;
}

HRESULT __stdcall CBLAS::Scal(LONG n, DOUBLE alpha, SAFEARRAY** x, LONG incX)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    hr = PrepareVectorView(*x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!xView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    cblas_dscal(static_cast<int>(n), alpha, xView.data, static_cast<int>(incX));
    return S_OK;
}

HRESULT __stdcall CBLAS::Copy(LONG n, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    hr = PrepareVectorView(x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, n, incY, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!xView.data || !yView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }
    cblas_dcopy(static_cast<int>(n), xView.data, static_cast<int>(incX), yView.data, static_cast<int>(incY));
    return S_OK;
}

HRESULT __stdcall CBLAS::Swap(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    hr = PrepareVectorView(*x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, n, incY, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!xView.data || !yView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }
    cblas_dswap(static_cast<int>(n), xView.data, static_cast<int>(incX), yView.data, static_cast<int>(incY));
    return S_OK;
}

HRESULT __stdcall CBLAS::Iamax(LONG n, SAFEARRAY* x, LONG incX, LONG* index1based)
{
    if (!index1based) {
        return ParameterError(L"index1based", L"must not be null.", E_POINTER);
    }
    if (n <= 0) {
        *index1based = 0;
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    HRESULT hr = PrepareVectorView(x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    if (!xView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    int idx = cblas_idamax(static_cast<int>(n), xView.data, static_cast<int>(incX));
    *index1based = static_cast<LONG>(idx + 1);
    return S_OK;
}

HRESULT __stdcall CBLAS::Rot(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, DOUBLE c, DOUBLE s)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }
    VectorView xView;
    hr = PrepareVectorView(*x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, n, incY, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!xView.data || !yView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }
    cblas_drot(static_cast<int>(n), xView.data, static_cast<int>(incX), yView.data, static_cast<int>(incY), c, s);
    return S_OK;
}

HRESULT __stdcall CBLAS::Rotg(DOUBLE* a, DOUBLE* b, DOUBLE* c, DOUBLE* s)
{
    if (!a || !b || !c || !s) {
        return SetComError(L"All pointers must be non-null.", E_POINTER);
    }
    cblas_drotg(a, b, c, s);
    return S_OK;
}

HRESULT __stdcall CBLAS::Rotm(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, SAFEARRAY* param)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }
    HRESULT hr = EnsureArrayPointer(x, L"x");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(y, L"y");
    if (FAILED(hr)) return hr;
    VectorView xView;
    hr = PrepareVectorView(*x, n, incX, L"x", xView);
    if (FAILED(hr)) return hr;
    VectorView yView;
    hr = PrepareVectorView(*y, n, incY, L"y", yView);
    if (FAILED(hr)) return hr;
    if (!xView.data || !yView.data) {
        return SetComError(L"Failed to access vector data.", E_FAIL);
    }
    VectorView paramView;
    hr = PrepareVectorView(param, 5, 1, L"param", paramView);
    if (FAILED(hr)) return hr;
    if (!paramView.data) {
        return SetComError(L"Failed to access param data.", E_FAIL);
    }
    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }
    cblas_drotm(static_cast<int>(n), xView.data, static_cast<int>(incX), yView.data, static_cast<int>(incY), paramView.data);
    return S_OK;
}

HRESULT __stdcall CBLAS::Rotmg(DOUBLE* d1, DOUBLE* d2, DOUBLE* x1, DOUBLE y1, SAFEARRAY** param)
{
    if (!d1 || !d2 || !x1 || !param) {
        return SetComError(L"Pointers must not be null.", E_POINTER);
    }
    SAFEARRAY* sa = SafeArrayCreateVector(VT_R8, 0, 5);
    if (!sa) {
        return E_OUTOFMEMORY;
    }
    SafeArrayAccessor paramAccessor;
    HRESULT hr = paramAccessor.Attach(sa);
    if (FAILED(hr) || !paramAccessor.ptr) {
        SafeArrayDestroy(sa);
        return FAILED(hr) ? hr : E_FAIL;
    }

    cblas_drotmg(d1, d2, x1, y1, paramAccessor.ptr);

    *param = sa;
    return S_OK;
}


HRESULT __stdcall CBLAS::ZGemmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasTranspose transA, BlasTranspose transB)
{
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZGemvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY** yReal, SAFEARRAY** yImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasTranspose transA)
{
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZAxpy(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY, DOUBLE alphaReal, DOUBLE alphaImag)
{
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZDot(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY* yReal, SAFEARRAY* yImag, LONG incY, DOUBLE* resultReal, DOUBLE* resultImag, VARIANT_BOOL conjugate)
{
    return E_NOTIMPL;
}


