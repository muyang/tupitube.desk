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

#include "tupcamerawidget.h"
#include "tconfig.h"
#include "tupinfodialog.h"
#include "tupprojectresponse.h"
#include "tuprequestbuilder.h"

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>

TupCameraWidget::TupCameraWidget(TupProject *work, bool isNetworked, QWidget *parent) : QFrame(parent)
{
    #ifdef TUP_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCameraWidget()]";
        #else
            TINIT;
        #endif
    #endif

    QDesktopWidget desktop;

    int percent = 40;
    int height = desktop.screenGeometry().height();
    if (height <= 700)
        percent = 30;
    if (height >= 1080)
        percent = 60;

    int desktopWidth = (percent * desktop.screenGeometry().width()) / 100;
    int desktopHeight = (percent * height) / 100;
    screenDimension = QSize(desktopWidth, desktopHeight);

    project = work;
    setObjectName("TupCameraWidget_");

    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    addVideoHeader();
    addTimerPanel();
    layout->addSpacing(10);

    addAnimationDisplay();

    layout->addSpacing(10);
    addControlsBar();
    addStatusPanel(isNetworked);
}

TupCameraWidget::~TupCameraWidget()
{
    #ifdef TUP_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupCameraWidget()]";
        #else
            TEND;
        #endif
    #endif

    if (cameraBar) {
        delete cameraBar;
        cameraBar = nullptr;
    }

    if (progressBar) {
       delete progressBar;
       progressBar = nullptr;
    }

    if (status) {
        delete status;
        status = nullptr;
    }

    if (screen) {
        delete screen;
        screen = nullptr;
    }

    // delete k;
}

void TupCameraWidget::addVideoHeader()
{
    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->setAlignment(Qt::AlignCenter);
    labelLayout->setSpacing(0);

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap(THEME_DIR + "icons/player.png"));
    QLabel *title = new QLabel(tr("Scene Preview"));
    QFont font = this->font();
    font.setBold(true);
    title->setFont(font);

    titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);

    setProgressBar();

    projectLabel = new QLabel(project->projectName() + " ");
    projectLabel->setFont(font);

    setDimensionLabel(project->dimension());

    QWidget *scaleWidget = new QWidget();
    QHBoxLayout *scaleLayout = new QHBoxLayout(scaleWidget);
    scaleLayout->setContentsMargins(0, 0, 0, 0);
    scaleLayout->setAlignment(Qt::AlignCenter);
    scaleLayout->addWidget(scaleLabel);

    QPushButton *editButton = new QPushButton();
    editButton->setIcon(QIcon(THEME_DIR + "icons/edit_sign.png"));
    editButton->setFocusPolicy(Qt::NoFocus);
    editButton->setToolTip(tr("Edit Project Information"));
    connect(editButton, SIGNAL(pressed()), this, SLOT(infoDialog()));

    labelLayout->addWidget(projectLabel);
    labelLayout->addSpacing(5);
    labelLayout->addWidget(editButton);
    labelLayout->addSpacing(20);
    labelLayout->addWidget(scaleWidget);

    layout->addWidget(titleWidget, 0, Qt::AlignCenter);
    layout->addWidget(progressBar, 0, Qt::AlignCenter);
    layout->addLayout(labelLayout, Qt::AlignCenter);
}

void TupCameraWidget::setProgressBar()
{
    TCONFIG->beginGroup("General");
    QString themeName = TCONFIG->value("Theme", "Light").toString();

    progressBar = new QProgressBar(this);
    QString style1 = "QProgressBar { background-color: #DDDDDD; text-align: center; color: #FFFFFF; border-radius: 2px; } ";
    QString color = "#009500";
    if (themeName.compare("Dark") == 0)
        color = "#444444";
    QString style2 = "QProgressBar::chunk { background-color: " + color + "; border-radius: 2px; }";

    progressBar->setStyleSheet(style1 + style2);
    progressBar->setMaximumHeight(5);
    progressBar->setTextVisible(false);
    progressBar->setRange(1, 100);
    progressBar->setVisible(false);
}

void TupCameraWidget::addTimerPanel()
{
    QFont font = this->font();
    font.setBold(true);

    QLabel *timerFramesLabel = new QLabel(tr("Current Frame: "));
    timerFramesLabel->setFont(font);
    currentFrameBox = new QLabel("1");
    currentFrameBox->setAlignment(Qt::AlignCenter);
    currentFrameBox->setMinimumWidth(40);
    QString style = "QLabel { background-color: #c8c8c8; border: 1px solid #777777; border-radius: 2px; }";
    currentFrameBox->setStyleSheet(style);

    QLabel *stopwatchLabel = new QLabel(tr("Timer: "));
    stopwatchLabel->setFont(font);
    timerSecsLabel = new QLabel("00.00");
    timerSecsLabel->setAlignment(Qt::AlignCenter);
    timerSecsLabel->setMinimumWidth(50);
    timerSecsLabel->setStyleSheet(style);

    QLabel *durationLabel = new QLabel(tr("Duration: "));
    durationLabel->setFont(font);
    duration = new QLabel("");

    QFrame *timerWidget = new QFrame(this);
    timerWidget->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    QHBoxLayout *timerLayout = new QHBoxLayout(timerWidget);
    timerLayout->setContentsMargins(10, 5, 10, 5);
    timerLayout->setAlignment(Qt::AlignCenter);
    timerLayout->addWidget(timerFramesLabel);
    timerLayout->addWidget(currentFrameBox);
    timerLayout->addSpacing(10);

    timerLayout->addWidget(stopwatchLabel);
    timerLayout->addWidget(timerSecsLabel);
    timerLayout->addWidget(new QLabel(tr("secs")));
    timerLayout->addSpacing(10);

    timerLayout->addWidget(durationLabel);
    timerLayout->addWidget(duration);

    layout->addWidget(timerWidget, 0, Qt::AlignCenter|Qt::AlignTop);
}

void TupCameraWidget::addAnimationDisplay()
{
    screen = new TupScreen(project, playerDimension, isScaled);
    connect(screen, SIGNAL(isRendering(int)), this, SLOT(updateProgressBar(int)));
    connect(screen, SIGNAL(frameChanged(int)), this, SLOT(updateTimerPanel(int)));

    layout->addWidget(screen, 0, Qt::AlignCenter);
}

void TupCameraWidget::addControlsBar()
{
    cameraBar = new TupCameraBar;

    connect(cameraBar, SIGNAL(play()), this, SLOT(doPlay()));
    connect(cameraBar, SIGNAL(playBack()), this, SLOT(doPlayBack()));
    connect(cameraBar, SIGNAL(pause()), screen, SLOT(pause()));
    connect(cameraBar, SIGNAL(stop()), screen, SLOT(stop()));
    connect(cameraBar, SIGNAL(ff()), screen, SLOT(nextFrame()));
    connect(cameraBar, SIGNAL(rew()), screen, SLOT(previousFrame()));

    layout->addWidget(cameraBar, 0, Qt::AlignCenter);
}

void TupCameraWidget::addStatusPanel(bool isNetworked)
{
    status = new TupCameraStatus(isNetworked);
    status->setScenes(project);
    connect(status, SIGNAL(sceneIndexChanged(int)), this, SLOT(selectScene(int)));
    connect(status, SIGNAL(muteEnabled(bool)), screen, SLOT(enableMute(bool)));
    // connect(status, SIGNAL(fpsChanged(int)), this, SLOT(setFPS(int)));
    // connect(status, SIGNAL(fpsChanged(int)), this, SLOT(setDuration(int)));
    connect(status, SIGNAL(fpsChanged(int)), this, SLOT(updateFPS(int)));
    connect(status, SIGNAL(loopChanged()), this, SLOT(setLoop()));
    connect(status, SIGNAL(exportChanged()), this, SLOT(exportDialog()));
    if (isNetworked)
        connect(status, SIGNAL(postChanged()), this, SLOT(postDialog()));

    updateFramesTotal(0);
    int fps = project->fps();
    fpsDelta = 1.0/fps;
    status->setFPS(fps);
    setLoop();
    layout->addWidget(status, 0, Qt::AlignCenter|Qt::AlignTop);
}

void TupCameraWidget::setDimensionLabel(const QSize dimension)
{
    QFont font = this->font();
    font.setBold(false);
    scaleLabel = new QLabel;
    scaleLabel->setFont(font);

    int screenWidth = screenDimension.width();
    int screenHeight = screenDimension.height();

    int projectWidth = dimension.width();
    int projectHeight = dimension.height();

    QString scale = "<b>[</b> " + tr("Scale") + " ";
    isScaled = false;

    if (projectWidth <= screenWidth && projectHeight <= screenHeight) {
        playerDimension = project->dimension();
        scale += "1:1";
    } else {
        double proportion = 1;

        if (projectWidth > projectHeight) {
            int newH = (projectHeight*screenWidth)/projectWidth;
            playerDimension = QSize(screenWidth, newH);
            proportion = static_cast<double>(projectWidth) / static_cast<double>(screenWidth);
        } else { // projectHeight > projectWidth
            int newW = (projectWidth*screenHeight)/projectHeight;
            playerDimension = QSize(newW, screenHeight);
            proportion = static_cast<double>(projectHeight) / static_cast<double>(screenHeight);
        }

        scale += "1:" + QString::number(proportion, 'g', 2);
        isScaled = true;
    }

    scale += " | " + tr("Size") + ": ";
    scale += QString::number(projectWidth) + "x" + QString::number(projectHeight);
    scale += " px <b>]</b>";

    scaleLabel->setText(scale);
}

void TupCameraWidget::setLoop()
{
    screen->setLoop(status->isLooping());
}

QSize TupCameraWidget::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void TupCameraWidget::doPlay()
{
#ifdef TUP_DEBUG
    #ifdef Q_OS_WIN
        qDebug() << "[TupCameraWidget::doPlay()]";
    #else
        T_FUNCINFO;
    #endif
#endif

    screen->play();
    bool flag = false;
    if (screen->currentSceneFrames() > 1)
        flag = true;
    status->enableExportButton(flag);
}

void TupCameraWidget::doPlayBack()
{
    screen->playBack();
}

void TupCameraWidget::doPause()
{
#ifdef TUP_DEBUG
    #ifdef Q_OS_WIN
        qDebug() << "[TupCameraWidget::doPause()]";
    #else
        T_FUNCINFO;
    #endif
#endif
    bool playOn = screen->isPlaying();
    cameraBar->updatePlayButton(!playOn);
    screen->pause();
}

void TupCameraWidget::doStop()
{
    cameraBar->updatePlayButton(false);
    screen->stop();
}

void TupCameraWidget::nextFrame()
{
    screen->nextFrame();
}

void TupCameraWidget::previousFrame()
{
    screen->previousFrame();
}

bool TupCameraWidget::handleProjectResponse(TupProjectResponse *response)
{
    #ifdef TUP_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCameraWidget::handleProjectResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (TupSceneResponse *sceneResponse = static_cast<TupSceneResponse *>(response)) {
        int index = sceneResponse->sceneIndex();

        switch (sceneResponse->action()) {
            case TupProjectRequest::Add:
            {
                 status->setScenes(project);
                 status->setCurrentScene(index);
                 updateFramesTotal(index);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 if (index < 0)
                     break;

                 if (index == project->scenesCount())
                     index--;

                 if (index >= 0) {
                     status->setScenes(project);
                     status->setCurrentScene(index);
                     updateFramesTotal(index);
                 }
            }
            break;
            case TupProjectRequest::Reset:
            {
                 status->setScenes(project);
            }
            break;
            case TupProjectRequest::Select:
            {
                 if (index >= 0) {
                     currentSceneIndex = index;
                     updateFramesTotal(index);
                     status->setCurrentScene(index);
                 }
            }
            break;
            case TupProjectRequest::Rename:
            {
                 status->setScenes(project);
                 status->setCurrentScene(index);
            }
            break;
            default:
            {
                 #ifdef TUP_DEBUG
                     QString msg = "TupCameraWidget::handleProjectResponse() - Unknown/Unhandled project action: " 
                                   + QString::number(sceneResponse->action());
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
            }
            break;
        }
    }

    return screen->handleResponse(response);
}

void TupCameraWidget::setFPS(int fps)
{
    project->setFPS(fps);
    screen->setFPS(fps);
    fpsDelta = 1.0/fps;
}

void TupCameraWidget::setDuration(int fps)
{
    qreal time = static_cast<qreal> (framesTotal) / static_cast<qreal> (fps);
    duration->setText(QString::number(time, 'f', 2) + QString(" " + tr("secs")));
}

void TupCameraWidget::updateFPS(int fps)
{
    setFPS(fps);
    setDuration(fps);
}

void TupCameraWidget::setStatusFPS(int fps)
{
    status->blockSignals(true);
    status->setFPS(fps);
    status->blockSignals(false);

    project->setFPS(fps);
    screen->setFPS(fps);
    setDuration(fps);
}

void TupCameraWidget::updateFramesTotal(int sceneIndex)
{
    TupScene *scene = project->sceneAt(sceneIndex);
    if (scene) {
        framesTotal = scene->framesCount();
        status->setFramesTotal(QString::number(framesTotal));
        progressBar->setRange(0, framesTotal);
        setDuration(project->fps());
    }
}

void TupCameraWidget::exportDialog()
{
    if (screen->isPlaying())
        screen->pause();

    QDesktopWidget desktop;
    TupExportWidget *exportWidget = new TupExportWidget(project, this);
    exportWidget->show();
    exportWidget->move(static_cast<int> ((desktop.screenGeometry().width() - exportWidget->width()) / 2),
                       static_cast<int> ((desktop.screenGeometry().height() - exportWidget->height()) / 2));
    exportWidget->exec();
}

void TupCameraWidget::postDialog()
{
    QDesktopWidget desktop;

    TupExportWidget *exportWidget = new TupExportWidget(project, this, false);
    exportWidget->show();
    exportWidget->move(static_cast<int> ((desktop.screenGeometry().width() - exportWidget->width()) / 2),
                       static_cast<int> ((desktop.screenGeometry().height() - exportWidget->height()) / 2));
    exportWidget->exec();

    if (exportWidget->isComplete() != QDialog::Rejected) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        emit requestForExportVideoToServer(exportWidget->videoTitle(), exportWidget->videoTopics(), 
                                           exportWidget->videoDescription(), status->getFPS(),
                                           exportWidget->videoScenes());
    }
}

void TupCameraWidget::selectScene(int index)
{
    if (index != screen->currentSceneIndex()) {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit requestTriggered(&event);

        doStop();
        screen->updateSceneIndex(index);
        screen->updateAnimationArea();
        doPlay();
    }
}

void TupCameraWidget::updateScenes(int sceneIndex)
{
    screen->resetPhotograms(sceneIndex);
}

void TupCameraWidget::updateFirstFrame()
{
    screen->updateAnimationArea();
    currentFrameBox->setText("1");
}

void TupCameraWidget::updateProgressBar(int advance)
{
    if (advance == 0) {
        QApplication::restoreOverrideCursor();
        titleWidget->setVisible(true);
        progressBar->setVisible(false);
    }

    if (advance == 1) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        titleWidget->setVisible(false);
        progressBar->setVisible(true);
    }

    progressBar->setValue(advance);
}

void TupCameraWidget::updateTimerPanel(int currentFrame)
{
    currentFrameBox->setText(QString::number(currentFrame));
    double time = fpsDelta * currentFrame;
    timerSecsLabel->setText(QString::number(time, 'f', 2));
}

void TupCameraWidget::updateSoundItems()
{
    screen->loadSoundRecords();
}

void TupCameraWidget::infoDialog()
{
    TupInfoDialog *settings = new TupInfoDialog(project->tags(),
                                                        project->author(), project->description(),
                                                        this);
    connect(settings, SIGNAL(dataSent(const QString &, const QString &, const QString &)), 
            this, SLOT(saveProjectInfo(const QString &, const QString &, const QString &)));

    settings->show();
}

void TupCameraWidget::saveProjectInfo(const QString &tags, const QString &author, 
                                      const QString &description) 
{
    project->setTags(tags);
    project->setAuthor(author);
    emit projectAuthorUpdated(author);
    project->setDescription(description);
}
