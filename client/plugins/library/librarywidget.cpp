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

#include "librarywidget.hpp"
#include "library.hpp"
#include <QFileDialog>
#include "librarydelegate.hpp"

using namespace Library;

LibraryWidget::LibraryWidget(LibraryPlugin *library, QWidget *parent) :
	ModuleWidget(parent), library(library)
{
	setupUi(this);
	updateProgressBar->hide();
	QPalette palette;
	palette.setColor(QPalette::Background, QColor::fromRgb(132,130,128));
	palette.setColor(QPalette::Base, QColor::fromRgb(192,192,192));
	palette.setColor(QPalette::AlternateBase, QColor::fromRgb(177,177,177));

	treeView->setPalette(palette);
	treeView->setDragEnabled(true);
	treeView->setDragDropMode(QAbstractItemView::DragOnly);
	treeView->setHeaderHidden(true);
	treeView->setAttribute(Qt::WA_AcceptTouchEvents);
	treeView->setItemDelegate(new LibraryDelegate);
	treeView->itemDelegate()->setParent(this);
	QObject::connect(treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(toogleItem(QModelIndex)));
}

void LibraryWidget::changeEvent(QEvent *e)
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

void LibraryWidget::toogleItem(const QModelIndex &index)
{
	if (treeView->isExpanded(index))
		treeView->collapse(index);
	else
		treeView->expand(index);
	//treeView->expandAll();
}

QTreeView *LibraryWidget::getTreeViewWidget() const
{
	return treeView;
}

QToolButton *LibraryWidget::getUpdateSourcesButton() const
{
	return updateSourcesButton;
}

QToolButton *LibraryWidget::getAddSourceButton() const
{
	return addSourceButton;
}

QProgressBar *LibraryWidget::getUpdateProgressBar() const
{
	return updateProgressBar;
}

void LibraryWidget::updatingSources()
{
	getTreeViewWidget()->setDisabled(true);
	addSourceButton->setDisabled(true);
	addSourceButton->hide();
	searchLineEdit->setDisabled(true);
	searchLineEdit->hide();
	sortComboBox->setDisabled(true);
	sortComboBox->hide();
	updateSourcesButton->setDisabled(true);
	updateSourcesButton->hide();
	updateProgressBar->setValue(0);
	updateProgressBar->setRange(0, 0);
	updateProgressBar->setEnabled(true);
	updateProgressBar->show();
}

void LibraryWidget::endUpdatingSources()
{
	getTreeViewWidget()->setDisabled(false);
	updateProgressBar->setEnabled(false);
	updateProgressBar->hide();
	addSourceButton->setDisabled(false);
	addSourceButton->show();
	updateSourcesButton->setDisabled(false);
	updateSourcesButton->show();
	searchLineEdit->setDisabled(false);
	searchLineEdit->show();
	sortComboBox->setDisabled(false);
	sortComboBox->show();
}

void Library::LibraryWidget::on_updateSourcesButton_clicked()
{
	library->updateSources();
}

void Library::LibraryWidget::on_addSourceButton_clicked()
{
	QString path;

	addSourceButton->setDisabled(true);
	updateSourcesButton->setDisabled(true);
	sortComboBox->setDisabled(true);
	searchLineEdit->setDisabled(true);
		if (!(path = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), "",
													   QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog)).isEmpty())
		library->addSource(QUrl::fromUserInput(path));
	addSourceButton->setDisabled(false);
	updateSourcesButton->setDisabled(false);
	if (!(path.isEmpty()))
		library->updateSources();
}

void Library::LibraryWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
	int id = library->getModel()->getIdByIndex(index);
	LibraryModel::SortKeys kind = library->getModel()->getKindByIndex(index);

	if (id && (kind == LibraryModel::TRACK || kind == LibraryModel::TITLE))
		library->addTrackToQueue(id);
}

void Library::LibraryWidget::on_sortComboBox_currentIndexChanged(int index)
{
	library->getModel()->update(index);
}

void Library::LibraryWidget::on_searchLineEdit_textChanged(const QString &arg1)
{
	library->getModel()->update(-1, arg1);
}
