/*******************************************************************************
**  defuze.me - modern radio automation software suite
**  
**  Copyright © 2012
**    Athena Calmettes - Jocelyn De La Rosa - Francois Gaillard
**    Adrien Jarthon - Alexandre Moore - Luc Peres - Arnaud Sellier
**
**  website: http://defuze.me
**  contact: team@defuze.me
**
**  This program is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  This software includes code from Nokia (Qt) under the GNU LGPLv2.1
**  This software uses libraries from the FFmpeg project under the GNU LGPLv2.1
**  This software uses libraries from the TagLib project under the GNU LGPLv2.1
**
*******************************************************************************/

namespace Notification {
	class StatusWidget;
}

#ifndef STATUSWIDGET_HPP
#define STATUSWIDGET_HPP

#include "ui_statuswidget.h"
#include "guicore.hpp"
#include "status.hpp"
#include "message.hpp"

namespace Notification
{
	/** This class is the UI module displaying global notifications
	  **/

	class StatusWidget : public Gui::ModuleWidget, private Ui::StatusWidget
	{
		Q_OBJECT
		friend class Status;

	public:
		explicit StatusWidget(Status *plugin);

	protected:
		void		changeEvent(QEvent *e);
		void		addMessage(Message*widget);

	private:
		Status*		plugin;
	};
}

#endif // STATUSWIDGET_HPP
