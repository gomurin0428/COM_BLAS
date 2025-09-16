// BLAS.h : CBLAS の宣言

#pragma once

#include "resource.h"       // メイン シンボル
#include <atlbase.h>
#include <atlcom.h>


#include "COMBLAS_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif

using namespace ATL;


// CBLAS

class ATL_NO_VTABLE CBLAS :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CBLAS, &CLSID_BLAS>,
    public ISupportErrorInfo,
    public IDispatchImpl<IBLAS, &IID_IBLAS, &LIBID_COMBLASLib, /*wMajor =*/ 1, /*wMinor =*/ 1>
{
public:
    CBLAS()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(106)


    BEGIN_COM_MAP(CBLAS)
        COM_INTERFACE_ENTRY(IBLAS)
        COM_INTERFACE_ENTRY(IDispatch)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);



    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

public:



    // IDispatchImpl を介して継承されました
    HRESULT __stdcall GemmSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasTranspose transA, BlasTranspose transB) override;

    HRESULT __stdcall SymmSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasSide side, BlasUplo uplo) override;

    HRESULT __stdcall SyrkSimple(SAFEARRAY* A, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose transA) override;

    HRESULT __stdcall Syr2kSimple(SAFEARRAY* A, SAFEARRAY* B, SAFEARRAY** C, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo, BlasTranspose trans) override;

    HRESULT __stdcall TrmmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag) override;

    HRESULT __stdcall TrsmSimple(SAFEARRAY* A, SAFEARRAY** B, DOUBLE alpha, BlasLayout layout, BlasSide side, BlasUplo uplo, BlasTranspose transA, BlasDiag diag) override;

    HRESULT __stdcall GemvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasTranspose transA) override;

    HRESULT __stdcall GerSimple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout) override;

    HRESULT __stdcall SymvSimple(SAFEARRAY* A, SAFEARRAY* x, SAFEARRAY** y, DOUBLE alpha, DOUBLE beta, BlasLayout layout, BlasUplo uplo) override;

    HRESULT __stdcall SyrSimple(SAFEARRAY* x, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo) override;

    HRESULT __stdcall Syr2Simple(SAFEARRAY* x, SAFEARRAY* y, SAFEARRAY** A, DOUBLE alpha, BlasLayout layout, BlasUplo uplo) override;

    HRESULT __stdcall TrmvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag) override;

    HRESULT __stdcall TrsvSimple(SAFEARRAY* A, SAFEARRAY** x, BlasLayout layout, BlasUplo uplo, BlasTranspose transA, BlasDiag diag) override;

    HRESULT __stdcall Axpy(LONG n, DOUBLE alpha, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY) override;

    HRESULT __stdcall Dot(LONG n, SAFEARRAY* x, LONG incX, SAFEARRAY* y, LONG incY, DOUBLE* result) override;

    HRESULT __stdcall Nrm2(LONG n, SAFEARRAY* x, LONG incX, DOUBLE* result) override;

    HRESULT __stdcall Asum(LONG n, SAFEARRAY* x, LONG incX, DOUBLE* result) override;

    HRESULT __stdcall Scal(LONG n, DOUBLE alpha, SAFEARRAY** x, LONG incX) override;

    HRESULT __stdcall Copy(LONG n, SAFEARRAY* x, LONG incX, SAFEARRAY** y, LONG incY) override;

    HRESULT __stdcall Swap(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY) override;

    HRESULT __stdcall Iamax(LONG n, SAFEARRAY* x, LONG incX, LONG* index1based) override;

    HRESULT __stdcall Rot(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, DOUBLE c, DOUBLE s) override;

    HRESULT __stdcall Rotg(DOUBLE* a, DOUBLE* b, DOUBLE* c, DOUBLE* s) override;

    HRESULT __stdcall Rotm(LONG n, SAFEARRAY** x, LONG incX, SAFEARRAY** y, LONG incY, SAFEARRAY* param) override;

    HRESULT __stdcall Rotmg(DOUBLE* d1, DOUBLE* d2, DOUBLE* x1, DOUBLE y1, SAFEARRAY** param) override;

};

OBJECT_ENTRY_AUTO(__uuidof(BLAS), CBLAS)
