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

#include <memory>
#include <utility>

namespace gr {

namespace impl
{
	template <typename Type> class FuncImpl ;
	
	template <typename R> class FuncImpl<R (void)>
	{
	public :
		virtual R operator()( ) = 0 ;
		virtual FuncImpl* Clone() const = 0 ;
		virtual ~FuncImpl() {}
	} ;

	template <typename R, typename P1> class FuncImpl<R (P1)>
	{
	public :
		virtual R operator()(P1) = 0 ;
		virtual FuncImpl* Clone() const = 0 ;
		virtual ~FuncImpl() {}
	} ;
	
	template <typename R, typename P1, typename P2> class FuncImpl<R (P1,P2)>
	{
	public :
		virtual R operator()(P1,P2) = 0 ;
		virtual FuncImpl* Clone() const = 0 ;
		virtual ~FuncImpl() {}
	} ;

	template <typename Type>
	struct FuncTrait ;
	
	struct NullType {} ;
	
	template <typename R> struct FuncTrait<R(void)>
	{
		typedef R			ReturnType ;
		typedef NullType	Param1Type ;
		typedef NullType	Param2Type ;
	} ;
	
	template <typename R, typename P1> struct FuncTrait<R(P1)>
	{
		typedef R			ReturnType ;
		typedef P1			Param1Type ;
		typedef NullType	Param2Type ;
	} ;
	
	template <typename R, typename P1, typename P2> struct FuncTrait<R(P1,P2)>
	{
		typedef R	ReturnType ;
		typedef P1	Param1Type ;
		typedef P2	Param2Type ;
	} ;

	template <class Type, typename F> class FuncHolder : public FuncImpl<Type>
	{
	public :
		explicit FuncHolder( const F& f ) : m_func( f ) { }
		FuncHolder* Clone() const {	return new FuncHolder( *this ) ; }
		
		typedef typename FuncTrait<Type>::ReturnType ReturnType ;
		
		ReturnType operator()() { return m_func(); }
		
		ReturnType operator()( typename FuncTrait<Type>::Param1Type p1)
		{
			return m_func(p1);
		}

		ReturnType operator()(
			typename FuncTrait<Type>::Param1Type p1,
			typename FuncTrait<Type>::Param2Type p2)
		{
			return m_func(p1,p2);
		}

	private :
		F m_func ;
	} ;
	
	template <typename Type>
	struct NullFunc
	{
		typedef typename FuncTrait<Type>::ReturnType ReturnType ;
		ReturnType operator()()
		{
			return ReturnType() ;
		}
		ReturnType operator()( typename FuncTrait<Type>::Param1Type p1)
		{
			return ReturnType() ;
		}
		ReturnType operator()(
			typename FuncTrait<Type>::Param1Type p1,
			typename FuncTrait<Type>::Param2Type p2)
		{
			return ReturnType() ;
		}
	} ;
	
	template <typename Type, typename F>
	FuncHolder<Type,F>* MakeFuncHolder( F func )
	{
		return new FuncHolder<Type,F>( func ) ;
	}
}

template <typename Type>
class Function
{
public :
	Function( ) :
		m_pimpl( impl::MakeFuncHolder<Type>( impl::NullFunc<Type>() ) )
	{
	}
	Function( const Function& f ) :
		m_pimpl( f.m_pimpl->Clone() )
	{
	}
	template <typename F>
	Function( const F& f ) :
		m_pimpl( impl::MakeFuncHolder<Type>( f ) )
	{
	}
	
	Function& operator=( const Function& f )
	{
		Function tmp( f ) ;
		std::swap( m_pimpl, tmp.m_pimpl ) ;
		return *this ;
	}
	~Function( )
	{
	}

	typedef typename impl::FuncTrait<Type>::ReturnType ReturnType ;
	
	ReturnType operator()( )
	{
		return (*m_pimpl)() ;
	}
	
	template <typename P1> ReturnType operator()( P1 p1 )
	{
		return (*m_pimpl)( p1 ) ;
	}
	
	template <typename P1, typename P2> ReturnType operator()( P1 p1, P2 p2 )
	{
		return (*m_pimpl)( p1, p2 ) ;
	} ;

private :
	typedef impl::FuncImpl<Type> Impl ;
	std::auto_ptr<Impl>	m_pimpl ;
} ;

} // end of namespace
