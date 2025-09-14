// BLAS.cpp : CBLAS の実装

#include "pch.h"
#include "BLAS.h"
#include "cblas.h"
#include <limits>
#include <string>
#include <sstream>


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
        explicit SafeArrayAccessor(SAFEARRAY* a) : sa(a) {
            if (sa) {
                // VT は IDL で double 固定、ここでは直接 double* として受ける
                SafeArrayAccessData(sa, reinterpret_cast<void**>(&ptr));
            }
        }
        ~SafeArrayAccessor() {
            if (sa && ptr) {
                SafeArrayUnaccessData(sa);
            }
        }
        SafeArrayAccessor(const SafeArrayAccessor&) = delete;
        SafeArrayAccessor& operator=(const SafeArrayAccessor&) = delete;
    };

    bool FitsInInt64(size_t v) noexcept {
        return v <= static_cast<size_t>((std::numeric_limits<long long>::max)());
    }

} // namespace


// CBLAS

STDMETHODIMP CBLAS::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IBLAS
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
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::SymmSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasSide side, BlasUplo uplo)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::SyrkSimple(SAFEARRAY* A, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose transA)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Syr2kSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose trans)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::TrmmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::TrsmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::GemvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasTranspose transA)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::GerSimple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::SymvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::SyrSimple(SAFEARRAY* x, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Syr2Simple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::TrmvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::TrsvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Axpy(LONG n, DOUBLE alpha, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY)
{
	return E_NOTIMPL;
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
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Asum(LONG n, SAFEARRAY* x, LONG incX, DOUBLE* result)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Scal(LONG n, DOUBLE alpha, SAFEARRAY** x, LONG incX)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Copy(LONG n, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Swap(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Iamax(LONG n, SAFEARRAY* x, LONG incX, LONG* index1based)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Rot(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, DOUBLE c, DOUBLE s)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Rotg(DOUBLE* a, DOUBLE* b, DOUBLE* c, DOUBLE* s)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Rotm(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, SAFEARRAY* param)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CBLAS::Rotmg(DOUBLE* d1, DOUBLE* d2, DOUBLE* x1, DOUBLE y1, SAFEARRAY** param)
{
	return E_NOTIMPL;
}

