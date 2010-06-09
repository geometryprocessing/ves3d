/**
 * @file   Surface.h
 * @author Rahimian, Abtin <arahimian@acm.org>
 * @date   Tue Feb  2 13:23:11 2010
 * 
 * @brief  The declaration for the Surface class.
 */

#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "HelperFuns.h"
#include "SHTrans.h"
#include "GLIntegrator.h"

template <typename ScalarContainer, typename VectorContainer> 
class Surface
{
  public:
    typedef typename ScalarContainer::value_type value_type;
    typedef VectorContainer Vec;
    typedef ScalarContainer Sca;

    ///@todo add a default constructor
    Surface(const Vec& x_in);
    ~Surface();

    void setPosition(const Vec& x_in);
    Vec& getPositionModifiable();
    const Vec& getPosition() const;
    const Vec& getNormal() const;
    const Sca& getAreaElement() const;
    const Sca& getMeanCurv() const;
    const Sca& getGaussianCurv() const;

    void updateFirstForms() const;
    void updateAll() const;

    void grad(const Sca &f_in, Vec &grad_f_out) const;
    void div(const Vec &f_in, Sca &div_f_out) const;
        
    void area(Sca &area) const;
    void volume(Sca &vol) const;
    void getCenters(Vec &centers) const;

  private:
    Vec x_;
    mutable Vec normal_;

    mutable Sca w_;
    mutable Sca h_;
    mutable Sca k_;

    mutable Vec cu_;
    mutable Vec cv_;
 
    SHTrans<Sca> sht_;
    GaussLegendreIntegrator<Sca> integrator_;

    mutable bool position_has_changed_outside_;
    mutable bool first_forms_are_stale_;
    mutable bool second_forms_are_stale_;
    
    void checkContainers() const;
    Surface(Surface<Sca, Vec> const& s_in);
    Surface<Sca, Vec>& operator=(const Surface<Sca, Vec>& rhs);

    ///@todo these can be removed, but updateAll should be rewritten
    mutable Sca E, F, G;
  
    mutable queue<Sca*> scalar_work_q_;
    Sca* produceSca(const Vec &ref) const;
    void recycleSca(Sca* scp) const;
    mutable int checked_out_work_sca_;

    mutable queue<Vec*> vector_work_q_;
    Vec* produceVec(const Vec &ref) const;
    void recycleVec(Vec* vcp) const;
    mutable int checked_out_work_vec_;
};

#include "Surface.cc"

#endif //_SURFACE_H_
