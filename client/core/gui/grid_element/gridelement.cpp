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

#include <QDebug>
#include "gridelement.hpp"
#include "gridwidget.hpp"

using namespace Gui;

GridElement::GridElement(const QString &title, GridWidget *parent) :
	QWidget(parent), gridWidget(parent)
{
	setupUi(this);
	titleLabel->setText("<b>" + title + "</b>");
	paramsButton->hide();
}

void GridElement::showParams()
{
	//module->parametersCategory->show();
}

void GridElement::setModule(Module *module)
{
	this->module = module;
	this->setSizePolicy(module->colSizePolicy, module->sizePolicy);
	elementWidget->addWidget(module->getWidget());
	elementWidget->setStretch(1, 1);
	setPosition(module->getPosition());
//	if (module->hasParameters())
//	{
//		paramsButton->show();
//		connect(paramsButton, SIGNAL(clicked()), this, SLOT(showParams()));
//	}
}

Module *GridElement::getModule() const
{
	return module;
}

GridWidget *GridElement::getGridWidget() const
{
	return gridWidget;
}

int GridElement::getColumn() const
{
	return position.x();
}

void GridElement::setColumn(int column)
{
	setPosition(QPoint(column, position.y()));
}

int GridElement::getRow() const
{
	return position.y();
}

void GridElement::setRow(int row)
{
	setPosition(QPoint(position.x(), row));
}

const QPoint& GridElement::getPosition() const
{
	return position;
}

void GridElement::setPosition(QPoint position)
{
	this->position = position;
	//positionLabel->setText("(" + QVariant(position.x()).toString() + "," + QVariant(position.y()).toString() + ")");
}

void GridElement::changeEvent(QEvent *e)
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
