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

#include "vumeterdisplay.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QGradient>
#include <QDebug>
#include <QTimer>
#include <QMutexLocker>
#include "math.h"

const int period = 20;

VUMeterDisplay::VUMeterDisplay(QWidget *parent) :
	QWidget(parent)
{
	level = 0;
	peak = 0;
	peakDuration = 0;
	setAutoFillBackground(false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	gradient.setColorAt(1, QColor(39, 197, 224));
	gradient.setColorAt(0.4, QColor(205, 228, 42));
	gradient.setColorAt(0.2, QColor(205, 204, 0));
	gradient.setColorAt(0, QColor(255, 84, 0));

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(refresh()));
	timer->start(period);
}

void	VUMeterDisplay::setLevel(double level)
{
	QMutexLocker	locker(&mutex);
	if (level > this->level)
		this->level = level;
	if (level > peak)
	{
		peak = level;
		peakDuration = 0;
	}
}

void	VUMeterDisplay::refresh()
{
	if (peak > 0)
		repaint();
	{
		QMutexLocker	locker(&mutex);
		if (level > 0)
			level = (level + 1) * 0.99 - 1;
		if (peak > 0)
			peak = fmax(peak - (peakDuration / 200000.), 0);
		peakDuration += period;
	}
}
/*
void	VUMeterDisplay::initializeGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glEnable(GL_DEPTH_TEST);
}

void	VUMeterDisplay::resizeGL(int w, int h)
{
	// setup viewport, projection etc.:
	glViewport(0, 0, (GLint)w, (GLint)h);
}

void	VUMeterDisplay::paintGL()
{
	// draw the scene:
	glBegin(GL_QUADS);
//	glVertex3f(...);
//	glVertex3f(...);
	glEnd();
}*/

void	VUMeterDisplay::resizeEvent(QResizeEvent *)
{
	// Set gradient
	gradient.setStart(rect().topLeft());
	gradient.setFinalStop(rect().bottomLeft());
}

void	VUMeterDisplay::paintEvent(QPaintEvent *)
{
	QMutexLocker	locker(&mutex);
	painter.begin(this);
	QRect		zone = rect();
	QRect		pZone = rect();

	// Draw background
	painter.fillRect(rect(), QColor(30, 30, 30));

	// Draw bar
	if (level >= 0.01)
	{
		zone.setTop(rect().height() * (1-level));
		painter.fillRect(zone, gradient);
	}

	// Draw peak
	if (peak >= 0.025)
	{
		pZone.setTop(rect().height() * (1-peak));
		pZone.setHeight(2);
		painter.fillRect(pZone, gradient);
	}

	painter.end();
}
