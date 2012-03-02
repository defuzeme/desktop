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

#ifndef MAINPLAYERWIDGET_HPP
#define MAINPLAYERWIDGET_HPP

#include "ui_mainplayerwidget.h"
#include "guicore.hpp"
#include "mainplayer.hpp"

namespace Player
{
	class MainPlayer;

	class MainPlayerWidget : public Gui::ModuleWidget, private Ui::MainPlayerWidget
	{
		Q_OBJECT
		friend class MainPlayer;

	public:
		explicit MainPlayerWidget(MainPlayer *player, QWidget *parent = 0);
		void	setNextButtonIcon(const QIcon &icon);
		void	setACurrent();
		void	setANext();
		void	setBNext();
		void	setBCurrent();
private slots:
		void on_nextButton_clicked();
	signals:
		void	nextButton_clicked();
	};
}
#endif // MAINPLAYERWIDGET_HPP
