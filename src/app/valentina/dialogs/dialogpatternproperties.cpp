/************************************************************************
 **
 **  @file   dialogpatternproperties.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   18 2, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
 **  <https://gitlab.com/smart-pattern/valentina> All Rights Reserved.
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

#include "dialogpatternproperties.h"
#include "ui_dialogpatternproperties.h"
#include <QBuffer>
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>
#include <QDate>
#include <QMessageBox>
#include <QRadioButton>

#include "../xml/vpattern.h"
#include "../vpatterndb/vcontainer.h"
#include "../core/vapplication.h"
#include "../vtools/dialogs/support/dialogeditlabel.h"
#include "dialogknownmaterials.h"
#include "../vmisc/vsettings.h"

//---------------------------------------------------------------------------------------------------------------------
DialogPatternProperties::DialogPatternProperties(VPattern *doc,  VContainer *pattern, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DialogPatternProperties),
      doc(doc),
      pattern(pattern),
      data(QMap<QCheckBox *, int>()),
      descriptionChanged(false),
      gradationChanged(false),
      defaultChanged(false),
      securityChanged(false),
      deleteAction(nullptr),
      changeImageAction(nullptr),
      saveImageAction(nullptr),
      showImageAction(nullptr)
{
    ui->setupUi(this);

#if defined(Q_OS_MAC)
    setWindowFlags(Qt::Window);
#endif

    SCASSERT(doc != nullptr)

    VSettings *settings = qApp->ValentinaSettings();
    settings->GetOsSeparator() ? setLocale(QLocale()) : setLocale(QLocale::c());

    if (qApp->GetPatternPath().isEmpty())
    {
        ui->lineEditPathToFile->setText(tr("<Empty>"));
        ui->lineEditPathToFile->setToolTip(tr("File was not saved yet."));
        ui->pushButtonShowInExplorer->setEnabled(false);
    }
    else
    {
        ui->lineEditPathToFile->setText(QDir::toNativeSeparators(qApp->GetPatternPath()));
        ui->lineEditPathToFile->setToolTip(QDir::toNativeSeparators(qApp->GetPatternPath()));
        ui->pushButtonShowInExplorer->setEnabled(true);
    }
    ui->lineEditPathToFile->setCursorPosition(0);

    connect(ui->pushButtonShowInExplorer, &QPushButton::clicked, this, []()
    {
        ShowInGraphicalShell(qApp->GetPatternPath());
    });
#if defined(Q_OS_MAC)
    ui->pushButtonShowInExplorer->setText(tr("Show in Finder"));
#endif //defined(Q_OS_MAC)

    //----------------------- Label language
    for (auto &name : VApplication::LabelLanguages())
    {
        ui->comboBoxLabelLanguage->addItem(QLocale(name).nativeLanguageName(), name);
    }

    int index = ui->comboBoxLabelLanguage->findData(qApp->ValentinaSettings()->GetLabelLanguage());
    if (index != -1)
    {
        ui->comboBoxLabelLanguage->setCurrentIndex(index);
    }

    connect(ui->comboBoxLabelLanguage, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &DialogPatternProperties::DescEdited);

    ui->plainTextEditDescription->setPlainText(doc->GetDescription());
    connect(ui->plainTextEditDescription, &QPlainTextEdit::textChanged, this, &DialogPatternProperties::DescEdited);

    ui->plainTextEditTechNotes->setPlainText(doc->GetNotes());
    connect(ui->plainTextEditTechNotes, &QPlainTextEdit::textChanged, this, &DialogPatternProperties::DescEdited);

    InitImage();

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &DialogPatternProperties::Ok);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this,
            &DialogPatternProperties::Apply);

    QPushButton *bCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
    SCASSERT(bCancel != nullptr)
    connect(bCancel, &QPushButton::clicked, this, &DialogPatternProperties::close);

    ui->tabWidget->setCurrentIndex(0);

    const bool readOnly = doc->IsReadOnly();
    ui->checkBoxPatternReadOnly->setChecked(readOnly);
    if (not readOnly)
    {
        connect(ui->checkBoxPatternReadOnly, &QRadioButton::toggled, this, [this](){securityChanged = true;});
    }
    else
    {
        ui->checkBoxPatternReadOnly->setDisabled(true);
    }

    //Initialization change value. Set to default value after initialization
    gradationChanged = false;
    defaultChanged = false;
    securityChanged = false;
}

//---------------------------------------------------------------------------------------------------------------------
DialogPatternProperties::~DialogPatternProperties()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::Apply()
{
    switch (ui->tabWidget->currentIndex())
    {
        case 0:
            SaveDescription();
            break;
        case 1:
            SaveReadOnlyState();
            break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::Ok()
{
    SaveDescription();
    SaveReadOnlyState();

    close();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::DescEdited()
{
    descriptionChanged = true;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::SaveDescription()
{
    if (descriptionChanged)
    {
        doc->SetNotes(ui->plainTextEditTechNotes->document()->toPlainText());
        doc->SetDescription(ui->plainTextEditDescription->document()->toPlainText());
        doc->SetLabelPrefix(qvariant_cast<QString>(ui->comboBoxLabelLanguage->currentData()));

        descriptionChanged = false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::SaveReadOnlyState()
{
    if (securityChanged)
    {
        doc->SetReadOnly(ui->checkBoxPatternReadOnly->isChecked());
        securityChanged = false;
        emit doc->patternChanged(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
QImage DialogPatternProperties::GetImage()
{
// we set an image from file.val
    QImage image;
    QByteArray byteArray;
    byteArray.append(doc->GetImage().toUtf8());
    QByteArray ba = QByteArray::fromBase64(byteArray);
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadOnly);
    QString extension = doc->GetImageExtension();
    image.load(&buffer, extension.toLatin1().data()); // writes image into ba in 'extension' format
    return image;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::InitImage()
{
    ui->imageLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->imageLabel->setScaledContents(true);
    connect(ui->imageLabel, &QWidget::customContextMenuRequested, this, [this]()
    {
        QMenu menu(this);
        menu.addAction(deleteAction);
        menu.addAction(changeImageAction);
        menu.addAction(saveImageAction);
        menu.addAction(showImageAction);
        menu.exec(QCursor::pos());
        menu.show();
    });

    deleteAction      = new QAction(tr("Delete image"), this);
    changeImageAction = new QAction(tr("Change image"), this);
    saveImageAction   = new QAction(tr("Save image to file"), this);
    showImageAction   = new QAction(tr("Show image"), this);

    connect(deleteAction, &QAction::triggered, this, [this]()
    {
        doc->DeleteImage();
        ui->imageLabel->setText(tr("Change image"));
        deleteAction->setEnabled(false);
        saveImageAction->setEnabled(false);
        showImageAction->setEnabled(false);
    });

    connect(changeImageAction, &QAction::triggered, this, &DialogPatternProperties::ChangeImage);
    connect(saveImageAction, &QAction::triggered, this, &DialogPatternProperties::SaveImage);
    connect(showImageAction, &QAction::triggered, this, [this]()
    {
        QLabel *label = new QLabel(this, Qt::Window);
        const QImage image = GetImage();
        label->setPixmap(QPixmap::fromImage(image));
        label->setGeometry(QRect(QCursor::pos(), image.size()));
        label->show();
    });

    const QImage image = GetImage();
    if (not image.isNull())
    {
        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    }
    else
    {
        deleteAction->setEnabled(false);
        saveImageAction->setEnabled(false);
        showImageAction->setEnabled(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::ChangeImage()
{
    const QString filter = tr("Images") + QLatin1String(" (*.png *.jpg *.jpeg *.bmp)");
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Image for pattern"), QString(), filter, nullptr,
                                                          qApp->NativeFileDialog());
    if (not fileName.isEmpty())
    {
        QImage image;
        if (not image.load(fileName))
        {
            return;
        }
        ui->imageLabel->setPixmap(QPixmap::fromImage(image));
        QFileInfo f(fileName);
        QString extension = f.suffix().toUpper();

        if (extension == QLatin1String("JPEG"))
        {
            extension = "JPG";
        }
        if (extension == QLatin1String("PNG") || extension == QLatin1String("JPG") || extension == QLatin1String("BMP"))
        {
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, extension.toLatin1().data()); //writes the image in 'extension' format inside the buffer
            QString iconBase64 = QString::fromLatin1(byteArray.toBase64().data());

            // save our image to file.val
            doc->SetImage(iconBase64, extension);
        }
        deleteAction->setEnabled(true);
        saveImageAction->setEnabled(true);
        showImageAction->setEnabled(true);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogPatternProperties::SaveImage()
{
    QByteArray byteArray;
    byteArray.append(doc->GetImage().toUtf8());
    QByteArray ba = QByteArray::fromBase64(byteArray);
    const QString extension = doc->GetImageExtension().prepend(QChar('.'));
    QString filter = tr("Images") + QStringLiteral(" (*") + extension + QChar(')');
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), tr("untitled"), filter, &filter,
                                                    qApp->NativeFileDialog());
    if (not filename.isEmpty())
    {
        if (not filename.endsWith(extension.toUpper()))
        {
            filename.append(extension);
        }
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(ba);
            file.close();
        }
    }
}
