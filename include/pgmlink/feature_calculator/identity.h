#ifndef FEATURE_CALCULATOR_IDENTITY_H
#define FEATURE_CALCULATOR_IDENTITY_H

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
//// IdentityCalculator
////
class IdentityCalculator : public FeatureCalculator
{
public:
    virtual PGMLINK_EXPORT ~IdentityCalculator();
    virtual PGMLINK_EXPORT feature_array calculate(const feature_array& f1) const;
    virtual PGMLINK_EXPORT const std::string& name() const;
private:
    static const std::string name_;
};

} // namespace feature_extraction

} // namespace pgmlink


#endif /* FEATURE_CALCULATOR_IDENTITY_H */

