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

namespace gr {

ValBuilder::ValBuilder( )
{
}

ValBuilder::~ValBuilder()
{
}

void ValBuilder::Visit( long long t )
{
	Build(Val(t)) ;
}

void ValBuilder::Visit( double t )
{
	Build(Val(t)) ;
}

void ValBuilder::Visit( const std::string& t )
{
	Build(Val(t)) ;
}

void ValBuilder::Visit( bool t )
{
	Build(Val(t)) ;
}

void ValBuilder::VisitNull()
{
	Build(Val()) ;
}

void ValBuilder::Build( const Val& t )
{
	if ( m_ctx.empty() )
	{
		Level l = { Val::Null(), t } ;
		m_ctx.push( l ) ;
	}
	
	else if ( m_ctx.top().val.Is<Val::Array>() )
	{
		Val::Array& ar = m_ctx.top().val.As<Val::Array>() ;
		ar.push_back( t ) ;
	}
	else if ( m_ctx.top().val.Is<Val::Object>() )
	{
		if ( !m_ctx.top().key.Is<std::string>() )
			BOOST_THROW_EXCEPTION( Error() << NoKey_(t) ) ;
	
		else
		{
			Val::Object& obj = m_ctx.top().val.As<Val::Object>() ;
			obj.insert( std::make_pair( m_ctx.top().key.Str(), t ) ) ;
			m_ctx.top().key = Val::Null() ;
		}
	}
	else
		BOOST_THROW_EXCEPTION( Error() << Unexpected_(m_ctx.top().val) ) ;
}

void ValBuilder::VisitKey( const std::string& t )
{
	m_ctx.top().key = t ;
}

void ValBuilder::StartArray()
{
	Level l = { Val::Null(), Val(Val::Array()) } ;
	m_ctx.push(l) ;
}

void ValBuilder::EndArray()
{
	End( Val::array_type ) ;
}

void ValBuilder::End( Val::TypeEnum type )
{
	if ( m_ctx.top().val.Type() == type )
	{
		if( !m_ctx.top().key.Is<void>() )
			BOOST_THROW_EXCEPTION( Error() << Unexpected_(m_ctx.top().key) ) ;
	
		// get top Val from stack
		Val current ;
		current.Swap( m_ctx.top().val ) ;
		m_ctx.pop() ;
		
		Build(current) ;
	}
}

void ValBuilder::StartObject()
{
	Level l = { Val::Null(), Val( Val::Object() ) } ;
	m_ctx.push(l) ;
}

void ValBuilder::EndObject()
{
	End( Val::object_type ) ;
}

Val ValBuilder::Result() const
{
	if ( !m_ctx.size() )
		BOOST_THROW_EXCEPTION( Error() << NoKey_( Val(std::string("")) ) ) ;
	Val r = m_ctx.top().val;
	if ( m_ctx.size() > 1 )
		BOOST_THROW_EXCEPTION( Error() << Unexpected_(m_ctx.top().val) ) ;
	return r;
}

} // end of namespace
