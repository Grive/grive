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
#include "JsonWriter.hh"
#include "ValVisitor.hh"
#include "util/StdStream.hh"

#include <iostream>

namespace gr {

const Val& Val::Null()
{
	static const Val null( null_type ) ;
	return null ;
}

Val::Val( ) :
	m_base( new Impl<Object> )
{
}

Val::Val( TypeEnum type )
{
	switch ( type )
	{
		case int_type:		m_base.reset( new Impl<long long> ) ;	break ;
		case bool_type:		m_base.reset( new Impl<bool> ) ; 		break ;
		case double_type:	m_base.reset( new Impl<double> ) ; 		break ;
		case string_type:	m_base.reset( new Impl<std::string> ) ;	break ;
		case array_type:	m_base.reset( new Impl<Array> ) ; break ;
		case object_type:	m_base.reset( new Impl<Object> ) ; break ;
		case null_type:	
		default:			m_base.reset( new Impl<void> ) ; break ;
	}
}

Val::Val( const Val& v ) :
	m_base( v.m_base->Clone() )
{
}

Val::~Val()
{
}

void Val::Swap( Val& val )
{
	std::swap( m_base, val.m_base ) ;
}

Val& Val::operator=( const Val& val )
{
	Val tmp(val) ;
	Swap(tmp) ;
	return *this ;
}

Val::TypeEnum Val::Type() const
{
	return m_base->Type() ;
}

const Val& Val::operator[]( const std::string& key ) const
{
	const Object& obj = As<Object>() ;
	Object::const_iterator i = obj.find(key) ;
	if ( i != obj.end() )
		return i->second ;
	
	// shut off compiler warning
	BOOST_THROW_EXCEPTION(Error() << NoKey_(key)) ;
	throw ;
}

Val& Val::operator[]( const std::string& key )
{
	Object& obj = As<Object>() ;
	Object::iterator i = obj.find(key) ;
	if ( i != obj.end() )
		return i->second ;
	
	// shut off compiler warning
	BOOST_THROW_EXCEPTION(Error() << NoKey_(key)) ;
	throw ;
}

const Val& Val::operator[]( std::size_t index ) const
{
	const Array& ar = As<Array>() ;
	if ( index < ar.size() )
		return ar[index] ;
	
	// shut off compiler warning
	BOOST_THROW_EXCEPTION(Error() << OutOfRange_(index)) ;
	throw ;
}

std::string Val::Str() const
{
	return As<std::string>() ;
}

Val::operator std::string() const
{
	return As<std::string>() ;
}

int Val::Int() const
{
	if ( Type() == string_type )
		return std::atoi( As<std::string>().c_str() );
	return static_cast<int>(As<long long>()) ;
}

unsigned long long Val::U64() const
{
	if ( Type() == string_type )
		return strtoull( As<std::string>().c_str(), NULL, 10 );
	return static_cast<unsigned long long>(As<long long>()) ;
}

double Val::Double() const
{
	if ( Type() == string_type )
		return std::atof( As<std::string>().c_str() );
	return As<double>() ;
}

bool Val::Bool() const
{
	return As<bool>() ;
}

const Val::Array& Val::AsArray() const
{
	return As<Array>() ;
}

Val::Array& Val::AsArray()
{
	return As<Array>() ;
}

const Val::Object& Val::AsObject() const
{
	return As<Object>() ;
}

Val::Object& Val::AsObject()
{
	return As<Object>() ;
}

bool Val::Has( const std::string& key ) const
{
	const Object& obj = As<Object>() ;
	return obj.find(key) != obj.end() ;
}

bool Val::Del( const std::string& key )
{
	Object& obj = As<Object>() ;
	return obj.erase(key) > 0 ;
}

Val& Val::Item( const std::string& key )
{
	return As<Object>()[key];
}

bool Val::Get( const std::string& key, Val& val ) const
{
	const Object& obj = As<Object>() ;
	Object::const_iterator i = obj.find(key) ;
	if ( i != obj.end() )
	{
		val = i->second ;
		return true ;
	}
	else
		return false ;
}

void Val::Add( const std::string& key, const Val& value )
{
	As<Object>().insert( std::make_pair(key, value) ) ;
}

void Val::Set( const std::string& key, const Val& value )
{
	Object& obj = As<Object>();
	Object::iterator i = obj.find(key);
	if (i == obj.end())
		obj.insert(std::make_pair(key, value));
	else
		i->second = value;
}

void Val::Add( const Val& json )
{
	As<Array>().push_back( json ) ;
}

void Val::Visit( ValVisitor *visitor ) const
{
	switch ( Type() )
	{
		case null_type:		visitor->VisitNull() ;					break ;
		case int_type:		visitor->Visit( As<long long>() ) ;		break ;
		case double_type:	visitor->Visit( As<double>() ) ;		break ;
		case string_type:	visitor->Visit( As<std::string>() ) ;	break ;
		case bool_type:		visitor->Visit( As<bool>() ) ;			break ;
		
		case object_type:
		{
			visitor->StartObject() ;
			
			const Object& obj = As<Object>() ;
			for ( Object::const_iterator i = obj.begin() ; i != obj.end() ; ++i )
			{
				visitor->VisitKey( i->first ) ;
				i->second.Visit( visitor ) ;
			}
			
			visitor->EndObject() ;
			break ;
		}

		case array_type:
		{
			visitor->StartArray() ;
			
			const Array& arr = As<Array>() ;
			for ( Array::const_iterator i = arr.begin() ; i != arr.end() ; ++i )
				i->Visit( visitor ) ;
			
			visitor->EndArray() ;
			break ;
		}
	}
}

void Val::Select( const Object& obj, const std::string& key, std::vector<Val>& result ) const
{
	Object::const_iterator i = obj.find(key) ;
	if ( i != obj.end() )
		result.push_back(i->second) ;
}

/**	If \a this is an array of objects, this function returns all values of
	the objects in the array with the key \a key. If \a this is an object,
	just return the value with the key \a key. 
*/
std::vector<Val> Val::Select( const std::string& key ) const
{
	std::vector<Val> result ;
	if ( Is<Object>() )
		Select( As<Object>(), key, result ) ;
	
	else if ( Is<Array>() )
	{
		const Array& array = As<Array>() ;
		for ( Array::const_iterator i = array.begin() ; i != array.end() ; ++i )
		{
			if ( i->Is<Object>() )
				Select( i->As<Object>(), key, result ) ;
		}
	}
	return result ;
}

std::ostream& operator<<( std::ostream& os, const Val& val )
{
	StdStream ss( os.rdbuf() ) ;
	JsonWriter wr( &ss ) ;
	val.Visit( &wr ) ;

	return os ;
}

} // end of namespace

namespace std
{
	void swap( gr::Val& v1, gr::Val& v2 )
	{
		v1.Swap( v2 ) ;
	}
	
	ostream& operator<<( ostream& os, gr::Val::TypeEnum t )
	{
		return os << static_cast<int>(t) ;
	}
}
