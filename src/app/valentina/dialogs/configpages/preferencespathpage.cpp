/************************************************************************
 **
 **  @file   preferencespathpage.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 4, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "preferencespathpage.h"
#include "ui_preferencespathpage.h"
#include "../vmisc/vsettings.h"
#include "../../options.h"
#include "../../core/vapplication.h"

#include <QDir>
#include <QFileDialog>

//---------------------------------------------------------------------------------------------------------------------
PreferencesPathPage::PreferencesPathPage(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PreferencesPathPage)
{
    ui->setupUi(this);

    InitTable();

    connect(ui->pathTable, &QTableWidget::itemSelectionChanged, this, [this]()
    {
        ui->defaultButton->setEnabled(not ui->pathTable->selectedItems().isEmpty());
        ui->defaultButton->setDefault(false);

        ui->editButton->setEnabled(not ui->pathTable->selectedItems().isEmpty());
        ui->editButton->setDefault(true);
    });

    connect(ui->defaultButton, &QPushButton::clicked, this, &PreferencesPathPage::DefaultPath);
    connect(ui->editButton, &QPushButton::clicked, this, &PreferencesPathPage::EditPath);
}

//---------------------------------------------------------------------------------------------------------------------
PreferencesPathPage::~PreferencesPathPage()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
QStringList PreferencesPathPage::Apply()
{
    VSettings *settings = qApp->ValentinaSettings();
    settings->SetPathIndividualMeasurements(ui->pathTable->item(0, 1)->text());
    settings->SetPathMultisizeMeasurements(ui->pathTable->item(1, 1)->text());
    settings->SetPathPattern(ui->pathTable->item(2, 1)->text());
    settings->SetPathLayout(ui->pathTable->item(3, 1)->text());
    settings->SetPathTemplate(ui->pathTable->item(4, 1)->text());
    settings->SetPathLabelTemplate(ui->pathTable->item(5, 1)->text());

    return QStringList(); // No changes those require restart.
}

//---------------------------------------------------------------------------------------------------------------------
void PreferencesPathPage::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // retranslate designer form (single inheritance approach)
        ui->retranslateUi(this);
        InitTable();
    }
    // remember to call base class implementation
    QWidget::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
void PreferencesPathPage::DefaultPath()
{
    const int row = ui->pathTable->currentRow();
    QTableWidgetItem *item = ui->pathTable->item(row, 1);
    SCASSERT(item != nullptr)

    QString path;

    switch (row)
    {
        case 1: // multisize measurements
            path = VCommonSettings::GetDefPathMultisizeMeasurements();
            break;
        case 2: // pattern path
            path = VSettings::GetDefPathPattern();
            break;
        case 0: // individual measurements
            path = VCommonSettings::GetDefPathIndividualMeasurements();
            break;
        case 3: // layout path
            path = VSettings::GetDefPathLayout();
            break;
        case 4: // templates
            path = VCommonSettings::GetDefPathTemplate();
            break;
        case 5: // label templates
            path = VCommonSettings::GetDefPathLabelTemplate();
            break;
        default:
            break;
    }

    item->setText(path);
    item->setToolTip(path);
}

//---------------------------------------------------------------------------------------------------------------------
void PreferencesPathPage::EditPath()
{
    const int row = ui->pathTable->currentRow();
    QTableWidgetItem *item = ui->pathTable->item(row, 1);
    SCASSERT(item != nullptr)

    QString path;
    switch (row)
    {
        case 0: // individual measurements
            path = qApp->ValentinaSettings()->GetPathIndividualMeasurements();
            break;
        case 1: // multisize measurements
            path = qApp->ValentinaSettings()->GetPathMultisizeMeasurements();
            path = VCommonSettings::PrepareMultisizeTables(path);
            break;
        case 2: // pattern path
            path = qApp->ValentinaSettings()->GetPathPattern();
            break;
        case 3: // layout path
            path = qApp->ValentinaSettings()->GetPathLayout();
            break;
        case 4: // templates
            path = qApp->ValentinaSettings()->GetPathTemplate();
            break;
        case 5: // label templates
            path = qApp->ValentinaSettings()->GetPathLabelTemplate();
            break;
        default:
            break;
    }

    bool usedNotExistedDir = false;
    QDir directory(path);
    if (not directory.exists())
    {
        usedNotExistedDir = directory.mkpath(QChar('.'));
    }

    const QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), path,
                                                          QFileDialog::ShowDirsOnly
                                                          | QFileDialog::DontResolveSymlinks
#ifdef Q_OS_LINUX
                                                          | QFileDialog::DontUseNativeDialog
#endif
                                                          );
    if (dir.isEmpty())
    {
        if (usedNotExistedDir)
        {
            QDir directory(path);
            directory.rmpath(QChar('.'));
        }
        DefaultPath();
        return;
    }

    item->setText(dir);
    item->setToolTip(dir);

    if (usedNotExistedDir)
    {
        QDir directory(path);
        directory.rmpath(QChar('.'));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void PreferencesPathPage::InitTable()
{
    ui->pathTable->clearContents();
    ui->pathTable->setRowCount(6);
    ui->pathTable->setColumnCount(2);

    const VSettings *settings = qApp->ValentinaSettings();

    {
        ui->pathTable->setItem(0, 0, new QTableWidgetItem(tr("My Individual Measurements")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathIndividualMeasurements());
        item->setToolTip(settings->GetPathIndividualMeasurements());
        ui->pathTable->setItem(0, 1, item);
    }

    {
        ui->pathTable->setItem(1, 0, new QTableWidgetItem(tr("My Multisize Measurements")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathMultisizeMeasurements());
        item->setToolTip(settings->GetPathMultisizeMeasurements());
        ui->pathTable->setItem(1, 1, item);
    }

    {
        ui->pathTable->setItem(2, 0, new QTableWidgetItem(tr("My Patterns")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathPattern());
        item->setToolTip(settings->GetPathPattern());
        ui->pathTable->setItem(2, 1, item);
    }

    {
        ui->pathTable->setItem(3, 0, new QTableWidgetItem(tr("My Layouts")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathLayout());
        item->setToolTip(settings->GetPathLayout());
        ui->pathTable->setItem(3, 1, item);
    }

    {
        ui->pathTable->setItem(4, 0, new QTableWidgetItem(tr("My Templates")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathTemplate());
        item->setToolTip(settings->GetPathTemplate());
        ui->pathTable->setItem(4, 1, item);
    }

    {
        ui->pathTable->setItem(5, 0, new QTableWidgetItem(tr("My label templates")));
        QTableWidgetItem *item = new QTableWidgetItem(settings->GetPathLabelTemplate());
        item->setToolTip(settings->GetPathLabelTemplate());
        ui->pathTable->setItem(5, 1, item);
    }

    ui->pathTable->verticalHeader()->setDefaultSectionSize(20);
    ui->pathTable->resizeColumnsToContents();
    ui->pathTable->resizeRowsToContents();
}
