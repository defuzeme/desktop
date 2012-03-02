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

#include "librarydelegate.hpp"
#include <QPainter>
#include <QDebug>

using namespace Library;

LibraryDelegate::LibraryDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
	note.load(":icons/note");
	genre = QPixmap(":/icons/cd-wheel").scaled(22, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QSize LibraryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);
	//LibraryItem *item = (LibraryItem*)index.data().value<void*>();
	int height = 2;

	return QSize(200, QFontMetrics(option.font).height() * height);
}

void LibraryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	LibraryItem *item = static_cast<LibraryItem*>(index.internalPointer());

	// Draw background when selected
	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, option.palette.highlight());

	// Draw shadows
	painter->setPen(QColor(255, 255, 255, 20));
	painter->drawLine(QPoint(0, option.rect.top()), option.rect.topRight());
	painter->setPen(QColor(0, 0, 0, 60));
	painter->drawLine(QPoint(0, option.rect.bottom()), option.rect.bottomRight());
	// Set text mode
	if (option.state & QStyle::State_Selected)
		painter->setPen(option.palette.highlightedText().color());
	else
		painter->setPen(option.palette.foreground().color());


	// Set the title font
	QFont	font(option.font.family());

	QRect	titleRect = option.rect.adjusted(31, 0, -31, 0);
	QRect	pixmapRect = QRect(option.rect.left(), option.rect.top() + (option.rect.height() - 22) / 2, 22, 22);

	font.setPointSize(11);
	painter->setFont(font);
	if (item->getKind() == LibraryItem::ALBUM)
	{
		// Draw album image
		if (AudioTrack::isLoaded(item->getId()))
			painter->drawPixmap(pixmapRect.topLeft(), item->getTrack()->getAlbumArtAtSize(pixmapRect.height()));
		else
			AudioTrack::loadAndCall(item->getId(), parent(), SLOT(update()));
	}
	else if (item->getKind() == LibraryItem::TRACK)
	{
		if (item->getIsInAlbum())
		{
			// Draw track number
			if (AudioTrack::isLoaded(item->getId()))
			{
				if (item->getTrack())
				{
					if (item->getTrack()->getTrack() > 0)
					{
						painter->setOpacity(0.6);
						painter->drawText(pixmapRect, Qt::AlignVCenter | Qt::AlignRight, QString("%1").arg(item->getTrack()->getTrack()));
						painter->setOpacity(1);
					}
					else
					{
						// Draw generic track icon
						painter->drawPixmap(pixmapRect.adjusted(4, 2, 0, 0).topLeft(), note);
					}
				}
				else
				{
					qDebug() << "invalid track" << item->getId();
					painter->drawText(pixmapRect, Qt::AlignVCenter | Qt::AlignRight, "/!\\");
				}
			}
			else
				AudioTrack::loadAndCall(item->getId(), parent(), SLOT(update()));
			// else ask preload
		}
		else
		{
			// Draw track icon
			painter->drawPixmap(pixmapRect.adjusted(4, 2, 0, 0).topLeft(), note);
		}
	}
	else if (item->getKind() == LibraryItem::ARTIST)
	{
		painter->drawPixmap(pixmapRect.topLeft(), item->getTrack()->getArtistArtAtSize(pixmapRect.height()));
	}
	else if (item->getKind() == LibraryItem::GENRE)
	{
		painter->drawPixmap(pixmapRect.topLeft(), genre);
	}
	else
		painter->fillRect(pixmapRect, QColor(255,255,255,51));

	font.setPointSize(13);
	painter->setFont(font);
	// Draw Title
	painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, item->data(0).toString());

	painter->restore();
}

