/**
   @file
   @ingroup tracking
   @brief predicting cellness
*/

#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H
#include <string>
#include <vector>

#include <vigra/multi_array.hxx>
#include <vigra/hdf5impex.hxx>
#include <vigra/random_forest.hxx>
#include <vigra/random_forest_hdf5_impex.hxx>

#include "pgmlink/traxels.h"
#include "pgmlink_export.h"

namespace pgmlink
{
namespace RF
{
typedef unsigned int RF_LABEL_TYPE;

typedef double feature_type; // all feature values have this type
typedef vigra::MultiArray<1, feature_type> feature_array;
typedef vigra::MultiArrayShape<1>::type array_shape;

typedef vigra::MultiArrayShape<2>::type matrix_shape;

/** getRandomForest
  * Load a trained random forest from a file.
  */
PGMLINK_EXPORT vigra::RandomForest<RF_LABEL_TYPE> getRandomForest(std::string filename);



/** createFeatureVector
  * Create a Random Forest compatible feature vector from Traxel
  * provided in 'tr'. 'selFeatures' determines which features are
  * selected and their order
  */
PGMLINK_EXPORT vigra::MultiArray<2, float> createFeatureVector(const Traxel &tr, const std::vector<std::string> &selFeatures);



/** getProbabilities
  *
  * Calculate the class probabilities by a Random Forest
  *
  * Input parameters:
  *
  * features: MultiArray<2,float> nSamples*nFeatures feature matrix
  * rf: trained RandomForest.
  *
  * Return value:
  * vigra::MultiArray<2,double> nSamples*nClasses probabilitiy matrix
  */
PGMLINK_EXPORT vigra::MultiArray<2, double> getProbabilities(const vigra::MultiArray<2, float> &features, const vigra::RandomForest<RF_LABEL_TYPE> &rf);



/**
  * Predict probabilities for all tracklets in 'ts' using random forest 'rf'
  * and features 'sel'. The probability of being label 'lbl' will be saved
  * as a feature with name 'lblname'.
  */
PGMLINK_EXPORT int predictTracklets( Traxels &ts,
                      vigra::RandomForest<RF_LABEL_TYPE> &rf,
                      std::vector<std::string> &sel,
                      unsigned int lbl,
                      std::string lblname);

PGMLINK_EXPORT void predict_traxels( TraxelStore&,
                      const vigra::RandomForest<RF_LABEL_TYPE>&,
                      const std::vector<std::string>& feature_names,
                      unsigned int cls,
                      const std::string& output_feat_name);

PGMLINK_EXPORT double predict( const Traxel&,
                const vigra::RandomForest<RF_LABEL_TYPE>&,
                const std::vector<std::string>& feature_names,
                unsigned int cls);



/** loadTracklets
  *
  * Load the data (ID and features) of all tracklets in file 'filename'.
  * Only tracklets containing data are stored (no dead labels)
  */
PGMLINK_EXPORT Traxels loadTracklets(std::string filename);

}
}

#endif /* RANDOMFOREST_H */
