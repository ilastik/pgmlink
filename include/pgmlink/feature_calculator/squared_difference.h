#ifndef FEATURE_CALCULATOR_SQUARED_DIFFERENCE_H
#define FEATURE_CALCULATOR_SQUARED_DIFFERENCE_H

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
//// SquaredDifferenceCalculator
////
class SquaredDifferenceCalculator : public FeatureCalculator
{
public:
    static const std::string name_;

    virtual PGMLINK_EXPORT ~SquaredDifferenceCalculator();
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& f1, const feature_array& f2) const;
    virtual PGMLINK_EXPORT const std::string& name() const;
};


} // namespace feature_extraction

} // namespace pgmlink


#endif /* FEATURE_CALCULATOR_SQUARED_DIFFERENCE_H */

