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

#include "Val.hh"

namespace gr {

Val::Val( ) :
	m_base( new Impl<void> )
{
}

Val::Val( const Val& v ) :
	m_base( v.m_base->Clone() )
{
}

Val::~Val()
{
}

Val::TypeEnum Val::Type() const
{
	return m_base->Type() ;
}

const Val& Val::operator[]( const std::string& key ) const
{
	static const Val null ;
	if ( Type() == object_type )
	{
		const Object& obj = As<Object>() ;
		Object::const_iterator i = obj.find(key) ;
		return i != obj.end() ? i->second : null ;
	}
	else
		return null ;
}

const Val& Val::operator[]( std::size_t index ) const
{
	static const Val null ;
	return Type() == array_type ? As<Array>().at(index) : null ;
}

void Val::Add( const std::string& key, const Val& value )
{
	As<Object>().insert( std::make_pair(key, value) ) ;
}

} // end of namespace
