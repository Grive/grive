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

#include "DriveModel.hh"

#include "drive2/Resource.hh"

#include <QtCore/QDebug>

namespace gr {

using namespace v2;

DriveModel::DriveModel( http::Agent *agent )
{
	m_drv.Refresh( agent ) ;
}

Qt::ItemFlags DriveModel::flags( const QModelIndex& ) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
}

QVariant DriveModel::data( const QModelIndex& index, int role ) const
{
	const Resource *res = Res(index) ;
	if ( role == Qt::DisplayRole && res != 0 )
	{
		switch ( index.column() )
		{
			case 0: 	return QString::fromUtf8(res->Title().c_str()) ;
			default:	break ;
		}
	}
	
	return QVariant() ;
}

QVariant DriveModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	return role == Qt::DisplayRole ? QString("header") : QVariant() ;
}

int DriveModel::rowCount( const QModelIndex& parent ) const
{
	return Res(parent)->ChildCount() ;
}

int DriveModel::columnCount( const QModelIndex& parent ) const
{
	return 1 ;
}

bool DriveModel::hasChildren( const QModelIndex& parent ) const
{
	return Res(parent)->ChildCount() > 0 ;
}

QModelIndex DriveModel::index( int row, int column, const QModelIndex & parent ) const
{
	return createIndex( row, column, const_cast<Resource*>(m_drv.Child(Res(parent), row)) ) ;
}

const Resource* DriveModel::Res( const QModelIndex& idx ) const
{
	return idx.isValid()
		? reinterpret_cast<const Resource*>(idx.internalPointer())
		: m_drv.Root() ;
}

QModelIndex DriveModel::parent( const QModelIndex& idx ) const
{
	// if I am root, my parent is myself
	const Resource *res = Res(idx) ;
	if ( res == m_drv.Root() )
		return QModelIndex() ;

	// if my parent is root, return model index of root (i.e. QModelIndex())
	const Resource *parent = m_drv.Parent(res) ;
	if ( parent == 0 || parent == m_drv.Root() || idx.column() != 0 )
		return QModelIndex() ;

	// check grand-parent to know the row() of my parent
	const Resource *grand = m_drv.Parent(parent) ;
	return createIndex( grand->Index(parent->ID()), 0, const_cast<Resource*>(parent) ) ;
}

} // end of namespace
