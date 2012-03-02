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

#include "cornerwidget.hpp"
#include "guicore.hpp"
#include "qtwin.h"

using namespace Gui;

CornerWidget::CornerWidget(GuiCore *guiCore, QWidget *parent) :
	QWidget(parent), guiCore(guiCore)
{
	setupUi(this);
	if (guiCore->getPublicParameter("aero", QVariant(true)).toBool())
		if (QtWin::isCompositionEnabled())
			setTextShadowEffects();
}

void CornerWidget::setTextShadowEffects()
{
	setTextShadow(fullscreenButtonEffect, fullscreenButton);
	setTextShadow(helpButtonEffect, helpButton);
	setTextShadow(aboutButtonEffect, aboutButton);
	setTextShadow(parametersButtonEffect, parametersButton);
	setTextShadow(quitButtonEffect, quitButton);
}

void CornerWidget::setTextShadow(QGraphicsDropShadowEffect *effect, QWidget *button)
{
	effect = new QGraphicsDropShadowEffect;
	effect->setBlurRadius(8);
	effect->setOffset(0, 0);
	effect->setColor(QColor("#FFFFFF"));
	button->setGraphicsEffect(effect);
}

void CornerWidget::changeEvent(QEvent *e)
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

void CornerWidget::on_fullscreenButton_clicked()
{
	if (guiCore->getMainWindow()->windowState() != Qt::WindowFullScreen)
	{
		guiCore->getMainWindow()->getTabWidget()->setAutoFillBackground(true);
		guiCore->getMainWindow()->setWindowState(Qt::WindowFullScreen);
		fullscreenButton->setText(tr("Windowed"));
	}
	else
	{
		guiCore->getMainWindow()->getTabWidget()->setAutoFillBackground(false);
		guiCore->getMainWindow()->setWindowState(Qt::WindowMaximized);
		fullscreenButton->setText(tr("Fullscreen"));
	}
}

void Gui::CornerWidget::on_quitButton_clicked()
{
	guiCore->getMainWindow()->close();
}


void Gui::CornerWidget::on_parametersButton_clicked()
{
	guiCore->toogleParametersModule();
}

void Gui::CornerWidget::on_aboutButton_clicked()
{
	guiCore->getAboutDialog()->show();
}
