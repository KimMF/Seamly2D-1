/************************************************************************
 **
 **  @file   vnodespline.cpp
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

#include "vnodespline.h"
#include "../../../vgeometry/vspline.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>

const QString VNodeSpline::TagName = QStringLiteral("spline");
const QString VNodeSpline::ToolType = QStringLiteral("modelingSpline");

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VNodeSpline constructor.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param idSpline object id in containerSpline.
 * @param typeCreation way we create this tool.
 * @param idTool id node.
 * @param qoParent QObject parent.
 * @param parent QGraphicsItem parent.
 */
VNodeSpline::VNodeSpline(VAbstractPattern *doc, VContainer *data, quint32 id, quint32 idSpline,
                         const Source &typeCreation, const quint32 &idTool, QObject *qoParent,
                         QGraphicsItem * parent)
    :VAbstractNode(doc, data, id, idSpline, idTool, qoParent), QGraphicsPathItem(parent)
{
    RefreshGeometry();
    this->setPen(QPen(baseColor, qApp->toPixel(WidthHairLine(*VAbstractTool::data.GetPatternUnit()))));

    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param idSpline object id in containerSpline.
 * @param parse parser file mode.
 * @param typeCreation way we create this tool.
 * @param idTool id node.
 * @return pointer to node.
 */
VNodeSpline *VNodeSpline::Create(VAbstractPattern *doc, VContainer *data, VMainGraphicsScene *scene, quint32 id,
                                 quint32 idSpline, const Document &parse,
                                 const Source &typeCreation, const quint32 &idTool, QObject *parent)
{
    VAbstractTool::AddRecord(id, Tool::NodeSpline, doc);
    VNodeSpline *spl = nullptr;
    if (parse == Document::FullParse)
    {
        spl = new VNodeSpline(doc, data, id, idSpline, typeCreation, idTool, parent);

        doc->AddTool(id, spl);
        if (idTool != NULL_ID)
        {
            //Some nodes we don't show on scene. Tool that create this nodes must free memory.
            VDataTool *tool = doc->getTool(idTool);
            SCASSERT(tool != nullptr);
            spl->setParent(tool);// Adopted by a tool
        }
        else
        {
            // Try to prevent memory leak
            scene->addItem(spl);// First adopted by scene
            spl->hide();// If no one will use node, it will stay hidden
            spl->SetParentType(ParentType::Scene);
        }
    }
    else
    {
        doc->UpdateToolData(id, data);
    }
    return spl;
}

//---------------------------------------------------------------------------------------------------------------------
QString VNodeSpline::getTagName() const
{
    return VNodeSpline::TagName;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FullUpdateFromFile update tool data form file.
 */
void VNodeSpline::FullUpdateFromFile()
{
    RefreshGeometry();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief AddToFile add tag with informations about tool into file.
 */
void VNodeSpline::AddToFile()
{
    QDomElement domElement = doc->createElement(TagName);

    doc->SetAttribute(domElement, VDomDocument::AttrId, id);
    doc->SetAttribute(domElement, AttrType, ToolType);
    doc->SetAttribute(domElement, AttrIdObject, idNode);
    if (idTool != NULL_ID)
    {
        doc->SetAttribute(domElement, AttrIdTool, idTool);
    }

    AddToModeling(domElement);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief RefreshDataInFile refresh attributes in file. If attributes don't exist create them.
 */
void VNodeSpline::RefreshDataInFile()
{
    QDomElement domElement = doc->elementById(id);
    if (domElement.isElement())
    {
        doc->SetAttribute(domElement, AttrIdObject, QString().setNum(idNode));
        if (idTool != NULL_ID)
        {
            doc->SetAttribute(domElement, AttrIdTool, idTool);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief mouseReleaseEvent handle mouse release events.
 * @param event mouse release event.
 */
void VNodeSpline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit ChoosedTool(id, SceneObject::Spline);
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief hoverMoveEvent handle hover move events.
 * @param event hover move event.
 */
void VNodeSpline::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setPen(QPen(currentColor, qApp->toPixel(WidthMainLine(*VAbstractTool::data.GetPatternUnit()))));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief hoverLeaveEvent handle hover leave events.
 * @param event hover leave event.
 */
void VNodeSpline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    this->setPen(QPen(currentColor, qApp->toPixel(WidthHairLine(*VAbstractTool::data.GetPatternUnit()))));
}

//---------------------------------------------------------------------------------------------------------------------
void VNodeSpline::ShowNode()
{
    if (parentType != ParentType::Scene)
    {
        show();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VNodeSpline::HideNode()
{
    hide();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief RefreshGeometry refresh item on scene.
 */
void VNodeSpline::RefreshGeometry()
{
//    const QSharedPointer<VSpline> spl = VAbstractTool::data.GeometricObject<VSpline>(id);
//    QPainterPath path;
//    path.addPath(spl->GetPath());
//    path.setFillRule( Qt::WindingFill );
//    this->setPath(path);
}