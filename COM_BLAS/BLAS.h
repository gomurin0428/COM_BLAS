

#pragma once

#include "resource.h"   
#include <atlbase.h>
#include <atlcom.h>


#include "COMBLAS_i.h"




#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "error"
#endif

using namespace ATL;


// CBLAS

class ATL_NO_VTABLE CBLAS :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CBLAS, &CLSID_BLAS>,
    public ISupportErrorInfo,
    public IDispatchImpl<IBLAS, &IID_IBLAS, &LIBID_COMBLASLib, /*wMajor =*/ 1, /*wMinor =*/ 3>,
    public IDispatchImpl<IBLASComplex, &IID_IBLASComplex, &LIBID_COMBLASLib, /*wMajor =*/ 1, /*wMinor =*/ 3>
{
public:
    CBLAS()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(106)


    BEGIN_COM_MAP(CBLAS)
        COM_INTERFACE_ENTRY(IBLAS)
        COM_INTERFACE_ENTRY(IBLASComplex)
        COM_INTERFACE_ENTRY2(IDispatch, IBLAS)
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

    HRESULT __stdcall ZGemmSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* BReal, SAFEARRAY* BImag, SAFEARRAY** CReal, SAFEARRAY** CImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasTranspose transA, BlasTranspose transB) override;

    HRESULT __stdcall ZGemvSimple(SAFEARRAY* AReal, SAFEARRAY* AImag, SAFEARRAY* xReal, SAFEARRAY* xImag, SAFEARRAY** yReal, SAFEARRAY** yImag, DOUBLE alphaReal, DOUBLE alphaImag, DOUBLE betaReal, DOUBLE betaImag, BlasLayout layout, BlasTranspose transA) override;

    HRESULT __stdcall ZAxpy(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY** yReal, SAFEARRAY** yImag, LONG incY, DOUBLE alphaReal, DOUBLE alphaImag) override;

    HRESULT __stdcall ZDot(LONG n, SAFEARRAY* xReal, SAFEARRAY* xImag, LONG incX, SAFEARRAY* yReal, SAFEARRAY* yImag, LONG incY, DOUBLE* resultReal, DOUBLE* resultImag, VARIANT_BOOL conjugate) override;

};

OBJECT_ENTRY_AUTO(__uuidof(BLAS), CBLAS)

