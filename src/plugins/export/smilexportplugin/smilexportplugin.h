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

#ifndef SMILEXPORTPLUGIN_H
#define SMILEXPORTPLUGIN_H

#include "ktexportpluginobject.h"

#include <QDomDocument>
#include <QDomElement>
#include <QColor>

/**
 * @author David Cuadrado
*/

class SmilExportPlugin : public KTExportPluginObject
{
    Q_OBJECT

    public:
        SmilExportPlugin();
        virtual ~SmilExportPlugin();
        virtual QString key() const;
        KTExportInterface::Formats availableFormats();

        virtual bool exportToFormat(const QColor color, const QString &filePath, const QList<KTScene *> &scenes, KTExportInterface::Format format, const QSize &size, int fps);
        virtual const char* getExceptionMsg();
        const char *errorMsg;

    private:
        QStringList createImages(const QList<KTScene *> &scenes, const QDir &dir,float sx = 1, float sy = 1, const char *format = "PNG");
        void initSmil();
        void createPar(const QString &filePath, double duration);

    private:
        QString m_baseName;

        QDomDocument m_smil;
        QDomElement m_body;

        int m_fps;
        QSize m_size;
};

#endif
