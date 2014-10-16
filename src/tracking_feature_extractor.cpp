#include "pgmlink/tracking_feature_extractor.h"

namespace pgmlink {
namespace features {

TrackingFeatureExtractor::TrackingFeatureExtractor(HypothesesGraph &graph):
    graph_(graph)
{
}

void TrackingFeatureExtractor::get_feature_vector(TrackingFeatureExtractor::JointFeatureVector &feature_vector) const
{
    feature_vector.clear();
    feature_vector.insert(feature_vector.begin(), joint_feature_vector_.begin(), joint_feature_vector_.end());
}

const std::string TrackingFeatureExtractor::get_feature_description(size_t feature_index) const
{
    return feature_descriptions_[feature_index];
}

void TrackingFeatureExtractor::compute_features()
{
    compute_velocity_features();
    //compute_size_difference_features();
}

void TrackingFeatureExtractor::compute_velocity_features()
{
    // extract all tracks
    TrackTraxels track_extractor;
    std::vector<ConstTraxelRefVector> track_traxels = track_extractor(graph_);

    size_t num_velocity_entries = 0;
    double sum_of_squared_velocities = 0;
    double min_squared_velocity = std::numeric_limits<double>::max();
    double max_squared_velocity = 0.0;

    // for each track:
    for(auto track : track_traxels)
    {
        // only compute velocities if track is longer than 1 element
        if(track.size() < 2)
            continue;

        // extract positions
        TraxelsFeaturesIdentity position_extractor("com");
        FeatureMatrix positions;
        position_extractor.extract(track, positions);

        // compute velocity vector's squared magnitude for all pairs of positions
        SquaredDiffCalculator velocity_calculator;
        FeatureMatrix velocities;
        velocity_calculator.calculate(positions, velocities);

        // compute per track min/max/mean of velocity
        MinCalculator<1> min_calculator;
        FeatureMatrix min_velocity;
        min_calculator.calculate(velocities, min_velocity);
        min_squared_velocity = std::min(min_squared_velocity, double(min_velocity(0,0)));

        MaxCalculator<1> max_calculator;
        FeatureMatrix max_velocity;
        max_calculator.calculate(velocities, max_velocity);
        max_squared_velocity = std::max(max_squared_velocity, double(max_velocity(0,0)));

        MeanCalculator<1> mean_calculator;
        FeatureMatrix mean_velocity;
        mean_calculator.calculate(velocities, mean_velocity);

        // accumulate all velocities
        SumCalculator<1> sum_calculator;
        FeatureMatrix sum_velocity;
        sum_calculator.calculate(velocities, sum_velocity);

        sum_of_squared_velocities += sum_velocity(0,0);
        num_velocity_entries += track.size() - 1;
    }

    double mean_squared_velocity = sum_of_squared_velocities / num_velocity_entries;

    joint_feature_vector_.push_back(mean_squared_velocity);
    feature_descriptions_.push_back("Mean of all velocities (squared)");

    joint_feature_vector_.push_back(min_squared_velocity);
    feature_descriptions_.push_back("Min of all velocities (squared)");

    joint_feature_vector_.push_back(max_squared_velocity);
    feature_descriptions_.push_back("Max of all velocities (squared)");
}

void TrackingFeatureExtractor::compute_size_difference_features()
{
    throw std::runtime_error("not yet implemented");
}

} // end namespace features
} // end namespace pgmlink
