/***************************************************************************
 *   Project TUPITUBE DESK                                                *
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
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "tupcolorpicker.h"

TupColorPicker::TupColorPicker(QWidget *parent) : QFrame(parent)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    pWidth = 280;
    pHeight = 200;
    hue = 0;
    saturation = 0;
    target = false;

    setColor(150, 255);

    QImage img(pWidth, pHeight, QImage::Format_RGB32);
    int x;
    int y;
    for (y = 0; y < pHeight; y++) {
         for (x = 0; x < pWidth; x++) {
              QPoint p(x, y);
              QColor c;
              c.setHsv(huePoint(p), saturationPoint(p), 200);
              img.setPixel(x, y, c.rgb());
         }
    }

    pix = new QPixmap(QPixmap::fromImage(img));
    setAttribute(Qt::WA_NoSystemBackground);
}

TupColorPicker::~TupColorPicker()
{
    #ifdef TUP_DEBUG
        qDebug() << "[~TupColorPicker()]";
    #endif
}

QPoint TupColorPicker::colorPoint()
{ 
    return QPoint((360 - hue)*(pWidth - 1)/360, (255 - saturation)*(pHeight - 1)/255);
}

int TupColorPicker::huePoint(const QPoint &pt)
{ 
    return 360 - ((pt.x() * 360) / (pWidth - 1));
}

int TupColorPicker::saturationPoint(const QPoint &pt)
{ 
    return 255 - ((pt.y() * 255) / (pHeight - 1));
}

void TupColorPicker::setColor(const QPoint &pt)
{ 
    target = true;
    setColor(huePoint(pt), saturationPoint(pt)); 
}

void TupColorPicker::clearSelection()
{
    target = false;
    QRect rect(colorPoint(), QSize(20,20));
    rect = rect.united(QRect(colorPoint(), QSize(20, 20)));
    rect.translate(contentsRect().x() - 9, contentsRect().y() - 9);

    repaint(rect);
}

QSize TupColorPicker::sizeHint() const
{
    return QSize(pWidth + 2*frameWidth(), pHeight + 2*frameWidth());
}

void TupColorPicker::setColor(int localHue, int localSat)
{
    int nhue = qMin(qMax(0, localHue), 359);
    int nsat = qMin(qMax(0, localSat), 255);

    if (nhue == hue && nsat == saturation)
        return;

    QRect rect(colorPoint(), QSize(20, 20));
    hue = nhue;
    saturation = nsat;
    rect = rect.united(QRect(colorPoint(), QSize(20, 20)));
    rect.translate(contentsRect().x() - 9, contentsRect().y() - 9);

    repaint(rect);
}

void TupColorPicker::setHUE(int localHue)
{
    int newHue = qMin(qMax(0, localHue), 359);
    if (newHue == hue)
        return;

    QRect rect(colorPoint(), QSize(20, 20));
    hue = newHue;
    rect = rect.united(QRect(colorPoint(), QSize(20, 20)));
    rect.translate(contentsRect().x() - 9, contentsRect().y() - 9);

    repaint(rect);
}

void TupColorPicker::setSaturation(int localSat)
{
    int newSat = qMin(qMax(0, localSat), 255);
    if (newSat == saturation)
        return;

    QRect rect(colorPoint(), QSize(20, 20));
    saturation = newSat;
    rect = rect.united(QRect(colorPoint(), QSize(20, 20)));
    rect.translate(contentsRect().x() - 9, contentsRect().y() - 9);

    repaint(rect);
}

void TupColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);
    emit newColor(hue, saturation);
}

void TupColorPicker::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);
    emit newColor(hue, saturation);
}

void TupColorPicker::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    drawFrame(&painter);
    QRect rect = contentsRect();

    painter.drawPixmap(rect.topLeft(), *pix);
    QPoint point = colorPoint() + rect.topLeft();

    if (target) {
        painter.setPen(Qt::white);
        painter.drawRect(point.x() - 4, point.y() - 4, 10, 10);
    }
}

int TupColorPicker::getHue()
{
    return hue;
}

int TupColorPicker::getSaturation()
{
    return saturation;
}
