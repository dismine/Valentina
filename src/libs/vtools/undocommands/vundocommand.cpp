/************************************************************************
 **
 **  @file   vundocommand.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 7, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
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

#include "vundocommand.h"

#include <QDomNode>
#include <QApplication>

#include "../ifc/ifcdef.h"
#include "../vmisc/def.h"
#include "../vmisc/customevents.h"
#include "../vpatterndb/vnodedetail.h"
#include "../vpatterndb/vpiecenode.h"
#include "../tools/drawTools/operation/vabstractoperation.h"

Q_LOGGING_CATEGORY(vUndo, "v.undo")

//---------------------------------------------------------------------------------------------------------------------
VUndoCommand::VUndoCommand(const QDomElement &xml, VAbstractPattern *doc, QUndoCommand *parent)
    :QObject(), QUndoCommand(parent), xml(xml), doc(doc), nodeId(NULL_ID), redoFlag(false)
{
    SCASSERT(doc != nullptr)
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::RedoFullParsing()
{
    if (redoFlag)
    {
        emit NeedFullParsing();
    }
    else
    {
        QApplication::postEvent(doc, new LiteParseEvent());
    }
    redoFlag = true;
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::UndoDeleteAfterSibling(QDomNode &parentNode, quint32 siblingId, const QString &tagName) const
{
    if (siblingId == NULL_ID)
    {
        parentNode.appendChild(xml);
    }
    else
    {
        const QDomElement refElement = doc->NodeById(siblingId, tagName);
        parentNode.insertAfter(xml, refElement);
        doc->RefreshElementIdCache();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::IncrementReferences(const QVector<quint32> &nodes) const
{
    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        try
        {
            doc->IncrementReferens(nodes.at(i));
        }
        catch (const VExceptionBadId &e)
        { // ignoring
            Q_UNUSED(e);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::DecrementReferences(const QVector<quint32> &nodes) const
{
    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        try
        {
            doc->DecrementReferens(nodes.at(i));
        }
        catch (const VExceptionBadId &e)
        { // ignoring
            Q_UNUSED(e);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::IncrementReferences(const QVector<CustomSARecord> &nodes) const
{
    QVector<quint32> n;

    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        n.append(nodes.at(i).path);
    }

    IncrementReferences(n);
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::DecrementReferences(const QVector<CustomSARecord> &nodes) const
{
    QVector<quint32> n;

    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        n.append(nodes.at(i).path);
    }

    DecrementReferences(n);
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::IncrementReferences(const QVector<VPieceNode> &nodes) const
{
    QVector<quint32> n;

    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        n.append(nodes.at(i).GetId());
    }

    IncrementReferences(n);
}

//---------------------------------------------------------------------------------------------------------------------
void VUndoCommand::DecrementReferences(const QVector<VPieceNode> &nodes) const
{
    QVector<quint32> n;

    for (qint32 i = 0; i < nodes.size(); ++i)
    {
        n.append(nodes.at(i).GetId());
    }

    DecrementReferences(n);
}

//---------------------------------------------------------------------------------------------------------------------
QDomElement VUndoCommand::GetDestinationObject(quint32 idTool, quint32 idPoint) const
{
    const QDomElement tool = doc->elementById(idTool, VAbstractPattern::TagOperation);
    if (tool.isElement())
    {
        QDomElement correctDest;
        const QDomNodeList nodeList = tool.childNodes();
        for (qint32 i = 0; i < nodeList.size(); ++i)
        {
            const QDomElement dest = nodeList.at(i).toElement();
            if (not dest.isNull() && dest.isElement() && dest.tagName() == VAbstractOperation::TagDestination)
            {
                correctDest = dest;
                break;
            }
        }

        if (not correctDest.isNull())
        {
            const QDomNodeList destObjects = correctDest.childNodes();
            for (qint32 i = 0; i < destObjects.size(); ++i)
            {
                const QDomElement obj = destObjects.at(i).toElement();
                if (not obj.isNull() && obj.isElement())
                {
                    const quint32 id = doc->GetParametrUInt(obj, AttrIdObject, NULL_ID_STR);
                    if (idPoint == id)
                    {
                        return obj;
                    }
                }
            }
        }
    }

    return QDomElement();
}
