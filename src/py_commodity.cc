/*
 * Copyright (c) 2003-2009, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <system.hh>

#include "pyinterp.h"
#include "commodity.h"
#include "annotate.h"
#include "pool.h"

namespace ledger {

using namespace boost::python;

void py_add_price(commodity_t&	    commodity,
		  const datetime_t& date,
		  const amount_t&   price)
{
  commodity.add_price(date, price);
}

namespace {

  commodity_t * py_create_1(commodity_pool_t& pool,
			    const string&     symbol)
  {
    return pool.create(symbol);
  }
  commodity_t * py_create_2(commodity_pool_t&	pool,
			    const string&	symbol,
			    const annotation_t& details)
  {
    return pool.create(symbol, details);
  }

  commodity_t * py_find_or_create_1(commodity_pool_t& pool,
				    const string&     symbol)
  {
    return pool.find_or_create(symbol);
  }
  commodity_t * py_find_or_create_2(commodity_pool_t&	 pool,
				     const string&	 symbol,
				     const annotation_t& details)
  {
    return pool.find_or_create(symbol, details);
  }

  commodity_t * py_find_1(commodity_pool_t& pool,
			  const string&	    name)
  {
    return pool.find(name);
  }

  commodity_t * py_find_2(commodity_pool_t&   pool,
			  const string&	      symbol,
			  const annotation_t& details)
  {
    return pool.find(symbol, details);
  }

  // Exchange one commodity for another, while recording the factored price.

  void py_exchange_3(commodity_pool_t& pool,
		     commodity_t&      commodity,
		     const amount_t&   per_unit_cost,
		     const datetime_t& moment)
  {
    pool.exchange(commodity, per_unit_cost, moment);
  }

  cost_breakdown_t py_exchange_5(commodity_pool_t&		    pool,
				 const amount_t&		    amount,
				 const amount_t&		    cost,
				 const bool			    is_per_unit,
				 const boost::optional<datetime_t>& moment,
				 const boost::optional<string>&     tag)
  {
    return pool.exchange(amount, cost, is_per_unit, moment, tag);
  }

} // unnamed namespace

void export_commodity()
{
  class_< commodity_pool_t, boost::noncopyable > ("CommodityPool", no_init)
    .add_property("null_commodity",
		  make_getter(&commodity_pool_t::null_commodity,
			      return_value_policy<reference_existing_object>()),
		  make_setter(&commodity_pool_t::null_commodity,
			      with_custodian_and_ward<1, 2>()))
    .add_property("default_commodity",
		  make_getter(&commodity_pool_t::default_commodity,
			      return_value_policy<reference_existing_object>()),
		  make_setter(&commodity_pool_t::default_commodity,
			      with_custodian_and_ward<1, 2>()))

    .add_property("keep_base",
		  make_getter(&commodity_pool_t::keep_base),
		  make_setter(&commodity_pool_t::keep_base))
    .add_property("price_db",
		  make_getter(&commodity_pool_t::price_db),
		  make_setter(&commodity_pool_t::price_db))
    .add_property("quote_leeway",
		  make_getter(&commodity_pool_t::quote_leeway),
		  make_setter(&commodity_pool_t::quote_leeway))
    .add_property("get_quotes",
		  make_getter(&commodity_pool_t::get_quotes),
		  make_setter(&commodity_pool_t::get_quotes))
    .add_property("get_commodity_quote",
		  make_getter(&commodity_pool_t::get_commodity_quote),
		  make_setter(&commodity_pool_t::get_commodity_quote))

    .def("make_qualified_name", &commodity_pool_t::make_qualified_name)

    .def("create", py_create_1, return_value_policy<reference_existing_object>())
    .def("create", py_create_2, return_value_policy<reference_existing_object>())

    .def("find_or_create", py_find_or_create_1,
	 return_value_policy<reference_existing_object>())
    .def("find_or_create", py_find_or_create_2,
	 return_value_policy<reference_existing_object>())

    .def("find", py_find_1, return_value_policy<reference_existing_object>())
    .def("find", py_find_2, return_value_policy<reference_existing_object>())

    .def("exchange", py_exchange_3, with_custodian_and_ward<1, 2>())
    .def("exchange", py_exchange_5)

    .def("parse_price_directive", &commodity_pool_t::parse_price_directive)
    .def("parse_price_expression", &commodity_pool_t::parse_price_expression,
	 return_value_policy<reference_existing_object>())
    ;

  scope().attr("COMMODITY_STYLE_DEFAULTS")  = COMMODITY_STYLE_DEFAULTS;
  scope().attr("COMMODITY_STYLE_SUFFIXED")  = COMMODITY_STYLE_SUFFIXED;
  scope().attr("COMMODITY_STYLE_SEPARATED") = COMMODITY_STYLE_SEPARATED;
  scope().attr("COMMODITY_STYLE_EUROPEAN")  = COMMODITY_STYLE_EUROPEAN;
  scope().attr("COMMODITY_STYLE_THOUSANDS") = COMMODITY_STYLE_THOUSANDS;
  scope().attr("COMMODITY_NOMARKET")        = COMMODITY_NOMARKET;
  scope().attr("COMMODITY_BUILTIN")         = COMMODITY_BUILTIN;
  scope().attr("COMMODITY_WALKED")          = COMMODITY_WALKED;
  scope().attr("COMMODITY_KNOWN")           = COMMODITY_KNOWN;
  scope().attr("COMMODITY_PRIMARY")         = COMMODITY_PRIMARY;

  class_< commodity_t, bases<>,
	  commodity_t, boost::noncopyable > ("Commodity", no_init)
    .def("symbol_needs_quotes", &commodity_t::symbol_needs_quotes)
    .staticmethod("symbol_needs_quotes")

    .def(self == self)

    .def("drop_flags", &commodity_t::drop_flags)

    .def("add_price", py_add_price)

    .def("precision", &commodity_t::precision)
    ;

#if 0
  class_< annotation_t, bases<>,
    commodity_t, boost::noncopyable > ("Annotation", no_init)
    ;
  class_< keep_details_t, bases<>,
    commodity_t, boost::noncopyable > ("KeepDetails", no_init)
    ;
  class_< annotated_commodity_t, bases<>,
    commodity_t, boost::noncopyable > ("AnnotatedCommodity", no_init)
    ;
#endif
}

} // namespace ledger
