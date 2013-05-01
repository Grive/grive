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

#include "ValBuilder.hh"

#include <cassert>

namespace gr {

ValBuilder::ValBuilder( )
{
}

ValBuilder::~ValBuilder()
{
}

void ValBuilder::Build( long long t )
{
	Build(Val(t)) ;
}

void ValBuilder::Build( double t )
{
	Build(Val(t)) ;
}

void ValBuilder::Build( const std::string& t )
{
	Build(Val(t)) ;
}

void ValBuilder::Build( bool t )
{
	Build(Val(t)) ;
}

void ValBuilder::BuildNull()
{
	Build(Val()) ;
}

void ValBuilder::Build( const Val& t )
{
	if ( m_ctx.empty() )
		m_ctx.push( t ) ;
	
	else if ( m_ctx.top().Is<Val::Array>() )
	{
		Val::Array& ar = m_ctx.top().As<Val::Array>() ;
		ar.push_back( t ) ;
	}
	else if ( m_ctx.top().Is<Val::Object>() )
	{
		if ( m_key.get() == 0 )
			BOOST_THROW_EXCEPTION( Error() << NoKey_(t) ) ;
	
		else
		{
			Val::Object& obj = m_ctx.top().As<Val::Object>() ;
			obj.insert( std::make_pair( m_key->As<std::string>(), t ) ) ;
			m_key.reset() ;
		}
	}
	else
		BOOST_THROW_EXCEPTION( Error() << Unexpected_(m_ctx.top()) ) ;
}

void ValBuilder::BuildKey( const std::string& t )
{
	m_key.reset( new Val(t) ) ;
}

void ValBuilder::StartArray()
{
	m_ctx.push( Val( Val::Array() ) ) ;
}

void ValBuilder::EndArray()
{
	End( Val::array_type ) ;
}

void ValBuilder::End( Val::TypeEnum type )
{
	if ( m_ctx.top().Type() == type )
	{
		// get top Val from stack
		Val current ;
		current.Swap( m_ctx.top() ) ;
		m_ctx.pop() ;
		
		Build(current) ;
	}
}

void ValBuilder::StartObject()
{
	m_ctx.push( Val( Val::Object() ) ) ;
}

void ValBuilder::EndObject()
{
	End( Val::object_type ) ;
}

Val ValBuilder::Result() const
{
	assert( m_ctx.size() == 1U ) ;
	return m_ctx.top() ;
}

} // end of namespace
