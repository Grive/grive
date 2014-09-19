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

#include "ValVisitor.hh"

#include "Val.hh"
#include "util/Exception.hh"

#include <stack>
#include <memory>

namespace gr {

class ValBuilder : public ValVisitor
{
public :
	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct Mismatch,		Val> Mismatch_ ;
	typedef boost::error_info<struct Unexpected,	Val> Unexpected_ ;
	typedef boost::error_info<struct NoKey,			Val> NoKey_ ;

public :
	ValBuilder( ) ;
	~ValBuilder() ;

	void Visit( long long t ) ;
	void Visit( double t ) ;
	void Visit( const std::string& t ) ;
	void Visit( bool t ) ;
	void VisitNull() ;
	void Build( const Val& t ) ;

	void StartArray() ;
	void EndArray() ;
	void StartObject() ;
	void VisitKey( const std::string& t ) ;
	void EndObject() ;

	Val Result() const ;

private :
	void End( Val::TypeEnum type ) ;

private :
	struct Level
	{
		Val	key ;
		Val	val ;
	} ;

	std::stack<Level> 	m_ctx ;
} ;

} // end of namespace

