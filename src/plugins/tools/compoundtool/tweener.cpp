/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "tweener.h"
#include "configurator.h"
#include "target.h"

#include <QPointF>
#include <QKeySequence>
#include <QMatrix>
#include <QGraphicsView>
#include <QDomDocument>

#include "kglobal.h"
#include "kdebug.h"
#include "kaction.h"
#include "kosd.h"
#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicsscene.h"
#include "ktgraphicobject.h"
#include "ktsvgitem.h"
#include "ktpathitem.h"
#include "ktpixmapitem.h"
#include "ktitemtweener.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktscene.h"
#include "ktlayer.h"

struct Tweener::Private
{
    QMap<QString, KAction *> actions;
    Configurator *configurator;

    KTGraphicsScene *scene;
    QList<QGraphicsItem *> objects;

    KTItemTweener *currentTween;
    int startPoint;

    QPointF origin;
    Target *target;

    TweenerPanel::Mode mode;
    TweenerPanel::EditMode editMode;
    TweenerPanel::TweenerType currentTweener;
};

Tweener::Tweener() : KTToolPlugin(), k(new Private)
{
    setupActions();

    k->configurator = 0;
    k->startPoint = 0;
}

Tweener::~Tweener()
{
    delete k;
}

/* This method initializes the plugin */

void Tweener::init(KTGraphicsScene *scene)
{
    k->scene = scene;
    k->objects.clear();

    k->mode = TweenerPanel::View;
    k->editMode = TweenerPanel::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(KTItemTweener::Compound);
    if (tweenList.size() > 0) {
        k->configurator->loadTweenList(tweenList);
        setCurrentTween(tweenList.at(0));
    }

    int total = framesTotal();
    k->configurator->initStartCombo(total, k->scene->currentFrameIndex());
}

void Tweener::updateStartPoint(int index)
{
     if (k->startPoint != index && index >= 0)
         k->startPoint = index;
}

/* This method returns the plugin name */

QStringList Tweener::keys() const
{
    return QStringList() << tr("Compound Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method is executed while the mouse is pressed and on movement */

void Tweener::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method finishes the action started on the press method depending
 * on the active mode: Selecting an object or Creating a path
*/

void Tweener::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->startPoint) {
        if (k->editMode == TweenerPanel::Selection) {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);
                QGraphicsItem *item = k->objects.at(0);
                QRectF rect = item->sceneBoundingRect();
                k->origin = rect.center();
            }
        }
    }
}

/* This method returns the list of actions defined in this plugin */

QMap<QString, KAction *> Tweener::actions() const
{
    return k->actions;
}

/* This method returns the list of actions defined in this plugin */

int Tweener::toolType() const
{
    return KTToolInterface::Tweener;
}

/* This method returns the tool panel associated to this plugin */

QWidget *Tweener::configurator()
{
    if (!k->configurator) {
        k->mode = TweenerPanel::View;

        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedTweenProperties()), this, SLOT(setPropertiesMode()));

        /*
        connect(k->configurator, SIGNAL(startingPointChanged(int)), this, SLOT(updateStartPoint(int)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(setMode(TweenerPanel::Mode)), this, SLOT(updateMode(TweenerPanel::Mode)));
        connect(k->configurator, SIGNAL(clickedDefineProperties()), this, SLOT(setPropertiesMode()));
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
        */
    } 

    return k->configurator;
}

/* This method is called when there's a change on/of scene */
void Tweener::aboutToChangeScene(KTGraphicsScene *)
{
}

/* This method is called when this plugin is off */

void Tweener::aboutToChangeTool()
{
    if (k->editMode == TweenerPanel::Selection) {
        clearSelection();
        disableSelection();

        return;
    }
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    KAction *translater = new KAction(QPixmap(THEME_DIR + "icons/compound_tween.png"), 
                                      tr("Compound Tween"), this);
    translater->setCursor(QCursor(THEME_DIR + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+X")));

    k->actions.insert(tr("Compound Tween"), translater);
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(KTGraphicsScene *scene)
{ 
}

void Tweener::setCurrentTween(const QString &name)
{
    KTScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, KTItemTweener::Compound);

    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

int Tweener::framesTotal()
{
    int total = 1;
    KTLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesNumber();

    return total;
}

/* This method clears selection */

void Tweener::clearSelection()
{
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 if (item->isSelected())
                     item->setSelected(false);
        }
        k->objects.clear();
        k->configurator->notifySelection(false);
    }
}

/* This method disables object selection */

void Tweener::disableSelection()
{
    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

/* This method initializes the "Select object" mode */

void Tweener::setSelect()
{
    if (k->mode == TweenerPanel::Edit) {
        if (k->startPoint != k->scene->currentFrameIndex()) {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, KTProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    k->editMode = TweenerPanel::Selection;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if ((item->zValue() >= 10000) && (item->toolTip().length()==0)) {
                          item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                      }
             }
    }

    // When Object selection is enabled, previous selection is set
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                 item->setSelected(true);
        }
        k->configurator->notifySelection(true);
    }

}

void Tweener::setPropertiesMode()
{
    k->editMode = TweenerPanel::Properties;
    disableSelection();

    if (k->objects.isEmpty()) {
        k->objects = k->scene->scene()->getItemsFromTween(k->currentTween->name(), KTItemTweener::Compound);
        k->origin = k->currentTween->transformOriginPoint();
    }
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    disableSelection();
    clearSelection();

    k->mode = TweenerPanel::View;
    k->editMode = TweenerPanel::None;

    k->startPoint = k->scene->currentFrameIndex();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        KOsd::self()->display(tr("Error"), tr("Tween name is missing!"), KOsd::Error);
        return;
    }

    if (k->startPoint != k->scene->currentFrameIndex()) {
        KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                       k->scene->currentLayerIndex(),
                                                                       k->startPoint, KTProjectRequest::Select, "1");
        emit requested(&request);
    }

    setCurrentTween(name);

    KOsd::self()->display(tr("Info"), tr("Tween %1 applied!").arg(name), KOsd::Info);
}

void Tweener::removeTweenFromProject(const QString &name)
{
    KTScene *scene = k->scene->scene();
    scene->removeTween(name, KTItemTweener::Compound);

    foreach (QGraphicsView * view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      QString tip = item->toolTip();
                      if (tip.startsWith(tr("Compound Tween") + ": " + name))
                          item->setToolTip("");
             }
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::updateOriginPoint(const QPointF &point)
{
    k->origin = point;
}

void Tweener::addTarget()
{
    if (k->mode == TweenerPanel::Add) {
        k->target = new Target(k->origin, maxZValue(), k->scene);
        connect(k->target, SIGNAL(positionUpdated(const QPointF &)), this, SLOT(updateOriginPoint(const QPointF &)));
    } else {
        if (k->objects.size() > 0) {
            QGraphicsItem *item = k->objects.at(0);
            k->origin = item->mapToParent(k->currentTween->transformOriginPoint());
            k->target = new Target(k->origin, maxZValue(), k->scene);
            connect(k->target, SIGNAL(positionUpdated(const QPointF &)), this, SLOT(updateOriginPoint(const QPointF &)));
        }
    }
}

void Tweener::updateMode(TweenerPanel::Mode mode)
{
    k->mode = mode;

    if (k->mode == TweenerPanel::Edit) {
        k->startPoint = k->currentTween->startFrame();
        if (k->startPoint != k->scene->currentFrameIndex()) {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, KTProjectRequest::Select, "1");
            emit requested(&request);
        }
    }
}

int Tweener::maxZValue()
{
    int max = -1;
    foreach (QGraphicsView *view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (item->zValue() > max)
                          max = item->zValue();

             }
    }

    return max + 1;
}

Q_EXPORT_PLUGIN2(kt_tweener, Tweener);
