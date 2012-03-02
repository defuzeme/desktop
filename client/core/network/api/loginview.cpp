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

#include <QMovie>
#include <QDebug>
#include "loginview.hpp"
#include "ui_loginview.h"

using namespace Network;

LoginView::LoginView(const QString& login) : QWidget(0), ui(new Ui::LoginView)
{
	ui->setupUi(this);
	if (login.length() > 0)
	{
		ui->login_field->setText(login);
		ui->password_field->setFocus();
	}
	success = false;
}

LoginView::~LoginView()
{
	delete ui;
}

void LoginView::clear_controls()
{
	ui->login_field->setEnabled(true);
	ui->password_field->setEnabled(true);
	ui->button_connect->setEnabled(true);
}

void LoginView::on_button_connect_clicked()
{
	ui->login_field->setEnabled(false);
	ui->password_field->setEnabled(false);
	ui->button_connect->setEnabled(false);
	emit authenticate(ui->login_field->text(), ui->password_field->text());
}

void LoginView::on_button_postpone_clicked()
{
	// TODO
}

void LoginView::closeEvent(QCloseEvent *event)
{
	if (!success)
		emit cancelled();
	event->accept();
}

void LoginView::on_button_cancel_clicked()
{
	close();
}

void LoginView::login_failed(const QString)
{
	clear_controls();
	ui->description_label->setText(tr("Authentication failed. Please check your credentials and retry."));
	ui->icon_label->setPixmap(QPixmap(":/icons/icons/error.png"));
}

void LoginView::login_sucess()
{
	success = true;
	close();
}

void LoginView::on_password_field_textChanged(QString text)
{
	ui->button_connect->setEnabled(text.size() > 0 && ui->login_field->text().size() > 0);
}

void LoginView::on_login_field_textChanged(QString text)
{
	ui->button_connect->setEnabled(text.size() > 0 && ui->password_field->text().size() > 0);
}

void Network::LoginView::on_password_field_returnPressed()
{
	on_button_connect_clicked();
}
