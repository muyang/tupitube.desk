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

#include "tanimwidget.h"

class TAnimWidget::Controller
{
    public:
        Controller(TAnimWidget *area) : m_area(area), m_timerId(-1)
         {
            started = false;
         }
        ~Controller()
         {
         }
        void start(int ms)
         {
             if (m_timerId != -1)
                 stop();

             m_timerId = m_area->startTimer(ms);
             started = true;
         }
        void stop()
         {
             m_area->killTimer(m_timerId);
             m_timerId = -1;
             started = false;
         }
        bool hasStarted()
         {
            return started;
         }
    private:
        TAnimWidget *m_area;
        int m_timerId;
        bool started;
};

TAnimWidget::TAnimWidget(const QPixmap &px, const QString &text, QWidget *parent) : QWidget(parent), m_type(AnimText),
                                                                                            m_controller(new Controller(this)),
                                                                                            m_background(px), m_text(text)
{
    resize(px.width()/2, px.height());

    fontSize = 10;
    #ifdef Q_OS_MAC
        fontSize = 12;
    #endif

    QFont tfont("lucida", fontSize, QFont::Bold, false);
    QFontMetrics fontMetrics(tfont);

    m_textRect = QRectF(QPointF(20, height()), fontMetrics.size(Qt::TextWordWrap, m_text).expandedTo(QSize(px.width(), 0)));
    m_counter = 0;
    m_lines = m_text.count("\n");
    int size = fontMetrics.height();
    m_end = (size*m_lines) + height() - 100;
}

TAnimWidget::TAnimWidget(ListOfPixmaps lop, QWidget *parent) : QWidget(parent), m_type(AnimPixmap), m_controller(new Controller(this)), m_pixmaps(lop), m_pixmapIndex(0)
{
    m_background = lop[0];
}

TAnimWidget::~ TAnimWidget()
{
    delete m_controller;
}

void TAnimWidget::setBackgroundPixmap(const QPixmap &px)
{
    m_background = px;
}

void TAnimWidget::showEvent(QShowEvent *event)
{
    switch (m_type) {
            case AnimText:
             {
                 m_controller->start(50);
             }
            break;
            case AnimPixmap:
             {
                 m_controller->start(5000);
             }
            break;
    }
    QWidget::showEvent(event);
}

void TAnimWidget::hideEvent(QHideEvent *event)
{
    m_controller->stop();
    QWidget::hideEvent(event);
}

void TAnimWidget::timerEvent(QTimerEvent *)
{
    switch (m_type) {
        case AnimText:
         {
             int yPos = (int)(m_textRect.y() - 1);
             m_textRect.setY(yPos);
             m_counter++;

             if (m_counter > m_end) {
                 m_counter = 0;
                 m_textRect.setY(height());
             }
         }
        break;
        case AnimPixmap:
         {
             m_pixmapIndex = (m_pixmapIndex + 1) % m_pixmaps.count();
             m_background = m_pixmaps[m_pixmapIndex];
         }
        break;
    }
    update();
}

void TAnimWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(0, 0, m_background);

    switch (m_type) {
        case AnimText:
         {
             painter.setRenderHint(QPainter::TextAntialiasing, true);
             painter.setFont(QFont("lucida", fontSize, QFont::Bold, false));
             painter.drawText(m_textRect, m_text);
         }
        break;
        case AnimPixmap:
         {
         }
        break;
    }
}

void TAnimWidget::activateAnimation()
{
    if (m_controller->hasStarted())
        m_controller->stop();
    else
        m_controller->start(50);
}
