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

#include "listsdelegate.hpp"
#include "listsitem.hpp"
#include "editabletreeitem.hpp"
#include <QPainter>
#include <QApplication>
#include <QLineEdit>
#include <QDebug>

using namespace Lists;

ListsDelegate::ListsDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
	editorRegEpx = new QRegExp("\\S.{0,99}");
	addList.load(":/icons/list-add");
	staticList.load(":/icons/static-playlist");
	dynamicList.load(":/icons/dynamic-playlist");
	artist.load(":/cover/artist");
	album.load(":/cover/default");
	genre = QPixmap(":/icons/cd-wheel").scaled(22, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	album = album.scaled(22, 22, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QSize ListsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);
	float height = 2;

	ListsItem *item = static_cast<ListsItem*>(index.internalPointer());
	if (item->isOfKind(ListsItem::ADD))
		height = 1.6;
	else if (item->isOfKind(ListsItem::SEPARATOR))
		height = 1.2;

	return QSize(200, QFontMetrics(option.font).height() * height);
}

void ListsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	ListsItem *item = static_cast<ListsItem*>(index.internalPointer());

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

	QRect	titleRect = option.rect.adjusted(31, 0, -3, 0);
	QRect	pixmapRect = QRect(option.rect.left(), option.rect.top() + (option.rect.height() - 22) / 2, 22, 22);

	font.setPointSize(11);
	painter->setFont(font);

	if (item->getSubKind() == ListsItem::NORMAL_PLAYLIST)
		painter->drawPixmap(pixmapRect, staticList);
	else if (item->getSubKind() == ListsItem::DYNAMIC_PLAYLIST)
		painter->drawPixmap(pixmapRect, dynamicList);
	else if (item->isOfKind(ListsItem::ADD))
		painter->drawPixmap(pixmapRect.adjusted(3, 3, -3, -3), addList);
	else if (item->getSubKind() == ListsItem::ARTIST)
		painter->drawPixmap(pixmapRect, artist);
	else if (item->getSubKind() == ListsItem::ALBUM)
		painter->drawPixmap(pixmapRect, album);
	else if (item->getSubKind() == ListsItem::TRACK)
		painter->drawPixmap(pixmapRect, item->getIcon());
	else if (item->getSubKind() == ListsItem::GENRE)
		painter->drawPixmap(pixmapRect, genre);
	else if (!item->isOfKind(ListsItem::SEPARATOR))
		painter->fillRect(pixmapRect, QColor(255,255,255,51));

	font.setPointSize(13);
	// Draw Title
	if (item->isOfKind(ListsItem::ADD))
	{
		font.setStyle(QFont::StyleItalic);
		font.setPointSize(11);
	}
	painter->setFont(font);
	if (item->isOfKind(ListsItem::SEPARATOR))
	{
		font.setPointSize(8);
		painter->setFont(font);
		QString text;
		if (item->childCount())
		{
			if (item->getSubKind() == ListsItem::INTRO)
				text = "Songs ";
			else
				text = "and ";
			if (item->child(0)->getSubKind() == ListsItem::ARTIST)
				text += "by artists";
			else if (item->child(0)->getSubKind() == ListsItem::ALBUM)
				text += "from albums";
			else if (item->child(0)->getSubKind() == ListsItem::GENRE)
				text += "of genres";
		}
		painter->drawText(titleRect.adjusted(-22, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text);
	}
	else
		painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, item->data(0).toString());

	painter->restore();
}

QWidget *ListsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QLineEdit *editor = new QLineEdit(parent);
	editor->setValidator(new QRegExpValidator(*editorRegEpx, 0));
	return editor;
}

void ListsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
	lineEdit->setText(value);
}

void ListsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
	QString value = lineEdit->text();

	model->setData(index, value, Qt::EditRole);
}

void ListsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}
