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

#include <string>
#include <vector>

namespace gr { namespace xml {

class Node
{
public :
	class iterator ;

public :
	Node() ;
	Node( const Node& node ) ;
	~Node() ;

	static Node Element( const std::string& name ) ;
	static Node Text( const std::string& name ) ;
	
	Node& operator=( const Node& node ) ;
	
	Node AddElement( const std::string& name ) ;
	Node AddText( const std::string& text ) ;
	void AddNode( const Node& node ) ;
	void AddAttribute( const std::string& name, const std::string& val ) ;

	Node operator[]( const std::string& name ) const ;
	
	const std::string& Name() const ;
	std::string Value() const ;
	
	// read-only access to the reference counter. for checking.
	std::size_t RefCount() const ;
	
	enum Type { element, attr, text } ;
	Type GetType() const ;

	static bool IsCompatible( Type parent, Type child ) ;

	// TODO: implement iterator begin/end functions instead
	std::vector<Node> Children() const ;
	
	// TODO: implement iterator begin/end functions instead
	std::vector<Node> Attr() const ;
	
private :
	class	Impl ;
	typedef std::vector<Impl*>	ImplVec ;
	
public :
	class iterator
	{
	public :
		iterator( ImplVec::iterator *impl ) ;
	
		typedef Node value_type ;
		
		value_type operator*() const ;
// 		value_type operator*() const ;
	
	private :
		ImplVec::iterator	*m_node ;
	} ;
	

private :
	explicit Node( Impl *impl ) ;

private :
	Impl *m_ptr ;
} ;

std::ostream& operator<<( std::ostream& os, const Node& node ) ;

} } // end of namespace
