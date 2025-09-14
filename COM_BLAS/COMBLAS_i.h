

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 12:14:07 2038
 */
/* Compiler settings for COMBLAS.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __COMBLAS_i_h__
#define __COMBLAS_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IBLAS_FWD_DEFINED__
#define __IBLAS_FWD_DEFINED__
typedef interface IBLAS IBLAS;

#endif 	/* __IBLAS_FWD_DEFINED__ */


#ifndef __BLAS_FWD_DEFINED__
#define __BLAS_FWD_DEFINED__

#ifdef __cplusplus
typedef class BLAS BLAS;
#else
typedef struct BLAS BLAS;
#endif /* __cplusplus */

#endif 	/* __BLAS_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_COMBLAS_0000_0000 */
/* [local] */ 

typedef /* [v1_enum] */ 
enum BlasLayout
    {
        RowMajor	= 101,
        ColumnMajor	= 102
    } 	BlasLayout;

typedef /* [v1_enum] */ 
enum BlasTranspose
    {
        NoTrans	= 111,
        Trans	= 112,
        ConjTrans	= 113
    } 	BlasTranspose;

typedef /* [v1_enum] */ 
enum BlasUplo
    {
        Upper	= 121,
        Lower	= 122
    } 	BlasUplo;

typedef /* [v1_enum] */ 
enum BlasDiag
    {
        NonUnit	= 131,
        Unit	= 132
    } 	BlasDiag;

typedef /* [v1_enum] */ 
enum BlasSide
    {
        Left	= 141,
        Right	= 142
    } 	BlasSide;



extern RPC_IF_HANDLE __MIDL_itf_COMBLAS_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_COMBLAS_0000_0000_v0_0_s_ifspec;

#ifndef __IBLAS_INTERFACE_DEFINED__
#define __IBLAS_INTERFACE_DEFINED__

/* interface IBLAS */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBLAS;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("19ef5ee4-5e52-47fa-ba23-c9f70bef7faa")
    IBLAS : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GemmSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans,
            /* [defaultvalue][in] */ BlasTranspose transB = NoTrans) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SymmSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasSide side = Left,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SyrkSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Syr2kSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose trans = NoTrans) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrmmSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *B,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasSide side = Left,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans,
            /* [defaultvalue][in] */ BlasDiag diag = NonUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrsmSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *B,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasSide side = Left,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans,
            /* [defaultvalue][in] */ BlasDiag diag = NonUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GemvSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *y,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GerSimple( 
            /* [in] */ SAFEARRAY * x,
            /* [in] */ SAFEARRAY * y,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SymvSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *y,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ DOUBLE beta = 0,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SyrSimple( 
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Syr2Simple( 
            /* [in] */ SAFEARRAY * x,
            /* [in] */ SAFEARRAY * y,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha = 1,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrmvSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *x,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans,
            /* [defaultvalue][in] */ BlasDiag diag = NonUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrsvSimple( 
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *x,
            /* [defaultvalue][in] */ BlasLayout layout = ColumnMajor,
            /* [defaultvalue][in] */ BlasUplo uplo = Upper,
            /* [defaultvalue][in] */ BlasTranspose transA = NoTrans,
            /* [defaultvalue][in] */ BlasDiag diag = NonUnit) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Axpy( 
            /* [in] */ LONG n,
            /* [in] */ DOUBLE alpha,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Dot( 
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [in] */ SAFEARRAY * y,
            /* [in] */ LONG incY,
            /* [retval][out] */ DOUBLE *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Nrm2( 
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ DOUBLE *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Asum( 
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ DOUBLE *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Scal( 
            /* [in] */ LONG n,
            /* [in] */ DOUBLE alpha,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Swap( 
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Iamax( 
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ LONG *index1based) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rot( 
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY,
            /* [in] */ DOUBLE c,
            /* [in] */ DOUBLE s) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rotg( 
            /* [out][in] */ DOUBLE *a,
            /* [out][in] */ DOUBLE *b,
            /* [out] */ DOUBLE *c,
            /* [out] */ DOUBLE *s) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rotm( 
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY,
            /* [in] */ SAFEARRAY * param) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rotmg( 
            /* [out][in] */ DOUBLE *d1,
            /* [out][in] */ DOUBLE *d2,
            /* [out][in] */ DOUBLE *x1,
            /* [in] */ DOUBLE y1,
            /* [out] */ SAFEARRAY * *param) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBLASVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBLAS * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBLAS * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBLAS * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBLAS * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBLAS * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBLAS * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBLAS * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(IBLAS, GemmSimple)
        HRESULT ( STDMETHODCALLTYPE *GemmSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasTranspose transA,
            /* [defaultvalue][in] */ BlasTranspose transB);
        
        DECLSPEC_XFGVIRT(IBLAS, SymmSimple)
        HRESULT ( STDMETHODCALLTYPE *SymmSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasSide side,
            /* [defaultvalue][in] */ BlasUplo uplo);
        
        DECLSPEC_XFGVIRT(IBLAS, SyrkSimple)
        HRESULT ( STDMETHODCALLTYPE *SyrkSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose transA);
        
        DECLSPEC_XFGVIRT(IBLAS, Syr2kSimple)
        HRESULT ( STDMETHODCALLTYPE *Syr2kSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * B,
            /* [out][in] */ SAFEARRAY * *C,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose trans);
        
        DECLSPEC_XFGVIRT(IBLAS, TrmmSimple)
        HRESULT ( STDMETHODCALLTYPE *TrmmSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *B,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasSide side,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose transA,
            /* [defaultvalue][in] */ BlasDiag diag);
        
        DECLSPEC_XFGVIRT(IBLAS, TrsmSimple)
        HRESULT ( STDMETHODCALLTYPE *TrsmSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *B,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasSide side,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose transA,
            /* [defaultvalue][in] */ BlasDiag diag);
        
        DECLSPEC_XFGVIRT(IBLAS, GemvSimple)
        HRESULT ( STDMETHODCALLTYPE *GemvSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *y,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasTranspose transA);
        
        DECLSPEC_XFGVIRT(IBLAS, GerSimple)
        HRESULT ( STDMETHODCALLTYPE *GerSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ SAFEARRAY * y,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ BlasLayout layout);
        
        DECLSPEC_XFGVIRT(IBLAS, SymvSimple)
        HRESULT ( STDMETHODCALLTYPE *SymvSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *y,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ DOUBLE beta,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo);
        
        DECLSPEC_XFGVIRT(IBLAS, SyrSimple)
        HRESULT ( STDMETHODCALLTYPE *SyrSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * x,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo);
        
        DECLSPEC_XFGVIRT(IBLAS, Syr2Simple)
        HRESULT ( STDMETHODCALLTYPE *Syr2Simple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ SAFEARRAY * y,
            /* [out][in] */ SAFEARRAY * *A,
            /* [defaultvalue][in] */ DOUBLE alpha,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo);
        
        DECLSPEC_XFGVIRT(IBLAS, TrmvSimple)
        HRESULT ( STDMETHODCALLTYPE *TrmvSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *x,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose transA,
            /* [defaultvalue][in] */ BlasDiag diag);
        
        DECLSPEC_XFGVIRT(IBLAS, TrsvSimple)
        HRESULT ( STDMETHODCALLTYPE *TrsvSimple )( 
            IBLAS * This,
            /* [in] */ SAFEARRAY * A,
            /* [out][in] */ SAFEARRAY * *x,
            /* [defaultvalue][in] */ BlasLayout layout,
            /* [defaultvalue][in] */ BlasUplo uplo,
            /* [defaultvalue][in] */ BlasTranspose transA,
            /* [defaultvalue][in] */ BlasDiag diag);
        
        DECLSPEC_XFGVIRT(IBLAS, Axpy)
        HRESULT ( STDMETHODCALLTYPE *Axpy )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ DOUBLE alpha,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY);
        
        DECLSPEC_XFGVIRT(IBLAS, Dot)
        HRESULT ( STDMETHODCALLTYPE *Dot )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [in] */ SAFEARRAY * y,
            /* [in] */ LONG incY,
            /* [retval][out] */ DOUBLE *result);
        
        DECLSPEC_XFGVIRT(IBLAS, Nrm2)
        HRESULT ( STDMETHODCALLTYPE *Nrm2 )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ DOUBLE *result);
        
        DECLSPEC_XFGVIRT(IBLAS, Asum)
        HRESULT ( STDMETHODCALLTYPE *Asum )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ DOUBLE *result);
        
        DECLSPEC_XFGVIRT(IBLAS, Scal)
        HRESULT ( STDMETHODCALLTYPE *Scal )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ DOUBLE alpha,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX);
        
        DECLSPEC_XFGVIRT(IBLAS, Copy)
        HRESULT ( STDMETHODCALLTYPE *Copy )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY);
        
        DECLSPEC_XFGVIRT(IBLAS, Swap)
        HRESULT ( STDMETHODCALLTYPE *Swap )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY);
        
        DECLSPEC_XFGVIRT(IBLAS, Iamax)
        HRESULT ( STDMETHODCALLTYPE *Iamax )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [in] */ SAFEARRAY * x,
            /* [in] */ LONG incX,
            /* [retval][out] */ LONG *index1based);
        
        DECLSPEC_XFGVIRT(IBLAS, Rot)
        HRESULT ( STDMETHODCALLTYPE *Rot )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY,
            /* [in] */ DOUBLE c,
            /* [in] */ DOUBLE s);
        
        DECLSPEC_XFGVIRT(IBLAS, Rotg)
        HRESULT ( STDMETHODCALLTYPE *Rotg )( 
            IBLAS * This,
            /* [out][in] */ DOUBLE *a,
            /* [out][in] */ DOUBLE *b,
            /* [out] */ DOUBLE *c,
            /* [out] */ DOUBLE *s);
        
        DECLSPEC_XFGVIRT(IBLAS, Rotm)
        HRESULT ( STDMETHODCALLTYPE *Rotm )( 
            IBLAS * This,
            /* [in] */ LONG n,
            /* [out][in] */ SAFEARRAY * *x,
            /* [in] */ LONG incX,
            /* [out][in] */ SAFEARRAY * *y,
            /* [in] */ LONG incY,
            /* [in] */ SAFEARRAY * param);
        
        DECLSPEC_XFGVIRT(IBLAS, Rotmg)
        HRESULT ( STDMETHODCALLTYPE *Rotmg )( 
            IBLAS * This,
            /* [out][in] */ DOUBLE *d1,
            /* [out][in] */ DOUBLE *d2,
            /* [out][in] */ DOUBLE *x1,
            /* [in] */ DOUBLE y1,
            /* [out] */ SAFEARRAY * *param);
        
        END_INTERFACE
    } IBLASVtbl;

    interface IBLAS
    {
        CONST_VTBL struct IBLASVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBLAS_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBLAS_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBLAS_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBLAS_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBLAS_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBLAS_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBLAS_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBLAS_GemmSimple(This,A,B,C,alpha,beta,layout,transA,transB)	\
    ( (This)->lpVtbl -> GemmSimple(This,A,B,C,alpha,beta,layout,transA,transB) ) 

#define IBLAS_SymmSimple(This,A,B,C,alpha,beta,layout,side,uplo)	\
    ( (This)->lpVtbl -> SymmSimple(This,A,B,C,alpha,beta,layout,side,uplo) ) 

#define IBLAS_SyrkSimple(This,A,C,alpha,beta,layout,uplo,transA)	\
    ( (This)->lpVtbl -> SyrkSimple(This,A,C,alpha,beta,layout,uplo,transA) ) 

#define IBLAS_Syr2kSimple(This,A,B,C,alpha,beta,layout,uplo,trans)	\
    ( (This)->lpVtbl -> Syr2kSimple(This,A,B,C,alpha,beta,layout,uplo,trans) ) 

#define IBLAS_TrmmSimple(This,A,B,alpha,layout,side,uplo,transA,diag)	\
    ( (This)->lpVtbl -> TrmmSimple(This,A,B,alpha,layout,side,uplo,transA,diag) ) 

#define IBLAS_TrsmSimple(This,A,B,alpha,layout,side,uplo,transA,diag)	\
    ( (This)->lpVtbl -> TrsmSimple(This,A,B,alpha,layout,side,uplo,transA,diag) ) 

#define IBLAS_GemvSimple(This,A,x,y,alpha,beta,layout,transA)	\
    ( (This)->lpVtbl -> GemvSimple(This,A,x,y,alpha,beta,layout,transA) ) 

#define IBLAS_GerSimple(This,x,y,A,alpha,layout)	\
    ( (This)->lpVtbl -> GerSimple(This,x,y,A,alpha,layout) ) 

#define IBLAS_SymvSimple(This,A,x,y,alpha,beta,layout,uplo)	\
    ( (This)->lpVtbl -> SymvSimple(This,A,x,y,alpha,beta,layout,uplo) ) 

#define IBLAS_SyrSimple(This,x,A,alpha,layout,uplo)	\
    ( (This)->lpVtbl -> SyrSimple(This,x,A,alpha,layout,uplo) ) 

#define IBLAS_Syr2Simple(This,x,y,A,alpha,layout,uplo)	\
    ( (This)->lpVtbl -> Syr2Simple(This,x,y,A,alpha,layout,uplo) ) 

#define IBLAS_TrmvSimple(This,A,x,layout,uplo,transA,diag)	\
    ( (This)->lpVtbl -> TrmvSimple(This,A,x,layout,uplo,transA,diag) ) 

#define IBLAS_TrsvSimple(This,A,x,layout,uplo,transA,diag)	\
    ( (This)->lpVtbl -> TrsvSimple(This,A,x,layout,uplo,transA,diag) ) 

#define IBLAS_Axpy(This,n,alpha,x,incX,y,incY)	\
    ( (This)->lpVtbl -> Axpy(This,n,alpha,x,incX,y,incY) ) 

#define IBLAS_Dot(This,n,x,incX,y,incY,result)	\
    ( (This)->lpVtbl -> Dot(This,n,x,incX,y,incY,result) ) 

#define IBLAS_Nrm2(This,n,x,incX,result)	\
    ( (This)->lpVtbl -> Nrm2(This,n,x,incX,result) ) 

#define IBLAS_Asum(This,n,x,incX,result)	\
    ( (This)->lpVtbl -> Asum(This,n,x,incX,result) ) 

#define IBLAS_Scal(This,n,alpha,x,incX)	\
    ( (This)->lpVtbl -> Scal(This,n,alpha,x,incX) ) 

#define IBLAS_Copy(This,n,x,incX,y,incY)	\
    ( (This)->lpVtbl -> Copy(This,n,x,incX,y,incY) ) 

#define IBLAS_Swap(This,n,x,incX,y,incY)	\
    ( (This)->lpVtbl -> Swap(This,n,x,incX,y,incY) ) 

#define IBLAS_Iamax(This,n,x,incX,index1based)	\
    ( (This)->lpVtbl -> Iamax(This,n,x,incX,index1based) ) 

#define IBLAS_Rot(This,n,x,incX,y,incY,c,s)	\
    ( (This)->lpVtbl -> Rot(This,n,x,incX,y,incY,c,s) ) 

#define IBLAS_Rotg(This,a,b,c,s)	\
    ( (This)->lpVtbl -> Rotg(This,a,b,c,s) ) 

#define IBLAS_Rotm(This,n,x,incX,y,incY,param)	\
    ( (This)->lpVtbl -> Rotm(This,n,x,incX,y,incY,param) ) 

#define IBLAS_Rotmg(This,d1,d2,x1,y1,param)	\
    ( (This)->lpVtbl -> Rotmg(This,d1,d2,x1,y1,param) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBLAS_INTERFACE_DEFINED__ */



#ifndef __COMBLASLib_LIBRARY_DEFINED__
#define __COMBLASLib_LIBRARY_DEFINED__

/* library COMBLASLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_COMBLASLib;

EXTERN_C const CLSID CLSID_BLAS;

#ifdef __cplusplus

class DECLSPEC_UUID("e8f3aed3-eec4-48ab-9925-c13253d4c396")
BLAS;
#endif
#endif /* __COMBLASLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize64(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal64(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal64(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree64(     unsigned long *, LPSAFEARRAY * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


