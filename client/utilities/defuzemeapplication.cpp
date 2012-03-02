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

#include "defuzemeapplication.hpp"
#include "logger.hpp"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

DefuzemeApplication::DefuzemeApplication(int &argc, char **argv) :
	QApplication(argc, argv)
{
	qRegisterMetaType<Exception>("Exception");
	connect(this, SIGNAL(forwardException(Exception)), SLOT(displayException(Exception)));
}

DefuzemeApplication::~DefuzemeApplication()
{
}

bool	DefuzemeApplication::notify(QObject *receiver, QEvent *event)
{
	try
	{
		return QApplication::notify(receiver, event);
	}
	catch (Exception &e)
	{
		qDebug() << "APP: " << e.description();
		emit forwardException(e);
	}
	return false;
}

void	DefuzemeApplication::displayException(const Exception& e) throw()
{
	if (!speedControl.isActive())
	{
		speedControl.setSingleShot(true);
		speedControl.start(5000);
		QMessageBox::critical(NULL, "defuze.me", "An fatal error occurred while running defuze.me.<br><i>" +
							  e.msg() + " (error " + e.hexCode() +
							  ")</i><br><br>Get online support about this error <a href='http://defuze.me/support/errors/" +
							  e.hexCode() + "?report=run'>here</a>.");
	}
}
