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

#include "message.hpp"
#include <QDateTime>

using namespace Notification;

Message::Message(QWidget *parent) : QWidget(parent), actionBtn(0), count(1)
{
	setupUi(this);
	count_label->hide();
	QDateTime	t = QDateTime::currentDateTime();
	date->setText(t.toString("hh:mm"));
}

void Message::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void	Message::setMessage(const QString& msg)
{
	message = msg;
	label->setText(msg);
}

void	Message::setIcon(const QPixmap& pix)
{
	icon->setPixmap(pix);
}

void	Message::incrementCount()
{
	count++;
	count_label->setText(QString("x%1").arg(count));
	count_label->show();
	QDateTime	t = QDateTime::currentDateTime();
	date->setText(t.toString("hh:mm"));
}

void	Message::setAction(const QString& name, const QObject* obj, const char* method)
{
	actionBtn = new QPushButton(name, this);

	connect(actionBtn, SIGNAL(clicked()), obj, method);
	frame->layout()->addWidget(actionBtn);
}

void	Message::disableAction()
{
	actionBtn->setEnabled(false);
}
