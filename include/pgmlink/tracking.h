/**
@file
@ingroup tracking
@brief tracking API
 */

#ifndef FUNKEY_BINARY_FILE
#define FUNKEY_BINARY_FILE @FUNKEY_BINARY_FILE@
#endif // FUNKEY_BINARY_FILE

#ifndef TRACKING_H
#define TRACKING_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "pgmlink/event.h"
#include "pgmlink/pgmlink_export.h"
#include "pgmlink/traxels.h"
#include "pgmlink/field_of_view.h"
#include "pgmlink/merger_resolving.h"
#include <boost/python.hpp>

namespace pgmlink {
  class ChaingraphTracking 
  {
   public:
    PGMLINK_EXPORT 
    ChaingraphTracking(const std::string& random_forest_filename = "none",
                       double appearance = 500, 
                       double disappearance = 500,
                       double detection = 10,
                       double misdetection = 500,
                       bool cellness_by_random_forest = false,
                       double opportunity_cost = 0,
                       double forbidden_cost = 0,
                       bool with_constraints = true,
                       bool fixed_detections = false,
                       double mean_div_dist=25,
                       double min_angle=0,
                       double ep_gap=0.01,
                       int n_neighbors=6,
                       bool with_divisions = true,
                       double cplex_timeout = 1e+75,
                       bool alternative_builder=false 
                       )
    : app_(appearance), dis_(disappearance), det_(detection), mis_(misdetection), 
      rf_fn_(random_forest_filename), use_rf_(cellness_by_random_forest), 
      opportunity_cost_(opportunity_cost), forbidden_cost_(forbidden_cost), with_constraints_(with_constraints),
      fixed_detections_(fixed_detections), mean_div_dist_(mean_div_dist), min_angle_(min_angle),
      ep_gap_(ep_gap), n_neighbors_(n_neighbors), with_divisions_(with_divisions),
      cplex_timeout_(cplex_timeout), alternative_builder_(alternative_builder)
    {}

    PGMLINK_EXPORT std::vector< std::vector<Event> > operator()(TraxelStore&);

    /**
     * Get state of detection variables after call to operator().
     */
    PGMLINK_EXPORT std::vector< std::map<unsigned int, bool> > detections();
    
    /**
     * Setter functions
     */
    PGMLINK_EXPORT void set_with_divisions(bool);
    PGMLINK_EXPORT void set_cplex_timeout(double);

  private:
    double app_, dis_, det_, mis_;
    const std::string rf_fn_;
    bool use_rf_;
    double opportunity_cost_;
    double forbidden_cost_;
    bool with_constraints_;
    bool fixed_detections_;
    double mean_div_dist_, min_angle_;
    double ep_gap_;
    int n_neighbors_;
    bool with_divisions_;
    double cplex_timeout_;
    bool alternative_builder_;
    boost::shared_ptr<std::vector< std::map<unsigned int, bool> > > last_detections_;
  };

  class NNTracking 
  {
   public:
    PGMLINK_EXPORT 
    NNTracking(double divDist = 30,
               double movDist = 10,
               std::vector<std::string> features = std::vector<std::string>(0),
               double divisionThreshold = 0.5,
               bool splitterHandling = true,
               bool mergerHandling = true,
               std::vector<int> maxTraxelIdAt = std::vector<int>(0))
      : divDist_(divDist), movDist_(movDist), distanceFeatures_(features),
        divisionThreshold_(divisionThreshold), splitterHandling_(splitterHandling),
        mergerHandling_(mergerHandling), maxTraxelIdAt_(maxTraxelIdAt)
    {}
    
    PGMLINK_EXPORT std::vector< std::vector<Event> > operator()(TraxelStore&);

    /**
     * Get state of detection variables after call to operator().
     */
    PGMLINK_EXPORT std::vector< std::map<unsigned int, bool> > detections();

   private:
    double divDist_, movDist_;
    std::vector<std::string> distanceFeatures_;
    double divisionThreshold_;
    bool splitterHandling_, mergerHandling_;
    boost::shared_ptr<std::vector< std::map<unsigned int, bool> > > last_detections_;
    std::vector<int> maxTraxelIdAt_;
  };


  class NNTrackletsTracking
  {
    public:
      PGMLINK_EXPORT 
      NNTrackletsTracking(double maxDist = 30,
                          std::vector<std::string> features = std::vector<std::string>(0),
                          double divisionThreshold = 0.5,
                          bool splitterHandling = true,
                          bool mergerHandling = true,
                          std::vector<int> maxTraxelIdAt = std::vector<int>(0))
      : maxDist_(maxDist), distanceFeatures_(features),
        divisionThreshold_(divisionThreshold), splitterHandling_(splitterHandling),
        mergerHandling_(mergerHandling), maxTraxelIdAt_(maxTraxelIdAt)
      {}
      
      PGMLINK_EXPORT std::vector< std::vector<Event> > operator()(TraxelStore&);

      /**
       * Get state of detection variables after call to operator().
       */
      PGMLINK_EXPORT std::vector< std::map<unsigned int, bool> > detections();

     private:
      double maxDist_;
      std::vector<std::string> distanceFeatures_;
      double divisionThreshold_;
      bool splitterHandling_, mergerHandling_;
      boost::shared_ptr<std::vector< std::map<unsigned int, bool> > > last_detections_;
      std::vector<int> maxTraxelIdAt_;
  };

  class ConsTracking
  {
    public:
      PGMLINK_EXPORT 
	ConsTracking(int max_number_objects=3,
		     bool size_dependent_detection_prob = false,
		     double avg_obj_size=30.0,
		     double max_neighbor_distance = 20,
		     bool with_divisions=true,
		     double division_threshold = 0.3,
		     const std::string& random_forest_filename = "none",
		     FieldOfView fov = FieldOfView(),
		     const std::string& event_vector_dump_filename = "none"
		     )
        :max_number_objects_(max_number_objects),
      max_dist_(max_neighbor_distance),
      with_divisions_(with_divisions),
      division_threshold_(division_threshold),
      detection_rf_fn_(random_forest_filename),
      use_size_dependent_detection_(size_dependent_detection_prob),
      avg_obj_size_(avg_obj_size),
      means_(std::vector<double>()),
      sigmas_(std::vector<double>()),
      fov_(fov),
      event_vector_dump_filename_(event_vector_dump_filename),
      export_from_labeled_graph_(false),
      with_optical_correction_(false),
      quit_before_inference_(false)
      {}


    PGMLINK_EXPORT EventVectorVectorVector operator()(TraxelStore& ts,
								double forbidden_cost = 0,
								double ep_gap=0.01,
								bool with_tracklets=true,
								double division_weight=10.0,
								double transition_weight=10.0,
								double disappearance_cost = 0,
								double appearance_cost = 0,
								bool with_merger_resolution = true,
								int n_dim = 3,
								double transition_parameter = 5.,
								double border_width = 0,
								bool with_constraints = true,
                                UncertaintyParameter uncertaintyParam = UncertaintyParameter(),
								double cplex_timeout = 1e+75,
                                TimestepIdCoordinateMapPtr coordinates = TimestepIdCoordinateMapPtr(),
                                boost::python::object transition_classifier = boost::python::object());


      
      /**
       * refactoring of operator().
       */

      PGMLINK_EXPORT boost::shared_ptr<HypothesesGraph> build_hypo_graph(TraxelStore& ts);

      PGMLINK_EXPORT boost::shared_ptr<HypothesesGraph> get_hypo_graph();
      PGMLINK_EXPORT boost::shared_ptr<HypothesesGraph> get_resolved_hypotheses_graph();

      PGMLINK_EXPORT EventVectorVectorVector track(double forbidden_cost = 0,
							    double ep_gap=0.01,
							    bool with_tracklets=true,
							    double detection_weight = 10., 
							    double division_weight=10.0,
							    double transition_weight=10.0,
							    double disappearance_cost = 0,
							    double appearance_cost = 0,
							    bool with_merger_resolution = true,
							    int n_dim = 3,
							    double transition_parameter = 5.,
							    double border_width = 0,
							    bool with_constraints = true,
                                UncertaintyParameter uncertaintyParam = UncertaintyParameter(),
							    double cplex_timeout = 1e+75,
                                boost::python::object TransitionClassifier = boost::python::object());

      PGMLINK_EXPORT ConservationTracking::Parameter get_conservation_tracking_parameters(double forbidden_cost = 0,
              double ep_gap=0.01,
              bool with_tracklets=true,
              double detection_weight = 10.,
              double division_weight=10.0,
              double transition_weight=10.0,
              double disappearance_cost = 0,
              double appearance_cost = 0,
              bool with_merger_resolution = true,
              int n_dim = 3,
              double transition_parameter = 5.,
              double border_width = 0,
              bool with_constraints = true,
              UncertaintyParameter uncertaintyParam = UncertaintyParameter(),
              double cplex_timeout = 1e+75,
              boost::python::object transition_classifier = boost::python::object()
              );

      PGMLINK_EXPORT EventVectorVector resolve_mergers(EventVectorVector &events,
                                TimestepIdCoordinateMapPtr coordinates = TimestepIdCoordinateMapPtr(),
                                double ep_gap=0.01,
                                double transition_weight=10.0,
                                bool with_tracklets=true,
                                int n_dim = 3,
                                double transition_parameter = 5.,
                                bool with_constraints = true,
                                boost::python::object transitionClassifier = boost::python::object());


      /**
       * Get state of detection variables after call to operator().
       */
      PGMLINK_EXPORT std::vector< std::map<unsigned int, bool> > detections();
      PGMLINK_EXPORT void write_funkey_set_output_files(std::string writeFeatures, std::string writeConstraints, std::string writeGroundTruth, bool reset = true, UncertaintyParameter uncertaintyParam = UncertaintyParameter());
      PGMLINK_EXPORT void write_funkey_features(TraxelStore &ts,
                                              std::vector<std::vector<double>> parameterlist,
                                              UncertaintyParameter uncertaintyParam = UncertaintyParameter(),
                                              double forbidden_cost=0, 
                                              int ndim=3,
                                              bool with_tracklets=true,
                                              double transition_parameter=5,
                                              double border_width=0,
                                              boost::python::object transitionClassifier = boost::python::object());
      PGMLINK_EXPORT void write_funkey_files(TraxelStore ts,
                                              std::string writeFeatures = "",
                                              std::string writeConstraints = "",
                                              std::string writeGroundTruth = "",
                                              const std::vector<double> weights = std::vector<double>(5,1.),
                                              UncertaintyParameter uncertaintyParam = UncertaintyParameter(),
                                              double forbidden_cost=0, int ndim=3, bool with_tracklets=true,
                                              double transition_parameter=5,
                                              double border_width=0,
                                              boost::python::object transitionClassifier = boost::python::object());
      PGMLINK_EXPORT std::vector<double> learn_from_funkey_files(std::string features,std::string constraints,std::string groundTruth,std::string weights = "",std::string options = "");
      PGMLINK_EXPORT void set_export_labeled_graph(bool in);
      PGMLINK_EXPORT double hammingloss_of_files(std::string f1,std::string f2);

      /// Return reference to the ilp solutions
      PGMLINK_EXPORT void save_ilp_solutions(const std::string& filename);

    private:
      int max_number_objects_;
      double max_dist_;
      bool with_divisions_;
      double division_threshold_;
      const std::string detection_rf_fn_;
      bool use_size_dependent_detection_;
      bool use_classifier_prior_;
      double avg_obj_size_;
      std::vector<double> means_, sigmas_;
      boost::shared_ptr<std::vector< std::map<unsigned int, bool> > > last_detections_;
      FieldOfView fov_;
      std::string event_vector_dump_filename_;
      std::vector<ConservationTracking::IlpSolution> ilp_solutions_;

      TraxelStore* traxel_store_;

      boost::shared_ptr<HypothesesGraph> hypotheses_graph_;
      boost::shared_ptr<HypothesesGraph> original_hypotheses_graph_;
      boost::shared_ptr<HypothesesGraph> resolved_graph_;
      boost::shared_ptr<ConservationTracking> pgm_;
							    
      std::string features_file_;
      std::string constraints_file_; 
      std::string ground_truth_file_;
      bool quit_before_inference_;

      bool export_from_labeled_graph_, with_optical_correction_;
							    
  };
}

#endif /* TRACKING_H */
