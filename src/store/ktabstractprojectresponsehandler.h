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

#ifndef KTABSTRACTPROJECTRESPONSEHANDLER_H
#define KTABSTRACTPROJECTRESPONSEHANDLER_H

#include "ktglobal_store.h"
#include <QObject>

class KTPaintAreaEvent;
class KTProjectResponse;
class KTItemResponse;
class KTFrameResponse;
class KTSceneResponse;
class KTLayerResponse;
class KTLibraryResponse;

/**
 * @author David Cuadrado
**/

class STORE_EXPORT KTAbstractProjectResponseHandler
{
    public:
        KTAbstractProjectResponseHandler();
        virtual ~KTAbstractProjectResponseHandler();
        
        virtual bool handleResponse(KTProjectResponse *response);
        
    protected:
        virtual void itemResponse(KTItemResponse *itemResponse) = 0;
        virtual void frameResponse(KTFrameResponse *frameResponse) = 0;
        virtual void layerResponse(KTLayerResponse *layerResponse) = 0;
        virtual void sceneResponse(KTSceneResponse *sceneResponse) = 0;
        virtual void projectResponse(KTProjectResponse *projectResponse) = 0;
        virtual void libraryResponse(KTLibraryResponse *libraryResponse) = 0;
};

#endif
