/************************************************************************
 **
 **  @file
 **  @author Valentina Zhuravska <zhuravska19(at)gmail.com>
 **  @date   19 7, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2016 Valentina project
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

#include "vabstractmainwindow.h"
#include "../vpropertyexplorer/checkablemessagebox.h"
#include "../vmisc/vabstractapplication.h"
#include "dialogs/dialogexporttocsv.h"

#include <QStyle>
#include <QToolBar>
#include <QFileDialog>

#if defined(Q_OS_MAC)
#include "../vwidgets/vmaingraphicsview.h"
#include <QStyleFactory>
#endif

VAbstractMainWindow::VAbstractMainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_curFileFormatVersion(0x0),
      m_curFileFormatVersionStr(QLatin1String("0.0.0"))
{}

//---------------------------------------------------------------------------------------------------------------------
bool VAbstractMainWindow::ContinueFormatRewrite(const QString &currentFormatVersion,
                                                const QString &maxFormatVersion)
{
    if (qApp->Settings()->GetConfirmFormatRewriting())
    {
        Utils::CheckableMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Confirm format rewriting"));
        msgBox.setText(tr("This file is using previous format version v%1. The current is v%2. "
                          "Saving the file with this app version will update the format version for this "
                          "file. This may prevent you from be able to open the file with older app versions. "
                          "Do you really want to continue?").arg(currentFormatVersion, maxFormatVersion));
        msgBox.setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);
        msgBox.setDefaultButton(QDialogButtonBox::No);
        msgBox.setIconPixmap(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(32, 32));

        int dialogResult = msgBox.exec();

        if (dialogResult == QDialog::Accepted)
        {
            qApp->Settings()->SetConfirmFormatRewriting(not msgBox.isChecked());
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::ToolBarStyle(QToolBar *bar)
{
    SCASSERT(bar != nullptr)
    if (qApp->Settings()->GetToolBarStyle())
    {
        bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else
    {
        bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

#if defined(Q_OS_MAC)
    // Temporary fix issue with toolbar black background on mac with OpenGL render
    if (qApp->getSceneView() && qApp->getSceneView()->IsOpenGLRender())
    {
        bar->setStyle(QStyleFactory::create("fusion"));
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QString VAbstractMainWindow::CSVFilePath()
{
    const QString filters = tr("Comma-Separated Values") + QLatin1String(" (*.csv)");
    const QString suffix("csv");
    const QString path = QDir::homePath() + QChar('/') + tr("values") + QChar('.') + suffix;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to CSV"), path, filters, nullptr
#ifdef Q_OS_LINUX
                                                    , QFileDialog::DontUseNativeDialog
#endif
                                                    );

    if (fileName.isEmpty())
    {
        return fileName;
    }

    QFileInfo f( fileName );
    if (f.suffix().isEmpty() && f.suffix() != suffix)
    {
        fileName += QChar('.') + suffix;
    }

    return fileName;
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::WindowsLocale()
{
    qApp->Settings()->GetOsSeparator() ? setLocale(QLocale()) : setLocale(QLocale::c());
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::ExportDataToCSV()
{
    QString fileName = CSVFilePath();

    if (fileName.isEmpty())
    {
        return;
    }

    DialogExportToCSV dialog(this);
    dialog.SetWithHeader(qApp->Settings()->GetCSVWithHeader());
    dialog.SetSelectedMib(qApp->Settings()->GetCSVCodec());
    dialog.SetSeparator(qApp->Settings()->GetCSVSeparator());

    if (dialog.exec() == QDialog::Accepted)
    {
        ExportToCSVData(fileName, dialog.IsWithHeader(), dialog.GetSelectedMib(), dialog.GetSeparator());

        qApp->Settings()->SetCSVSeparator(dialog.GetSeparator());
        qApp->Settings()->SetCSVCodec(dialog.GetSelectedMib());
        qApp->Settings()->SetCSVWithHeader(dialog.IsWithHeader());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::UpdateVisibilityGroups()
{
    // do nothing
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::UpdateDetailsList()
{
    // do nothing
}

//---------------------------------------------------------------------------------------------------------------------
void VAbstractMainWindow::ZoomFitBestCurrent()
{
    // do nothing
}
