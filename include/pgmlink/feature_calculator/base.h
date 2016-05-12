#ifndef FEATURE_CALCULATOR_BASE_H
#define FEATURE_CALCULATOR_BASE_H

// stl
#include <string>

// pgmlink
#include "pgmlink/features/feature.h"
#include "pgmlink/feature_calculator/base.h"
#include "pgmlink/features/feature_extraction.h"
#include "../../pgmlink_export.h"

namespace pgmlink
{


namespace feature_extraction
{
////
//// class FeatureCalculator
////
class FeatureCalculator
{
public:
    virtual PGMLINK_EXPORT ~FeatureCalculator();
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& /* f1 */ ) const;
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& /* f1 */, const feature_array& /* f2 */) const;
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& /* f1 */, const feature_array& /* f2 */, const feature_array& /* f3 */) const;
    virtual PGMLINK_EXPORT const std::string& name() const;

    PGMLINK_EXPORT bool operator==(const FeatureCalculator& other);
    PGMLINK_EXPORT bool operator!=(const FeatureCalculator& other);

private:
    static const std::string name_;
};

} // namespace feature_extraction

} // namespace pgmlink


#endif /* FEATURE_CALCULATOR_BASE_H */

