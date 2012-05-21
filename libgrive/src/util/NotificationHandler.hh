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


#pragma once

#include <string>
#include <list>

namespace gr {

    class NotifyItem 
    {
        public:

            NotifyItem(std::string name, std::string type);

            const std::string name() const;
            const std::string type() const;

        private:
            std::string m_name;
            std::string m_type;
    };

    class Notify
    {

    public :
	    Notify(std::string name = "Grive");
        ~Notify();

	    void Emit(std::string message, std::string stock_icon = "",  std::string categ = "Info", int tmo = 3000);
        void Emit(NotifyItem itm);

        static void Push(std::string name, std::string type);
        static void Commit();

    private:
        std::string m_name;

        static std::list<NotifyItem> s_cache;
    };
}
