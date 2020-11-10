/************************************************************************
 **
 **  @file   vformulapropertyeditor.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   28 8, 2014
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

#include "vformulapropertyeditor.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QKeyEvent>
#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QRegularExpression>
#include <QSpacerItem>

#include "../vpropertyexplorer/vproperty.h"
#include "../vtools/dialogs/support/dialogeditwrongformula.h"

// VFormulaPropertyEditor
//---------------------------------------------------------------------------------------------------------------------
VFormulaPropertyEditor::VFormulaPropertyEditor(QWidget *parent)
    : QWidget(parent), formula(VFormula()), ToolButton(nullptr), TextLabel(nullptr)
{
    setAutoFillBackground(true);

    // Create the tool button
    ToolButton = new QToolButton(this);
    ToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ToolButton->setText("...");
    ToolButton->setIcon(QIcon("://icon/16x16/fx.png"));
    ToolButton->setIconSize(QSize(16, 16));
    ToolButton->setFixedSize(24, 24);
    ToolButton->installEventFilter(this);
    setFocusProxy(ToolButton);  // Make the ToolButton the focus proxy
    setFocusPolicy(ToolButton->focusPolicy());
    connect(ToolButton, &QToolButton::clicked, this, &VFormulaPropertyEditor::onToolButtonClicked);

    // Create the text label
    TextLabel = new QLabel(this);
    TextLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    TextLabel->setText(formula.getStringValue());

    // The layout (a horizontal layout)
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(3);
    layout->setMargin(0);
    layout->addWidget(TextLabel);
    // Spacer (this is needed for proper display of the label and button)
    layout->addSpacerItem(new QSpacerItem(1000000000, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));
    layout->addWidget(ToolButton);
}

//---------------------------------------------------------------------------------------------------------------------
void VFormulaPropertyEditor::SetFormula(const VFormula& formula)
{
    if (this->formula != formula)
    {
        this->formula = formula;
        TextLabel->setText(this->formula.getStringValue());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VFormulaPropertyEditor::onToolButtonClicked()
{
    QScopedPointer<DialogEditWrongFormula> tmpWidget(new DialogEditWrongFormula(formula.getData(), formula.getToolId(),
                                                                                qApp->getMainWindow()));
    tmpWidget->setCheckZero(formula.getCheckZero());
    tmpWidget->setPostfix(formula.getPostfix());
    tmpWidget->SetFormula(formula.GetFormula(FormulaType::FromUser));

    if (tmpWidget->exec() == QDialog::Accepted)
    {
        formula.SetFormula(tmpWidget->GetFormula(), FormulaType::ToUser);
        formula.Eval();
        TextLabel->setText(formula.getStringValue());
        emit dataChangedByUser(formula, this);
        QCoreApplication::postEvent(this, new VPE::UserChangeEvent());
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool VFormulaPropertyEditor::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ToolButton && ev->type() == QEvent::KeyPress)
    {
        // Ignore the event, so that eventually the delegate gets the event.
        ev->ignore();
        return true;
    }

    return QWidget::eventFilter(obj, ev);
}

//---------------------------------------------------------------------------------------------------------------------
VFormula VFormulaPropertyEditor::GetFormula() const
{
    return formula;
}
