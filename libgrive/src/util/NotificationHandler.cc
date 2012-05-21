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

    AUTHORS++
    Notification Handler by Daniele "OpenNingia" Simonetti
*/


#include "NotificationHandler.hh"
#include <libnotify/notify.h>
#include <sstream>

namespace gr {

    NotifyItem::NotifyItem(std::string name, std::string type)
        : m_name(name), m_type(type)
    {
    }

    const std::string NotifyItem::name() const
    {
        return m_name;
    }

    const std::string NotifyItem::type() const
    {
        return m_type;
    }

    std::list<NotifyItem> Notify::s_cache;

    Notify::Notify(std::string name)
        : m_name(name)
    {
        notify_init(name.c_str());
    }

    Notify::~Notify()
    {
        notify_uninit();
    }

    void Notify::Emit(std::string message, std::string stock_icon, std::string categ, int tmo)
    {
        NotifyNotification* ntf = notify_notification_new(m_name.c_str(), message.c_str(), stock_icon.c_str());
        notify_notification_set_timeout(ntf, 3000);
        notify_notification_set_category(ntf, categ.c_str());
        notify_notification_set_urgency (ntf, NOTIFY_URGENCY_NORMAL);
        GError *error = NULL;
        notify_notification_show(ntf, &error);
    }

    void Notify::Emit(NotifyItem itm)
    {
            std::stringstream ss;
            if ( itm.type() == "download" ) 
            {
                ss << "Downloaded " << itm.name();
                Notify().Emit(ss.str(), "download", "sync");
            }
            else if (itm.type() == "delete")
            {
                ss << "Deleted " << itm.name();
                Notify().Emit(ss.str(), "delete", "sync");                
            }
            else if (itm.type() == "create")
            {
                ss << "Created " << itm.name();
                Notify().Emit(ss.str(), "new", "sync");                
            }
            else if (itm.type() == "changed")
            {
                ss << "Uploaded changes in " << itm.name();
                Notify().Emit(ss.str(), "upload", "sync");                
            }
            else
            {
                ss << itm.name() << " is now in sync";
                Notify().Emit(ss.str(), "download", "sync");
            }
    }

    void Notify::Push(std::string name, std::string type)
    {
        s_cache.push_back(NotifyItem(name, type));
    }

    void Notify::Commit()
    {
        if ( s_cache.size() == 0 )
            return;

        if ( s_cache.size() == 1 )
            Notify().Emit(s_cache.front());
        else 
        {
            std::stringstream ss;
            ss << "Synchonized " << s_cache.size() << " files";
            Notify().Emit(ss.str(), "download", "sync");
        }
        s_cache.clear();
    }
}