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
	class Status;
}

#ifndef STATUS_HPP
#define STATUS_HPP

#include "staticplugin.hpp"
#include "statuswidget.hpp"
#include "message.hpp"

namespace Notification
{
	// Some pre-defined icons
	const QString	OK = ":/icons/tick";
	const QString	INFO = ":/icons/info";
	const QString	SYNC = ":/icons/sync";
	const QString	WARN = ":/icons/warning";
	const QString	ERR = ":/icons/error";
	const QString	DEVICE = ":/icons/device-small";

	/** This class provide central notification methods.
	  * Messages are then stored and displayed in a UI module
	  **/

	struct WaitMSG
	{
		WaitMSG(const QString& msg, const QString& pix, QObject* obj,
				const char* slot) : msg(msg), pix(pix), obj(obj), slot(slot) {}
		QString		msg, pix;
		QObject		*obj;
		const char	*slot;
	};

	class Status : public QObject, public StaticPlugin
	{
		Q_OBJECT
	public:
		explicit Status();
		~Status();
		void				aboutToQuit();
		void				init();
		static void			gMessage(const QString &msg, const QString& pix = INFO,
									 QObject *obj = 0, const char *slot = 0);			///< Add a notification message with an optional icon (thread safe)

	signals:
		void				forwardMessage(QString, QString, QObject*, const char*);
		void				initialized(Notification::Status*);

	public slots:
		Message*			message(const QString &msg, const QString& pix = INFO,
									QObject *obj = 0, const char *slot = 0);		///< Add a notification message and returns it for futher customization (thread UNSAFE)

	private:
		StatusWidget*		widget;
		Gui::Module			*uiModule;
		Message*			lastMessage;
		static Status		*instance;
		static QList<WaitMSG>	waitList;
	};
}

#endif // STATUS_HPP
