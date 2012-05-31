/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include "util/Exception.hh"

#include <string>
#include <map>
#include <vector>

struct json_object ;

namespace gr {

class StdioFile ;

class Json
{
public :
	typedef std::map<std::string, Json>	Object ;
	typedef std::vector<Json>			Array ;

	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct JsonTag, Json>	JsonInfo ;
	
public :
	template <typename T>
	explicit Json( const T& val ) ;
	
	Json() ;
	Json( const Json& rhs ) ;
	Json( const char *str ) ;
	~Json( ) ;
	
	static Json Parse( const std::string& str ) ;
	static Json ParseFile( const std::string& filename ) ;
	
	Json operator[]( const std::string& key ) const ;
	Json operator[]( const std::size_t& idx ) const ;
	Json& operator=( const Json& rhs ) ;
	
	void Swap( Json& other ) ;

	std::string Str() const ;
	int		Int() const ;
	double	Double() const ;
	bool	Bool() const ;
	Array	AsArray() const ;
	Object	AsObject() const ;

	template <typename T>
	bool Is() const ;
	
	bool Has( const std::string& key ) const ;
	bool Get( const std::string& key, Json& json ) const ;
	void Add( const std::string& key, const Json& json ) ;
	void Add( const Json& json ) ;
	Json FindInArray( const std::string& key, const std::string& value ) const ;
	bool FindInArray( const std::string& key, const std::string& value, Json& result ) const ;
	
	friend std::ostream& operator<<( std::ostream& os, const Json& json ) ;
	void Write( StdioFile& file ) const ;

	enum Type { null_type, bool_type, double_type, int_type, object_type, array_type, string_type } ;
	
	Type DataType() const ;
	
private :
	Json( struct json_object *json ) ;
	
	struct NotOwned {} ;
	Json( struct json_object *json, NotOwned ) ;
	
private :
public :
	struct json_object	*m_json ;
} ;

}