/**
   @file
   @ingroup pgm
   @brief opengm extensions
*/

#ifndef GRAPHICAL_MODEL_H
#define GRAPHICAL_MODEL_H

#include <vector>

#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/loglinearmodel.hxx>
#include <opengm/inference/inference.hxx>
#include <opengm/functions/explicit_function.hxx>
#include <opengm/functions/decorator_weighted.hxx>
#include <opengm/functions/indicator_function.hxx>
#include <opengm/operations/adder.hxx>
#include <opengm/utilities/metaprogramming.hxx>

#include "pgmlink/hypotheses.h"
#include "pgmlink/graph.h"
#include "pgmlink/util.h"

namespace pgmlink {
namespace pgm {

typedef opengm::ExplicitFunction<double> ExplicitFunction;
typedef opengm::FunctionDecoratorWeighted< opengm::IndicatorFunction<double> > FeatureFunction;
typedef opengm::LoglinearModel<double,
  opengm::meta::TypeList<ExplicitFunction,
  opengm::meta::TypeList<FeatureFunction, opengm::meta::ListEnd > > > OpengmModel;


class OpengmModelDeprecated {
   public:
   typedef double Energy;
   typedef opengm::GraphicalModel<Energy, opengm::Adder> ogmGraphicalModel;
   typedef opengm::Factor<ogmGraphicalModel> ogmFactor;
   typedef opengm::Minimizer ogmAccumulator;
   typedef opengm::Inference<ogmGraphicalModel, ogmAccumulator> ogmInference;
   typedef opengm::meta::TypeAtTypeList<ogmGraphicalModel::FunctionTypeList, 0>::type ExplicitFunctionType;
   typedef ogmGraphicalModel::FunctionIdentifier FunctionIdentifier;
   
   OpengmModelDeprecated() {
      model_ = new ogmGraphicalModel();
   }
   ~OpengmModelDeprecated() {
      delete model_;
   }

   ogmGraphicalModel* Model() {return model_; }
   const ogmGraphicalModel* Model() const {return model_; }
 
   private:
   OpengmModelDeprecated(const OpengmModelDeprecated&);
   OpengmModelDeprecated& operator=(const OpengmModelDeprecated&);
   
   ogmGraphicalModel* model_;
};





template <typename OGM_FUNCTION>
 class OpengmFactor {
 public:
  typedef OGM_FUNCTION FunctionType;

  OpengmFactor( const FunctionType&, const std::vector<size_t>& ogm_var_indices );
  template <typename ITER>
    OpengmFactor( const FunctionType&, ITER first_ogm_idx, ITER last_ogm_idx );

  typename FunctionType::ValueType get_value( std::vector<size_t> coords ) const;
  template <typename OGM_GRAPHICAL_MODEL>
    void add_to( OGM_GRAPHICAL_MODEL& ) const;

  FunctionType& function() { return ogmfunction_; }
  const FunctionType& function() const { return ogmfunction_; }
  const std::vector<size_t>& var_indices() const { return vi_; }
  const std::vector<size_t>& var_order() const { return order_; }

 protected:
  FunctionType ogmfunction_;
  std::vector<size_t> vi_;
  std::vector<size_t> order_;
 };


 template <typename VALUE>
   class OpengmExplicitFactor : public OpengmFactor<opengm::ExplicitFunction<VALUE> > {
 public:
  typedef typename OpengmFactor<opengm::ExplicitFunction<VALUE> >::FunctionType FunctionType; 
  
  OpengmExplicitFactor( const std::vector<size_t>& ogm_var_indices, VALUE init=0, size_t states_per_var=2 );
  template <typename ITER>
    OpengmExplicitFactor( ITER first_ogm_idx, ITER last_ogm_idx, VALUE init=0, size_t states_per_var=2 );  
  
  void set_value( std::vector<size_t> coords, VALUE v);
  
 private:
  void init_( VALUE init, size_t states_per_var );
};    
 
 template <typename VALUE>
   class OpengmWeightedFeature
   : public OpengmFactor< opengm::FunctionDecoratorWeighted< opengm::IndicatorFunction<VALUE> > >
 {
  public:
    typedef typename opengm::FunctionDecoratorWeighted< opengm::IndicatorFunction<VALUE> > FunctionType;

    template<class IT1, class IT2>
      OpengmWeightedFeature(const std::vector<size_t>& ogm_var_indices,
			    IT1 shapeBegin, IT1 shapeEnd,
			    IT2 indicate,
			    VALUE indicate_value = 1., 
			    VALUE weight = 1.,
			    VALUE default_value = 0);

    template <typename OGM_LOGLINEARMODEL>
      void add_as_feature_to( OGM_LOGLINEARMODEL&, typename OGM_LOGLINEARMODEL::IndexType weight_index ) const;

    void weight( VALUE w );
    VALUE weight() const;

    void indicate_value( VALUE v );
    VALUE indicate_value() const;
    
    void default_value( VALUE v );
    VALUE default_value() const;
 };


/******************/
/* Implementation */
/******************/

////
//// class OpengmFactor
////
 template <typename OGM_FUNCTION>
   OpengmFactor<OGM_FUNCTION>::OpengmFactor( const FunctionType& f, const std::vector<size_t>& ogm_var_indices )
   : ogmfunction_(f), vi_(ogm_var_indices) {
  // store sorted order of indices
  indexsorter::sort_indices( vi_.begin(), vi_.end(), order_ );
}

 template <typename OGM_FUNCTION>
   template< typename ITER >
   OpengmFactor<OGM_FUNCTION>::OpengmFactor( const FunctionType& f, ITER first_ogm_idx, ITER last_ogm_idx )
   : ogmfunction_(f), vi_(first_ogm_idx, last_ogm_idx) {
  // store sorted order of indices
  indexsorter::sort_indices( vi_.begin(), vi_.end(), order_ );
 }

 template <typename OGM_FUNCTION>
   typename OpengmFactor<OGM_FUNCTION>::FunctionType::ValueType OpengmFactor<OGM_FUNCTION>::get_value( std::vector<size_t> coords ) const {
   if( coords.size() != vi_.size() ) {
     throw std::invalid_argument("OpengmFactor::get_value(): coordinate dimension differs from factor dimension");
   }
   indexsorter::reorder( coords, order_ );
   return ogmfunction_(coords.begin());
 }

 template <typename OGM_FUNCTION>
   template <typename OGM_GRAPHICAL_MODEL>
   void OpengmFactor<OGM_FUNCTION>::add_to( OGM_GRAPHICAL_MODEL& m ) const {
   std::vector<size_t> sorted_vi(vi_);
   std::sort(sorted_vi.begin(), sorted_vi.end());
   // opengm expects a monotonic increasing sequence
   if(!(m.isValidIndexSequence(sorted_vi.begin(), sorted_vi.end()))) {
     throw std::runtime_error("OpengmExplicitFactor::add_to(): invalid index sequence");
   }
   
   typename OGM_GRAPHICAL_MODEL::FunctionIdentifier id=m.addFunction( ogmfunction_ );
   m.addFactor(id, sorted_vi.begin(), sorted_vi.end());
 }

 
////
//// class OpengmExplicitFactor
////
 template <typename VALUE>
   OpengmExplicitFactor<VALUE>::OpengmExplicitFactor( const std::vector<size_t>& ogm_var_indices, VALUE init, size_t states_per_var )
   : OpengmFactor<opengm::ExplicitFunction<VALUE> >(opengm::ExplicitFunction<VALUE>(), ogm_var_indices) {
  init_( init, states_per_var );
}
 template <typename VALUE>
   template< typename ITER >
   OpengmExplicitFactor<VALUE>::OpengmExplicitFactor( ITER first_ogm_idx, ITER last_ogm_idx, VALUE init, size_t states_per_var )
   : OpengmFactor<opengm::ExplicitFunction<VALUE> >(opengm::ExplicitFunction<VALUE>(),first_ogm_idx, last_ogm_idx) {
  init_( init, states_per_var );
 }

 template <typename VALUE>  
   void OpengmExplicitFactor<VALUE>::set_value( std::vector<size_t> coords, VALUE v) {
   if( coords.size() != this->vi_.size() ) {
     throw std::invalid_argument("OpengmExplicitFactor::set_value(): coordinate dimension differs from factor dimension");
   }
   typename opengm::ExplicitFunction<VALUE>::iterator element( this->ogmfunction_ );
   size_t index;

   indexsorter::reorder( coords, this->order_ );
   this->ogmfunction_.coordinatesToIndex(coords.begin(), index);
   element[index] = v;
 }

 template <typename VALUE>
   void OpengmExplicitFactor<VALUE>::init_( VALUE init, size_t states_per_var ) {
   std::vector<size_t> shape( this->vi_.size(), states_per_var );
   this->ogmfunction_ = opengm::ExplicitFunction<VALUE>( shape.begin(), shape.end(), init );
 }



////
//// class OpengmWeightedFeature
////
template<class VALUE>
  template<class IT1, class IT2>
  OpengmWeightedFeature<VALUE>::OpengmWeightedFeature(
			  const std::vector<size_t>& ogm_var_indices,
		          IT1 shapeBegin, IT1 shapeEnd,
			  IT2 indicate,
			  VALUE indicate_value, 
			  VALUE weight,
			  VALUE default_value)
  : OpengmFactor<FunctionType >(FunctionType(new opengm::IndicatorFunction<VALUE>( shapeBegin,
										   shapeEnd,
										   indicate,
										   indicate_value,
										   default_value),
					     weight)
				, ogm_var_indices) {
   // replace function with a sorted variant
   std::vector<size_t> sorted_shape( shapeBegin, shapeEnd );
   indexsorter::reorder( sorted_shape, this->order_ );
   std::vector<size_t> sorted_indicate( this->ogmfunction_.innerFunction()->indicate() );
   indexsorter::reorder( sorted_indicate, this->order_ );

   this->ogmfunction_ = FunctionType(new opengm::IndicatorFunction<VALUE>( sorted_shape.begin(),
									   sorted_shape.end(),
									   sorted_indicate.begin(),
									   indicate_value,
									   default_value),
				     weight);
 }

template<class VALUE>
  template <typename OGM_LOGLINEARMODEL>
  void OpengmWeightedFeature<VALUE>::add_as_feature_to( OGM_LOGLINEARMODEL& m, typename OGM_LOGLINEARMODEL::IndexType weight_index ) const {
  std::vector<size_t> sorted_vi(this->vi_);
  std::sort(sorted_vi.begin(), sorted_vi.end());
  // opengm expects a monotonic increasing sequence
  if(!(m.isValidIndexSequence(sorted_vi.begin(), sorted_vi.end()))) {
    throw std::runtime_error("OpengmExplicitFactor::add_to(): invalid index sequence");
  }
  
  typename OGM_LOGLINEARMODEL::FunctionIdentifier id=m.addFeature( this->ogmfunction_, weight_index );
  m.addFactor(id, sorted_vi.begin(), sorted_vi.end());
 }

template<class VALUE>
  void OpengmWeightedFeature<VALUE>::weight( VALUE w ) {
  this->ogmfunction_.weight(w);
 }

template<class VALUE>
  VALUE OpengmWeightedFeature<VALUE>::weight() const {
  return this->ogmfunction_.weight();
 }

template<class VALUE>
  void OpengmWeightedFeature<VALUE>::indicate_value( VALUE v ) {
  this->ogmfunction_.innerFunction()->indicate_value(v);
 }
  
template<class VALUE>
  VALUE OpengmWeightedFeature<VALUE>::indicate_value() const {
  return this->ogmfunction_.innerFunction()->indicate_value();
 }
  
template<class VALUE>    
  void OpengmWeightedFeature<VALUE>::default_value( VALUE v ) {
  this->ogmfunction_.innerFunction()->default_value(v);
 }

template<class VALUE>
  VALUE OpengmWeightedFeature<VALUE>::default_value() const {
  return this->ogmfunction_.innerFunction()->default_value();
 }


} /* namespace pgm */
} /* namespace pgmlink */

#endif /* GRAPHICAL_MODEL_H */