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

#ifndef AUTHVIEW_H
#define AUTHVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include "remoteevent.hpp"

namespace Ui {
    class AuthView;
}

namespace Network {
	/** This is the autorization window displayed by the remote device doorman
	  * to querry the user's approval when a device is trying to connect
	  **/

	class AuthView : public QWidget
	{
		Q_OBJECT

	public:
		explicit AuthView(const RemoteEvent& event);
		~AuthView();

	protected:
		void	closeEvent(QCloseEvent *close_event);

	private slots:
		void	on_button_accept_clicked();
		void	on_button_reject_clicked();

	signals:
		void	answered(RemoteSock*, unsigned, bool, bool);	///< Signal emited when the user answer

	private:
		Ui::AuthView	*ui;
		bool			accept, permanent;						///< Stores what the use choosed
		RemoteSock		*remote;								///< Pointer to the remote device
		unsigned		msgid;									///< Authentication request message id
	};
}

#endif // AUTHVIEW_H
