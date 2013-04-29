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

#include <QtCore/QAbstractItemModel>

#include "drive2/Drive.hh"

namespace gr {

namespace http
{
	class Agent ;
}

class DriveModel : public QAbstractItemModel
{
public :
	DriveModel( http::Agent *agent ) ;
	
	// QAbstractItemModel overrides
	Qt::ItemFlags flags( const QModelIndex & index ) const ;
	QVariant data( const QModelIndex& index, int role ) const ;
	QVariant headerData( int section, Qt::Orientation orientation, int role ) const ;
	int rowCount( const QModelIndex& parent ) const ;
	int columnCount( const QModelIndex& parent ) const ;
	bool hasChildren ( const QModelIndex& parent ) const ;
	QModelIndex index( int row, int column, const QModelIndex& parent ) const ;
	QModelIndex parent( const QModelIndex& idx ) const ;

private :
	const v2::Resource* Res( const QModelIndex& idx ) const ;

private :
	v2::Drive	m_drv ;
} ;

} // end of namespace

