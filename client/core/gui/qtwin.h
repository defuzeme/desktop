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

#ifndef QTWIN_H
#define QTWIN_H

#include <QColor>
#include <QWidget>

class WindowNotifier;

/**
  * This is a helper class for using the Desktop Window Manager
  * functionality on Windows 7 and Windows Vista. On other platforms
  * these functions will simply not do anything.
  * \author Nokia Corporation
  **/

class QtWin
{
public:
	static bool enableBlurBehindWindow(QWidget *widget, bool enable = true);
	static bool extendFrameIntoClientArea(QWidget *widget,
										  int left = -1, int top = -1,
										  int right = -1, int bottom = -1);
	static bool isCompositionEnabled();
	static QColor colorizatinColor();

private:
	static WindowNotifier *windowNotifier();
};

#endif // QTWIN_H
