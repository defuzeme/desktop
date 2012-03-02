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

#include "authview.hpp"
#include "ui_authview.h"

using namespace Network;

AuthView::AuthView(const RemoteEvent& event) :
	QWidget(0), ui(new Ui::AuthView), remote(event.getRemote()), msgid(event.getUid())
{
	accept = false;
	permanent = false;
	ui->setupUi(this);
	ui->device_name->setText(QString("%1 (defuze.me v%2)").arg(remote->getDevice(), remote->getVersion()));
	ui->device_ip->setText(remote->ip());
}

AuthView::~AuthView()
{
    delete ui;
}

void AuthView::closeEvent(QCloseEvent *close_event)
{
	emit answered(remote, msgid, accept, permanent);
	close_event->accept();
}

void AuthView::on_button_accept_clicked()
{
	accept = true;
	permanent = ui->permanent_check->isChecked();
	close();
}

void AuthView::on_button_reject_clicked()
{
	permanent = ui->permanent_check->isChecked();
	close();
}
