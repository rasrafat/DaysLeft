/****************************************************************************
**
** Copyright (c) 2015, Rafat Rashid <enemyunited@gmail.com>
**
** Permission to use, copy, modify, and/or distribute this software for any
** purpose with or without fee is hereby granted, provided that the above
** copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
** WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
** ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
** WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
** ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
****************************************************************************/

#include "MainWindow.h"
#include "constants.h"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QTextEdit>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("DaysLeft " APPVERSTR);
	createMenus();

	dlView = new DLView(this);
	setCentralWidget(dlView);

    //setWindowIcon(QIcon(":/resources/days-left_32x32.ico"));

	QSettings settings("DaysLeft", "DaysLeft");
	resize(settings.value("winSize", QSize(320, 300)).toSize());
}

MainWindow::~MainWindow() {
}

void MainWindow::createMenus() {
	// File Menu Actions
    QAction *openNewListAct = new QAction("&Open New List...", this);
	openNewListAct->setShortcut(QKeySequence("Ctrl+O"));
	connect(openNewListAct, SIGNAL(triggered()), this, SLOT(onOpenNewListAction()));

    QAction *openAppendToListAct = new QAction("&Append to Current List...", this);
	openAppendToListAct->setShortcut(QKeySequence("Shift+A"));
	connect(openAppendToListAct, SIGNAL(triggered()), this, SLOT(onOpenAppendToListAction()));

    saveListAct = new QAction("&Save List", this);
	saveListAct->setShortcut(QKeySequence("Ctrl+S"));
	saveListAct->setEnabled(false);
	connect(saveListAct, SIGNAL(triggered()), this, SLOT(onSaveListAction()));

    QAction *saveListAsAct = new QAction("Save List &As...", this);
    saveListAsAct->setShortcut(QKeySequence("Shift+S"));
	connect(saveListAsAct, SIGNAL(triggered()), this, SLOT(onSaveListAsAction()));

	// Create File Menu
    QMenu *fileMenu = new QMenu("&File", this);
	fileMenu->addAction(openNewListAct);
	fileMenu->addAction(openAppendToListAct);
	fileMenu->addAction(saveListAct);
	fileMenu->addAction(saveListAsAct);

	// Help Menu Actions
    QAction *dlHelpAct = new QAction("DaysLeft Help", this);
	connect(dlHelpAct, SIGNAL(triggered()), this, SLOT(onDLHelpAction()));

    QAction *aboutAppAct = new QAction("About App", this);
	connect(aboutAppAct, SIGNAL(triggered()), this, SLOT(onAboutAppAction()));

    QAction *aboutQtAct = new QAction("About Qt", this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// Create Help Menu
    QMenu *helpMenu = new QMenu("&Help", this);
	helpMenu->addAction(dlHelpAct);
	helpMenu->addAction(aboutAppAct);
	helpMenu->addAction(aboutQtAct);

	// Add menus to menuBar
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(helpMenu);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	QSettings settings("DaysLeft", "DaysLeft");
	settings.setValue("winSize", size());

	QWidget::closeEvent(event);
}

void MainWindow::onOpenNewListAction() {
	onOpenAction(false);
}

void MainWindow::onOpenAppendToListAction() {
	onOpenAction(true);
}

void MainWindow::onOpenAction(bool appendToList) {
	QSettings settings("DaysLeft", "DaysLeft");
	QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), myPath, "XML files (*.xml)");

	// Test to see if we can open the file.
	if (!filePath.isEmpty()) {
		QFileInfo fileInfo(filePath);

		if (!fileInfo.isDir()) {
			settings.setValue("lastFilePath", QVariant(filePath));
			if (dlView->getModel()->readEventsFromFile(filePath, appendToList)) {
				openedFile = filePath;
				saveListAct->setEnabled(true);
			}
			else QMessageBox::warning(this, "Warning", "Invalid file chosen.");
		}
		else QMessageBox::warning(this, "Warning", "Selection is a directory and cannot be opened.");
	}
}

void MainWindow::onSaveListAction() {
	if (!dlView->getModel()->writeEventsToFile(openedFile))
		QMessageBox::warning(this, "Warning", "Unable to save events to file."
							 " Use 'SaveAs' to save to a different file.");
}

void MainWindow::onSaveListAsAction() {
	QSettings settings("DaysLeft", "DaysLeft");
	QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();

	QString filePath = QFileDialog::getSaveFileName(this, tr("Save As"), myPath, "XML files (*.xml)");
	if (!filePath.isEmpty()) {
		settings.setValue("lastFilePath", QVariant(filePath));

		if (dlView->getModel()->writeEventsToFile(filePath)) {
			openedFile = filePath;
			saveListAct->setEnabled(true);
		}
		else QMessageBox::warning(this, "Warning", "Unable to save events to file.");
	}
}

void MainWindow::onDLHelpAction() {
	QFile file(":/resources/helpPage.html");
	if (!file.open(QFile::ReadOnly)) {
		QMessageBox::warning(this, "Warning", "Unable to open help page.");
		return;
	}

	QTextStream in(&file);
	QString htmlText = in.readAll();

	file.close();

	QTextEdit *helpPage = new QTextEdit();
	helpPage->setReadOnly(true);
    helpPage->setWindowTitle("DaysLeft " APPVERSTR " Help");
	helpPage->setHtml(htmlText);
	helpPage->resize(570, 450);
	helpPage->show();
}

void MainWindow::onAboutAppAction() {
    QMessageBox::about(this, "DaysLeft " APPVERSTR,
					   "<div style='background-color: #FFF; text-align: center'>"
							  "<table style='background-color: #FFF'><tr><td width='160px'>"
							  "<b>Developed by:</b> Rafat Rashid<br>"
                              "<b>Version:</b> " APPVERSTR " (" APPDATESTR ")<br><br>"
							  "<a href='http://individual.utoronto.ca/rafatrashid/'>http://individual.utoronto.ca/rafatrashid/</a>"
							  "</td></tr></table></div>");
}