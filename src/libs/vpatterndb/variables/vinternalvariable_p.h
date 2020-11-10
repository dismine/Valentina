/************************************************************************
 **
 **  @file   vinternalvariable_p.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   20 8, 2014
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

#ifndef VINTERNALVARIABLE_P_H
#define VINTERNALVARIABLE_P_H

#include <QSharedData>
#include "../vmisc/def.h"
#include "../vmisc/diagnostic.h"
#include "../vmisc/defglobal.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

// cppcheck-suppress copyCtorAndEqOperator
class VInternalVariableData : public QSharedData
{
public:
    VInternalVariableData()
    {}

    VInternalVariableData(const VInternalVariableData &var)
        :QSharedData(var),
         type(var.type),
         value(var.value),
         name(var.name),
         alias(var.alias)
    {}

    virtual ~VInternalVariableData();

    VarType type{VarType::Unknown};

    /** @brief value variable's value */
    qreal   value{0};

    QString name{};
    QString alias{};

private:
    Q_DISABLE_ASSIGN(VInternalVariableData)
};

VInternalVariableData::~VInternalVariableData()
{}

QT_WARNING_POP

#endif // VINTERNALVARIABLE_P_H
