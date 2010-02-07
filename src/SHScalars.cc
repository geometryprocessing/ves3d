/**
 * @file   SHScalars.cc
 * @author Abtin Rahimian <arahimian@acm.org>
 * @date   Tue Jan 26 13:48:19 2010
 * 
 * @brief  Implementation for the SHScalars class. 
 */

//Constructors
template<typename ScalarType> 
SHScalars<ScalarType>::SHScalars() :
    data_(NULL), p_(0), number_of_functions_(0){}

template<typename ScalarType> 
SHScalars<ScalarType>::SHScalars(int p_in, int num_funs_in) : 
    p_(p_in), number_of_functions_(num_funs_in)
{
    data_ = new ScalarType[GetDataLength()];
}

template<typename ScalarType> 
SHScalars<ScalarType>::SHScalars(int p_in, int num_funs_in, const ScalarType *data_in) :
    p_(p_in), number_of_functions_(num_funs_in)
{
    data_ = new ScalarType[GetDataLength()];
    SetData(data_in);
}

//Destructor
template<typename ScalarType> 
SHScalars<ScalarType>::~SHScalars()
{
    delete[] data_;
    data_ = NULL;
}

//Utility functions
template<typename ScalarType> 
int SHScalars<ScalarType>::GetFunLength() const
{
    return(2*p_*(p_ + 1));
}

template<typename ScalarType> 
int SHScalars<ScalarType>::GetDataLength() const
{
    return(number_of_functions_ * GetFunLength());
}

template<typename ScalarType> 
void SHScalars<ScalarType>::SetData(const ScalarType *data_in)
{
    ///If memory is not allocated return.
    if ( data_ == NULL )
    {
        throw std::range_error(" data_ is not yet allocated."); 
    }
    
    int data_length = GetDataLength();
    for(int idx=0;idx<data_length;++idx)
        data_[idx] = data_in[idx];
}

template<typename ScalarType> 
const ScalarType* SHScalars<ScalarType>::GetFunctionAt(int fun_idx_in) const
{
    if ( data_ == NULL )
    {
        throw std::range_error(" data_ is not yet allocated."); 
    }
    else if ( fun_idx_in>=number_of_functions_ || fun_idx_in<0 )
    {
        throw std::range_error(" function index fun_idx_in is out of range.");
    }
    return(data_ + fun_idx_in*GetFunLength());
}

template<typename ScalarType> 
void SHScalars<ScalarType>::SetFunctionAt(const ScalarType *fun_in, int fun_idx_in)
{
    if ( data_ == NULL )
    {
        throw std::range_error(" data_ is not yet allocated."); 
    }
    else if ( fun_idx_in>=number_of_functions_ || fun_idx_in<0 )
    {
        throw std::range_error(" function index fun_idx_in is out of range.");
    }
    int fun_length = GetFunLength();
    ScalarType *fun_head = data_ + fun_idx_in*GetFunLength();

    for(int idx=0;idx<fun_length;++idx)
        *(fun_head+idx) = fun_in[idx];

}

template<typename ScalarType> 
void AxPy(ScalarType a_in, const SHScalars<ScalarType>& x_in,
    const SHScalars<ScalarType>& y_in, SHScalars<ScalarType>& res_out)
{
    int data_length = res_out.GetDataLength();
    for(int idx=0;idx<data_length;++idx)
        res_out.data_[idx] = a_in*x_in.data_[idx] + y_in.data_[idx];
}

template<typename ScalarType> 
void AxPy(ScalarType a_in, const SHScalars<ScalarType>& x_in,
    ScalarType y_in, SHScalars<ScalarType>& res_out)
{
    int data_length = res_out.GetDataLength();
    for(int idx=0;idx<data_length;++idx)
        res_out.data_[idx] = a_in*x_in.data_[idx] + y_in;
}

template<typename ScalarType> 
void xTy(const SHScalars<ScalarType>& x_in, const SHScalars<ScalarType>& y_in, 
    SHScalars<ScalarType>& xTy_out)
{
    int data_length = xTy_out.GetDataLength();
    for(int idx=0;idx<data_length;++idx)
        xTy_out.data_[idx] = x_in.data_[idx]*y_in.data_[idx];
}

template<typename ScalarType> 
void xDy(const SHScalars<ScalarType>& x_in, const SHScalars<ScalarType>& y_in, 
    SHScalars<ScalarType>& xDy_out)
{
    int data_length = xDy_out.GetDataLength();
    for(int idx=0;idx<data_length;++idx)
        xDy_out.data_[idx] = x_in.data_[idx]/y_in.data_[idx];

}
