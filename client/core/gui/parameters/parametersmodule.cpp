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

#include "parametersmodule.hpp"
#include "qtwin.h"
#include "guicore.hpp"
#include <QLabel>

using namespace Gui;

ParametersModule::ParametersModule(GuiCore *guiCore, QWidget *parent):
	QWidget(parent), guiCore(guiCore)
{
	setupUi(this);
	setWindowFlags(Qt::Tool);
	if (guiCore->getPublicParameter("aero", QVariant(true)).toBool())
	{
		if (QtWin::isCompositionEnabled())
		{
			QtWin::extendFrameIntoClientArea(this);
			setContentsMargins(0, 0, -2, -2);
		}
	}
}

void ParametersModule::registerCategory(ParametersCategory *category)
{
	categories[category->getPath()] = category;
	stackedWidget->addWidget(category);
}

QHash<QString, ParametersCategory*>& ParametersModule::getCategories()
{
	return categories;
}

void ParametersModule::changeEvent(QEvent *e)
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
