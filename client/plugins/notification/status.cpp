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

#include "status.hpp"

using namespace Notification;

Status	*Status::instance = 0;
QList<WaitMSG>	Status::waitList;

Status::Status() : lastMessage(0)
{
	instance = this;
	widget = new StatusWidget(this);
	uiModule = Gui::ModuleFactory::create("Status", QPoint(2, 1), widget);
	uiModule->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	uiModule->submitForDisplay();
	connect(this, SIGNAL(forwardMessage(QString,QString, QObject*, const char*)), SLOT(message(QString,QString, QObject*, const char*)));
	while (!waitList.empty())
	{
		WaitMSG	&elem = waitList.first();
		emit forwardMessage(elem.msg, elem.pix, elem.obj, elem.slot);
		waitList.pop_front();
	}
	emit initialized(this);
}

Status::~Status()
{
	delete widget;
}

void	Status::init()
{
}

void	Status::aboutToQuit()
{
}

Message*	Status::message(const QString &msg, const QString& pix, QObject *obj, const char *slot)
{
	if (lastMessage && lastMessage->message == msg)
	{
		// Message already exists, just increment count
		lastMessage->incrementCount();
		if (obj && slot)
			QMetaObject::invokeMethod(obj, slot, Q_ARG(Notification::Message*, lastMessage));
		return lastMessage;
	}
	else
	{
		Message*	message = new Message(widget);
		QPixmap		pixmap(pix);

		message->setMessage(msg);
		if (!pixmap.isNull())
			message->setIcon(pixmap);
		widget->addMessage(message);
		lastMessage = message;
		if (obj && slot)
			QMetaObject::invokeMethod(obj, slot, Q_ARG(Notification::Message*, lastMessage));
		return message;
	}
}

void	Status::gMessage(const QString &msg, const QString& pix,
						 QObject *obj, const char *slot)
{
	if (instance)
		emit instance->forwardMessage(msg, pix, obj, slot);
	else
		waitList << WaitMSG(msg, pix, obj, slot);
}
