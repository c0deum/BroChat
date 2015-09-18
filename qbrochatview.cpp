#ifdef Q_OS_WIN
#include<windows.h>
#endif

#include <QWebFrame>
#include <QWheelEvent>
#include <QDesktopServices>
#include <QApplication>
#include <QPalette>
#include <QFileInfo>
#include <QScrollBar>

#include <QMessageBox>

#include <QWebFrame>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QAction>
#include <QPushButton>

#include <QStackedLayout>

#include <QBitmap>
#include <QPainter>

#include "qaceschat.h"
#include "qcybergamechat.h"
#include "qfunstreamchat.h"
#include "qgamerstvchat.h"
#include "qgipsyteamchat.h"
#include "qgoodgamechat.h"
#include "qhitboxchat.h"
#include "qrealltvchat.h"
#include "qsc2tvchat.h"
#include "qstreamboxchat.h"
#include "qtwitchchat.h"

#include "qyoutubechat.h"

#include "qchatmessage.h"
#include "qchatstatistic.h"

#include "qchatupdateserver.h"

#include "qsettingsdialog.h"

#include "settingsconsts.h"

#include "qbrochatview.h"

#ifdef Q_CC_MSVC
#pragma comment (lib, "user32.lib")
#endif

const int MIN_WINDOW_WIDTH = 196;
const int MIN_WINDOW_HEIGHT = 64;

const int DEFAULT_WINDOW_WIDTH = 256;
const int DEFAULT_WINDOW_HEIGHT = 512;

const int DEFAULT_SCROLL_SPEED = 32;

QBroChatView::QBroChatView( QWidget *parent )
: QWebView( parent )
, acesChat_( new QAcesChat( this ) )
, cybergameChat_( new QCyberGameChat( this ) )
, funstreamChat_( new QFunStreamChat( this ) )
, gamerstvChat_( new QGamersTvChat( this ) )
, gipsyteamChat_( new QGipsyTeamChat( this ) )
, goodgameChat_( new QGoodGameChat( this ) )
, hitboxChat_( new QHitBoxChat( this ) )
, realltvChat_( new QReallTvChat( this ) )
, sc2tvChat_( new QSc2tvChat( this ) )
, streamboxChat_( new QStreamBoxChat( this ) )
, twitchChat_( new QTwitchChat( this ) )

, youtubeChat_( new QYoutubeChat( this ) )

, chatUpdateServer_( 0 )
, settings_()
, moveState_( false )
, mouseStartPos_( 0 ,0 )
, updatePictureId_( -1 )
, updatePictureInterval_( DEFAULT_SAVE_TO_FILE_INTERVAL )
, showSystemMessages_( DEFAULT_SHOW_SYSTEM_MESSAGES )
, showImages_( DEFAULT_SHOW_IMAGES )
, saveToFile_( DEFAULT_SAVE_TO_FILE )
{
    page()->settings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
    page()->settings()->setAttribute( QWebSettings::Accelerated2dCanvasEnabled, true );
    page()->settings()->setAttribute( QWebSettings::WebGLEnabled, true );

    setMinimumSize( MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT );

    setContextMenuPolicy( Qt::ActionsContextMenu );

    setStyleSheet("QWebView { background: transparent }");

    QObject::connect( this, SIGNAL( linkClicked( const QUrl & ) ), this, SLOT( onLinkClicked( const QUrl & ) ) );

    page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

    changeShowSystemMessagesState();
    changeShowImagesState();
    changeUseServerState();
    changeSaveToFileState();

    loadSettings();

    QObject::connect( this, SIGNAL( closeWindow() ), this, SLOT( saveSettings() ) );

    QAction *settingsAction = new QAction( tr( "&Settings..." ), this );
    QAction *exitAction = new QAction( tr( "&Exit" ), this );

    QAction *reconnectAllAction = new QAction( tr( "Reconnect All Chats" ), this );
    QAction *reconnectAcesAction = new QAction( QIcon( ":/resources/aceslogo.png" ), tr( "Reconnect Aces Chat" ), this );
    QAction *reconnectCybergameAction = new QAction( QIcon( ":/resources/cybergamelogo.png" ), tr( "Reconnect Cybergame Chat" ), this );
    QAction *reconnectFunstreamAction = new QAction( QIcon( ":/resources/funstreamlogo.png" ), tr( "Reconnect Funstream Chat" ), this );
    QAction *reconnectGamersTvAction = new QAction( QIcon( ":/resources/gamerstvlogo.png" ), tr( "Reconnect Gamerstv Chat" ), this );
    QAction *reconnectGipsyTeamAction = new QAction( QIcon( ":/resources/gipsyteamlogo.png" ), tr( "Reconnect Gipsyteam Chat" ), this );
    QAction *reconnectGoodgameAction = new QAction( QIcon( ":/resources/goodgamelogo.png" ), tr( "Reconnect Goodgame Chat" ), this );
    QAction *reconnectHitboxAction = new QAction( QIcon( ":/resources/hitboxlogo.png" ), tr( "Reconnect Hitbox Chat" ), this );
    QAction *reconnectRealltvAction = new QAction( QIcon( ":/resources/realltvlogo.png" ), tr( "Reconnect Realltv Chat" ), this );
    QAction *reconnectSc2tvAction = new QAction( QIcon( ":/resources/sc2tvlogo.png" ), tr( "Reconnect Sc2tv Chat" ), this );
    QAction *reconnectStreamboxAction = new QAction( QIcon( ":/resources/streamboxlogo.png" ), tr( "Reconnect Streambox Chat" ), this );
    QAction *reconnectTwitchAction = new QAction( QIcon( ":/resources/twitchlogo.png" ), tr( "Reconnect Twitch Chat" ), this );
    QAction *reconnectYoutubeAction = new QAction( QIcon( ":/resources/youtubelogo.png" ), tr( "Reconnect Youtube Chat" ), this );

    QObject::connect( settingsAction, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
    QObject::connect( exitAction, SIGNAL( triggered() ), this, SIGNAL( closeWindow() ) );

    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), cybergameChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), funstreamChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), gipsyteamChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), goodgameChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), realltvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), sc2tvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), streamboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), twitchChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), youtubeChat_, SLOT( reconnect() ) );

    QObject::connect( reconnectAcesAction, SIGNAL( triggered() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectCybergameAction, SIGNAL( triggered() ), cybergameChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectFunstreamAction, SIGNAL( triggered() ), funstreamChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectGamersTvAction, SIGNAL( triggered() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectGipsyTeamAction, SIGNAL( triggered() ), gipsyteamChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectGoodgameAction, SIGNAL( triggered() ), goodgameChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectHitboxAction, SIGNAL( triggered() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectRealltvAction, SIGNAL( triggered() ), realltvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectSc2tvAction, SIGNAL( triggered() ), sc2tvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectStreamboxAction, SIGNAL( triggered() ), streamboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectTwitchAction, SIGNAL( triggered() ), twitchChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectYoutubeAction, SIGNAL( triggered() ), youtubeChat_, SLOT( reconnect() ) );

    addAction( settingsAction );

    addAction( reconnectAllAction );
    addAction( reconnectAcesAction );
    addAction( reconnectCybergameAction );
    addAction( reconnectFunstreamAction );
    addAction( reconnectGamersTvAction );
    addAction( reconnectGipsyTeamAction );
    addAction( reconnectGoodgameAction );
    addAction( reconnectHitboxAction );
    addAction( reconnectRealltvAction );
    addAction( reconnectSc2tvAction );
    addAction( reconnectStreamboxAction );
    addAction( reconnectTwitchAction );
    addAction( reconnectYoutubeAction );

    addAction( exitAction );

    QObject::connect( acesChat_, SIGNAL( newMessage ( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), acesChat_, SLOT( reconnect() ) );

    QObject::connect( cybergameChat_, SIGNAL( newMessage ( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( cybergameChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), cybergameChat_, SLOT( reconnect() ) );

    QObject::connect( gamerstvChat_, SIGNAL( newMessage( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( gamerstvChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), gamerstvChat_, SLOT( reconnect() ) );

    QObject::connect( goodgameChat_, SIGNAL( newMessage ( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( goodgameChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), goodgameChat_, SLOT( reconnect() ) );

    QObject::connect( sc2tvChat_, SIGNAL( newMessage ( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), sc2tvChat_, SLOT( reconnect() ) );

    QObject::connect( twitchChat_, SIGNAL( newMessage( QChatMessage * ) ), this, SLOT( slotNewMessage( QChatMessage * ) ) );
    QObject::connect( twitchChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), twitchChat_, SLOT( reconnect() ) );

    QObject::connect( funstreamChat_, SIGNAL( newMessage( QChatMessage * ) ), this, SLOT( slotNewMessage( QChatMessage * ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), funstreamChat_, SLOT( reconnect() ) );

    QObject::connect( streamboxChat_, SIGNAL( newMessage( QChatMessage * ) ), this, SLOT( slotNewMessage( QChatMessage * ) ) );
    QObject::connect( streamboxChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), streamboxChat_, SLOT( reconnect() ) );

    QObject::connect( hitboxChat_, SIGNAL( newMessage( QChatMessage * ) ), this, SLOT( slotNewMessage( QChatMessage * ) ) );
    QObject::connect( hitboxChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), hitboxChat_, SLOT( reconnect() ) );

    QObject::connect( gipsyteamChat_, SIGNAL( newMessage( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), gipsyteamChat_, SLOT( reconnect() ) );

    QObject::connect( realltvChat_, SIGNAL( newMessage( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( realltvChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), realltvChat_, SLOT( reconnect() ) );


    //youtube
    QObject::connect( youtubeChat_, SIGNAL( newMessage( QChatMessage* ) ), this, SLOT( slotNewMessage( QChatMessage* ) ) );
    QObject::connect( this, SIGNAL( loadFinished( bool ) ), youtubeChat_, SLOT( reconnect() ) );


    QObject::connect( this, SIGNAL( loadFinished( bool ) ), this, SLOT( loadFlagsAndAttributes() ) );
}

QBroChatView::~QBroChatView()
{
    page()->settings()->clearMemoryCaches();
}

void QBroChatView::mousePressEvent( QMouseEvent *event )
{
    if( ( event->modifiers() & Qt::ControlModifier ) && ( event->buttons() & Qt::LeftButton ) )
    {
        moveState_ = true;
        mouseStartPos_ = event->globalPos();
    }
    else
    {
        QWebView::mousePressEvent( event );
    }
    event->accept();
}

void QBroChatView::mouseMoveEvent( QMouseEvent *event )
{
    if( moveState_ && ( event->modifiers() & Qt::ControlModifier ) && ( event->buttons() & Qt::LeftButton ) )
    {
        move( pos() + ( event->globalPos() - mouseStartPos_ ) );
        mouseStartPos_ = event->globalPos();
    }
    else
    {
        QWebView::mouseMoveEvent( event );
    }
    event->accept();
}

void QBroChatView::mouseReleaseEvent( QMouseEvent *event )
{
    if( moveState_ && ( event->modifiers() & Qt::ControlModifier ) && ( event->buttons() & Qt::LeftButton ) )
        moveState_ = false;
    else
    {
        QWebView::mouseReleaseEvent( event );
    }
    event->accept();
}

/*
void QBroChatView::keyPressEvent( QKeyEvent *event )
{
    if( event->key() == Qt::Key_S && event->modifiers() == Qt::CTRL )
        showSettings();
    QWebView::keyPressEvent( event );
}
*/

void QBroChatView::timerEvent( QTimerEvent *event )
{
    if(event->timerId() == updatePictureId_ && saveToFile_ )
    {
        QImage image( page()->viewportSize(), QImage::Format_ARGB32 );
        image.fill( Qt::transparent );

        QPainter painter( &image );

        page()->mainFrame()->render( &painter );

        painter.end();

        image.save( QApplication::applicationDirPath() + "/output.png", "PNG" );
    }
}

void QBroChatView::onLinkClicked( const QUrl &url )
{
    QDesktopServices::openUrl( url );
}

void QBroChatView::addMessage( const QString &service, const QString &nickName, const QString &message, const QString &type )
{
    QString messageWithLinks = QChatMessage::insertLinks( message, showImages_ );

    QString formattedNickName = nickName;
    formattedNickName.replace( "\"", "\\\"" );
    formattedNickName.replace( "\'", "\\\'" );

    QString formattedMessage = messageWithLinks;
    formattedMessage.replace( 1, "" );
    formattedMessage.replace( "\\", "\\\\" );
    formattedMessage.replace( "\"", "\\\"" );

    QString js = "onNewMessage( \"" + service + "\", \"" + formattedNickName + "\", \"" + formattedMessage + "\", \"" + type +"\");";
    page()->mainFrame()->evaluateJavaScript( js );

    if( chatUpdateServer_ )
    {
        QString jsonMessage ="{\"service\":\"" + service + "\",\"nick\":\"" + formattedNickName + "\",\"message\":\"" + formattedMessage + "\",\"type\":\"" + type + "\"}";
        chatUpdateServer_->sendMessage( jsonMessage );
    }
}

void QBroChatView::changeStyle( const QString &styleName )
{
    QString pathToStyle;
    if( styleName == DEFAULT_STYLE_NAME )
    {
        pathToStyle = ":/resources/default.html";
    }
    else
    {
       pathToStyle = QApplication::applicationDirPath() + "/styles/" + styleName;
    }

    if( QFileInfo::exists( pathToStyle ) )
    {
        QFile file;
        file.setFileName( pathToStyle );
        file.open( QIODevice::ReadOnly );
        QString style = file.readAll();

        int startScriptCode = style.indexOf( "<!--START_BROWSER_CODE-->" );
        int endScriptCode = style.indexOf( "<!--END_BROWSER_CODE-->" );

        style.remove( startScriptCode, endScriptCode - startScriptCode );

        file.close();

        if( styleName == DEFAULT_STYLE_NAME )
        {
            QColor color;
            QString colorString;
            style.replace( "%FONT_NAME%", "'" + settings_.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() + "'" );

            style.replace( "%NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );
            color = settings_.value( GENERATED_STYLE_NICKNAMES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_TEXT_COLOR ).toUInt();
            colorString = "rgba(" + QString::number( color.red() ) + "," + QString::number( color.green() ) + "," + QString::number( color.blue() ) + "," + QString::number( color.alpha() ) + ")";
            style.replace( "%NICKNAMES_TEXT_COLOR%", colorString );

            style.replace( "%MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );
            color = settings_.value( GENERATED_STYLE_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt();
            colorString = "rgba(" + QString::number( color.red() ) + "," + QString::number( color.green() ) + "," + QString::number( color.blue() ) + "," + QString::number( color.alpha() ) + ")";
            style.replace( "%MESSAGES_TEXT_COLOR%", colorString );

            int viewersFontSize = settings_.value( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE ).toInt();
            style.replace( "%VIEWERS_FONT_SIZE%", QString::number( viewersFontSize ) + "px" );

            color = settings_.value( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR ).toUInt();
            colorString = "rgba(" + QString::number( color.red() ) + "," + QString::number( color.green() ) + "," + QString::number( color.blue() ) + "," + QString::number( color.alpha() ) + ")";
            style.replace( "%VIEWERS_TEXT_COLOR%", colorString );

            style.replace( "%ALIASES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_ALIASES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_FONT_SIZE ).toInt() ) + "px" );
            color = settings_.value( GENERATED_STYLE_ALIASES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_TEXT_COLOR ).toUInt();
            colorString = "rgba(" + QString::number( color.red() ) + "," + QString::number( color.green() ) + "," + QString::number( color.blue() ) + "," + QString::number( color.alpha() ) + ")";
            style.replace( "%ALIASES_TEXT_COLOR%", colorString );

            style.replace( "%SMILES_SIZE%", QString::number( settings_.value( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SMILES_SIZE ).toInt() ) + "px" );

            int serviceIconsSize = settings_.value( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE ).toInt();
            style.replace( "%SERVICE_ICONS_SIZE%", QString::number( serviceIconsSize ) + "px" );

            style.replace( "%ANIMATION_TYPE%", settings_.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE).toString() + "Anim" );

            int viewersHeight = ( viewersFontSize > serviceIconsSize ) ? viewersFontSize : serviceIconsSize;

            viewersHeight += 8;

            style.replace( "%VIEWERS_HEIGHT%",  QString::number( viewersHeight ) + "px" );

            setHtml( style, QUrl( "qrc:/resources/" ) );
        }
        else
        {
            setHtml( style, QUrl::fromLocalFile( QApplication::applicationDirPath() + "/styles/" ) );
        }
    }
    else
    {
        QMessageBox::information( 0, "Error", "Can not apply " + pathToStyle + "...File not exists...", QMessageBox::Ok );
    }

}

void QBroChatView::changeShowSystemMessagesState()
{
    showSystemMessages_ = settings_.value( SHOW_SYSTEM_MESSAGES_SETTING_PATH, DEFAULT_SHOW_SYSTEM_MESSAGES ).toBool();

    acesChat_->setShowSystemMessages( showSystemMessages_ );
    cybergameChat_->setShowSystemMessages( showSystemMessages_ );
    funstreamChat_->setShowSystemMessages( showSystemMessages_ );
    gamerstvChat_->setShowSystemMessages( showSystemMessages_ );
    gipsyteamChat_->setShowSystemMessages( showSystemMessages_ );
    goodgameChat_->setShowSystemMessages( showSystemMessages_ );
    hitboxChat_->setShowSystemMessages( showSystemMessages_ );
    realltvChat_->setShowSystemMessages( showSystemMessages_ );
    sc2tvChat_->setShowSystemMessages( showSystemMessages_ );
    streamboxChat_->setShowSystemMessages( showSystemMessages_ );
    twitchChat_->setShowSystemMessages( showSystemMessages_ );
    youtubeChat_->setShowSystemMessages( showSystemMessages_ );
}

void QBroChatView::changeShowImagesState()
{
    showImages_ = settings_.value( SHOW_IMAGES_SETTING_PATH, DEFAULT_SHOW_IMAGES ).toBool();
}

void QBroChatView::changeUseServerState()
{
    if( chatUpdateServer_ )
    {
        chatUpdateServer_->close();
        chatUpdateServer_->deleteLater();
        chatUpdateServer_ = 0;
    }

    if( settings_.value( USE_SERVER_SETTING_PAH, DEFAULT_USE_SERVER ).toBool() )
        chatUpdateServer_ = new QChatUpdateServer( settings_.value( SERVER_PORT_SETTING_PATH, DEFAULT_SERVER_PORT ).toInt(), this );
}
void QBroChatView::changeSaveToFileState()
{
    if( updatePictureId_ >= 0 )
    {
        killTimer( updatePictureId_ );
        updatePictureId_ = -1;
    }

    saveToFile_ = settings_.value( SAVE_TO_FILE_SETTING_PATTH, DEFAULT_SAVE_TO_FILE ).toBool();

    if( saveToFile_ )
    {
        updatePictureInterval_ = settings_.value( SAVE_TO_FILE_INTERVAL_SETTING_PATH, DEFAULT_SAVE_TO_FILE_INTERVAL ).toInt();
        if( updatePictureId_ == - 1 )
            updatePictureId_ = startTimer( updatePictureInterval_ );
    }
}

void QBroChatView::slotNewMessage( QChatMessage *message )
{
    addMessage( message->service(), message->nickName(), message->message(), message->type() );
    message->deleteLater();
}

void QBroChatView::onNewStatistic( QChatStatistic *statistic )
{
    QString js = "onStatisticReceived( \"" + statistic->service() + "\", \"" +statistic->statistic() + "\");";
    page()->mainFrame()->evaluateJavaScript( js );

    if( chatUpdateServer_ )
    {
        //QString jsonMessage ="{\"service\":\"" + service + "\",\"nick\":\"" + formattedNickName + "\",\"message\":\"" + formattedMessage + "\",\"type\":\"" + type + "\"}";
        //chatUpdateServer_->sendMessage( jsonMessage );
    }
    statistic->deleteLater();
}

void QBroChatView::loadSettings()
{
    int width = settings_.value( "/Settings/Window/Width", DEFAULT_WINDOW_WIDTH ).toInt();
    int height = settings_.value( "/Settings/Window/Height", DEFAULT_WINDOW_HEIGHT ).toInt();

    int xPos = settings_.value( "/Settings/Window/X", ( QApplication::desktop()->width() - width ) / 2 ).toInt();
    int yPos = settings_.value( "/Settings/Window/Y", ( QApplication::desktop()->height() - height ) / 2 ).toInt();

    move( xPos, yPos );
    resize( width, height );

    changeOpacity();
    changeStyle();
}

void QBroChatView::saveSettings()
{
    settings_.setValue( "/Settings/Window/X", pos().x() );
    settings_.setValue( "/Settings/Window/Y", pos().y() );
    settings_.setValue( "/Settings/Window/Width", width() );
    settings_.setValue( "/Settings/Window/Height", height() );
    settings_.setValue( "/Settings/Window/Opacity", static_cast<int>( windowOpacity() * 100 ) );
}

void QBroChatView::closeEvent( QCloseEvent *event )
{
    saveSettings();
    event->accept();
}

void QBroChatView::showSettings()
{
    QSettingsDialog *settingsDialog = new QSettingsDialog( this );

    QObject::connect( settingsDialog, SIGNAL( acesChannelChanged() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameChannelChanged() ), cybergameChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( funstreamChannelChanged() ), funstreamChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( gamerstvChannelChanged() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamChannelChanged() ), gipsyteamChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( goodGameChannelChanged() ), goodgameChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxChannelChanged() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( realltvChannelChanged() ), realltvChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvChannelChanged() ), sc2tvChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( streamboxChannelChanged() ), streamboxChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( twitchChannelChanged() ), twitchChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( youtubeChannelChanged() ), youtubeChat_, SLOT( reconnect() ) );


    QObject::connect( settingsDialog, SIGNAL( acesAliasesChanged( QString ) ), acesChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameAliasesChanged( QString ) ), cybergameChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( funstreamAliasesChanged( QString ) ), funstreamChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( gamerstvAliasesChanged(QString) ), gamerstvChat_, SLOT( setAliasesList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamAliasesChanged(QString) ), gipsyteamChat_, SLOT( setAliasesList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( goodGameAliasesChanged( QString ) ), goodgameChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxAliasesChanged( QString ) ), hitboxChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( realltvAliasesChanged( QString ) ), realltvChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvAliasesChanged( QString ) ), sc2tvChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( streamboxAliasesChanged( QString ) ), streamboxChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( twitchAliasesChanged( QString ) ), twitchChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( youtubeAliasesChanged( QString ) ), youtubeChat_, SLOT( setAliasesList( QString ) ) );

    QObject::connect( settingsDialog, SIGNAL( acesSupportersListChanged( QString ) ), acesChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameSupportersListChanged( QString ) ), cybergameChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( funstreamSupportersListChanged( QString ) ), funstreamChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( gamerstvSupportersListChanged(QString) ), gamerstvChat_, SLOT( setSupportersList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamSupportersListChanged(QString) ), gipsyteamChat_, SLOT( setSupportersList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( goodGameSupportersListChanged( QString ) ), goodgameChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxSupportersListChanged( QString ) ), hitboxChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( realltvSupportersListChanged( QString ) ), realltvChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvSupportersListChanged( QString ) ), sc2tvChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( streamboxSupportersListChanged( QString ) ), streamboxChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( twitchSupportersListChanged( QString ) ), twitchChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( youtubeSupportersListChanged( QString ) ), youtubeChat_, SLOT( setSupportersList( QString ) ) );

    QObject::connect( settingsDialog, SIGNAL( acesBlackListChanged( QString ) ), acesChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameBlackListChanged( QString ) ), cybergameChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( funstreamBlackListChanged( QString ) ), funstreamChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( gamerstvBlackListChanged(QString) ), gamerstvChat_, SLOT( setBlackList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamBlackListChanged(QString) ), gipsyteamChat_, SLOT( setBlackList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( goodGameBlackListChanged( QString ) ), goodgameChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxBlackListChanged( QString ) ), hitboxChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( realltvBlackListChanged( QString ) ), realltvChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvBlackListChanged( QString ) ), sc2tvChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( streamboxBlackListChanged( QString ) ), streamboxChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( twitchBlackListChanged( QString ) ), twitchChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( youtubeBlackListChanged( QString ) ), youtubeChat_, SLOT( setBlackList( QString ) ) );

    QObject::connect( settingsDialog, SIGNAL( acesRemoveBlackListUsersChanged( bool ) ), acesChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameRemoveBlackListUsersChanged( bool ) ), cybergameChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( funstreamRemoveBlackListUsersChanged( bool ) ), funstreamChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( gamerstvRemoveBlackListUsersChanged(bool) ), gamerstvChat_, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamRemoveBlackListUsersChanged(bool) ), gipsyteamChat_, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( goodGameRemoveBlackListUsersChanged( bool ) ), goodgameChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxRemoveBlackListUsersChanged( bool ) ), hitboxChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( realltvRemoveBlackListUsersChanged( bool ) ), realltvChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvRemoveBlackListUsersChanged( bool ) ), sc2tvChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( streamboxRemoveBlackListUsersChanged( bool ) ), streamboxChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( twitchRemoveBlackListUsersChanged( bool ) ), twitchChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( youtubeRemoveBlackListUsersChanged( bool ) ), youtubeChat_, SLOT( setRemoveBlackListUsers(bool ) ) );


    QObject::connect( settingsDialog, SIGNAL( acesOriginalColorsChanged( bool ) ), acesChat_, SLOT( changeOriginalColors( bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxOriginalColorsChanged( bool ) ), hitboxChat_, SLOT( changeOriginalColors( bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( sc2tvOriginalColorsChanged( bool ) ), sc2tvChat_, SLOT( changeOriginalColors( bool ) ) );

    QObject::connect( settingsDialog, SIGNAL( goodGameUseAnimatedSmilesChanged( bool ) ), goodgameChat_, SLOT( changeUseAnimatedSmiles( bool ) ) );

    QObject::connect( settingsDialog, SIGNAL( styleChanged() ), this, SLOT( changeStyle() ) );
    QObject::connect( settingsDialog, SIGNAL( opacityChanged() ), this, SLOT( changeOpacity() ) );
    QObject::connect( settingsDialog, SIGNAL( flagsOrAttributesChanged() ), this, SLOT( loadFlagsAndAttributes() ) );

    QObject::connect( settingsDialog, SIGNAL( showSystemMessagesChanged() ), this, SLOT( changeShowSystemMessagesState() ) );

    QObject::connect( settingsDialog, SIGNAL( showImagesChanged() ), this, SLOT( changeShowImagesState() ) );
    QObject::connect( settingsDialog, SIGNAL( useServerStateChanged() ), this, SLOT( changeUseServerState() ) );
    QObject::connect( settingsDialog, SIGNAL( saveToFileStateChanged() ), this, SLOT( changeSaveToFileState() ) );

    settingsDialog->exec();

    settingsDialog->deleteLater();
}

void QBroChatView::changeStyle()
{
    changeStyle( settings_.value( STYLE_SETTING_PATH, DEFAULT_STYLE_NAME ).toString() );
}

void QBroChatView::changeOpacity()
{
    setWindowOpacity( settings_.value( OPACITY_SETTING_PATH, DEFAULT_OPACITY_VALUE ).toInt() / 100.0f );
}

void QBroChatView::loadFlagsAndAttributes()
{
    Qt::WindowFlags flags = windowFlags();

    if( settings_.value( STAY_ON_TOP_SETTING_PATH, DEFAULT_STAY_ON_TOP ).toBool() )
    {
        flags |= Qt::WindowStaysOnTopHint;
        flags &= ~( Qt::WindowStaysOnBottomHint );
    }
    else
    {
        flags |= Qt::WindowStaysOnBottomHint;
        flags &= ~( Qt::WindowStaysOnTopHint );
    }

    if( settings_.value( FRAMELESS_WINDOW_SETTING_PATH, DEFAULT_FRAMELESS_WINDOW ).toBool() )
    {
        flags |= Qt::FramelessWindowHint;
    }
    else
    {
        flags &= ~( Qt::FramelessWindowHint );
    }

    setWindowFlags( flags );
    show();

#ifdef Q_OS_WIN
    if( settings_.value( STAY_ON_TOP_SETTING_PATH, DEFAULT_STAY_ON_TOP ).toBool() )
    {
        SetWindowPos( reinterpret_cast< HWND >( winId() ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
    }
    else
    {
        SetWindowPos( reinterpret_cast< HWND >( winId() ), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
    }

#endif
}
