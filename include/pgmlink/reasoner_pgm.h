/**
   @file
   @ingroup pgm
   @brief graphical model-based reasoner
*/

#ifndef REASONER_PGM_H
#define REASONER_PGM_H

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <utility>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bimap.hpp>
#include <opengm/inference/inference.hxx>

#ifdef WITH_GUROBI
#include <opengm/inference/lpgurobi.hxx>
#else
#include <opengm/inference/lpcplex.hxx>
#endif

#include "pgmlink/event.h"
#include "pgmlink/features/feature.h"
#include "pgmlink/pgm.h"
#include "pgmlink/hypotheses.h"
#include "pgmlink/reasoner.h"
#include "pgmlink/pgm_chaingraph.h"
#include "pgmlink_export.h"

namespace pgmlink 
{
class Traxel;

namespace pgm 
{
#ifdef WITH_GUROBI
  typedef opengm::LPGurobi<OpengmModel, opengm::Minimizer> OpengmLPCplex;
#else
  typedef opengm::LPCplex<OpengmModel, opengm::Minimizer> OpengmLPCplex;
#endif
} /* namespace pgm */

class Chaingraph : public Reasoner {
public:
    typedef pgm::chaingraph::Model::node_var_map node_var_map;
    typedef pgm::chaingraph::Model::arc_var_map arc_var_map;

    PGMLINK_EXPORT Chaingraph(bool with_constraints = true,
               double ep_gap = 0.01,
               bool fixed_detections = false,
               double cplex_timeout = 1e+75
              )
        : optimizer_(NULL),
          with_constraints_(with_constraints),
          fixed_detections_(fixed_detections),
          ep_gap_(ep_gap),
          cplex_timeout_(cplex_timeout),
          builder_(NULL)
    {
        builder_ = new pgm::chaingraph::ECCV12ModelBuilder();
        (*builder_).with_detection_vars().with_divisions();
    }


    PGMLINK_EXPORT Chaingraph(const pgm::chaingraph::ModelBuilder& builder,
               bool with_constraints = true,
               double ep_gap = 0.01,
               bool fixed_detections = false,
               double cplex_timeout = 1e+75
              )
        : optimizer_(NULL),
          with_constraints_(with_constraints),
          fixed_detections_(fixed_detections),
          ep_gap_(ep_gap),
          cplex_timeout_(cplex_timeout),
          builder_(builder.clone())
    {};
    PGMLINK_EXPORT ~Chaingraph();

    virtual PGMLINK_EXPORT void formulate( const HypothesesGraph& );
    virtual PGMLINK_EXPORT void infer();
    virtual PGMLINK_EXPORT void conclude( HypothesesGraph& );

    PGMLINK_EXPORT double forbidden_cost() const;
    PGMLINK_EXPORT bool with_constraints() const;
    PGMLINK_EXPORT const pgm::chaingraph::ModelBuilder& builder()
    {
        return *builder_;
    }
    PGMLINK_EXPORT void builder(const pgm::chaingraph::ModelBuilder& builder)
    {
        if(builder_)
        {
            delete builder_;
        }
        builder_ = builder.clone();
    }

    /** Return current state of graphical model
     *
     * The returned pointer may be NULL before formulate() is called
     * the first time.
     **/
    PGMLINK_EXPORT const pgm::OpengmModel* get_graphical_model() const;

    /** Return mapping from HypothesesGraph nodes to graphical model variable ids
     *
     * The map is populated after the first call to formulate().
     */
    PGMLINK_EXPORT const node_var_map& get_node_map() const;

    /** Return mapping from HypothesesGraph arcs to graphical model variable ids
     *
     * The map is populated after the first call to formulate().
     */
    PGMLINK_EXPORT const arc_var_map& get_arc_map() const;


private:
    // copy and assingment have to be implemented, yet
    Chaingraph(const Chaingraph&) {};
    Chaingraph& operator=(const Chaingraph&)
    {
        return *this;
    };
    void reset();

    pgm::OpengmLPCplex* optimizer_;
    boost::shared_ptr<pgm::chaingraph::Model> linking_model_;

    bool with_constraints_;
    bool fixed_detections_;

    double ep_gap_;
    double cplex_timeout_;
    pgm::chaingraph::ModelBuilder* builder_;
  };

} /* namespace pgmlink */
#endif /* REASONER_PGM_H */
