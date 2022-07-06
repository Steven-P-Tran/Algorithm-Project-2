///////////////////////////////////////////////////////////////////////////////
// maxtime_test.cc
//
// Unit tests for maxtime.hh
//
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <sstream>


#include "maxtime.hh"
#include "rubrictest.hh"


int main()
{
	Rubric rubric;
	
	RideVector trivial_rides;
	trivial_rides.push_back(std::shared_ptr<RideItem>(new RideItem("test Ferris Wheel", 100.0, 20.0)));
	trivial_rides.push_back(std::shared_ptr<RideItem>(new RideItem("test Speedway", 40.0, 5.0)));
	
	auto all_rides = load_ride_database("ride.csv");
	assert( all_rides );
	
	auto filtered_rides = filter_ride_vector(*all_rides, 1, 2500, all_rides->size());
	
	//
	rubric.criterion(
		"load_ride_database still works", 2,
		[&]()
		{
			TEST_TRUE("non-null", all_rides);
			TEST_EQUAL("size", 8064, all_rides->size());
		}
	);
	
	//
	rubric.criterion(
		"filter_ride_vector", 2,
		[&]()
		{
			auto
				three = filter_ride_vector(*all_rides, 100, 500, 3),
				ten = filter_ride_vector(*all_rides, 100, 500, 10);
			TEST_TRUE("non-null", three);
			TEST_TRUE("non-null", ten);
			TEST_EQUAL("total_size", 3, three->size());
			TEST_EQUAL("total_size", 10, ten->size());
			TEST_EQUAL("contents", "again amazing mystical vertigo", (*ten)[0]->description());
			TEST_EQUAL("contents", "A short enchanted starship", (*ten)[9]->description());
			for (int i = 0; i < 3; i++) {
				TEST_EQUAL("contents", (*three)[i]->description(), (*ten)[i]->description());
			}
		}
	);
	
	//
	rubric.criterion(
		"greedy_max_time trivial cases", 2,
		[&]()
		{
			std::unique_ptr<RideVector> soln;
			
			soln = greedy_max_time(trivial_rides, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());
			
			soln = greedy_max_time(trivial_rides, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Ferris Wheel only", 1, soln->size());
			TEST_EQUAL("Ferris Wheel only", "test Ferris Wheel", (*soln)[0]->description());
			
			soln = greedy_max_time(trivial_rides, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Speedway only", 1, soln->size());
			TEST_EQUAL("Speedway only", "test Speedway", (*soln)[0]->description());
			
			soln = greedy_max_time(trivial_rides, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Ferris Wheel and Speedway", 2, soln->size());
			TEST_EQUAL("Ferris Wheel and Speedway", "test Ferris Wheel", (*soln)[0]->description());
			TEST_EQUAL("Ferris Wheel and Speedway", "test Speedway", (*soln)[1]->description());
		}
	);
	
	//
	rubric.criterion(
		"greedy_max_time correctness", 4,
		[&]()
		{
			std::unique_ptr<RideVector> soln_small, soln_large;
			
			soln_small = greedy_max_time(*filtered_rides, 500),
			soln_large = greedy_max_time(*filtered_rides, 5000);
			
			//print_ride_vector(*soln_small);
			//print_ride_vector(*soln_large);
			
			TEST_TRUE("non-null", soln_small);
			TEST_TRUE("non-null", soln_large);
			
			TEST_FALSE("non-empty", soln_small->empty());
			TEST_FALSE("non-empty", soln_large->empty());
			
			double
				cost_small, time_small,
				cost_large, time_large
				;
			sum_ride_vector(*soln_small, cost_small, time_small);
			sum_ride_vector(*soln_large, cost_large, time_large);
			
			//	Precision
			cost_small	= std::round( cost_small	* 100 ) / 100;
			time_small	= std::round( time_small	* 100 ) / 100;
			cost_large	= std::round( cost_large	* 100 ) / 100;
			time_large	= std::round( time_large	* 100 ) / 100;
			
			TEST_EQUAL("Small solution cost",	481.48,	cost_small);
			TEST_EQUAL("Small solution time",	950.19,	time_small);
			TEST_EQUAL("Large solution cost",	4990.35,	cost_large);
			TEST_EQUAL("Large solution time",	9209.82,	time_large);
		}
	);
	
	//
	rubric.criterion(
		"exhaustive_max_time trivial cases", 2,
		[&]()
		{
			std::unique_ptr<RideVector> soln;
			
			soln = exhaustive_max_time(trivial_rides, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());
			
			soln = exhaustive_max_time(trivial_rides, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Ferris Wheel only", 1, soln->size());
			TEST_EQUAL("Ferris Wheel only", "test Ferris Wheel", (*soln)[0]->description());
			
			soln = exhaustive_max_time(trivial_rides, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Speedway only", 1, soln->size());
			TEST_EQUAL("Speedway only", "test Speedway", (*soln)[0]->description());
			
			soln = exhaustive_max_time(trivial_rides, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("Ferris Wheel and Speedway", 2, soln->size());
			TEST_EQUAL("Ferris Wheel and Speedway", "test Ferris Wheel", (*soln)[0]->description());
			TEST_EQUAL("Ferris Wheel and Speedway", "test Speedway", (*soln)[1]->description());
		}
	);
	
	//
	rubric.criterion(
		"exhaustive_max_time correctness", 4,
		[&]()
		{
			std::vector<double> optimal_time_totals =
			{
				500,		1033.05,	1100,	1600,	1600,
				1600,		1900,		2100,	2300,	2300,
				2300,		2300,		2400,	2400,	2400,
				2400,		2400,		2400,	2400,	2400
			};
			
			for ( int optimal_index = 0; optimal_index < optimal_time_totals.size(); optimal_index++ )
			{
				int n = optimal_index + 1;
				double expected_time = optimal_time_totals[optimal_index];
				
				auto small_rides = filter_ride_vector(*filtered_rides, 1, 2000, n);
				TEST_TRUE("non-null", small_rides);
				
				auto solution = exhaustive_max_time(*small_rides, 2000);
				TEST_TRUE("non-null", solution);
				
				double actual_cost, actual_time;
				sum_ride_vector(*solution, actual_cost, actual_time);
				
				// Round
				expected_time	= std::round( expected_time	/ 100.0) * 100;
				actual_time		= std::round( actual_time	/ 100.0) * 100;
				
				std::stringstream ss;
				ss
					<< "exhaustive search n = " << n << " (optimal index = " << optimal_index << ")"
					<< ", expected time = " << expected_time
					<< " but algorithm found = " << actual_time
					;
				TEST_EQUAL(ss.str(), expected_time, actual_time);
				
				auto greedy_solution = greedy_max_time(*small_rides, 2000);
				double greedy_actual_cost, greedy_actual_time;
				sum_ride_vector(*solution, greedy_actual_cost, greedy_actual_time);
				greedy_actual_time	= std::round( greedy_actual_time	/ 100.0) * 100;
				TEST_EQUAL("Exhaustive and greedy get the same answer", actual_time, greedy_actual_time);
			}
		}
	);

	return rubric.run();
}




