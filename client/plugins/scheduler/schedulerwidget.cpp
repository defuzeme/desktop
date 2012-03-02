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

#include "schedulerwidget.hpp"
#include "scheduler.hpp"

using namespace Scheduler;

SchedulerWidget::SchedulerWidget(SchedulerPlugin *scheduler, QWidget *parent) :
	ModuleWidget(parent), scheduler(scheduler)
{
	setupUi(this);

	this->setHorizontalHeader();
	this->setVerticalHeader();
	this->setGrid();

	this->connect(this, SIGNAL(clicked()), this, SLOT(gridClicked()));

	SchedulerLayout->setContentsMargins(0, 0, 0, 0);
}

QGridLayout	*SchedulerWidget::getLayout() const
{
	return SchedulerLayout;
}

void	SchedulerWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			retranslateUi(this);
			break;
		default:
			break;
		SchedulerLayout->update();
	}
}

void	SchedulerWidget::mousePressEvent(QMouseEvent *)
{
	emit clicked();
}

void	SchedulerWidget::gridClicked()
{
	this->scheduler->getEventWidget()->clear();
}

void	SchedulerWidget::setHorizontalHeader()
{
	QStringList	days;
	days << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday") << tr("Sunday");

	for (int index = 0; index < days.size(); index += 1)
	{
		QLabel *day	= new QLabel(days.at(index));

		day->setAlignment(Qt::AlignCenter);
		day->setStyleSheet("border: none; min-height: 28px; padding-bottom: 10px;");

		SchedulerLayout->addWidget(day, 0, index + 1, SCALE, 1);
		SchedulerLayout->setColumnMinimumWidth(index + 1, 0);
		SchedulerLayout->setColumnStretch(index + 1, 1);
	}
}

void	SchedulerWidget::setVerticalHeader()
{
	for (int index = 0; index < 24; index += 1)
	{
		QLabel *time	= new QLabel(QString().sprintf("%02d", index) + ":00");

		time->setStyleSheet((index == (24 - 1)) ? "border-bottom: 1px solid rgb(65, 65, 65); padding: 0 10px;": "padding: 0 10px;");
		time->setAlignment(Qt::AlignTop);

		SchedulerLayout->addWidget(time, (index * SCALE) + SCALE, 0, SCALE, 1);
		SchedulerLayout->setRowMinimumHeight((index * SCALE) + SCALE, 0);
		SchedulerLayout->setRowStretch((index * SCALE) + SCALE, 1);
	}
}

void	SchedulerWidget::setGrid()
{
	for (int index = 0; index < 24 * SCALE; index += 1)
	{
		for (int subIndex = 0; subIndex < 7; subIndex += 1)
		{
			QLabel *content	= new QLabel(" ");
			if ((index == (24 * SCALE - 1)) && (subIndex == (7 - 1)))
			{
				content->setStyleSheet(((index % SCALE) != 0) ? "border-bottom: 1px solid rgb(65, 65, 65); border-top: 1px solid rgb(50, 50, 50); border-right: 1px solid rgb(65, 65, 65);" :
				"border-bottom: 1px solid rgb(65, 65, 65); border-right: 1px solid rgb(65, 65, 65);");
			}
			else if (index == (24 * SCALE - 1))
			{
				content->setStyleSheet(((index % SCALE) != 0) ? "border-bottom: 1px solid rgb(65, 65, 65); border-top: 1px solid rgb(50, 50, 50);" :
				"border-bottom: 1px solid rgb(65, 65, 65);");
			}
			else if (subIndex == (7 - 1))
			{
				content->setStyleSheet(((index % SCALE) != 0) ? "border-right: 1px solid rgb(65, 65, 65); border-top: 1px solid rgb(50, 50, 50);" :
				"border-right: 1px solid rgb(65, 65, 65);");
			}
			else if ((index % SCALE) != 0)
			{
				content->setStyleSheet("border-top: 1px solid rgb(50, 50, 50);");
			}

			content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

			SchedulerLayout->addWidget(content, index + SCALE, subIndex + 1, 1, 1);
			SchedulerLayout->setRowMinimumHeight(index + SCALE, 0);
			SchedulerLayout->setRowStretch(index + SCALE, 1);
		}
	}
}

void	SchedulerWidget::displayEvent(EventModel *event)
{
	int	row		= (event->getStartTime() * SCALE) / 60 + SCALE;
	int	rowSpan	= ((event->getDuration() * SCALE / 60) == 0 ) ? 1 : event->getDuration() * SCALE / 60;

	this->SchedulerLayout->addWidget(event, row, event->getDay() + 1, rowSpan, 1);
}

void	SchedulerWidget::removeEvent(EventModel *event)
{
	SchedulerLayout->removeWidget(event);
	SchedulerLayout->update();
}
