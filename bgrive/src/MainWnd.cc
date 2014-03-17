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

#include "MainWnd.hh"

#include "drive2/Resource.hh"

#include <QtCore/QDebug>

#include <cassert>

namespace gr {

using namespace v2;

MainWnd::MainWnd(http::Agent *agent) : m_drive(agent)
{
	m_mainwindow.setupUi(this);
	m_mainwindow.m_dir->setModel(&m_drive);

	connect(
			m_mainwindow.m_dir, SIGNAL(activated(const QModelIndex&)),
			this, SLOT(OnClick(const QModelIndex&))
			);
}

void MainWnd::OnClickItem(const QModelIndex& index)
		{
	const Resource *res = m_drive.Res(index);
	if (res != 0)
		ShowResource(res);
}

void MainWnd::ShowResource(const v2::Resource *res)
		{
	m_mainwindow.m_title->setText(QString::fromUtf8(res->Title().c_str()));
	m_mainwindow.m_mime_type->setText(QString::fromUtf8(res->Mime().c_str()));
}

void MainWnd::createTrayIcon() {

}

void MainWnd::setIcon(int index)
{
   // TODO: Fix icon
   // trayIcon->setIcon(m_mainwindow.get);
   // setWindowIcon(icon);
}

void MainWnd::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	/*
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
    default:
    }
    */
}

void MainWnd::setVisible(bool visible)
 {
     minimizeAction->setEnabled(visible);
     maximizeAction->setEnabled(!isMaximized());
     restoreAction->setEnabled(isMaximized() || !visible);
//     m_mainwindow::setVisible(visible);
 }

void MainWnd::OnClose(QCloseEvent *event)
 {

 }

void MainWnd::OnClickDetailTab() {

}
void MainWnd::OnClickPreviewTab() {

}
} // end of namespace
