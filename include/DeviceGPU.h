/**
 * @file   DeviceGPU.h
 * @author Abtin Rahimian <arahimian@acm.org>
 * @date   Mon Mar  1 13:04:33 2010
 */

#ifndef _DEVICEGPU_H_
#define _DEVICEGPU_H_

#include "Device.h"
#include <iostream>
#include <cassert>
#include "cuda_runtime.h"
#include "CudaKernels.h"
#include <cublas.h>
#include "CudaSht.h"
#include "OperatorsMats.h"

using namespace std;

struct GpuTime{
    static double gemm_time;
    static double stokes_time;
    static double xvpb_time;
    static double xy_time;
    static double DotProduct_time;
    static double Shift_time;
};

double GpuTime::gemm_time = 0;
double GpuTime::stokes_time = 0;
double GpuTime::xvpb_time = 0;
double GpuTime::xy_time = 0;
double GpuTime::DotProduct_time = 0;
double GpuTime::Shift_time = 0;

///The GPU subclass of the Device class.
template<typename T>
class DeviceGPU : public Device<T>
{
  public:
    CudaSht sht_;
    CudaSht sht_up_sample_;
    int p_, p_up_;

    DeviceGPU(int cuda_device_id = 0);
    ~DeviceGPU();

    virtual T* Malloc(unsigned long int length);
    virtual void Free(T* ptr);
    virtual T* Calloc(unsigned long int num);
    virtual T* Memcpy(T* destination, const T* source, unsigned long int num, enum MemcpyKind kind);
    virtual T* Memset (T *ptr, int value, size_t num);
    
    virtual T* DotProduct(const T* u_in, const T* v_in, int stride, int num_surfs, T* x_out);
    virtual T* CrossProduct(const T* u_in, const T* v_in, int stride, int num_surfs, T* w_out);
    virtual T* Sqrt(const T* x_in, int stride, int num_surfs, T* sqrt_out);

    virtual T* xInv(const T* x_in, int stride, int num_surfs, T* xInv_out);
    virtual T* xy(const T* x_in, const T* y_in, int stride, int num_surfs, T* xy_out);
    virtual T* xyInv(const T* x_in, const T* y_in, int stride, int num_surfs, T* xyInv_out);
    virtual T* uyInv(const T* u_in, const T* y_in, int stride, int num_surfs, T* uyInv_out);
    virtual T* axpy(T a_in, const T* x_in, const T* y_in, int stride, int num_surfs , T* axpy_out);
    virtual T* axpb(T a_in, const T*  x_in, T b_in, int stride, int num_surfs , T*  axpb_out);
    virtual T* avpw(const T* a_in, const T*  v_in, const T*  w_in, int stride, int num_surfs, T*  avpw_out);
    virtual T* xvpw(const T* x_in, const T*  v_in, const T*  w_in, int stride, int num_surfs, T*  xvpw_out);
    virtual T* xvpb(const T* x_in, const T*  v_in, T b_in, int stride, int num_surfs, T*  xvpb_out);

    virtual T* Reduce(const T *x_in, const T *w_in, const T *quad_w_in, int stride, int num_surfs, T  *int_x_dw);
    virtual T* gemm(const char *transA, const char *transB, const int *m, const int *n, const int *k, const T *alpha, 
        const T *A, const int *lda, const T *B, const int *ldb, const T *beta, T *C, const int *ldc);

    virtual T* CircShift(const T *arr_in, int n_vecs, int vec_length, int shift, T *arr_out);
    
    virtual void DirectStokes(int stride, int n_surfs, int trg_idx_head, int trg_idx_tail, 
        const T *qw, const T *trg, const T *src, const T *den, T *pot);

    virtual T* ShufflePoints(T *x_in, CoordinateOrder order_in, int stride, int n_surfs, T *x_out);
    virtual T Max(T *x_in, int length);

    virtual void InitializeSHT(OperatorsMats<T> &mats);
    virtual void ShAna(const T *x_in, T *work_arr, int p, int num_funs, T *sht_out);
    virtual void ShSyn(const T *shc_in, T *work_arr, int p, int num_funs, T *y_out);
    virtual void ShSynDu(const T *shc_in, T *work_arr, int p, int num_funs, T *xu_out);
    virtual void ShSynDv(const T *shc_in, T *work_arr, int p, int num_funs, T *xv_out);
    virtual void ShSynDuu(const T *shc_in, T *work_arr, int p, int num_funs, T *xuu_out);
    virtual void ShSynDvv(const T *shc_in, T *work_arr, int p, int num_funs, T *xvv_out);
    virtual void ShSynDuv(const T *shc_in, T *work_arr, int p, int num_funs, T *xuv_out);
    virtual void AllDerivatives(const T *x_in, T *work_arr, int p, int num_funs, T* shc_x, T *Dux_out, T *Dvx_out, 
        T *Duux_out, T *Duvx_out, T *Dvvx_out);

    virtual void FirstDerivatives(const T *x_in, T *work_arr, int p, int num_funs, T* shc_x, T *Dux_out, T *Dvx_out);

    ///Filter
    virtual void Filter(int p, int n_funs, const T *x_in, const T *alpha, T* work_arr, T *shc_out, T *x_out);
    virtual void ScaleFreqs(int p, int n_funs, const T *inputs, const T *alphas, T *outputs);
    virtual void Resample(int p, int n_funs, int q, const T *shc_p, T *shc_q);
    virtual void InterpSh(int p, int n_funs, const T *x_in, T* work_arr, T *shc, int q, T *x_out);
};

#include "DeviceGPU.cc"
#endif //_DEVICEGPU_H_
