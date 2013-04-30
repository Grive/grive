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

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace gr {

class Val
{
public :
	enum TypeEnum { null_type, bool_type, double_type, int_type, object_type, array_type, string_type } ;

private :
	template <typename T>
	struct Type2Enum ;

	typedef std::vector<Val>			Array ;
	typedef std::map<std::string, Val>	Object ;

public :
	Val() ;
	Val( const Val& v ) ;
	~Val() ;

	template <typename T>
	Val( const T& t ) ;
	
	template <typename T>
	const T& As() const ;

	template <typename T>
	T& As() ;

	template <typename T>
	bool Is() const ;

	TypeEnum Type() const ;

	const Val& operator[]( const std::string& key ) const ;
	const Val& operator[]( std::size_t index ) const ;

	void Add( const std::string& key, const Val& value ) ;

private :
	struct Base ;
	
	template <typename T>
	struct Impl ;
	
	std::auto_ptr<Base>	m_base ;

private :
	// callback functions
	static int OnNull( void *ctx ) ;
	static int OnBool( void *ctx, int value ) ;
	static int OnInt( void *ctx, long long value ) ;
	static int OnDouble( void *ctx, double value ) ;
	static int OnStr( void *ctx, const unsigned char *str, std::size_t len ) ;
	static int StartMap( void *ctx ) ;
	static int EndMap( void *ctx ) ;
	static int StartArray( void *ctx ) ;
	static int EndArray( void *ctx ) ;
} ;

template <>
struct Val::Type2Enum<void>
{
	static const TypeEnum type = null_type ;
} ;

template <>
struct Val::Type2Enum<int>
{
	static const TypeEnum type = int_type ;
} ;

template <>
struct Val::Type2Enum<bool>
{
	static const TypeEnum type = bool_type ;
} ;

template <>
struct Val::Type2Enum<double>
{
	static const TypeEnum type = double_type ;
} ;

template <>
struct Val::Type2Enum<std::string>
{
	static const TypeEnum type = string_type ;
} ;

template <>
struct Val::Type2Enum<Val::Array>
{
	static const TypeEnum type = array_type ;
} ;

template <>
struct Val::Type2Enum<Val::Object>
{
	static const TypeEnum type = object_type ;
} ;

struct Val::Base
{
	virtual ~Base() {}
	virtual Base* Clone() const = 0 ;
	virtual TypeEnum Type() const = 0 ;
} ;

template <typename T>
struct Val::Impl		: public Base
{
	T val ;
	Impl( const T& t ) : val(t) {}
	Impl<T>* Clone() const { return new Impl<T>(val); }
	TypeEnum Type() const { return Type2Enum<T>::type ; }
} ;

template <>
struct Val::Impl<void>	: public Base
{
	Impl<void>* Clone() const { return new Impl<void>; }
	TypeEnum Type() const { return null_type ; }
} ;

template <typename T>
Val::Val( const T& t ) :
	m_base( new Impl<T>(t) )
{
}

template <typename T>
const T& Val::As() const
{
	const Impl<T> *impl = &dynamic_cast<const Impl<T>&>( *m_base ) ;
	return impl->val ;
}

template <typename T>
T& Val::As()
{
	Impl<T> *impl = &dynamic_cast<Impl<T>&>( *m_base ) ;
	return impl->val ;
}

template <typename T>
bool Val::Is() const
{
	return Type() == Type2Enum<T>::type ;
}

} // end of namespace

