#ifndef FEATURESTORE_H
#define FEATURESTORE_H

#include <iostream>
#include <map>
#include <vector>

#include "../pgmlink_export.h"
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/shared_ptr.hpp>

namespace pgmlink
{

class Traxel;

//
// feature data structures
//

typedef double feature_type;
typedef std::vector<feature_type> feature_array;
typedef std::vector<feature_array> feature_arrays;
typedef std::map<std::string, feature_array> FeatureMap;
//
// FeatureStore
//
class FeatureStore
{
public:
    PGMLINK_EXPORT FeatureStore();

    /// Get the features corresponding to a single traxel given by timestep and id
    PGMLINK_EXPORT FeatureMap& get_traxel_features(int timestep, unsigned int id);

    /// Get the features corresponding to a single traxel
    PGMLINK_EXPORT FeatureMap& get_traxel_features(const Traxel& traxel);

    /// Get the features corresponding to a pair of traxels (e.g. a Move)
    PGMLINK_EXPORT FeatureMap& get_traxel_features(const Traxel& traxel_a, const Traxel& traxel_b);

    /// Get the features corresponding to a triplet (e.g. a division)
    PGMLINK_EXPORT FeatureMap& get_traxel_features(const Traxel& traxel_a, const Traxel& traxel_b, const Traxel& traxel_c);

    /// Generic traxel feature retrieval
    PGMLINK_EXPORT FeatureMap& get_traxel_features(const std::vector<const Traxel*>& traxels);

    /// dump contents to a stream
    PGMLINK_EXPORT void dump(std::ostream &stream);

    /// dump features of a traxel to a stream
    PGMLINK_EXPORT void dump(int timestep, unsigned int id, std::ostream &stream);
private:
    /// Store a feature map for each traxel, but also for pairs,triplets,... of traxels,
    /// to be able to save features corresponding to moves,divisions,.. as well.
    typedef std::pair<int, unsigned int> TimeId;
    typedef std::map< std::vector<TimeId>, FeatureMap> TraxelFeatureMap;
    TraxelFeatureMap traxel_feature_map_;

    // boost serialize for FeatureStore
    friend class boost::serialization::access;
    template< typename Archive >
    void serialize( Archive&, const unsigned int /*version*/ );
};

template< typename Archive >
void FeatureStore::serialize( Archive& ar, const unsigned int /*version*/ )
{
    ar & traxel_feature_map_;
}

} // end namespace pgmlink
#endif // FEATURESTORE_H
