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

#ifndef CORNERWIDGET_HPP
#define CORNERWIDGET_HPP

#include "ui_cornerwidget.h"
#include "about.hpp"
#include <QGraphicsDropShadowEffect>

namespace Gui
{
	class GuiCore;

	/** The top-left menu.
	  **/

	class CornerWidget : public QWidget, private Ui::CornerWidget
	{
		Q_OBJECT
		friend class TabWidget;
	public:
		explicit CornerWidget(GuiCore *guiCore, QWidget *parent = 0);

	protected:
		void changeEvent(QEvent *e);

	private slots:
		void on_fullscreenButton_clicked();
		void on_quitButton_clicked();
		void on_parametersButton_clicked();
		void on_aboutButton_clicked();

	private:
		GuiCore *guiCore;
		void setTextShadowEffects();
		void setTextShadow(QGraphicsDropShadowEffect *effect, QWidget *button);
		QGraphicsDropShadowEffect *parametersButtonEffect;
		QGraphicsDropShadowEffect *quitButtonEffect;
		QGraphicsDropShadowEffect *fullscreenButtonEffect;
		QGraphicsDropShadowEffect *helpButtonEffect;
		QGraphicsDropShadowEffect *aboutButtonEffect;
	};
}

#endif // CORNERWIDGET_HPP
