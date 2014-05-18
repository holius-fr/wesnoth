/*
   Copyright (C) 2014 by Chris Beck <render787@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-test"

#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/test/unit_test.hpp>

#include "log.hpp"
#include "map_location.hpp"

static std::vector<map_location> preset_locs;
static map_location va,vb,vc,vz,vt1,vt2,vt3;
static map_location::DIRECTION n = map_location::NORTH;
static map_location::DIRECTION ne = map_location::NORTH_EAST;
static map_location::DIRECTION nw = map_location::NORTH_WEST;
static map_location::DIRECTION s = map_location::SOUTH;
static map_location::DIRECTION se = map_location::SOUTH_EAST;
static map_location::DIRECTION sw = map_location::SOUTH_WEST;


struct MLFixture 
{
	MLFixture()
	{
		va = map_location(3,4);
		vb = map_location(10,8);
		vc = map_location(0,9);
		vz = map_location(0,0);
		vt1 = map_location(va.vector_negation());
		vt2 = map_location(vb.vector_sum(vc));
		vt3 = map_location(va.vector_sum(vc.vector_negation()));
		
		preset_locs.push_back(va);
		preset_locs.push_back(vb);
		preset_locs.push_back(vc);
		preset_locs.push_back(vz);
		preset_locs.push_back(vt1);
		preset_locs.push_back(vt2);
		preset_locs.push_back(vt3);
	}

	~MLFixture() {}
};

BOOST_GLOBAL_FIXTURE ( MLFixture )

BOOST_AUTO_TEST_SUITE ( test_map_location )

//#define MAP_LOCATION_GET_OUTPUT

static map_location vector_difference(const map_location & v1, const map_location & v2)
{
	map_location ret(v1);
	ret.vector_difference_assign(v2);
	return ret;
}

static void characterization_distance_direction (const std::vector<map_location> & locs, const std::vector<map_location::DIRECTION> & dir_answers, const std::vector<size_t> & int_answers)
{
	BOOST_CHECK_EQUAL(dir_answers.size(), int_answers.size());

	std::vector<map_location::DIRECTION>::const_iterator dir_it = dir_answers.begin();
	std::vector<size_t>::const_iterator int_it = int_answers.begin();

	for (std::vector<map_location>::const_iterator it_a = locs.begin(); it_a != locs.end(); ++it_a) {
		for (std::vector<map_location>::const_iterator it_b = it_a + 1; it_b != locs.end(); ++it_b) {
			const map_location & a = *it_a;
			const map_location & b = *it_b;
#ifdef MAP_LOCATION_GET_OUTPUT
			std::cout << "(std::make_pair(" << distance_between(a,b) << ",\t\"" 
				<< map_location::write_direction( a.get_relative_dir(b)) << "\"))" << std::endl;
#else
			int expected_dist = *(int_it++);
			map_location::DIRECTION expected_dir = *(dir_it++);
			BOOST_CHECK_EQUAL( expected_dist, distance_between(a,b) );
			BOOST_CHECK_EQUAL( expected_dist, distance_between(b,a) );
			BOOST_CHECK_EQUAL( expected_dir, a.get_relative_dir(b) );			
			//Note: This is not a valid assertion. get_relative_dir has much symmetry but not radial.
			//BOOST_CHECK_EQUAL( map_location::get_opposite_dir(expected_dir), b.get_relative_dir(a) );
			BOOST_CHECK_EQUAL( a.vector_sum(b), b.vector_sum(a));
			map_location temp1 = a;
			temp1.vector_difference_assign(b);
			map_location temp2 = b;
			temp2.vector_difference_assign(a);
			BOOST_CHECK_EQUAL( temp1, temp2.vector_negation());
			BOOST_CHECK_EQUAL( a, a.vector_negation().vector_negation());

			for (std::vector<map_location>::const_iterator it_c = it_b + 1; it_c != locs.end(); ++it_c) {
				const map_location & c = *it_c;
				BOOST_CHECK_EQUAL(a.vector_sum(b.vector_sum(c)) , a.vector_sum(b).vector_sum(c));
				BOOST_CHECK_EQUAL(a.vector_sum(vector_difference(b,c)) , vector_difference(a.vector_sum(b),c));
				BOOST_CHECK_EQUAL(vector_difference(a,b.vector_sum(c)) , vector_difference(vector_difference(a,b),c));
				//TODO: Investigate why this doesn't work
				//BOOST_CHECK_EQUAL(expected_dir, (a.vector_sum(c)).get_relative_dir(b.vector_sum(c)));
			}
#endif
		}
	}

	BOOST_CHECK_MESSAGE( dir_it == dir_answers.end(), "Did not exhaust answers list.");
	BOOST_CHECK_MESSAGE( int_it == int_answers.end(), "Did not exhaust answers list.");
}

static size_t get_first (std::pair<size_t, std::string> arg) {return arg.first; }
static map_location::DIRECTION get_second (std::pair<size_t, std::string> arg) {return map_location::parse_direction(arg.second); }

BOOST_AUTO_TEST_CASE ( map_location_characterization_test )
{
	std::vector<std::pair<size_t, std::string> > generated_answers = boost::assign::list_of(std::make_pair(7,	"se"))
(std::make_pair(6,	"s"))
(std::make_pair(6,	"nw"))
(std::make_pair(12,	"n"))
(std::make_pair(16,	"s"))
(std::make_pair(9,	"n"))
(std::make_pair(10,	"sw"))
(std::make_pair(13,	"nw"))
(std::make_pair(19,	"nw"))
(std::make_pair(9,	"s"))
(std::make_pair(16,	"n"))
(std::make_pair(9,	"n"))
(std::make_pair(15,	"n"))
(std::make_pair(13,	"se"))
(std::make_pair(15,	"n"))
(std::make_pair(6,	"n"))
(std::make_pair(22,	"s"))
(std::make_pair(6,	"n"))
(std::make_pair(28,	"s"))
(std::make_pair(6,	"se"))
(std::make_pair(25,	"n")).to_container(generated_answers);

	std::vector<size_t> ans1;
	std::vector<map_location::DIRECTION> ans2;
	std::transform(generated_answers.begin(), generated_answers.end(), back_inserter(ans1), &get_first);
	std::transform(generated_answers.begin(), generated_answers.end(), back_inserter(ans2), &get_second);

	characterization_distance_direction(preset_locs, ans2, ans1);
}

static std::pair<map_location , map_location> mirror_walk( std::pair<map_location,map_location> p, map_location::DIRECTION d)
{
	p.first = p.first.get_direction(d);
	p.second = p.second.get_direction(map_location::get_opposite_dir(d));
	BOOST_CHECK_EQUAL(p.first, p.second.vector_negation());
	return p;
}

BOOST_AUTO_TEST_CASE ( reality_check_vector_negation )
{
	std::pair<map_location, map_location> p(vz,vz);

	p = mirror_walk(p, n);
	p = mirror_walk(p, n);
	p = mirror_walk(p, ne);
	p = mirror_walk(p, nw);
	p = mirror_walk(p, s);
	p = mirror_walk(p, nw);
	p = mirror_walk(p, se);
	p = mirror_walk(p, sw);
	p = mirror_walk(p, n);
	p = mirror_walk(p, n);
	p = mirror_walk(p, sw);
	p = mirror_walk(p, sw);
	p = mirror_walk(p, sw);
}

static void reality_check_get_direction_helper(const map_location & loc, const map_location::DIRECTION d)
{
	map_location lz(vz.get_direction(d));

	map_location temp(loc.vector_sum(lz));
	BOOST_CHECK_EQUAL(temp, loc.get_direction(d));
	BOOST_CHECK(tiles_adjacent(loc,temp));
	BOOST_CHECK(tiles_adjacent(temp,loc));
	BOOST_CHECK_EQUAL(distance_between(loc,temp), 1);
}

BOOST_AUTO_TEST_CASE ( reality_check_get_direction )
{
	map_location a(3,4);
	map_location b(6,5);

	reality_check_get_direction_helper(a,n);
	reality_check_get_direction_helper(a,nw);
	reality_check_get_direction_helper(a,ne);
	reality_check_get_direction_helper(a,s);
	reality_check_get_direction_helper(a,sw);
	reality_check_get_direction_helper(a,se);

	reality_check_get_direction_helper(b,n);
	reality_check_get_direction_helper(b,nw);
	reality_check_get_direction_helper(b,ne);
	reality_check_get_direction_helper(b,s);
	reality_check_get_direction_helper(b,sw);
	reality_check_get_direction_helper(b,se);
}

/**
 * This commented block was used to visualize the output of get_relative_dir
 * and to help derive the implementation in commit 
 * 829b74c2beaa18eda42710c364b12c987f9caed5
 */

/*
static std::string dir_to_terrain (const map_location::DIRECTION dir) 
{
	switch(dir) {
		case map_location::NORTH:      return "Gg";
		case map_location::SOUTH:      return "Ai";
		case map_location::SOUTH_EAST: return "Gs^Fp";
		case map_location::SOUTH_WEST: return "Ss";
		case map_location::NORTH_EAST: return "Hd";
		case map_location::NORTH_WEST: return "Md";
		default: return "Xv";
	}
}

static std::string visualize_helper ( int x , int y, const map_location & c )
{
	map_location temp(x,y);
	return dir_to_terrain(c.get_relative_dir(temp));
}

BOOST_AUTO_TEST_CASE ( visualize_get_relative_dir )
{
	map_location c7(7,8), c8(8,8);

	std::cout << "***" << std::endl;
	int x;
	int y;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 15; x++) {
			std::cout << visualize_helper(x,y,c7) << ", ";
		}
		std::cout << visualize_helper(x,y,c7) << std::endl;
	}

	std::cout << "***" << std::endl;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 15; x++) {
			std::cout << visualize_helper(x,y,c8) << ", ";
		}
		std::cout << visualize_helper(x,y,c8) << std::endl;
	}

	std::cout << "***" << std::endl;
}*/

BOOST_AUTO_TEST_SUITE_END()
