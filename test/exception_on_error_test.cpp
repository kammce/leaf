// Copyright (c) 2018-2021 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/config.hpp>
#ifdef BOOST_LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/pred.hpp>
#   include <boost/leaf/exception.hpp>
#   include <boost/leaf/on_error.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct e_info { int value; };

struct some_exception: std::exception { };

void do_stuff_inner()
{
    throw leaf::exception(some_exception());
}

template <class Thrower>
void do_stuff( Thrower thrower )
{
  auto augment = leaf::on_error(e_info<1>{1});
  return leaf::try_catch(
    [&]
    {
        thrower();
        BOOST_ERROR("thrower must throw");
    },

    [ ]( leaf::catch_<some_exception> )
    {
        auto augment = leaf::on_error(e_info<2>{2});
        throw;
    } );
}

int main()
{
    {
        auto r = leaf::try_catch(
            [ ]
            {
                do_stuff( [ ] { throw leaf::exception(some_exception()); } );
                return 1;
            },

            [ ]( leaf::match_value<e_info<1>, 1>, leaf::match_value<e_info<2>, 2> )
            {
                return 0;
            },

            [ ]( leaf::verbose_diagnostic_info const & err )
            {
                std::cerr << err;
                return 2;
            } );
        BOOST_TEST_EQ(r, 0);
    }

    {
        auto r = leaf::try_catch(
            [ ]
            {
                do_stuff( [ ] { throw some_exception(); } );
                return 1;
            },

            [ ]( leaf::match_value<e_info<1>, 1>, leaf::match_value<e_info<2>, 2> )
            {
                return 0;
            },

            [ ]( leaf::verbose_diagnostic_info const & err )
            {
                std::cerr << err;
                return 2;
            } );
        BOOST_TEST_EQ(r, 0);
    }

    return boost::report_errors();
}

#endif
