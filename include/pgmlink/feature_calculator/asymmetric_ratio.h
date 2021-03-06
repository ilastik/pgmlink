#ifndef FEATURE_CALCULATOR_ASYMMETRIC_RATIO_H
#define FEATURE_CALCULATOR_ASYMMETRIC_RATIO_H

// stl
#include <string>

// pgmlink
#include "pgmlink/features/feature.h"
#include "pgmlink/feature_calculator/base.h"
#include "pgmlink/features/feature_extraction.h"

namespace pgmlink
{

namespace feature_extraction
{
////
//// AssymetricRatioCalculator
////
class AsymmetricRatioCalculator : public FeatureCalculator
{
public:
    static PGMLINK_EXPORT const std::string name_;

    virtual PGMLINK_EXPORT ~AsymmetricRatioCalculator();
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& f1, const feature_array& f2) const;
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& f1, const feature_array& f2, const feature_array& f3) const;
    virtual PGMLINK_EXPORT const std::string& name() const;
};


} // namespace feature_extraction

} // namespace pgmlink


#endif /* FEATURE_CALCULATOR_ASYMMETRIC_RATIO_H */

