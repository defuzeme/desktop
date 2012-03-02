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

#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
	class LoginView;
}

namespace Network {
	/** This is the authentication window displayed by the authenticator
	  * to querry the user's credentials
	  **/

	class LoginView : public QWidget
	{
		Q_OBJECT

	public:
		explicit LoginView(const QString& login);
		~LoginView();

	signals:
		void	authenticate(const QString& login, const QString& password);
		void	cancelled();

	protected:
		void	closeEvent(QCloseEvent *event);

	private slots:
		void	on_button_connect_clicked();
		void	on_button_postpone_clicked();
		void	on_button_cancel_clicked();
		void	login_failed(const QString error);
		void	login_sucess();
		void	on_password_field_textChanged(QString );
		void	on_login_field_textChanged(QString );

		void on_password_field_returnPressed();

	private:
		void	clear_controls();
		bool	success;
		Ui::LoginView *ui;
	};
}

#endif // LOGINVIEW_H
