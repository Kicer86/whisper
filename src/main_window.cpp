/*
 * Main Window
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "main_window.hpp"

#include <QDockWidget>

#include "user_list_widget.hpp"

MainWindow::MainWindow()
{
    setCentralWidget(new QWidget(this));

    QDockWidget* userListDock = new QDockWidget(tr("Users"), this);
    userListDock->setWidget(new UserListWidget(this));
    addDockWidget(Qt::RightDockWidgetArea, userListDock);
}


MainWindow::~MainWindow()
{
}
