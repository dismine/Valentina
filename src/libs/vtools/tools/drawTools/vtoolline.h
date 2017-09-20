/************************************************************************
 **
 **  @file   vtoolline.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
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

#ifndef VTOOLLINE_H
#define VTOOLLINE_H

#include <qcompilerdetection.h>
#include <QDomElement>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "../ifc/xml/vabstractpattern.h"
#include "../vmisc/def.h"
#include "vdrawtool.h"

template <class T> class QSharedPointer;

struct VToolLineInitData : public VAbstractToolInitData
{
    VToolLineInitData()
        : VAbstractToolInitData(),
          firstPoint(NULL_ID),
          secondPoint(NULL_ID),
          typeLine(TypeLineLine),
          lineColor(ColorBlack)
    {}

    quint32 firstPoint;
    quint32 secondPoint;
    QString typeLine;
    QString lineColor;
};

/**
 * @brief The VToolLine class tool for creation line.
 */
class VToolLine: public VDrawTool, public QGraphicsLineItem
{
    Q_OBJECT
public:
    virtual void     setDialog() Q_DECL_OVERRIDE;
    static VToolLine *Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene  *scene, VAbstractPattern *doc,
                             VContainer *data);
    static VToolLine *Create(VToolLineInitData initData);

    virtual int      type() const Q_DECL_OVERRIDE {return Type;}
    enum { Type = UserType + static_cast<int>(Tool::Line)};
    virtual QString  getTagName() const Q_DECL_OVERRIDE;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) Q_DECL_OVERRIDE;

    QString FirstPointName() const;
    QString SecondPointName() const;

    quint32 GetFirstPoint() const;
    void    SetFirstPoint(const quint32 &value);

    quint32 GetSecondPoint() const;
    void    SetSecondPoint(const quint32 &value);

    QString GetLineColor() const;
    void    SetLineColor(const QString &value);

    virtual void     ShowVisualization(bool show) Q_DECL_OVERRIDE;

    virtual void     SetTypeLine(const QString &value) Q_DECL_OVERRIDE;
    virtual void     GroupVisibility(quint32 object, bool visible) Q_DECL_OVERRIDE;
public slots:
    virtual void     FullUpdateFromFile() Q_DECL_OVERRIDE;
    virtual void     ShowTool(quint32 id, bool enable) Q_DECL_OVERRIDE;
    virtual void     Disable(bool disable, const QString &namePP) Q_DECL_OVERRIDE;
    virtual void     AllowHover(bool enabled) Q_DECL_OVERRIDE;
    virtual void     AllowSelecting(bool enabled) Q_DECL_OVERRIDE;
protected slots:
    virtual void ShowContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id=NULL_ID) Q_DECL_OVERRIDE;
protected:
    virtual void     contextMenuEvent ( QGraphicsSceneContextMenuEvent * event ) Q_DECL_OVERRIDE;
    virtual void     AddToFile() Q_DECL_OVERRIDE;
    virtual void     hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;
    virtual void     hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;
    virtual void     RemoveReferens() Q_DECL_OVERRIDE;
    virtual QVariant itemChange ( GraphicsItemChange change, const QVariant &value ) Q_DECL_OVERRIDE;
    virtual void     keyReleaseEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    virtual void     SaveDialog(QDomElement &domElement) Q_DECL_OVERRIDE;
    virtual void     SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) Q_DECL_OVERRIDE;
    virtual void     ReadToolAttributes(const QDomElement &domElement) Q_DECL_OVERRIDE;
    virtual void     SetVisualization() Q_DECL_OVERRIDE;
    virtual QString  MakeToolTip() const Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(VToolLine)

    /** @brief firstPoint id first line point. */
    quint32           firstPoint;

    /** @brief secondPoint id second line point. */
    quint32           secondPoint;

    /** @brief lineColor color of a line. */
    QString           lineColor;

    bool m_isHovered;

    VToolLine(const VToolLineInitData &initData, QGraphicsItem *parent = nullptr);

    void RefreshGeometry();
};

#endif // VTOOLLINE_H
