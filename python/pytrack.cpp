#define PY_ARRAY_UNIQUE_SYMBOL pgmlink_pyarray
#define NO_IMPORT_ARRAY
#define BOOST_PYTHON_MAX_ARITY 30

#ifndef OPENGM_UNSIGNED_INTEGER_POW_HXX_
#define OPENGM_UNSIGNED_INTEGER_POW_HXX_
#endif

#include <vector>

#include "../include/pgmlink/field_of_view.h"
#include "../include/pgmlink/features/tracking_feature_extractor.h"
#include "../include/pgmlink/features/feature_extraction.h"
#include "../include/pgmlink/reasoner_constracking.h"
#include "../include/pgmlink/tracking.h"
#include "../include/pgmlink/structuredLearningTracking.h"
#include "../include/pgmlink/log.h"
#include <boost/utility.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python.hpp>

#include "pytemplated_pickle_suite.h"

using namespace std;
using namespace pgmlink;
using namespace boost::python;

#ifndef NO_ILP
vector<vector<Event> > pythonChaingraphTracking(ChaingraphTracking& tr, TraxelStore& ts)
{
    vector<vector<Event> > result;
    // release the GIL
    Py_BEGIN_ALLOW_THREADS
    try
    {
        result = tr(ts);
    }
    catch (std::exception& e)
    {
        Py_BLOCK_THREADS
        throw;
    }
    Py_END_ALLOW_THREADS
    return result;
}
#endif

vector<vector<vector<Event> > > pythonConsTracking(
        ConsTracking& tr,
        TraxelStore& ts,
        Parameter& param,
        TimestepIdCoordinateMapPtr& coordinates,
        double forbidden_cost,
        double ep_gap,
        bool   with_tracklets,
        double division_weight,
        double transition_weight,
        double disappearance_cost,
        double appearance_cost,
        bool   with_merger_resolution,
        int    n_dim,
        double transition_parameter,
        std::vector<int> max_traxel_id_at,
        double border_width,
        UncertaintyParameter& uncertaintyParam,
        bool   with_constraints,
        double cplex_timeout,
        object transition_classifier)
{
    vector<vector<vector<Event> > > result;
    // release the GIL
    Py_BEGIN_ALLOW_THREADS
    try
    {
        result = tr(ts,
                    param,
                    forbidden_cost,
                    ep_gap,
                    with_tracklets,
                    division_weight,
                    transition_weight,
                    disappearance_cost,
                    appearance_cost,
                    with_merger_resolution,
                    n_dim,
                    transition_parameter,
                    max_traxel_id_at,
                    border_width,
                    with_constraints,
                    uncertaintyParam,
                    cplex_timeout,
                    coordinates,
                    transition_classifier);
    }
    catch (std::exception& e)
    {
        Py_BLOCK_THREADS
        throw;
    }
    Py_END_ALLOW_THREADS
    return result;
}

#ifndef NO_ILP
vector<vector<vector<Event> > > pythonStructuredLearningTracking(
    StructuredLearningTracking& tr,
    TraxelStore& ts,
    TimestepIdCoordinateMapPtr& coordinates,
    double forbidden_cost,
    double ep_gap,
    bool   with_tracklets,
    double division_weight,
    double transition_weight,
    double disappearance_cost,
    double appearance_cost,
    bool   with_merger_resolution,
    int    n_dim,
    double transition_parameter,
    double border_width,
    UncertaintyParameter& uncertaintyParam,
    bool   with_constraints,
    double cplex_timeout,
    object transition_classifier)
{
    vector<vector<vector<Event> > > result;
    return result;
}
#endif

EventVectorVector python_resolve_mergers(ConsTracking& tracker,
        EventVectorVector& events,
        Parameter& param,
        TimestepIdCoordinateMapPtr& coordinates,
        double ep_gap,
        double transition_weight,
        bool with_tracklets,
        int n_dim,
        double transition_parameter,
        const std::vector<int>& max_traxel_id_at,
        bool with_constraints,
        object transitionClassifier)
{
    EventVectorVector result;
    // release the GIL
    Py_BEGIN_ALLOW_THREADS
    try
    {
        result = tracker.resolve_mergers(events, param, coordinates, ep_gap, transition_weight,
                                         with_tracklets, n_dim, transition_parameter, max_traxel_id_at, with_constraints, transitionClassifier);
    }
    catch (std::exception& e)
    {
        Py_BLOCK_THREADS
        throw;
    }
    Py_END_ALLOW_THREADS
    return result;
}

feature_array    (pgmlink::feature_extraction::FeatureExtractor::*extract1)(const Traxel& t1) const = &pgmlink::feature_extraction::FeatureExtractor::extract;
feature_array    (pgmlink::feature_extraction::FeatureExtractor::*extract2)(const Traxel& t1, const Traxel& t2) const = &pgmlink::feature_extraction::FeatureExtractor::extract;
feature_array    (pgmlink::feature_extraction::FeatureExtractor::*extract3)(const Traxel& t1, const Traxel& t2, const Traxel& t3) const = &pgmlink::feature_extraction::FeatureExtractor::extract;

std::vector<double> pyextractor_get_feature_vector(pgmlink::features::TrackingFeatureExtractor& fe)
{
    std::vector<double> feature_vector;
    fe.get_feature_vector(feature_vector);
    return feature_vector;
}

void export_track()
{

    class_<vector<Event> >("EventVector")
    .def(vector_indexing_suite<vector<Event> >())
    .def_pickle(TemplatedPickleSuite<EventVector>() )
    ;

    class_<vector<Traxel> >("TraxelVector")
    .def(vector_indexing_suite<vector<Traxel> >())
    .def_pickle(TemplatedPickleSuite< std::vector<Traxel> >() )
    ;

    class_<vector<vector<Event> > >("NestedEventVector")
    .def(vector_indexing_suite<vector<vector<Event> > >())
    .def_pickle(TemplatedPickleSuite<EventVectorVector>() )
    ;

    class_<vector<vector<vector<Event> > > >("NestedNestedEventVector")
    .def(vector_indexing_suite<vector<vector<vector<Event> > > >())
    .def_pickle(TemplatedPickleSuite<EventVectorVectorVector>() )
    ;

    class_<map<unsigned int, bool> >("DetectionMap")
    .def(map_indexing_suite<map<unsigned int, bool> >())
    .def_pickle(TemplatedPickleSuite< map<unsigned int, bool> >() )
    ;

    class_<vector<map<unsigned int, bool> > >("DetectionMapsVector")
    .def(vector_indexing_suite<vector<map<unsigned int, bool> > >())
    .def_pickle(TemplatedPickleSuite< vector< map<unsigned int, bool> > >() )
    ;

    class_<vector<double>>("WeightVector")
                        .def(vector_indexing_suite<vector<double>>())
                        .def_pickle(TemplatedPickleSuite< vector<double> >() )
                        ;

#ifndef NO_ILP
    class_<ChaingraphTracking>("ChaingraphTracking",
                               init<string, double, double, double, double,
                               bool, double, double, bool,
                               bool, double, double, double, double>(
                                   args("random_forest_filename", "appearance", "disappearance", "detection", "misdetection",
                                        "use_random_forest", "opportunity_cost", "forbidden_cost", "with_constraints",
                                        "fixed_detections", "mean_div_dist", "min_angle", "ep_gap", "n_neighbors"
                                       )))
    .def("__call__", &pythonChaingraphTracking)
    .def("detections", &ChaingraphTracking::detections)
    .def("set_with_divisions", &ChaingraphTracking::set_with_divisions)
    .def("set_cplex_timeout", &ChaingraphTracking::set_cplex_timeout)
    ;
#endif

    class_<Parameter>("ConservationTrackingParameter")
    .def("setWithNonNegativeWeights", &Parameter::setWithNonNegativeWeights)
    .def("register_detection_func", &Parameter::register_detection_func)
    .def("register_division_func", &Parameter::register_division_func)
    .def("register_transition_func", &Parameter::register_transition_func)
    .def("register_appearance_func", &Parameter::register_appearance_func)
    .def("register_disappearance_func", &Parameter::register_disappearance_func)
    .def("register_motion_model3_func", &Parameter::register_motion_model3_func)
    .def("register_motion_model4_func", &Parameter::register_motion_model4_func)
    .def_readwrite("max_number_objects", &Parameter::max_number_objects)
    .def_readwrite("motion_model3_default", &Parameter::motion_model3_default)
    .def_readwrite("motion_model4_default", &Parameter::motion_model4_default)
    .def_readwrite("forbidden_cost", &Parameter::forbidden_cost)
    .def_readwrite("ep_gap", &Parameter::ep_gap)
    .def_readwrite("with_tracklets", &Parameter::with_tracklets)
    .def_readwrite("with_divisions", &Parameter::with_divisions)
    .def_readwrite("with_misdetections_allowed", &Parameter::with_misdetections_allowed)
    .def_readwrite("with_appearance", &Parameter::with_appearance)
    .def_readwrite("with_disappearance", &Parameter::with_disappearance)
    .def_readwrite("transition_parameter", &Parameter::transition_parameter)
    .def_readwrite("with_constraints", &Parameter::with_constraints)
    .def_readwrite("uncertainty_param", &Parameter::uncertainty_param)
    .def_readwrite("cplex_timeout", &Parameter::cplex_timeout)
    .def_readwrite("division_weight", &Parameter::division_weight)
    .def_readwrite("detection_weight", &Parameter::detection_weight)
    .def_readwrite("transition_weight", &Parameter::transition_weight)
    .def_readwrite("border_width", &Parameter::border_width)
    .def_readwrite("transition_classifier", &Parameter::transition_classifier)
    .def_readwrite("with_optical_correction", &Parameter::with_optical_correction)
    .def_readwrite("solver", &Parameter::solver)
    .def_readwrite("num_threads", &Parameter::num_threads)
    .def_readwrite("max_number_paths", &Parameter::max_number_paths)
    .def_readwrite("with_swap", &Parameter::with_swap)
    ;

    class_<ConsTracking>("ConsTracking",
                         init<unsigned int, bool, double, double, bool, double, string, FieldOfView, string, SolverType, unsigned int>(
                             args("max_number_objects",
                                  "size_dependent_detection_prob",
                                  "avg_obj_size",
                                  "max_neighbor_distance",
                                  "with_division",
                                  "division_threshold",
                                  "detection_rf_filename",
                                  "fov",
                                  "event_vector_dump_filename",
                                  "solver_type",
                                  "spatial dimension")))
    .def(init<boost::shared_ptr<HypothesesGraph>,
         TraxelStore&,
         Parameter&,
         UncertaintyParameter,
         FieldOfView,
         bool,
         double,
         double,
         double>())
    .def("__call__", &pythonConsTracking)
    .def("buildGraph", &ConsTracking::build_hypo_graph)
    .def("track", &ConsTracking::track)
    .def("track", &ConsTracking::track_from_param)
    .def("plot_hypotheses_graph", &ConsTracking::plot_hypotheses_graph)
    .def("resolve_mergers", &python_resolve_mergers)
    .def("detections", &ConsTracking::detections)
    .def("get_hypotheses_graph", &ConsTracking::get_hypo_graph)
    .def("get_resolved_hypotheses_graph", &ConsTracking::get_resolved_hypotheses_graph)
    .def("createStructuredLearningFiles", &ConsTracking::createStructuredLearningFiles)
    .def("setTrackLabelingExportFile", &ConsTracking::setTrackLabelingExportFile)
    .def("writeStructuredLearningFiles", &ConsTracking::writeStructuredLearningFiles)
    .def("learnTrackingWeights", &ConsTracking::learnTrackingWeights)
    .def("HamminglossOfFiles", &ConsTracking::hammingloss_of_files)
    .def("save_ilp_solutions", &ConsTracking::save_ilp_solutions)
    .def("get_conservation_tracking_parameters", &ConsTracking::get_conservation_tracking_parameters)
    .def("addLabels", &ConsTracking::addLabels)
    .def("addAppearanceLabel", &ConsTracking::addAppearanceLabel)
    .def("addDisappearanceLabel", &ConsTracking::addDisappearanceLabel)
    .def("addDivisionLabel", &ConsTracking::addDivisionLabel)
    .def("addArcLabel", &ConsTracking::addArcLabel)
    .def("addFirstLabels", &ConsTracking::addFirstLabels)
    .def("addLastLabels", &ConsTracking::addLastLabels)
    .def("addIntermediateLabels", &ConsTracking::addIntermediateLabels)
    ;

    class_<ConservationTracking, boost::noncopyable>("ConservationTracking",
            init<Parameter>(args("parameters")))
    .def("perturbedInference", &ConservationTracking::perturbedInference)
    .def("fixLabeledAppearanceNodes", &ConservationTracking::enableFixingLabeledAppearanceNodes)
    ;

    enum_<SolverType>("ConsTrackingSolverType")
#ifdef WITH_DPCT
    .value("DynProgSolver", SolverType::DynProgSolver)
    .value("FlowSolver", SolverType::FlowSolver)
#endif
#ifndef NO_ILP
    .value("CplexSolver", SolverType::CplexSolver)
    .value("DPInitCplexSolver", SolverType::DPInitCplexSolver)
    .value("FlowInitCplexSolver", SolverType::FlowInitCplexSolver)
#endif
    ;

    enum_<Event::EventType>("EventType")
    .value("Move", Event::Move)
    .value("Division", Event::Division)
    .value("Appearance", Event::Appearance)
    .value("Disappearance", Event::Disappearance)
    .value("Merger", Event::Merger)
    .value("ResolvedTo", Event::ResolvedTo)
    .value("MultiFrameMove", Event::MultiFrameMove)
    .value("Void", Event::Void)
    ;

    class_<pgmlink::feature_extraction::FeatureExtractor>("FeatureExtractor",
            init<string, string>(args("Calculator, Feature")))
    .def("extract", extract1)
    .def("extract", extract2)
    .def("extract", extract3)
    ;

    class_<pgmlink::features::TrackingFeatureExtractor>("TrackingFeatureExtractor",
            init<boost::shared_ptr<HypothesesGraph>, FieldOfView>(args("HypothesesGraph, FieldOfView")))
    .def("compute_features", &pgmlink::features::TrackingFeatureExtractor::compute_features)
    .def("append_feature_vector_to_file", &pgmlink::features::TrackingFeatureExtractor::append_feature_vector_to_file)
#ifdef WITH_DLIB
    .def("train_track_svm", &pgmlink::features::TrackingFeatureExtractor::train_track_svm)
    .def("get_track_svm", &pgmlink::features::TrackingFeatureExtractor::get_track_svm)
    .def("set_track_svm", &pgmlink::features::TrackingFeatureExtractor::set_track_svm)
    .def("train_division_svm", &pgmlink::features::TrackingFeatureExtractor::train_division_svm)
    .def("get_division_svm", &pgmlink::features::TrackingFeatureExtractor::get_division_svm)
    .def("set_division_svm", &pgmlink::features::TrackingFeatureExtractor::set_division_svm)
#endif
    .def("get_feature_vector", &pyextractor_get_feature_vector)
    .def("get_feature_description", &pgmlink::features::TrackingFeatureExtractor::get_feature_description)
    .def("set_track_feature_output_file", &pgmlink::features::TrackingFeatureExtractor::set_track_feature_output_file)
    ;

#ifdef WITH_DLIB
    class_<pgmlink::features::SVMOutlierCalculator, boost::shared_ptr<pgmlink::features::SVMOutlierCalculator> >("SVMOutlierCalculator")
    .def_pickle(TemplatedPickleSuite<pgmlink::features::SVMOutlierCalculator>());
#endif


    class_<vector<vigra::UInt64> >("IdVector")
    .def(vector_indexing_suite<vector<vigra::UInt64> >())
    ;

    class_<Event>("Event")
    .def_readonly("type", &Event::type)
    .def_readonly("traxel_ids", &Event::traxel_ids)
    .add_property("energy", &Event::energy)
    .def_pickle(TemplatedPickleSuite<Event>() )
    ;

#ifndef NO_ILP
    class_<StructuredLearningTracking>("StructuredLearningTracking",
                                       init<boost::shared_ptr<HypothesesGraph>, unsigned int, bool, double, double, bool, double,
                                       string, FieldOfView, string, SolverType,unsigned int>(
                             args("hypotheses_graph",
                                  "max_number_objects",
                                  "size_dependent_detection_prob",
                                  "avg_obj_size",
                                  "max_neighbor_distance",
                                  "with_division",
                                  "division_threshold",
                                  "detection_rf_filename",
                                  "fov",
                                  "event_vector_dump_filename",
                                  "solver_type",
                                  "spatial dimension")))
    .def("track", &StructuredLearningTracking::track)
    .def("exportCrop", &StructuredLearningTracking::exportCrop)
    .def("hypothesesGraphTest", &StructuredLearningTracking::hypothesesGraphTest)
    .def("addLabels", &StructuredLearningTracking::addLabels)
    .def("addAppearanceLabel", &StructuredLearningTracking::addAppearanceLabel)
    .def("addDisappearanceLabel", &StructuredLearningTracking::addDisappearanceLabel)
    .def("addDivisionLabel", &StructuredLearningTracking::addDivisionLabel)
    .def("addArcLabel", &StructuredLearningTracking::addArcLabel)
    .def("addFirstLabels", &StructuredLearningTracking::addFirstLabels)
    .def("addLastLabels", &StructuredLearningTracking::addLastLabels)
    .def("addIntermediateLabels", &StructuredLearningTracking::addIntermediateLabels)
    .def("structuredLearning", &StructuredLearningTracking::structuredLearning)
    .def("structuredLearning", &StructuredLearningTracking::structuredLearningFromParam)
    .def("weight", &StructuredLearningTracking::weight)
    .def("setWeight", &StructuredLearningTracking::setWeight)
    .def("getStructuredLearningTrackingParameters", &StructuredLearningTracking::get_structured_learning_tracking_parameters)
    ;
#endif

}
