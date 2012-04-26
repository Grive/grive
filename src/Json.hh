/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include <string>
#include <map>
#include <vector>

struct json_object ;

namespace gr {

class Json
{
public :
	typedef std::map<std::string, Json>	Object ;
	typedef std::vector<Json>			Array ;

public :
	template <typename T>
	explicit Json( const T& val ) ;
	
	Json() ;
	Json( const Json& rhs ) ;
	~Json( ) ;
	
	static Json Parse( const std::string& str ) ;
	
	Json operator[]( const std::string& key ) const ;
	Json operator[]( const std::size_t& idx ) const ;
	Json& operator=( const Json& rhs ) ;
	
	void Swap( Json& other ) ;
	
	template <typename T>
	T As() const ;
	
	template <typename T>
	bool Is() const ;
	
	bool Has( const std::string& key ) const ;
	void Add( const std::string& key, const Json& json ) ;
	
	friend std::ostream& operator<<( std::ostream& os, const Json& json ) ;

	enum Type { null_type, bool_type, double_type, int_type, object_type, array_type, string_type } ;
	
	Type DataType() const ;
	
private :
	Json( struct json_object *json ) ;
	
	struct NotOwned {} ;
	Json( struct json_object *json, NotOwned ) ;
	
private :
	struct json_object	*m_json ;
} ;

}