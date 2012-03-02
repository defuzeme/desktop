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

#include <QPainter>
#include <QDebug>
#include "playqueuedelegate.hpp"
#include "queueable.hpp"
#include "queuetrack.hpp"
#include "queuebreak.hpp"
#include "scheduler.hpp"
#include "eventmodel.hpp"
#include "playerlabel.hpp"

using namespace Queue;

PlayQueueDelegate::PlayQueueDelegate(PlayQueue *queue) :
	QStyledItemDelegate(),
	positionPolygon(4),
	timePolygon(4),
	queue(queue)
{
	positionPolygon.setPoint(0, 0, 0);
	positionPolygon.setPoint(1, 30, 0);
	positionPolygon.setPoint(2, 20, 20);
	positionPolygon.setPoint(3, 0, 20);
	timePolygon.setPoint(0, 30, 0);
	timePolygon.setPoint(1, 100, 0);
	timePolygon.setPoint(2, 90, 20);
	timePolygon.setPoint(3, 20, 20);
}

void		PlayQueueDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	Queue::Queueable* queueable = (Queue::Queueable*) index.data().value<void*>();

	// Draw background when selected
	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, option.palette.highlight());

	if (queueable->getEvent() > 0)
	{
		QColor	eventColor(100, 100, 100);
		int id = queueable->getEvent();
		Scheduler::EventModel *event = queue->getSchedulerPlugin()->getEvent(id);
		if (event)
			eventColor = QColor("#" + event->getColor());
		QRect	block = QRect(option.rect.adjusted(option.rect.width() - 10, 0, 0, 0));
		if (index.row() % 3 == 0)
		{
			painter->save();
			QFont	font = option.font;
			font.setPointSize(8);
			painter->setPen(eventColor);
			painter->setFont(font);
			painter->translate(block.topLeft());
			painter->translate(1, 3);
			painter->rotate(90);
			painter->drawText(0, 0, event->getTitle());
			painter->restore();
		}
		eventColor.setAlpha(20);
		painter->fillRect(block, eventColor);
	}

	// Draw shadows
	painter->setPen(QColor(255, 255, 255, 20));
	painter->drawLine(option.rect.topLeft(), option.rect.topRight());
	painter->setPen(QColor(0, 0, 0, 60));
	painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

	// Draw position background
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(Qt::NoPen);
	// Player color
	if (index.row() == 0)
		painter->setBrush(QBrush(Player::PlayerLabel::currentColor()));
	else if (index.row() == 1)
		painter->setBrush(QBrush(Player::PlayerLabel::nextColor()));
	else
		painter->setBrush(QBrush(QColor(255, 255, 255, 30)));
	QPolygon	positionZone = positionPolygon.translated(option.rect.topLeft());
	painter->drawConvexPolygon(positionZone);

	// Draw time background
	painter->setBrush(QBrush(QColor(0, 0, 0, 60)));
	QPolygon	timeZone = timePolygon.translated(option.rect.topLeft());
	painter->drawConvexPolygon(timeZone);

	// Set text mode
	if (option.state & QStyle::State_Selected)
		painter->setPen(option.palette.highlightedText().color());
	else
		painter->setPen(option.palette.foreground().color());

	// Draw position id
	painter->drawText(positionZone.boundingRect().adjusted(0, 0, -10, 0), Qt::AlignCenter, QString::number(index.row() + 1));

	// Draw play time
	//	QRect	timeRect = QRect(0, positionZone.boundingRect().bottom(), 40, option.rect.height() - positionZone.boundingRect().height());
	QRect	timeRect = timeZone.boundingRect();
	QString	time = tr("--:--");
	if (queueable->getPlayTime().isValid())
		time = queueable->getPlayTime().toString(tr("h:mm ap"));
	painter->drawText(timeRect, Qt::AlignCenter, time);

	// Draw details
	int margin = 2;	// top/bottom pix margin
	if (queueable->isTrack())
	{
		// Get the queuetrack element
		Queue::QueueTrack	*qtrack = queueable->toQueueTrack();

		// Set the title font
		QFont	font = option.font;
		font.setPointSize(13);
		painter->setFont(font);

		// Calculate position
		QRect	titleRect = QRect(110, option.rect.top() + margin, option.rect.width() - 215, painter->fontMetrics().height());
		int width = painter->fontMetrics().boundingRect(qtrack->getTrack()->getTitle()).width();
		if (width > titleRect.width())
			width = titleRect.width();
		// Draw title
		painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, qtrack->getTrack()->getTitle());

		// Set default font
		painter->setFont(option.font);
		painter->setOpacity(0.6);
		// Calculate duration position
		QRect	lengthRect = titleRect.adjusted(width + 5, 0, 60, 0);
		// Draw duration
		painter->drawText(lengthRect, Qt::AlignLeft | Qt::AlignVCenter, QString("(%1)").arg(qtrack->textDuration()));
		painter->setOpacity(1);

		// Calculate position
		QRect	artistRect = QRect(10, titleRect.bottom(), option.rect.width() - 60, 20);
		// Draw artist
		painter->drawText(artistRect, Qt::AlignLeft | Qt::AlignVCenter, qtrack->getTrack()->getArtist() + " - " + qtrack->getTrack()->getAlbum());

		// Draw album image
		QPixmap	albumArt = qtrack->getTrack()->getAlbumArtAtSize(32);
		QRect	pixmapRect = QRect(option.rect.width() - (albumArt.width() + 17),
								   option.rect.top() + (option.rect.height() - albumArt.height())/2,
								   albumArt.width(),
								   albumArt.height());
		painter->drawPixmap(pixmapRect, albumArt);
		// Draw jewelcase
		//QRect	jewelcaseRect = pixmapRect.adjusted(-3, -2, 2, 2);
		//painter->drawPixmap(jewelcaseRect, QPixmap(":/jewelcase/32"));
	}
	else if (queueable->isBreak())
	{
		// Get the queuebreak element
		Queue::QueueBreak	*qbreak = queueable->toQueueBreak();

		// Calculate position
		QRect	textRect = QRect(10, option.rect.top() + 24, option.rect.width() - 70, 20);
		// Draw message
		painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, qbreak->message());


		// Draw break image
		QPixmap	image(":/icons/clock");
		QRect	pixmapRect = QRect(option.rect.width() - (image.width() + 18),
								   option.rect.top() + (option.rect.height() - image.height())/2,
								   image.width(),
								   image.height());
		painter->drawPixmap(pixmapRect, image);
	}


	painter->restore();
}

QSize		PlayQueueDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex&) const
{
	return QSize(200, QFontMetrics(option.font).height()*3);
}
