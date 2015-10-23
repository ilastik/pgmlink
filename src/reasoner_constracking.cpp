#include <algorithm>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <string.h>
#include <sstream>
#include <memory.h>
#include <opengm/inference/lpcplex.hxx>
#include <opengm/datastructures/marray/marray.hxx>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/python.hpp>

#include "pgmlink/hypotheses.h"
#include "pgmlink/log.h"
#include "pgmlink/reasoner_constracking.h"
#include "pgmlink/traxels.h"
#include "pgmlink/inferencemodel/constrackinginferencemodel.h"
#include "pgmlink/inferencemodel/perturbedinferencemodel.h"
#include "pgmlink/inferencemodel/dynprog_constrackinferencemodel.h"
#include "pgmlink/inferencemodel/dynprog_perturbedinferencemodel.h"

// perturbations
#include "pgmlink/inferencemodel/perturbation/gaussian_perturbation.h"
#include "pgmlink/inferencemodel/perturbation/divmbest_perturbation.h"
#include "pgmlink/inferencemodel/perturbation/classifier_uncertainty_perturbation.h"
#include "pgmlink/inferencemodel/perturbation/perturbandmap_perturbation.h"

//added for view-support
#include "opengm/opengm.hxx"
#include "opengm/graphicalmodel/graphicalmodel.hxx"
#include "opengm/functions/modelviewfunction.hxx"
#include "opengm/functions/view.hxx"

//for computing inverse_sigmoid
#include <boost/math/distributions/normal.hpp>


using namespace std;

namespace pgmlink
{

ConservationTracking::ConservationTracking(const Parameter &param)
    : max_number_objects_(param.max_number_objects),
      detection_(param.detection),
      division_(param.division),
      transition_(param.transition),
      forbidden_cost_(param.forbidden_cost),
      ep_gap_(param.ep_gap),
      with_tracklets_(param.with_tracklets),
      with_divisions_(param.with_divisions),
      disappearance_cost_(param.disappearance_cost_fn),
      appearance_cost_(param.appearance_cost_fn),
      with_misdetections_allowed_(param.with_misdetections_allowed),
      with_appearance_(param.with_appearance),
      with_disappearance_(param.with_disappearance),
      transition_parameter_(param.transition_parameter),
      with_constraints_(param.with_constraints),
      uncertainty_param_(param.uncertainty_param),
      cplex_timeout_(param.cplex_timeout),
      num_threads_(param.num_threads),
      isMAP_(true),
      division_weight_(param.division_weight),
      detection_weight_(param.detection_weight),
      transition_weight_(param.transition_weight),
      transition_classifier_(param.transition_classifier),
      with_optical_correction_(param.with_optical_correction),
      solver_(param.solver),
      use_app_node_labels_to_fix_values_(false),
      param_(param)
{
    perturbed_inference_model_param_.distributionId = uncertainty_param_.distributionId;
    perturbed_inference_model_param_.distributionParam = uncertainty_param_.distributionParam;
    perturbed_inference_model_param_.detection_weight = detection_weight_;
    perturbed_inference_model_param_.division_weight = division_weight_;
    perturbed_inference_model_param_.transition_weight = transition_weight_;
}

ConservationTracking::~ConservationTracking()
{
}

double ConservationTracking::forbidden_cost() const
{
    return forbidden_cost_;
}


std::string ConservationTracking::get_export_filename(size_t iteration, const std::string& orig_file_name)
{
    std::stringstream export_filename;
    if(!orig_file_name.empty())
    {
        std::string orig_basename = orig_file_name;
        std::string extension = ".txt";

        // remove extension
        std::string::size_type extension_pos = orig_file_name.find_last_of(".");
        if(extension_pos != orig_file_name.npos)
        {
            extension = orig_file_name.substr(extension_pos);
            orig_basename = orig_file_name.substr(0, extension_pos);
        }

        export_filename << orig_basename << "_" << std::setfill('0') << std::setw(3) << iteration << extension;
    }

    return export_filename.str();
}

boost::shared_ptr<Perturbation> ConservationTracking::create_perturbation()
{
    // instanciate perturbation depending on distribution type
    switch(perturbed_inference_model_param_.distributionId)
    {
        case Gaussian:
            return boost::make_shared<GaussianPerturbation>(perturbed_inference_model_param_, param_);
            break;
        case PerturbAndMAP:
            return boost::make_shared<PerturbAndMapPerturbation>(perturbed_inference_model_param_, param_);
            break;
        case DiverseMbest:
            return boost::make_shared<DivMBestPerturbation>(perturbed_inference_model_param_, param_);
            break;
        case ClassifierUncertainty:
            return boost::make_shared<ClassifierUncertaintyPerturbation>(perturbed_inference_model_param_, param_);
            break;
        default:
            throw std::runtime_error("The chosen perturbation distribution is not available "
                                     "with the current inference model");
    }
}

boost::shared_ptr<InferenceModel> ConservationTracking::create_inference_model()
{
    if(solver_ == SolverType::CplexSolver)
    {
        return boost::make_shared<ConsTrackingInferenceModel>(param_);
    }
#ifdef WITH_DPCT
    else if(solver_ == SolverType::DynProgSolver)
    {
        return boost::make_shared<DynProgConsTrackInferenceModel>(param_);
    }
#else
    else if(solver_ == SolverType::DynProgSolver)
    {
        throw std::runtime_error("Support for dynamic programming solver not built!");
    }
#endif // WITH_DPCT
    else
        throw std::runtime_error("No solver type available to set up inference model");
}

boost::shared_ptr<InferenceModel> ConservationTracking::create_perturbed_inference_model(boost::shared_ptr<Perturbation> perturb)
{
    if(solver_ == SolverType::CplexSolver)
    {
        return boost::make_shared<PerturbedInferenceModel>(
                    param_,
                    perturb);
    }
#ifdef WITH_DPCT
    else if (solver_ == SolverType::DynProgSolver)
    {
        return boost::make_shared<DynProgPerturbedInferenceModel>(
                    param_,
                    perturb);
    }
    else if (solver_ == SolverType::DPInitCplexSolver)
    {
        throw std::runtime_error("DynProg-initialized CPLEX cannot handle perturbations (yet)");
    }
#endif
    else
        throw std::runtime_error("No solver type available to set up perturbed inference model");
}

HypothesesGraph* ConservationTracking::get_prepared_graph(HypothesesGraph & hypotheses)
{
    HypothesesGraph *graph;

    // for formulate, add_constraints, add_finite_factors: distinguish graph & tracklet_graph
    if (with_tracklets_)
    {
        LOG(logINFO) << "ConservationTracking::perturbedInference: generating tracklet graph";
//        tracklet_graph_.clear();
        tracklet2traxel_node_map_ = generateTrackletGraph2(hypotheses, tracklet_graph_);
        graph = &tracklet_graph_;
    }
    else
    {
        graph = &hypotheses;
    }

    graph->add(relative_uncertainty()).add(node_active_count());

    return graph;
}

void ConservationTracking::twoStageInference(HypothesesGraph & hypotheses)
{
#ifndef WITH_DPCT
    throw std::runtime_error("Dynamic Programming Solver has not been built!");
#else
    if(solver_ != SolverType::DPInitCplexSolver)
        throw std::logic_error("TwoStageInference should only be called with DPInitCplexSolver");
    HypothesesGraph *graph = get_prepared_graph(hypotheses);

    // 1st stage: dyn prog
    boost::shared_ptr<DynProgConsTrackInferenceModel> dyn_prog_inf_model = 
        boost::make_shared<DynProgConsTrackInferenceModel>(param_);
    dyn_prog_inf_model->build_from_graph(*graph);
    IlpSolution temp = dyn_prog_inf_model->infer();
    dyn_prog_inf_model->conclude(hypotheses, tracklet_graph_, tracklet2traxel_node_map_, temp);
    if(!hypotheses.has_property(arc_value_count()))
        throw std::logic_error("Something went wrong with concluding, arc_value_count() property is missing!");

    // set up 2nd stage: CPLEX
    boost::shared_ptr<ConsTrackingInferenceModel> constrack_inf_model = 
        boost::make_shared<ConsTrackingInferenceModel>(param_);
    constrack_inf_model->build_from_graph(*graph);

    // extract solution from 1st stage and use as initialization

    IlpSolution initialization = constrack_inf_model->extract_solution_from_graph(hypotheses, tracklet_graph_, tracklet2traxel_node_map_);
    
    /*std::cout << "Extracted solution: ";
    for(size_t i : initialization)
        std::cout << i << " ";
    std::cout << std::endl;*/

    // run final inference
    constrack_inf_model->set_inference_params(1,"","","");

    std::cout << "DP model eval: " << constrack_inf_model->get_model().evaluate(initialization) << std::endl;

    constrack_inf_model->set_starting_point(initialization);
    IlpSolution sol = constrack_inf_model->infer();
    constrack_inf_model->conclude(hypotheses, tracklet_graph_, tracklet2traxel_node_map_, sol);
    std::cout << "ILP model eval: " << constrack_inf_model->get_model().evaluate(sol) << std::endl;
#endif
}

void ConservationTracking::perturbedInference(HypothesesGraph & hypotheses)
{
    HypothesesGraph *graph = get_prepared_graph(hypotheses);

//    graph->save_to_graphviz_dot_file("/Users/chaubold/Desktop/sabrina_mnd200_trackletgraph.dot",
//                                     inference_model_param_.with_tracklets,
//                                     inference_model_param_.with_divisions,
//                                     inference_model_param_.detection,
//                                     inference_model_param_.division,
//                                     inference_model_param_.transition,
//                                     inference_model_param_.disappearance_cost,
//                                     inference_model_param_.appearance_cost,
//                                     inference_model_param_.max_number_objects,
//                                     inference_model_param_.transition_parameter);

    LOG(logINFO) << "ConservationTracking::perturbedInference: number of iterations: " << uncertainty_param_.numberOfIterations;
    LOG(logINFO) << "ConservationTracking::perturbedInference: perturb using method with Id " << uncertainty_param_.distributionId;
    LOG(logDEBUG) << "ConservationTracking::perturbedInference: formulate ";

    LOG(logDEBUG) << "ConservationTracking::perturbedInference: uncertainty parameter print";
    uncertainty_param_.print();

    //m-best: if perturbation is set to m-best, specify number of solutions. Otherwise, we expect only one solution.
    size_t numberOfSolutions = 1;
    if (uncertainty_param_.distributionId == MbestCPLEX)
    {
        numberOfSolutions = uncertainty_param_.numberOfIterations;
#ifndef WITH_MODIFIED_OPENGM
        throw std::runtime_error("PGMlink needs to be compiled against a modified OpenGM to support MbestCPLEX!");
#endif
    }

    LOG(logINFO) << "------------> Beginning Iteration 0 <-----------\n";

    // instanciate inference model
    boost::shared_ptr<InferenceModel> inference_model = create_inference_model();

    // build inference model
    inference_model->build_from_graph(*graph);

    // fix some node values beforehand
    if(use_app_node_labels_to_fix_values_)
    {
        inference_model->fixFirstDisappearanceNodesToLabels(hypotheses, tracklet_graph_, tracklet2traxel_node_map_);
    }

    if(solver_ == SolverType::CplexSolver)
    {
        boost::static_pointer_cast<ConsTrackingInferenceModel>(inference_model)->set_inference_params(
            numberOfSolutions,
            get_export_filename(0, features_file_),
            constraints_file_,
            get_export_filename(0, labels_export_file_name_));
    }    

    // run inference & conclude
    solutions_.push_back(inference_model->infer());

    LOG(logINFO) << "conclude MAP";
    inference_model->conclude(hypotheses, tracklet_graph_, tracklet2traxel_node_map_, solutions_.back());

    // print solution energies
    if(solver_ == SolverType::CplexSolver)
        std::cout << "ILP model eval: " << boost::static_pointer_cast<ConsTrackingInferenceModel>(inference_model)->get_model().evaluate(solutions_.back()) << std::endl;
    else if(solver_ == SolverType::DynProgSolver)
    {
        boost::shared_ptr<ConsTrackingInferenceModel> constrack_inf_model = 
        boost::make_shared<ConsTrackingInferenceModel>(param_);
        constrack_inf_model->build_from_graph(*graph);
        IlpSolution initialization = constrack_inf_model->extract_solution_from_graph(hypotheses, tracklet_graph_, tracklet2traxel_node_map_);
        constrack_inf_model->set_inference_params(1,"","","");
        std::cout << "DP model eval: " << constrack_inf_model->get_model().evaluate(initialization) << std::endl;
    }

    // run perturbations etc.
    for (size_t k = 1; k < numberOfSolutions; ++k)
    {
        if(solver_ != SolverType::CplexSolver)
            throw std::runtime_error("When using CPlex MBest perturbations you need to use the Cplex solver!");
        LOG(logINFO) << "conclude " << k + 1 << "-best solution";
        solutions_.push_back(boost::static_pointer_cast<ConsTrackingInferenceModel>(
                                 inference_model)->extractSolution(k, get_export_filename(k, labels_export_file_name_)));

        inference_model->conclude(hypotheses, tracklet_graph_, tracklet2traxel_node_map_, solutions_.back());
    }

    size_t numberOfIterations = uncertainty_param_.numberOfIterations;
    if (uncertainty_param_.distributionId == MbestCPLEX)
    {
        numberOfIterations = 1;
    }

    if(numberOfIterations > 1)
    {
        boost::shared_ptr<Perturbation> perturbation = create_perturbation();
        boost::shared_ptr<InferenceModel> perturbed_inference_model;

        for (size_t iterStep = 1; iterStep < numberOfIterations; ++iterStep)
        {
            LOG(logINFO) << "------------> Beginning Iteration " << iterStep << " <-----------\n";
            if (uncertainty_param_.distributionId == DiverseMbest)
            {
                if(solver_ == SolverType::CplexSolver)
                {
                    boost::static_pointer_cast<DivMBestPerturbation>(perturbation)->push_away_from_solution(
                                boost::static_pointer_cast<ConsTrackingInferenceModel>(inference_model)->get_model(),
                                solutions_.back());
                }

                boost::static_pointer_cast<DivMBestPerturbation>(perturbation)->registerOriginalGraph(&hypotheses,
                                                                                                      &tracklet2traxel_node_map_);
            }

            perturbed_inference_model = create_perturbed_inference_model(perturbation);

            perturbed_inference_model->use_transition_prediction_cache(inference_model.get());
            perturbed_inference_model->build_from_graph(*graph);

            // fix some node values beforehand
            if(use_app_node_labels_to_fix_values_)
            {
                perturbed_inference_model->fixFirstDisappearanceNodesToLabels(hypotheses, tracklet_graph_, tracklet2traxel_node_map_);
            }

            if(solver_ == SolverType::CplexSolver)
            {
                boost::static_pointer_cast<ConsTrackingInferenceModel>(perturbed_inference_model)->set_inference_params(1,
                                                                get_export_filename(iterStep, features_file_),
                                                                "",
                                                                get_export_filename(iterStep, labels_export_file_name_));
            }

            solutions_.push_back(perturbed_inference_model->infer());
            LOG(logINFO) << "conclude iteration " << iterStep;
            perturbed_inference_model->conclude(hypotheses,
                                                tracklet_graph_,
                                                tracklet2traxel_node_map_,
                                                solutions_.back());
        }
    }

    compute_relative_uncertainty(graph);
}

void ConservationTracking::enableFixingLabeledAppearanceNodes()
{
    // use all active nodes and fix them to the active value in the respective inference model
    use_app_node_labels_to_fix_values_ = true;
}

void ConservationTracking::compute_relative_uncertainty(HypothesesGraph* graph)
{
    graph->add(relative_uncertainty());

    property_map<node_active_count, HypothesesGraph::base_graph>::type &active_nodes = graph->get(node_active_count());
    property_map<relative_uncertainty, HypothesesGraph::base_graph>::type &rel_uncertainty = graph->get(relative_uncertainty());
    for (HypothesesGraph::NodeIt n(*graph); n != lemon::INVALID; ++n)
    {
        double count = 0;
        vector<size_t> *active_list = &active_nodes.get_value(n);
        for (vector<size_t>::iterator is_active = active_list->begin(); is_active != active_list->end(); is_active++)
        {
            if (*is_active != 0)
            {
                ++count;
            }
        }

        rel_uncertainty.set(n, count / uncertainty_param_.numberOfIterations);
    }
}

void ConservationTracking::infer()
{
    throw std::runtime_error("Not implemented");
}

void ConservationTracking::conclude(HypothesesGraph &)
{
    throw std::runtime_error("Not implemented");
}

void ConservationTracking::formulate(const HypothesesGraph &)
{
    // nothing to do
}

const std::vector<ConservationTracking::IlpSolution> &ConservationTracking::get_ilp_solutions() const
{
    return solutions_;
}

void ConservationTracking::set_ilp_solutions(const std::vector<ConservationTracking::IlpSolution>& solutions)
{
    if(solutions.size() == 0)
    {
        LOG(logWARNING) << "No solutions given to set";
        return;
    }

    solutions_.clear();
    for(std::vector<ConservationTracking::IlpSolution>::const_iterator sol_it = solutions.begin();
            sol_it != solutions.end();
            ++sol_it)
    {
        solutions_.push_back(ConservationTracking::IlpSolution(*sol_it));
    }
}

void ConservationTracking::reset()
{
//    solutions_.clear();
}

boost::python::dict convertFeatureMapToPyDict(FeatureMap map)
{
    boost::python::dict dictionary;
    for (FeatureMap::iterator iter = map.begin(); iter != map.end(); ++iter)
    {
        dictionary[iter->first] = iter->second;
    }
    return dictionary;
}

} /* namespace pgmlink */
