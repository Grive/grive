/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#include "Resource.hh"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>

namespace gr {

namespace http
{
	class Agent ;
}

class Json ;

namespace v2 {

namespace details
{
	using namespace boost::multi_index ;
	struct ByID {} ;
	struct ByHref {} ;
	struct ByIdentity {} ;

	typedef multi_index_container<
		Resource*,
		indexed_by<
			hashed_unique<tag<ByIdentity>,	identity<Resource*> >,
			hashed_non_unique<tag<ByID>,	const_mem_fun<Resource, std::string,	&Resource::ID> >
		>
	> DB ;
	
	typedef DB::index<ByID>::type		ID ;
	typedef DB::index<ByIdentity>::type	Set ;
}

class Drive
{
public :
	Drive( ) ;
	
	void Refresh( http::Agent *agent ) ;

	Resource* Find( const std::string& id ) ;
	const Resource* Find( const std::string& id ) const ;

	Resource* Root() ;
	const Resource* Root() const ;
	
	const Resource* Child( const Resource *parent, std::size_t idx ) const ;

private :
	Resource* NewResource( const Json& item ) ;
	Resource* FindRoot( http::Agent *agent ) ;
	
private :
	details::DB	m_db ;
	
	Resource	m_root ;
} ;

} } // end of namespace gr::v2
