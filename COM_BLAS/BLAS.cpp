

#include "pch.h"
#include "BLAS.h"
#include "cblas.h"
#include <limits>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <complex>

namespace {

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
    template <typename... Args>
    void IgnoreUnused(Args&&...) noexcept {}

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
        // SAFEARRAY の次元 1 (index=1) は行、次元 2 は列に対応する。
        LONG lbRow = 0, ubRow = -1;
        LONG lbCol = 0, ubCol = -1;
        hr = SafeArrayGetLBound(sa, 1, &lbRow);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetUBound(sa, 1, &ubRow);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetLBound(sa, 2, &lbCol);
        if (FAILED(hr)) return hr;
        hr = SafeArrayGetUBound(sa, 2, &ubCol);
        if (FAILED(hr)) return hr;
        if (ubRow < lbRow) {
            view.rows = 0;
        } else {
            view.rows = static_cast<size_t>(static_cast<long long>(ubRow) - static_cast<long long>(lbRow) + 1);
        }
        if (ubCol < lbCol) {
            view.cols = 0;
        } else {
            view.cols = static_cast<size_t>(static_cast<long long>(ubCol) - static_cast<long long>(lbCol) + 1);
        }
        view.lboundRow = lbRow;
        view.lboundCol = lbCol;
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

    HRESULT ValidateComplexMatrixPair(const MatrixView& realView, const MatrixView& imagView, const wchar_t* name) noexcept {
        if (realView.rows != imagView.rows || realView.cols != imagView.cols) {
            std::wostringstream oss;
            oss << L"real and imaginary parts have mismatched dimensions (" << realView.rows << L"x" << realView.cols
                << L" vs " << imagView.rows << L"x" << imagView.cols << L").";
            return ParameterError(name, oss.str());
        }
        size_t total = realView.rows * realView.cols;
        if (total > 0 && (!realView.accessor.ptr || !imagView.accessor.ptr)) {
            return ParameterError(name, L"Failed to access real or imaginary SAFEARRAY data.", E_FAIL);
        }
        return S_OK;
    }

    HRESULT ValidateComplexVectorPair(const VectorView& realView, const VectorView& imagView, LONG n, const wchar_t* name) noexcept {
        if (realView.length != imagView.length) {
            std::wostringstream oss;
            oss << L"real and imaginary parts differ in length (" << realView.length << L" vs " << imagView.length << L").";
            return ParameterError(name, oss.str());
        }
        if (realView.inc != imagView.inc) {
            return ParameterError(name, L"real and imaginary parts must use the same increment.");
        }
        if (n > 0 && (!realView.data || !imagView.data)) {
            return ParameterError(name, L"Failed to access real or imaginary SAFEARRAY data.", E_FAIL);
        }
        return S_OK;
    }

    void GatherComplexMatrix(const MatrixView& realView, const MatrixView& imagView, std::vector<std::complex<double>>& out) {
        const size_t total = realView.rows * realView.cols;
        out.resize(total);
        if (total == 0) {
            return;
        }
        const double* realPtr = realView.accessor.ptr;
        const double* imagPtr = imagView.accessor.ptr;
        for (size_t i = 0; i < total; ++i) {
            out[i] = std::complex<double>(realPtr[i], imagPtr[i]);
        }
    }

    void ScatterComplexMatrix(const std::vector<std::complex<double>>& data, MatrixView& realView, MatrixView& imagView) {
        const size_t total = realView.rows * realView.cols;
        if (total == 0) {
            return;
        }
        double* realPtr = realView.accessor.ptr;
        double* imagPtr = imagView.accessor.ptr;
        for (size_t i = 0; i < total; ++i) {
            realPtr[i] = data[i].real();
            imagPtr[i] = data[i].imag();
        }
    }

    template <typename T>
    void ConvertColumnMajorToRowMajor(size_t rows, size_t cols, std::vector<T>& data) {
        if (rows <= 1 || cols <= 1) {
            return;
        }
        std::vector<T> buffer(data.size());
        for (size_t col = 0; col < cols; ++col) {
            for (size_t row = 0; row < rows; ++row) {
                const size_t src = col * rows + row;
                const size_t dst = row * cols + col;
                buffer[dst] = data[src];
            }
        }
        data.swap(buffer);
    }

    template <typename T>
    void ConvertRowMajorToColumnMajor(size_t rows, size_t cols, std::vector<T>& data) {
        if (rows <= 1 || cols <= 1) {
            return;
        }
        std::vector<T> buffer(data.size());
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                const size_t src = row * cols + col;
                const size_t dst = col * rows + row;
                buffer[dst] = data[src];
            }
        }
        data.swap(buffer);
    }

    void GatherRealMatrix(const MatrixView& view, std::vector<double>& out) {
        const size_t total = view.rows * view.cols;
        out.resize(total);
        if (total == 0) {
            return;
        }
        const double* src = view.accessor.ptr;
        std::copy(src, src + total, out.begin());
    }

    void ScatterRealMatrix(const std::vector<double>& data, MatrixView& view) {
        const size_t total = view.rows * view.cols;
        if (total == 0) {
            return;
        }
        double* dst = view.accessor.ptr;
        std::copy(data.begin(), data.begin() + total, dst);
    }

    const double* GetMatrixInputPointer(CBLAS_LAYOUT order, const MatrixView& view, std::vector<double>& buffer) {
        if (order == CblasRowMajor) {
            GatherRealMatrix(view, buffer);
            ConvertColumnMajorToRowMajor(view.rows, view.cols, buffer);
            return buffer.empty() ? nullptr : buffer.data();
        }
        return view.accessor.ptr;
    }

    double* GetMatrixOutputPointer(CBLAS_LAYOUT order, MatrixView& view, std::vector<double>& buffer) {
        if (order == CblasRowMajor) {
            GatherRealMatrix(view, buffer);
            ConvertColumnMajorToRowMajor(view.rows, view.cols, buffer);
            return buffer.empty() ? nullptr : buffer.data();
        }
        return view.accessor.ptr;
    }

    void CommitMatrixOutput(CBLAS_LAYOUT order, MatrixView& view, std::vector<double>& buffer) {
        if (order == CblasRowMajor) {
            ConvertRowMajorToColumnMajor(view.rows, view.cols, buffer);
            ScatterRealMatrix(buffer, view);
        }
    }

    void GatherComplexVector(const VectorView& realView, const VectorView& imagView, LONG n, std::vector<std::complex<double>>& out) {
        if (n < 0) {
            out.clear();
            return;
        }
        out.resize(static_cast<size_t>(n));
        if (n == 0) {
            return;
        }
        const double* realPtr = realView.data;
        const double* imagPtr = imagView.data;
        LONG incReal = realView.inc;
        LONG incImag = imagView.inc;
        for (LONG i = 0; i < n; ++i) {
            out[static_cast<size_t>(i)] = std::complex<double>(*realPtr, *imagPtr);
            realPtr += incReal;
            imagPtr += incImag;
        }
    }

    void ScatterComplexVector(const std::vector<std::complex<double>>& data, LONG n, VectorView& realView, VectorView& imagView) {
        if (n <= 0) {
            return;
        }
        double* realPtr = realView.data;
        double* imagPtr = imagView.data;
        LONG incReal = realView.inc;
        LONG incImag = imagView.inc;
        for (LONG i = 0; i < n; ++i) {
            const auto& value = data[static_cast<size_t>(i)];
            *realPtr = value.real();
            *imagPtr = value.imag();
            realPtr += incReal;
            imagPtr += incImag;
        }
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

    std::vector<double> aBuffer;
    std::vector<double> bBuffer;
    std::vector<double> cBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    const double* bPtr = GetMatrixInputPointer(order, b, bBuffer);
    double* cPtr = GetMatrixOutputPointer(order, c, cBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((b.rows * b.cols) != 0 && !bPtr) || ((c.rows * c.cols) != 0 && !cPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dgemm(order, transAFlag, transBFlag, M, N, K, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
                bPtr ? bPtr : b.accessor.ptr, ldb,
                beta,
                cPtr ? cPtr : c.accessor.ptr, ldc);

    CommitMatrixOutput(order, c, cBuffer);
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

    std::vector<double> aBuffer;
    std::vector<double> bBuffer;
    std::vector<double> cBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    const double* bPtr = GetMatrixInputPointer(order, b, bBuffer);
    double* cPtr = GetMatrixOutputPointer(order, c, cBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((b.rows * b.cols) != 0 && !bPtr) || ((c.rows * c.cols) != 0 && !cPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dsymm(order, sideFlag, uploFlag, M, N, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
                bPtr ? bPtr : b.accessor.ptr, ldb,
                beta,
                cPtr ? cPtr : c.accessor.ptr, ldc);

    CommitMatrixOutput(order, c, cBuffer);
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

    std::vector<double> aBuffer;
    std::vector<double> cBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    double* cPtr = GetMatrixOutputPointer(order, c, cBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((c.rows * c.cols) != 0 && !cPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dsyrk(order, uploFlag, transFlag, N, K, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
                beta,
                cPtr ? cPtr : c.accessor.ptr, ldc);

    CommitMatrixOutput(order, c, cBuffer);
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

    std::vector<double> aBuffer;
    std::vector<double> bBuffer;
    std::vector<double> cBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    const double* bPtr = GetMatrixInputPointer(order, b, bBuffer);
    double* cPtr = GetMatrixOutputPointer(order, c, cBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((b.rows * b.cols) != 0 && !bPtr) || ((c.rows * c.cols) != 0 && !cPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dsyr2k(order, uploFlag, transFlag, N, K, alpha,
                 aPtr ? aPtr : a.accessor.ptr, lda,
                 bPtr ? bPtr : b.accessor.ptr, ldb,
                 beta,
                 cPtr ? cPtr : c.accessor.ptr, ldc);

    CommitMatrixOutput(order, c, cBuffer);
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

    std::vector<double> aBuffer;
    std::vector<double> bBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    double* bPtr = GetMatrixOutputPointer(order, b, bBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((b.rows * b.cols) != 0 && !bPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dtrmm(order, sideFlag, uploFlag, transFlag, diagFlag, M, N, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
                bPtr ? bPtr : b.accessor.ptr, ldb);

    CommitMatrixOutput(order, b, bBuffer);
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

    std::vector<double> aBuffer;
    std::vector<double> bBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    double* bPtr = GetMatrixOutputPointer(order, b, bBuffer);

    if (((a.rows * a.cols) != 0 && !aPtr) || ((b.rows * b.cols) != 0 && !bPtr)) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    cblas_dtrsm(order, sideFlag, uploFlag, transFlag, diagFlag, M, N, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
                bPtr ? bPtr : b.accessor.ptr, ldb);

    CommitMatrixOutput(order, b, bBuffer);
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

    std::vector<double> aBuffer;
    const double* aPtr = GetMatrixInputPointer(order, a, aBuffer);
    if ((a.rows * a.cols) != 0 && !aPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (expectedX == 0) ? &dummy : xView.data;
    double* yPtr = (expectedY == 0) ? &dummy : yView.data;

    cblas_dgemv(order, transFlag, M, N, alpha,
                aPtr ? aPtr : a.accessor.ptr, lda,
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

    std::vector<double> matBuffer;
    double* matPtr = GetMatrixOutputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
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

    cblas_dger(order, M, N, alpha, xPtr, 1, yPtr, 1, matPtr ? matPtr : mat.accessor.ptr, lda);

    CommitMatrixOutput(order, mat, matBuffer);
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

    std::vector<double> matBuffer;
    const double* matPtr = GetMatrixInputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;
    double* yPtr = (n == 0) ? &dummy : yView.data;

    cblas_dsymv(order, uploFlag, N, alpha, matPtr ? matPtr : mat.accessor.ptr, lda, xPtr, 1, beta, yPtr, 1);

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

    std::vector<double> matBuffer;
    double* matPtr = GetMatrixOutputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dsyr(order, uploFlag, N, alpha, xPtr, 1, matPtr ? matPtr : mat.accessor.ptr, lda);

    CommitMatrixOutput(order, mat, matBuffer);
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

    std::vector<double> matBuffer;
    double* matPtr = GetMatrixOutputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;
    double* yPtr = (n == 0) ? &dummy : yView.data;

    cblas_dsyr2(order, uploFlag, N, alpha, xPtr, 1, yPtr, 1, matPtr ? matPtr : mat.accessor.ptr, lda);

    CommitMatrixOutput(order, mat, matBuffer);
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

    std::vector<double> matBuffer;
    const double* matPtr = GetMatrixInputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dtrmv(order, uploFlag, transFlag, diagFlag, N, matPtr ? matPtr : mat.accessor.ptr, lda, xPtr, 1);

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

    std::vector<double> matBuffer;
    const double* matPtr = GetMatrixInputPointer(order, mat, matBuffer);
    if ((mat.rows * mat.cols) != 0 && !matPtr) {
        return SetComError(L"Failed to prepare matrix data.", E_FAIL);
    }

    double dummy = 0.0;
    double* xPtr = (n == 0) ? &dummy : xView.data;

    cblas_dtrsv(order, uploFlag, transFlag, diagFlag, N, matPtr ? matPtr : mat.accessor.ptr, lda, xPtr, 1);

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
    MatrixView aRealView;
    HRESULT hr = PrepareMatrixView(AReal, L"AReal", aRealView);
    if (FAILED(hr)) return hr;
    MatrixView aImagView;
    hr = PrepareMatrixView(AImag, L"AImag", aImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(aRealView, aImagView, L"A");
    if (FAILED(hr)) return hr;

    MatrixView bRealView;
    hr = PrepareMatrixView(BReal, L"BReal", bRealView);
    if (FAILED(hr)) return hr;
    MatrixView bImagView;
    hr = PrepareMatrixView(BImag, L"BImag", bImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(bRealView, bImagView, L"B");
    if (FAILED(hr)) return hr;

    hr = EnsureArrayPointer(CReal, L"CReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(CImag, L"CImag");
    if (FAILED(hr)) return hr;
    MatrixView cRealView;
    hr = PrepareMatrixView(*CReal, L"CReal", cRealView);
    if (FAILED(hr)) return hr;
    MatrixView cImagView;
    hr = PrepareMatrixView(*CImag, L"CImag", cImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(cRealView, cImagView, L"C");
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

    size_t m = (transAFlag == CblasNoTrans) ? aRealView.rows : aRealView.cols;
    size_t kA = (transAFlag == CblasNoTrans) ? aRealView.cols : aRealView.rows;
    size_t kB = (transBFlag == CblasNoTrans) ? bRealView.rows : bRealView.cols;
    size_t n = (transBFlag == CblasNoTrans) ? bRealView.cols : bRealView.rows;

    if (kA != kB) {
        std::wostringstream oss;
        oss << L"Incompatible inner dimensions: " << kA << L" vs " << kB << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }
    if (cRealView.rows != m || cRealView.cols != n) {
        std::wostringstream oss;
        oss << L"Output matrix has size " << cRealView.rows << L"x" << cRealView.cols
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
    hr = ToIntChecked(GetLeadingDimension(order, aRealView), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, bRealView), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, cRealView), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    std::vector<std::complex<double>> aData;
    std::vector<std::complex<double>> bData;
    std::vector<std::complex<double>> cData;
    GatherComplexMatrix(aRealView, aImagView, aData);
    GatherComplexMatrix(bRealView, bImagView, bData);
    GatherComplexMatrix(cRealView, cImagView, cData);

    if (order == CblasRowMajor) {
        ConvertColumnMajorToRowMajor(aRealView.rows, aRealView.cols, aData);
        ConvertColumnMajorToRowMajor(bRealView.rows, bRealView.cols, bData);
        ConvertColumnMajorToRowMajor(cRealView.rows, cRealView.cols, cData);
    }

    std::complex<double> alpha(alphaReal, alphaImag);
    std::complex<double> beta(betaReal, betaImag);
    std::complex<double> dummy(0.0, 0.0);

    if (M > 0 && N > 0) {
        const void* aPtr = aData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(aData.data());
        const void* bPtr = bData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(bData.data());
        void* cPtr = cData.data();
        cblas_zgemm(order, transAFlag, transBFlag, M, N, K,
                    reinterpret_cast<const void*>(&alpha),
                    aPtr, lda,
                    bPtr, ldb,
                    reinterpret_cast<const void*>(&beta),
                    cPtr, ldc);
    }

    if (order == CblasRowMajor) {
        ConvertRowMajorToColumnMajor(cRealView.rows, cRealView.cols, cData);
    }

    ScatterComplexMatrix(cData, cRealView, cImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZGemvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY** yReal, SAFEARRAY** yImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasTranspose transA)
{
    MatrixView aRealView;
    HRESULT hr = PrepareMatrixView(AReal, L"AReal", aRealView);
    if (FAILED(hr)) return hr;
    MatrixView aImagView;
    hr = PrepareMatrixView(AImag, L"AImag", aImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(aRealView, aImagView, L"A");
    if (FAILED(hr)) return hr;

    hr = EnsureArrayPointer(yReal, L"yReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yImag, L"yImag");
    if (FAILED(hr)) return hr;

    CBLAS_LAYOUT order;
    hr = ToLayout(L"layout", layout, order);
    if (FAILED(hr)) return hr;
    CBLAS_TRANSPOSE transFlag;
    hr = ToTranspose(L"transA", transA, transFlag);
    if (FAILED(hr)) return hr;

    size_t m = aRealView.rows;
    size_t n = aRealView.cols;
    size_t expectedX = (transFlag == CblasNoTrans) ? n : m;
    size_t expectedY = (transFlag == CblasNoTrans) ? m : n;

    if (expectedX > static_cast<size_t>((std::numeric_limits<LONG>::max)()) ||
        expectedY > static_cast<size_t>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Vector length is too large.", E_INVALIDARG);
    }

    LONG lenX = static_cast<LONG>(expectedX);
    LONG lenY = static_cast<LONG>(expectedY);

    VectorView xRealView;
    hr = PrepareVectorView(xReal, lenX, 1, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, lenX, 1, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, lenX, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(*yReal, lenY, 1, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(*yImag, lenY, 1, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, lenY, L"y");
    if (FAILED(hr)) return hr;

    int M, N, lda;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, aRealView), L"lda", lda);
    if (FAILED(hr)) return hr;

    std::vector<std::complex<double>> aData;
    std::vector<std::complex<double>> xData;
    std::vector<std::complex<double>> yData;
    GatherComplexMatrix(aRealView, aImagView, aData);
    if (order == CblasRowMajor) {
        ConvertColumnMajorToRowMajor(aRealView.rows, aRealView.cols, aData);
    }
    GatherComplexVector(xRealView, xImagView, lenX, xData);
    GatherComplexVector(yRealView, yImagView, lenY, yData);

    std::complex<double> alpha(alphaReal, alphaImag);
    std::complex<double> beta(betaReal, betaImag);
    std::complex<double> dummy(0.0, 0.0);

    if (lenY > 0) {
        const void* aPtr = aData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(aData.data());
        const void* xPtr = xData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(xData.data());
        void* yPtr = yData.data();
        cblas_zgemv(order, transFlag, M, N,
                    reinterpret_cast<const void*>(&alpha),
                    aPtr, lda,
                    xPtr, 1,
                    reinterpret_cast<const void*>(&beta),
                    yPtr, 1);
    }

    ScatterComplexVector(yData, lenY, yRealView, yImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZAxpy(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY, DOUBLE alphaReal, DOUBLE alphaImag)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(yReal, L"yReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yImag, L"yImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(*yReal, n, incY, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(*yImag, n, incY, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, n, L"y");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    std::vector<std::complex<double>> yData;
    GatherComplexVector(xRealView, xImagView, n, xData);
    GatherComplexVector(yRealView, yImagView, n, yData);

    std::complex<double> alpha(alphaReal, alphaImag);
    cblas_zaxpy(static_cast<int>(n),
                reinterpret_cast<const void*>(&alpha),
                xData.data(), 1,
                yData.data(), 1);

    ScatterComplexVector(yData, n, yRealView, yImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZDot(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY* yReal, SAFEARRAY* yImag, LONG incY, DOUBLE* resultReal, DOUBLE* resultImag, VARIANT_BOOL conjugate)
{
    if (!resultReal || !resultImag) {
        return SetComError(L"Result pointers must not be null.", E_POINTER);
    }
    *resultReal = 0.0;
    *resultImag = 0.0;

    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    HRESULT hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(yReal, n, incY, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(yImag, n, incY, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, n, L"y");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    std::vector<std::complex<double>> yData;
    GatherComplexVector(xRealView, xImagView, n, xData);
    GatherComplexVector(yRealView, yImagView, n, yData);

    std::complex<double> result(0.0, 0.0);
    if (conjugate != VARIANT_FALSE) {
        cblas_zdotc_sub(static_cast<int>(n),
                        xData.data(), 1,
                        yData.data(), 1,
                        &result);
    } else {
        cblas_zdotu_sub(static_cast<int>(n),
                        xData.data(), 1,
                        yData.data(), 1,
                        &result);
    }

    *resultReal = result.real();
    *resultImag = result.imag();
    return S_OK;
}





HRESULT __stdcall CBLAS::ZSymmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasSide side, BlasUplo uplo)
{
    MatrixView aRealView;
    HRESULT hr = PrepareMatrixView(AReal, L"AReal", aRealView);
    if (FAILED(hr)) return hr;
    MatrixView aImagView;
    hr = PrepareMatrixView(AImag, L"AImag", aImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(aRealView, aImagView, L"A");
    if (FAILED(hr)) return hr;

    MatrixView bRealView;
    hr = PrepareMatrixView(BReal, L"BReal", bRealView);
    if (FAILED(hr)) return hr;
    MatrixView bImagView;
    hr = PrepareMatrixView(BImag, L"BImag", bImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(bRealView, bImagView, L"B");
    if (FAILED(hr)) return hr;

    hr = EnsureArrayPointer(CReal, L"CReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(CImag, L"CImag");
    if (FAILED(hr)) return hr;
    MatrixView cRealView;
    hr = PrepareMatrixView(*CReal, L"CReal", cRealView);
    if (FAILED(hr)) return hr;
    MatrixView cImagView;
    hr = PrepareMatrixView(*CImag, L"CImag", cImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(cRealView, cImagView, L"C");
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

    size_t m = cRealView.rows;
    size_t n = cRealView.cols;
    if (bRealView.rows != m || bRealView.cols != n) {
        std::wostringstream oss;
        oss << L"Matrix B has size " << bRealView.rows << L"x" << bRealView.cols
            << L" but expected " << m << L"x" << n << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    if (sideFlag == CblasLeft) {
        if (aRealView.rows != m || aRealView.cols != m) {
            std::wostringstream oss;
            oss << L"Matrix A must be square of size " << m << L" when side=Left.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    } else {
        if (aRealView.rows != n || aRealView.cols != n) {
            std::wostringstream oss;
            oss << L"Matrix A must be square of size " << n << L" when side=Right.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    }

    int M, N, lda, ldb, ldc;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, aRealView), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, bRealView), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, cRealView), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    std::vector<std::complex<double>> aData;
    std::vector<std::complex<double>> bData;
    std::vector<std::complex<double>> cData;
    GatherComplexMatrix(aRealView, aImagView, aData);
    GatherComplexMatrix(bRealView, bImagView, bData);
    GatherComplexMatrix(cRealView, cImagView, cData);

    if (order == CblasRowMajor) {
        ConvertColumnMajorToRowMajor(aRealView.rows, aRealView.cols, aData);
        ConvertColumnMajorToRowMajor(bRealView.rows, bRealView.cols, bData);
        ConvertColumnMajorToRowMajor(cRealView.rows, cRealView.cols, cData);
    }

    std::complex<double> alpha(alphaReal, alphaImag);
    std::complex<double> beta(betaReal, betaImag);
    std::complex<double> dummy(0.0, 0.0);

    if (M > 0 && N > 0) {
        const void* aPtr = aData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(aData.data());
        const void* bPtr = bData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(bData.data());
        void* cPtr = cData.data();
        cblas_zsymm(order, sideFlag, uploFlag, M, N,
                    reinterpret_cast<const void*>(&alpha),
                    aPtr, lda,
                    bPtr, ldb,
                    reinterpret_cast<const void*>(&beta),
                    cPtr, ldc);
    }

    if (order == CblasRowMajor) {
        ConvertRowMajorToColumnMajor(cRealView.rows, cRealView.cols, cData);
    }

    ScatterComplexMatrix(cData, cRealView, cImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZHemmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasSide side, BlasUplo uplo)
{
    MatrixView aRealView;
    HRESULT hr = PrepareMatrixView(AReal, L"AReal", aRealView);
    if (FAILED(hr)) return hr;
    MatrixView aImagView;
    hr = PrepareMatrixView(AImag, L"AImag", aImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(aRealView, aImagView, L"A");
    if (FAILED(hr)) return hr;

    MatrixView bRealView;
    hr = PrepareMatrixView(BReal, L"BReal", bRealView);
    if (FAILED(hr)) return hr;
    MatrixView bImagView;
    hr = PrepareMatrixView(BImag, L"BImag", bImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(bRealView, bImagView, L"B");
    if (FAILED(hr)) return hr;

    hr = EnsureArrayPointer(CReal, L"CReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(CImag, L"CImag");
    if (FAILED(hr)) return hr;
    MatrixView cRealView;
    hr = PrepareMatrixView(*CReal, L"CReal", cRealView);
    if (FAILED(hr)) return hr;
    MatrixView cImagView;
    hr = PrepareMatrixView(*CImag, L"CImag", cImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexMatrixPair(cRealView, cImagView, L"C");
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

    size_t m = cRealView.rows;
    size_t n = cRealView.cols;
    if (bRealView.rows != m || bRealView.cols != n) {
        std::wostringstream oss;
        oss << L"Matrix B has size " << bRealView.rows << L"x" << bRealView.cols
            << L" but expected " << m << L"x" << n << L".";
        return SetComError(oss.str(), E_INVALIDARG);
    }

    if (sideFlag == CblasLeft) {
        if (aRealView.rows != m || aRealView.cols != m) {
            std::wostringstream oss;
            oss << L"Hermitian matrix A must be square of size " << m << L" when side=Left.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    } else {
        if (aRealView.rows != n || aRealView.cols != n) {
            std::wostringstream oss;
            oss << L"Hermitian matrix A must be square of size " << n << L" when side=Right.";
            return SetComError(oss.str(), E_INVALIDARG);
        }
    }

    int M, N, lda, ldb, ldc;
    hr = ToIntChecked(m, L"M", M);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(n, L"N", N);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, aRealView), L"lda", lda);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, bRealView), L"ldb", ldb);
    if (FAILED(hr)) return hr;
    hr = ToIntChecked(GetLeadingDimension(order, cRealView), L"ldc", ldc);
    if (FAILED(hr)) return hr;

    std::vector<std::complex<double>> aData;
    std::vector<std::complex<double>> bData;
    std::vector<std::complex<double>> cData;
    GatherComplexMatrix(aRealView, aImagView, aData);
    GatherComplexMatrix(bRealView, bImagView, bData);
    GatherComplexMatrix(cRealView, cImagView, cData);

    if (order == CblasRowMajor) {
        ConvertColumnMajorToRowMajor(aRealView.rows, aRealView.cols, aData);
        ConvertColumnMajorToRowMajor(bRealView.rows, bRealView.cols, bData);
        ConvertColumnMajorToRowMajor(cRealView.rows, cRealView.cols, cData);
    }

    std::complex<double> alpha(alphaReal, alphaImag);
    std::complex<double> beta(betaReal, betaImag);
    std::complex<double> dummy(0.0, 0.0);

    if (M > 0 && N > 0) {
        const void* aPtr = aData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(aData.data());
        const void* bPtr = bData.empty() ? static_cast<const void*>(&dummy) : static_cast<const void*>(bData.data());
        void* cPtr = cData.data();
        cblas_zhemm(order, sideFlag, uploFlag, M, N,
                    reinterpret_cast<const void*>(&alpha),
                    aPtr, lda,
                    bPtr, ldb,
                    reinterpret_cast<const void*>(&beta),
                    cPtr, ldc);
    }

    if (order == CblasRowMajor) {
        ConvertRowMajorToColumnMajor(cRealView.rows, cRealView.cols, cData);
    }

    ScatterComplexMatrix(cData, cRealView, cImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZSyrkSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasUplo uplo, BlasTranspose transA)
{
    IgnoreUnused(AReal, AImag, CReal, CImag, alphaReal, alphaImag, betaReal, betaImag, layout, uplo, transA);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZSyr2kSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasUplo uplo, BlasTranspose trans)
{
    IgnoreUnused(AReal, AImag, BReal, BImag, CReal, CImag, alphaReal, alphaImag, betaReal, betaImag, layout, uplo, trans);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZHerkSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE betaReal, BlasLayout layout, BlasUplo uplo, BlasTranspose transA)
{
    IgnoreUnused(AReal, AImag, CReal, CImag, alphaReal, betaReal, layout, uplo, transA);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZHerk2kSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, BlasLayout layout, BlasUplo uplo, BlasTranspose transA)
{
    IgnoreUnused(AReal, AImag, BReal, BImag, CReal, CImag, alphaReal, alphaImag, betaReal, layout, uplo, transA);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZTrmmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** BReal, SAFEARRAY** BImag, DOUBLE alphaReal, DOUBLE alphaImag, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    IgnoreUnused(AReal, AImag, BReal, BImag, alphaReal, alphaImag, layout, side, uplo, transA, diag);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZTrsmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** BReal, SAFEARRAY** BImag, DOUBLE alphaReal, DOUBLE alphaImag, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    IgnoreUnused(AReal, AImag, BReal, BImag, alphaReal, alphaImag, layout, side, uplo, transA, diag);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZGerSimple(SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY* yReal, SAFEARRAY* yImag, SAFEARRAY** AReal, SAFEARRAY** AImag, DOUBLE alphaReal, DOUBLE alphaImag, BlasLayout layout, VARIANT_BOOL conjugateX)
{
    IgnoreUnused(xReal, xImag, yReal, yImag, AReal, AImag, alphaReal, alphaImag, layout, conjugateX);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZHemvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY** yReal, SAFEARRAY** yImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasUplo uplo)
{
    IgnoreUnused(AReal, AImag, xReal, xImag, yReal, yImag, alphaReal, alphaImag, betaReal, betaImag, layout, uplo);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZHerSimple(SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY** AReal, SAFEARRAY** AImag, DOUBLE alphaReal, BlasLayout layout, BlasUplo uplo)
{
    IgnoreUnused(xReal, xImag, AReal, AImag, alphaReal, layout, uplo);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZHer2Simple(SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY* yReal, SAFEARRAY* yImag, SAFEARRAY** AReal, SAFEARRAY** AImag, DOUBLE alphaReal, DOUBLE alphaImag, BlasLayout layout, BlasUplo uplo)
{
    IgnoreUnused(xReal, xImag, yReal, yImag, AReal, AImag, alphaReal, alphaImag, layout, uplo);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZTrmvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** xReal, SAFEARRAY** xImag, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    IgnoreUnused(AReal, AImag, xReal, xImag, layout, uplo, transA, diag);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZTrsvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY** xReal, SAFEARRAY** xImag, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
    IgnoreUnused(AReal, AImag, xReal, xImag, layout, uplo, transA, diag);
    return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::ZNrm2(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, DOUBLE* norm)
{
    if (!norm) {
        return ParameterError(L"norm", L"must not be null.", E_POINTER);
    }
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        *norm = 0.0;
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    HRESULT hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    *norm = cblas_dznrm2(static_cast<int>(n), xData.data(), 1);
    return S_OK;
}



HRESULT __stdcall CBLAS::ZAsum(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, DOUBLE* result)
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

    VectorView xRealView;
    HRESULT hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    *result = cblas_dzasum(static_cast<int>(n), xData.data(), 1);
    return S_OK;
}



HRESULT __stdcall CBLAS::ZScal(LONG n, SAFEARRAY** xReal, SAFEARRAY** xImag, LONG incX, DOUBLE alphaReal, DOUBLE alphaImag)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(xReal, L"xReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(xImag, L"xImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(*xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(*xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    std::complex<double> alpha(alphaReal, alphaImag);
    cblas_zscal(static_cast<int>(n),
                reinterpret_cast<const void*>(&alpha),
                xData.data(), 1);

    ScatterComplexVector(xData, n, xRealView, xImagView);
    return S_OK;
}



HRESULT __stdcall CBLAS::ZScalReal(LONG n, DOUBLE alphaReal, SAFEARRAY** xReal, SAFEARRAY** xImag, LONG incX)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(xReal, L"xReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(xImag, L"xImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(*xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(*xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    cblas_zdscal(static_cast<int>(n), alphaReal, xData.data(), 1);

    ScatterComplexVector(xData, n, xRealView, xImagView);
    return S_OK;
}



HRESULT __stdcall CBLAS::ZCopy(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(yReal, L"yReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yImag, L"yImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(*yReal, n, incY, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(*yImag, n, incY, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, n, L"y");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    std::vector<std::complex<double>> yData(static_cast<size_t>(n));
    cblas_zcopy(static_cast<int>(n), xData.data(), 1, yData.data(), 1);

    ScatterComplexVector(yData, n, yRealView, yImagView);
    return S_OK;
}



HRESULT __stdcall CBLAS::ZSwap(LONG n, SAFEARRAY** xReal, SAFEARRAY** xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(xReal, L"xReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(xImag, L"xImag");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yReal, L"yReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yImag, L"yImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(*xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(*xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(*yReal, n, incY, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(*yImag, n, incY, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, n, L"y");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);
    std::vector<std::complex<double>> yData;
    GatherComplexVector(yRealView, yImagView, n, yData);

    cblas_zswap(static_cast<int>(n), xData.data(), 1, yData.data(), 1);

    ScatterComplexVector(xData, n, xRealView, xImagView);
    ScatterComplexVector(yData, n, yRealView, yImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZIamax(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, LONG* index1based)
{
    if (!index1based) {
        return ParameterError(L"index1based", L"must not be null.", E_POINTER);
    }
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    if (n == 0) {
        *index1based = 0;
        return S_OK;
    }
    if (incX == 0) {
        return SetComError(L"incX must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    HRESULT hr = PrepareVectorView(xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);

    CBLAS_INDEX idx = cblas_izamax(static_cast<int>(n), xData.data(), 1);
    if (idx < 0) {
        return SetComError(L"cblas_izamax returned an invalid index.", E_FAIL);
    }
    if (idx >= static_cast<CBLAS_INDEX>((std::numeric_limits<LONG>::max)())) {
        return SetComError(L"Result index exceeds LONG range.", E_FAIL);
    }

    *index1based = static_cast<LONG>(idx) + 1;
    return S_OK;
}

HRESULT __stdcall CBLAS::ZRot(LONG n, SAFEARRAY** xReal, SAFEARRAY** xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY, DOUBLE c, DOUBLE s)
{
    if (n < 0) {
        return ParameterError(L"n", L"must be non-negative.");
    }
    HRESULT hr = EnsureArrayPointer(xReal, L"xReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(xImag, L"xImag");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yReal, L"yReal");
    if (FAILED(hr)) return hr;
    hr = EnsureArrayPointer(yImag, L"yImag");
    if (FAILED(hr)) return hr;
    if (n == 0) {
        return S_OK;
    }
    if (incX == 0 || incY == 0) {
        return SetComError(L"Increments must not be zero.", E_INVALIDARG);
    }

    VectorView xRealView;
    hr = PrepareVectorView(*xReal, n, incX, L"xReal", xRealView);
    if (FAILED(hr)) return hr;
    VectorView xImagView;
    hr = PrepareVectorView(*xImag, n, incX, L"xImag", xImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(xRealView, xImagView, n, L"x");
    if (FAILED(hr)) return hr;

    VectorView yRealView;
    hr = PrepareVectorView(*yReal, n, incY, L"yReal", yRealView);
    if (FAILED(hr)) return hr;
    VectorView yImagView;
    hr = PrepareVectorView(*yImag, n, incY, L"yImag", yImagView);
    if (FAILED(hr)) return hr;
    hr = ValidateComplexVectorPair(yRealView, yImagView, n, L"y");
    if (FAILED(hr)) return hr;

    if (incX < (std::numeric_limits<int>::min)() || incX > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incX is out of supported range.", E_INVALIDARG);
    }
    if (incY < (std::numeric_limits<int>::min)() || incY > (std::numeric_limits<int>::max)()) {
        return SetComError(L"incY is out of supported range.", E_INVALIDARG);
    }

    std::vector<std::complex<double>> xData;
    GatherComplexVector(xRealView, xImagView, n, xData);
    std::vector<std::complex<double>> yData;
    GatherComplexVector(yRealView, yImagView, n, yData);

    cblas_zdrot(static_cast<int>(n), xData.data(), 1, yData.data(), 1, c, s);

    ScatterComplexVector(xData, n, xRealView, xImagView);
    ScatterComplexVector(yData, n, yRealView, yImagView);
    return S_OK;
}

HRESULT __stdcall CBLAS::ZRotg(DOUBLE* aReal, DOUBLE* aImag, DOUBLE* bReal, DOUBLE* bImag, DOUBLE* c, DOUBLE* sReal, DOUBLE* sImag)
{
    IgnoreUnused(aReal, aImag, bReal, bImag, c, sReal, sImag);
    return E_NOTIMPL;
}

