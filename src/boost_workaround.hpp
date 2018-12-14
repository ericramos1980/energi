//-----------------------------------------------------------------------------
// Copyright (c) 2018 Andrey Galkin
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//-----------------------------------------------------------------------------
// Current boost dep produces warnings on MinGW builds.
//-----------------------------------------------------------------------------

#ifndef ENERGI_BOOST_WIN32_HPP
#define ENERGI_BOOST_WIN32_HPP

#pragma GCC diagnostic push

#ifdef WIN32
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#	pragma GCC diagnostic ignored "-Wshift-negative-value"
#	pragma GCC diagnostic ignored "-Wstack-protector"
#endif

#include <boost/thread.hpp> // TODO: change to c++11
#include <boost/signals2/last_value.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>

#pragma GCC diagnostic pop

#endif // ENERGI_BOOST_WIN32_HPP
