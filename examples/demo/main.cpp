/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *w = new QWidget();

    QtIntPropertyManager *intManager = new QtIntPropertyManager(w);
    QObject::connect(intManager,&QtIntPropertyManager::valueChanged,[=](QtProperty* property, int val){
        qDebug()<<"VVV"<<val;
    });

    QtEnumPropertyManager *enumManager = new QtEnumPropertyManager(w);


    QtProperty *item8 = enumManager->addProperty("direction");
    QStringList enumNames;
    enumNames << "Up" << "Right" << "Down" << "Left";
    enumManager->setEnumNames(item8, enumNames);
    QMap<int, QIcon> enumIcons;
    enumIcons[0] = QIcon(":/demo/images/up.png");
    enumIcons[1] = QIcon(":/demo/images/right.png");
    enumIcons[2] = QIcon(":/demo/images/down.png");
    enumIcons[3] = QIcon(":/demo/images/left.png");
    enumManager->setEnumIcons(item8, enumIcons);


    QtProperty *item9 = intManager->addProperty("value");
    intManager->setRange(item9, -100, 100);



    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(w);

    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(w);

    QtAbstractPropertyBrowser *editor1 = new QtTreePropertyBrowser();
    editor1->setFactoryForManager(intManager, spinBoxFactory);
    editor1->setFactoryForManager(enumManager, comboBoxFactory);

    editor1->addProperty(item8);
    editor1->addProperty(item9);



    QGridLayout *layout = new QGridLayout(w);


    layout->addWidget(editor1, 1, 0);
    w->show();

    int ret = app.exec();
    delete w;
    return ret;
}
