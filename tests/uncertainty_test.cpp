#define BOOST_TEST_MODULE hypotheses_test

#include <vector>
#include <string>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <lemon/core.h>
#include <lemon/concepts/digraph.h>
#include <lemon/list_graph.h>
#include <lemon/maps.h>

#include "pgmlink/hypotheses.h"
#include "pgmlink/traxels.h"
#include "pgmlink/tracking.h"
#include "pgmlink/reasoner_constracking.h"

using namespace pgmlink;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( GumbelPerturbAndMAP ) {
  
	std::cout << "Constructing HypothesesGraph" << std::endl;
	std::cout << std::endl;

	typedef HypothesesGraph::ArcIt ArcIt2;
	typedef HypothesesGraph::Arc Arc;
	typedef HypothesesGraph::NodeIt NodeIt;
	typedef HypothesesGraph::Node Node;
	using lemon::INVALID;

	std::cout << "Adding Traxels to TraxelStore" << std::endl;
	std::cout << std::endl;
	
	//  t=1       2       3
	//   o                 o
	//    |               |
	//     ------ o ------
	//    |               |
	//   o                 o
	TraxelStore ts;
	Traxel n11, n12, n21, n31, n32;
	feature_array com(feature_array::difference_type(3));
	feature_array divProb(feature_array::difference_type(1));
	feature_array detProb(feature_array::difference_type(2));
	//detProb[2]=0;
	n11.Id = 11; n11.Timestep = 1; com[0] = 1; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.4;detProb[1]=0.6;
	n11.features["com"] = com; n11.features["divProb"] = divProb; n11.features["detProb"] = detProb;
	add(ts,n11);
	n12.Id = 12; n12.Timestep = 1; com[0] = 3; com[1] = 2; com[2] = 3; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n12.features["com"] = com; n12.features["divProb"] = divProb; n12.features["detProb"] = detProb;
	add(ts,n12);
	
	n21.Id = 21; n21.Timestep = 2; com[0] = 2; com[1] = 2; com[2] = 3; divProb[0] = 0.39; detProb[0] = 0.1;detProb[1]=0.9;
	n21.features["com"] = com; n21.features["divProb"] = divProb; n21.features["detProb"] = detProb;
	add(ts,n21);
	
	n31.Id = 31; n31.Timestep = 3; com[0] = 2; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n31.features["com"] = com; n31.features["divProb"] = divProb; n31.features["detProb"] = detProb;
	add(ts,n31);
	n32.Id = 32; n32.Timestep = 3; com[0] = 3; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.2;detProb[1]=0.8;
	n32.features["com"] = com; n32.features["divProb"] = divProb; n32.features["detProb"] = detProb;
	add(ts,n32);

	vector<double> sigmas(5);
	sigmas[0]=1;
	sigmas[1]=0;
	sigmas[2]=0;
	sigmas[3]=0;
	sigmas[4]=0;

	UncertaintyParameter uparam(10,PerturbAndMAP,sigmas);//10 iterations, Gumbel distribution, sigma=1

	FieldOfView fov(0, 0, 0, 0, 3, 5, 5, 5); // tlow, xlow, ylow, zlow, tup, xup, yup, zup
	ConsTracking tracking = ConsTracking(
				  1, // max_number_objects
	              20, // max_neighbor_distance
				  0.3, // division_threshold
				  "none", // random_forest_filename
	              false, // detection_by_volume
	              0, // forbidden_cost
	              0.0, // ep_gap
	              double(1.1), // avg_obj_size
				  false, // with_tracklets
				  10.0, //division_weight
				  10.0, //transition_weight
				  true, //with_divisions
				  10., // disappearance_cost,
				  10., // appearance_cost
				  false, //with_merger_resolution
				  3, //n_dim
				  5, //transition_parameter
				  0, //border_width for app/disapp costs
	              fov, //field of view
	              true, //with_constraints
	              uparam
		  	      );
	tracking(ts, TimestepIdCoordinateMapPtr());
}


BOOST_AUTO_TEST_CASE( diverseUncertainty ) {
  
	std::cout << "Constructing HypothesesGraph" << std::endl;
	std::cout << std::endl;

	typedef HypothesesGraph::ArcIt ArcIt2;
	typedef HypothesesGraph::Arc Arc;
	typedef HypothesesGraph::NodeIt NodeIt;
	typedef HypothesesGraph::Node Node;
	using lemon::INVALID;

	std::cout << "Adding Traxels to TraxelStore" << std::endl;
	std::cout << std::endl;
	
	//  t=1       2       3
	//   o                 o
	//    |               |
	//     ------ o ------
	//    |               |
	//   o                 o
	TraxelStore ts;
	Traxel n11, n12, n21, n31, n32;
	feature_array com(feature_array::difference_type(3));
	feature_array divProb(feature_array::difference_type(1));
	feature_array detProb(feature_array::difference_type(2));
	//detProb[2]=0;
	n11.Id = 11; n11.Timestep = 1; com[0] = 1; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.4;detProb[1]=0.6;
	n11.features["com"] = com; n11.features["divProb"] = divProb; n11.features["detProb"] = detProb;
	add(ts,n11);
	n12.Id = 12; n12.Timestep = 1; com[0] = 3; com[1] = 2; com[2] = 3; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n12.features["com"] = com; n12.features["divProb"] = divProb; n12.features["detProb"] = detProb;
	add(ts,n12);
	
	n21.Id = 21; n21.Timestep = 2; com[0] = 2; com[1] = 2; com[2] = 3; divProb[0] = 0.5; detProb[0] = 0;detProb[1]=1;
	n21.features["com"] = com; n21.features["divProb"] = divProb; n21.features["detProb"] = detProb;
	add(ts,n21);
	
	n31.Id = 31; n31.Timestep = 3; com[0] = 2; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n31.features["com"] = com; n31.features["divProb"] = divProb; n31.features["detProb"] = detProb;
	add(ts,n31);
	n32.Id = 32; n32.Timestep = 3; com[0] = 3; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.3;detProb[1]=0.7;
	n32.features["com"] = com; n32.features["divProb"] = divProb; n32.features["detProb"] = detProb;
	add(ts,n32);

	std::cout << "Initialize Conservation tracking" << std::endl;
	std::cout << std::endl;

	vector<double> sigmas(5);
		sigmas[0]=0;
		sigmas[1]=0;
		sigmas[2]=10;
		sigmas[3]=10;
		sigmas[4]=10;

	UncertaintyParameter uparam(3,DiverseMbest,sigmas);//2 iterations, diverse, diverse_lambda=10

	FieldOfView fov(0, 0, 0, 0, 3, 5, 5, 5); // tlow, xlow, ylow, zlow, tup, xup, yup, zup
	ConsTracking tracking = ConsTracking(
				  1, // max_number_objects
	              20, // max_neighbor_distance
				  0.3, // division_threshold
				  "none", // random_forest_filename
	              false, // detection_by_volume
	              0, // forbidden_cost
	              0.0, // ep_gap
	              double(1.1), // avg_obj_size
				  false, // with_tracklets
				  10.0, //division_weight
				  10.0, //transition_weight
				  true, //with_divisions
				  10., // disappearance_cost,
				  10., // appearance_cost
				  false, //with_merger_resolution
				  3, //n_dim
				  5, //transition_parameter
				  0, //border_width for app/disapp costs
	              fov, //field of view
	              true, //with_constraints
	              uparam
		  	      );
	
	std::vector<std::vector< std::vector<Event> > >events = tracking(ts, TimestepIdCoordinateMapPtr());

	//two iterations: two event vectors
	BOOST_CHECK_EQUAL(events.size(),3);
	
	BOOST_CHECK_EQUAL(events[0][1][0].type,Event::Division);
	BOOST_CHECK_EQUAL(events[1][1][0].type,Event::Move);
	BOOST_CHECK_EQUAL(events[2][1][0].type,Event::Move);

}

BOOST_AUTO_TEST_CASE( mbestUncertainty ) {
  
	std::cout << "Constructing HypothesesGraph" << std::endl;
	std::cout << std::endl;

	typedef HypothesesGraph::ArcIt ArcIt2;
	typedef HypothesesGraph::Arc Arc;
	typedef HypothesesGraph::NodeIt NodeIt;
	typedef HypothesesGraph::Node Node;
	using lemon::INVALID;

	std::cout << "Adding Traxels to TraxelStore" << std::endl;
	std::cout << std::endl;
	
	//  t=1       2       3
	//   o ------ o ------ o
	//        X        X     
	//   o ------ o ------ o
	//        X        X   
	//   o ------ o ------ o
	TraxelStore ts;
	Traxel n11, n12, n21, n22, n31, n32;
	feature_array com(feature_array::difference_type(3));
	feature_array divProb(feature_array::difference_type(1));
	feature_array detProb(feature_array::difference_type(2));
	//detProb[2]=0;
	n11.Id = 11; n11.Timestep = 1; com[0] = 1; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.4;detProb[1]=0.6;
	n11.features["com"] = com; n11.features["divProb"] = divProb; n11.features["detProb"] = detProb;
	add(ts,n11);
	n12.Id = 12; n12.Timestep = 1; com[0] = 3; com[1] = 2; com[2] = 3; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n12.features["com"] = com; n12.features["divProb"] = divProb; n12.features["detProb"] = detProb;
	add(ts,n12);
	
	n21.Id = 21; n21.Timestep = 2; com[0] = 2; com[1] = 2; com[2] = 3; divProb[0] = 0.5; detProb[0] = 0;detProb[1]=1;
	n21.features["com"] = com; n21.features["divProb"] = divProb; n21.features["detProb"] = detProb;
	add(ts,n21);
	
	n22.Id = 22; n22.Timestep = 2; com[0] = 3; com[1] = 1; com[2] = 3; divProb[0] = 0.8; detProb[0] = 0.2;detProb[1]=0.8;
	n22.features["com"] = com; n22.features["divProb"] = divProb; n22.features["detProb"] = detProb;
	add(ts,n22);
	
	
	n31.Id = 31; n31.Timestep = 3; com[0] = 2; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.6;detProb[1]=0.4;
	n31.features["com"] = com; n31.features["divProb"] = divProb; n31.features["detProb"] = detProb;
	add(ts,n31);
	n32.Id = 32; n32.Timestep = 3; com[0] = 3; com[1] = 1; com[2] = 1; divProb[0] = 0; detProb[0] = 0.8;detProb[1]=0.2;
	n32.features["com"] = com; n32.features["divProb"] = divProb; n32.features["detProb"] = detProb;
	add(ts,n32);
		
	int mbest = 5;
	UncertaintyParameter uparam(mbest,MbestCPLEX,0);
	
	FieldOfView fov(0, 0, 0, 0, 3, 5, 5, 5); // tlow, xlow, ylow, zlow, tup, xup, yup, zup
	ConsTracking tracking = ConsTracking(
				  1, // max_number_objects
	              20, // max_neighbor_distance
				  0.3, // division_threshold
				  "none", // random_forest_filename
	              false, // detection_by_volume
	              0, // forbidden_cost
	              0.0, // ep_gap
	              double(1.1), // avg_obj_size
				  false, // with_tracklets
				  10.0, //division_weight
				  10.0, //transition_weight
				  true, //with_divisions
				  10., // disappearance_cost,
				  10., // appearance_cost
				  false, //with_merger_resolution
				  3, //n_dim
				  5, //transition_parameter
				  0, //border_width for app/disapp costs
	              fov, //field of view
	              true, //with_constraints
	              uparam
		  	      );
	
	std::vector<std::vector< std::vector<Event> > >events = tracking(ts, TimestepIdCoordinateMapPtr());
	int counter = 0;
	BOOST_CHECK_EQUAL(events.size(),mbest);
			
	for(size_t timeStep=0;timeStep<events[1].size() && timeStep<events[2].size();++timeStep){
		for(size_t factorIndex=0;factorIndex<events[1][timeStep].size()&&factorIndex<events[2][timeStep].size();++factorIndex){
		
			if (events[1][timeStep][factorIndex].type == events[2][timeStep][factorIndex].type){
				counter++;
			}
		}
	}
	
	BOOST_CHECK_EQUAL(counter,4);
}

// EOF

