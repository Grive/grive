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

#include <QtCore/QDebug>

namespace gr {

DriveModel::DriveModel( )
{
}

Qt::ItemFlags DriveModel::flags( const QModelIndex& ) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
}

QVariant DriveModel::data( const QModelIndex& index, int role ) const
{
	return role == Qt::DisplayRole ? QString("wow") : QVariant() ;
}

QVariant DriveModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	return role == Qt::DisplayRole ? QString("header") : QVariant() ;
}

int DriveModel::rowCount( const QModelIndex& parent ) const
{
	return 10 ;
}

int DriveModel::columnCount( const QModelIndex& parent ) const
{
	return 1 ;
}

bool DriveModel::hasChildren( const QModelIndex& parent ) const
{
	return parent.isValid() ? false : true ;
}

QModelIndex DriveModel::index( int row, int column, const QModelIndex & parent ) const
{
	return parent.isValid() ? QModelIndex() : createIndex( row, column, 0 ) ;
}

QModelIndex DriveModel::parent( const QModelIndex& idx ) const
{
	return QModelIndex() ;
}

} // end of namespace
