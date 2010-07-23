template<typename Container>
RepartitionGateway<Container>::RepartitionGateway(GlobalRepart_t fun_ptr, 
    int num_threads) :
    g_repart_handle_(fun_ptr),
    num_threads_(num_threads),
    each_thread_nv_(new size_t[num_threads_]),
    each_thread_idx_(new size_t[num_threads_ + 1]),
    nv_(0),
    capacity_(0),
    all_pos_(NULL),
    all_tension_(NULL),
    posr_(NULL),
    tensionr_(NULL),
    nvr_(0)
{
    each_thread_idx_[0] = 0;
}

template<typename Container>
RepartitionGateway<Container>::~RepartitionGateway()
{
    delete[] each_thread_nv_;
    delete[] each_thread_idx_;

    delete[] all_pos_;
    delete[] all_tension_;
}    

    
template<typename Container>
void RepartitionGateway<Container>::operator()(Container &coord, 
    Container &tension, void* user_ptr) const
{  
    if ( g_repart_handle_ == NULL ) 
        return;

    //Getting the sizes
    size_t nv(tension.getNumSubs());
    size_t stride(tension.getStride());
    size_t idx(this->getCpyIdx(nv, stride));
    
#pragma omp barrier
    {
        checkContainersSize(stride);
    }
    
    //Copying to the host 
    Container::getDevice().Memcpy(all_pos_ + DIM * idx, coord.begin(),
        coord.size() * sizeof(value_type), MemcpyDeviceToHost);
    
    Container::getDevice().Memcpy(all_tension_ + idx, tension.begin(),
        tension.size() * sizeof(value_type), MemcpyDeviceToHost);

    // call user interaction routine
#pragma omp barrier 
    
#pragma omp master 
    g_repart_handle_(nv_, stride, all_pos_, all_tension_, &nvr_, 
        &posr_, &tensionr_, user_ptr);

#pragma omp barrier 
    
    nv = getNvShare();
    idx = this->getCpyIdx(nv, stride);
    coord.resize(DIM * nv);
    tension.resize(nv);
    
    //Copying back the new values to the device(s)
    Container::getDevice().Memcpy(coord.begin(), posr_ + DIM * idx, 
        coord.size() * sizeof(value_type), MemcpyHostToDevice);
    
    Container::getDevice().Memcpy(tension.begin(), tensionr_ + idx, 
        tension.size() * sizeof(value_type), MemcpyHostToDevice);
    
#pragma omp master
    {
        delete[] posr_;
        delete[] tensionr_;
    }
}

template<typename Container>
size_t RepartitionGateway<Container>::getCpyIdx(size_t this_thread_nv, 
    size_t stride) const
{
    int threadNum = omp_get_thread_num();
    each_thread_nv_[threadNum] = this_thread_nv;
    
#pragma omp barrier
    {
        if ( threadNum == 0 )
        {
            nv_ = 0;
                        
            for(int ii=1; ii<=num_threads_; ++ii)
            {
                nv_ += each_thread_nv_[ii-1];
                each_thread_idx_[ii] = each_thread_idx_[ii-1] + 
                    stride * each_thread_nv_[ii-1]; 
            }
        }
    }

#pragma omp barrier
    
    return(each_thread_idx_[threadNum]);
}

template<typename Container>
size_t RepartitionGateway<Container>::getNvShare() const
{
    size_t nv(nvr_/num_threads_);
    
    if ( omp_get_thread_num() == 0 )
        nv = nvr_ - (num_threads_ - 1) * nv;
    
    return nv;
}

template<typename Container>
void RepartitionGateway<Container>::checkContainersSize(size_t stride) const
{
#pragma omp master
    {
        if ( capacity_ < nv_ )
        {

            delete[] all_pos_;
            all_pos_ = new value_type[nv_ * DIM * stride];

            delete[] all_tension_;
            all_tension_ = new value_type[nv_ * stride];
                    
            capacity_ = nv_;
        }
    }
#pragma omp barrier
}


