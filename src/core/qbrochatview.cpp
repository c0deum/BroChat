#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDesktopWidget>


#include <QWheelEvent>
#include <QPalette>
#include <QFileInfo>
#include <QScrollBar>

#include <QMouseEvent>
#include <QAction>
#include <QPushButton>

#include <QStackedLayout>

#include <QBitmap>
#include <QPainter>

#include <QDir>

#include <QJsonDocument>

#include <QWebEngineSettings>

#include "../aces/qaceschat.h"
#include "../azubu/qazubuchat.h"
#include "../beampro/qbeamprochat.h"
#include "../cybergame/qcybergamechat.h"
#include "../gamerstv/qgamerstvchat.h"
#include "../gipsyteam/qgipsyteamchat.h"
#include "../goodgame/qgoodgamechat.h"
#include "../hitbox/qhitboxchat.h"
#include "../igdc/qigdcchat.h"
#include "../livecoding/qlivecodingchat.h"
#include "../peka2tv/qpeka2chat.h"
#include "../streamcube/qstreamcubechat.h"
#include "../twitch/qtwitchchat.h"
#include "../vidi/qvidichat.h"
#include "../youtube/qyoutubechatex.h"


#include "qchatmessage.h"
#include "qchatstatistic.h"

#include "qchatupdateserver.h"

#include "qsettingsdialog.h"

#include "qpollsettingsdialog.h"

#include "settingsconsts.h"


#include "qbrochatwebpage.h"
#include "qbrochatview.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_CC_MSVC
#pragma comment (lib, "user32.lib")
#endif

const int MIN_WINDOW_WIDTH = 196;
const int MIN_WINDOW_HEIGHT = 64;

const int DEFAULT_WINDOW_WIDTH = 256;
const int DEFAULT_WINDOW_HEIGHT = 512;

const int DEFAULT_SCROLL_SPEED = 32;



QBroChatView::QBroChatView( QWidget *parent )
: QWebEngineView( parent )
, acesChat_( new QAcesChat( this ) )
, azubuChat_( new QAzubuChat( this ) )
, beamproChat_( new QBeamProChat( this ) )
, cybergameChat_( new QCyberGameChat( this ) )
, gamerstvChat_( new QGamersTvChat( this ) )
, gipsyteamChat_( new QGipsyTeamChat( this ) )
, goodgameChat_( new QGoodGameChat( this ) )
, hitboxChat_( new QHitBoxChat( this ) )
, igdcChat_( new QIgdcChat( this ) )
, livecodingChat_( new QLivecodingChat( this ) )
, peka2Chat_( new QPeka2Chat( this ) )
, streamcubeChat_( new QStreamCubeChat( this ) )
, twitchChat_( new QTwitchChat( this ) )
, vidiChat_( new QVidiChat( this ) )
, youtubeChat_( new QYoutubeChatEx( this) )
, chatUpdateServer_( 0 )
, settings_()
, moveState_( false )
, mouseStartPos_( 0 ,0 )
, updatePictureId_( -1 )
, updatePictureInterval_( DEFAULT_SAVE_TO_FILE_INTERVAL )
, showSystemMessages_( DEFAULT_SHOW_SYSTEM_MESSAGES )
, showImages_( DEFAULT_SHOW_IMAGES )
, saveToFile_( DEFAULT_SAVE_TO_FILE )
, saveMessagesToLog_( DEFAULT_SAVE_MESSAGES_TO_LOG_FILE )
{

    //setAttribute( Qt::WA_TransparentForMouseEvents, true );
    //setAttribute( Qt::WA_KeyboardFocusChange );

    chats_[ChatTypeEnum::Goodgame] = goodgameChat_;
    chats_[ChatTypeEnum::Youtube] = youtubeChat_;
    chats_[ChatTypeEnum::Twitch] = twitchChat_;
    chats_[ChatTypeEnum::Vidi] = vidiChat_;
    chats_[ChatTypeEnum::Peka2tv] = peka2Chat_;



    settings()->setAttribute( QWebEngineSettings::AutoLoadImages, true );
    settings()->setAttribute( QWebEngineSettings::JavascriptEnabled, true );
    settings()->setAttribute( QWebEngineSettings::JavascriptCanOpenWindows, false );
    settings()->setAttribute( QWebEngineSettings::JavascriptCanAccessClipboard, false );
    settings()->setAttribute( QWebEngineSettings::LinksIncludedInFocusChain, false );
    settings()->setAttribute( QWebEngineSettings::LocalStorageEnabled, false );
    settings()->setAttribute( QWebEngineSettings::LocalContentCanAccessRemoteUrls, true );
    settings()->setAttribute( QWebEngineSettings::XSSAuditingEnabled, false );
    settings()->setAttribute( QWebEngineSettings::SpatialNavigationEnabled, false );
    settings()->setAttribute( QWebEngineSettings::LocalContentCanAccessFileUrls, true );
    settings()->setAttribute( QWebEngineSettings::HyperlinkAuditingEnabled, false );
    settings()->setAttribute( QWebEngineSettings::ScrollAnimatorEnabled, false );
    settings()->setAttribute( QWebEngineSettings::ErrorPageEnabled, false );
    settings()->setAttribute( QWebEngineSettings::PluginsEnabled, false );
    settings()->setAttribute( QWebEngineSettings::FullScreenSupportEnabled, false );
    settings()->setAttribute( QWebEngineSettings::ScreenCaptureEnabled, false );
    settings()->setAttribute( QWebEngineSettings::WebGLEnabled, true );
    settings()->setAttribute( QWebEngineSettings::Accelerated2dCanvasEnabled, true );
    settings()->setAttribute( QWebEngineSettings::AutoLoadIconsForPage, false );
    settings()->setAttribute( QWebEngineSettings::TouchIconsEnabled, false );

    setPage( new QBroChatWebPage( this ) );


    page()->setBackgroundColor( QColor( 0, 0, 0, 255 ) );


    setMinimumSize( MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT );

    setContextMenuPolicy( Qt::ActionsContextMenu );

    setStyleSheet("QWebView { background: transparent }");

    //TODO: туду
    //QObject::connect( this, SIGNAL( linkClicked( const QUrl & ) ), this, SLOT( onLinkClicked( const QUrl & ) ) );

    //TODO: туду
    //page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );



    changeShowSystemMessagesState();
    changeShowImagesState();
    changeUseServerState();
    changeSaveToFileState();
    changeSaveMessagesToLogState();

    loadSettings();

    QObject::connect( this, SIGNAL( closeWindow() ), this, SLOT( saveSettings() ) );

    QAction *settingsAction = new QAction( tr( "&Settings..." ), this );
    QAction *exitAction = new QAction( tr( "&Exit" ), this );

    QAction * pollSettingsAction = new QAction( tr( "&Poll Settings..." ), this );

    QAction *reconnectAllAction = new QAction( tr( "Reconnect All Chats" ), this );
    QAction *reconnectAcesAction = new QAction( QIcon( ":/resources/aceslogo.png" ), tr( "Reconnect Aces Chat" ), this );
    QAction *reconnectAzubuAction = new QAction( QIcon( ":/resources/azubulogo.png" ), tr( "Reconnect Azubu Chat" ), this );
    QAction *reconnectBeamproAction = new QAction( QIcon( ":/resources/beamprologo.png" ), tr( "Reconnect Beampro Chat" ), this );
    QAction *reconnectCybergameAction = new QAction( QIcon( ":/resources/cybergamelogo.png" ), tr( "Reconnect Cybergame Chat" ), this );
    QAction *reconnectGamersTvAction = new QAction( QIcon( ":/resources/gamerstvlogo.png" ), tr( "Reconnect Gamerstv Chat" ), this );
    QAction *reconnectGipsyTeamAction = new QAction( QIcon( ":/resources/gipsyteamlogo.png" ), tr( "Reconnect Gipsyteam Chat" ), this );
    QAction *reconnectGoodgameAction = new QAction( QIcon( ":/resources/goodgamelogo.png" ), tr( "Reconnect Goodgame Chat" ), this );
    QAction *reconnectHitboxAction = new QAction( QIcon( ":/resources/hitboxlogo.png" ), tr( "Reconnect Hitbox Chat" ), this );
    QAction *reconnectIgdcAction = new QAction( QIcon( ":/resources/igdclogo.png" ), tr( "Reconnect Igdc Chat" ), this );
    QAction *reconnectLivecodingAction = new QAction( QIcon( ":/resources/livecodinglogo.png" ), tr( "Reconnect Livecoding Chat" ), this );        
    QAction *reconnectPeka2Action = new QAction( QIcon( ":/resources/peka2logo.png" ), tr( "Reconnect Peka2 Chat" ), this );
    QAction *reconnectStreamcubeAction = new QAction( QIcon( ":/resources/streamcubelogo.png" ), tr( "Reconnect Streamcube Chat" ), this );    
    QAction *reconnectTwitchAction = new QAction( QIcon( ":/resources/twitchlogo.png" ), tr( "Reconnect Twitch Chat" ), this );
    QAction *reconnectVidiAction = new QAction( QIcon( ":/resources/vidilogo.png" ), tr( "Reconnect Vidi Chat" ), this );
    QAction *reconnectYoutubeAction = new QAction( QIcon( ":/resources/youtubelogo.png" ), tr( "Reconnect Youtube Chat" ), this );

    QObject::connect( settingsAction, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
    QObject::connect( exitAction, SIGNAL( triggered() ), this, SIGNAL( closeWindow() ) );

    QObject::connect( pollSettingsAction, SIGNAL( triggered() ), this, SLOT( showPollSettings() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), azubuChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), beamproChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), cybergameChat_, SLOT( reconnect() ) );    
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), gipsyteamChat_, SLOT( reconnect() ) );    
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), igdcChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), livecodingChat_, SLOT( reconnect() ) );        
    QObject::connect( reconnectAllAction, SIGNAL( triggered() ), streamcubeChat_, SLOT( reconnect() ) ); 


    QObject::connect( reconnectAcesAction, SIGNAL( triggered() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAzubuAction, SIGNAL( triggered() ), azubuChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectAzubuAction, SIGNAL( triggered() ), beamproChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectCybergameAction, SIGNAL( triggered() ), cybergameChat_, SLOT( reconnect() ) );    
    QObject::connect( reconnectGamersTvAction, SIGNAL( triggered() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectGipsyTeamAction, SIGNAL( triggered() ), gipsyteamChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectGoodgameAction, SIGNAL( triggered() ), goodgameChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectHitboxAction, SIGNAL( triggered() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectIgdcAction, SIGNAL( triggered() ), igdcChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectLivecodingAction, SIGNAL( triggered() ), livecodingChat_, SLOT( reconnect() ) );        
    QObject::connect( reconnectPeka2Action, SIGNAL( triggered() ), peka2Chat_, SLOT( reconnect() ) );
    QObject::connect( reconnectStreamcubeAction, SIGNAL( triggered() ), streamcubeChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectTwitchAction, SIGNAL( triggered() ), twitchChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectVidiAction, SIGNAL( triggered() ), vidiChat_, SLOT( reconnect() ) );
    QObject::connect( reconnectYoutubeAction, SIGNAL( triggered() ), youtubeChat_, SLOT( reconnect() ) );


    connectChatsToReconnectAllSignal(reconnectAllAction);

    addAction( settingsAction );
    addAction( pollSettingsAction );

    addAction( reconnectAllAction );
    addAction( reconnectAcesAction );
    addAction( reconnectAzubuAction );
    addAction( reconnectBeamproAction );
    addAction( reconnectCybergameAction );    
    addAction( reconnectGamersTvAction );
    addAction( reconnectGipsyTeamAction );
    addAction( reconnectGoodgameAction );
    addAction( reconnectHitboxAction );
    addAction( reconnectIgdcAction );
    addAction( reconnectLivecodingAction );        
    addAction( reconnectPeka2Action );
    addAction( reconnectStreamcubeAction );
    addAction( reconnectTwitchAction );
    addAction( reconnectVidiAction );
    addAction( reconnectYoutubeAction );


    addAction( exitAction );       

    QObject::connect( acesChat_, SIGNAL( message( QJsonObject, const QChatService* ) ), this, SLOT( onNewMessage( QJsonObject, const QChatService * ) ) );
    QObject::connect( acesChat_, SIGNAL( newMessage ( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );            
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), acesChat_, SLOT( reconnect() ) );

    QObject::connect( azubuChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( azubuChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), azubuChat_, SLOT( reconnect() ) );

    QObject::connect( beamproChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( beamproChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), beamproChat_, SLOT( reconnect() ) );

    QObject::connect( cybergameChat_, SIGNAL( newMessage ( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( cybergameChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), cybergameChat_, SLOT( reconnect() ) );

    QObject::connect( gamerstvChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( gamerstvChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), gamerstvChat_, SLOT( reconnect() ) );

    QObject::connect( gipsyteamChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), gipsyteamChat_, SLOT( reconnect() ) );


    QObject::connect( hitboxChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( hitboxChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), hitboxChat_, SLOT( reconnect() ) );

    QObject::connect( igdcChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( igdcChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), igdcChat_, SLOT( reconnect() ) );

    QObject::connect( livecodingChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    QObject::connect( livecodingChat_, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), livecodingChat_, SLOT( reconnect() ) );

    QObject::connect( streamcubeChat_, SIGNAL( newMessage( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), streamcubeChat_, SLOT( reconnect() ) );  


    connectChatsToLocalSlots();


    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), this, SLOT( loadFlagsAndAttributes() ) );

    loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), this, SLOT( testLoadFinished(bool) ) );


    //this string should be last! as it will call unsubscribe for each each attached subscriber to loadFinished event!
    //It should work because docs says:
    //http://doc.qt.io/qt-4.8/signalsandslots.html#signals
    //"If several slots are connected to one signal, the slots will be executed one after the other, in the order they have been connected, when the signal is emitted."
    QObject::connect(this, SIGNAL( loadFinished( bool ) ), this, SLOT( signalLoadFinishedCatched()));


    //setMouseTracking( true );
}

QBroChatView::~QBroChatView()
{
    if( saveMessagesToLog_ )
    {
        QString logDir = qApp->applicationDirPath();

        //if( !messagesManager_.save( logDir + QDir::separator() + "logs" + QDir::separator() ) )
        if( !messagesManager_.save( logDir + "/logs/" ) )
        {
            QMessageBox::warning( this, "Error", "Can not save messages log"  );
        }
    }
}

void QBroChatView::signalLoadFinishedCatched()
{
    for (auto& conn: loadFinishedConnections_)
    {
        QObject::disconnect(conn);
    }
    loadFinishedConnections_.clear();
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
        QWebEngineView::mousePressEvent( event );
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
        QWebEngineView::mouseMoveEvent( event );
    }
    event->accept();
}

void QBroChatView::mouseReleaseEvent( QMouseEvent *event )
{
    if( moveState_ && ( event->modifiers() & Qt::ControlModifier ) && ( event->buttons() & Qt::LeftButton ) )
        moveState_ = false;
    else
    {
        QWebEngineView::mouseReleaseEvent( event );
    }
    event->accept();
}

/*
void QBroChatView::keyPressEvent( QKeyEvent *event )
{
    qDebug() << "keyPressEvent";
    if( event->key() == Qt::Key_Control )
    {
        qDebug() << "keyPressEvent - false";
        setAttribute( Qt::WA_TransparentForMouseEvents, false );
    }

    QWebView::keyPressEvent( event );

    event->accept();
}


void QBroChatView::keyReleaseEvent( QKeyEvent * event )
{
    qDebug() << "keyReleaseEvent";
    if( event->key() == Qt::Key_Control )
    {
        qDebug() << "keyReleaseEvent - true";
        setAttribute( Qt::WA_TransparentForMouseEvents, true );
    }

    QWebView::keyReleaseEvent( event );

    event->accept();
}
*/

void QBroChatView::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == updatePictureId_ && saveToFile_ )
    {
        QImage image( page()->view()->size(), QImage::Format_ARGB32 );
        image.fill( Qt::transparent );
        QPainter painter( &image );
        page()->view()->render( &painter );
        painter.end();
        image.save( QApplication::applicationDirPath() + "/output.png", "PNG" );
    }
}

void QBroChatView::onLinkClicked( const QUrl &url )
{
    QDesktopServices::openUrl( url );
}

void QBroChatView::addMessage( const QString & service, const QString & nickName, const QString & message, const QString & type )
{
    QString messageWithLinks = ChatMessage::insertLinks( message, showImages_ );

    QString formattedNickName = nickName;
    formattedNickName.replace( "\"", "\\\"" );
    formattedNickName.replace( "\'", "\\\'" );

    QString formattedMessage = messageWithLinks;
    formattedMessage.replace( 1, "" );
    formattedMessage.replace( "\\", "\\\\" );
    formattedMessage.replace( "\"", "\\\"" );

    QString js = "onNewMessage( \"" + service + "\", \"" + formattedNickName + "\", \"" + formattedMessage + "\", \"" + type +"\");";
    //page()->mainFrame()->evaluateJavaScript( js );
    page()->runJavaScript( js );

    if( chatUpdateServer_ )
    {
        QString jsonMessage ="{\"type\":\"message\",\"message\":{\"service\":\"" + service + "\",\"nick\":\"" + formattedNickName + "\",\"message\":\"" + formattedMessage + "\",\"type\":\"" + type + "\"}}";
        chatUpdateServer_->sendMessage( jsonMessage );
    }
}

void QBroChatView::changeStyle( const QString & styleName )
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
            //const double ALPHA_DIVIDER = 255;
            //QColor color;
            //QString colorString;
            style.replace( "%FONT_NAME%", "'" + settings_.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() + "'" );

            replaceColorString( style, "%BODY_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_BODY_BACKGROUND_COLOR, GENERATED_STYLE_BODY_BACKGROUND_COLOR_SETTING_PATH );

            replaceColorString( style, "%EVEN_MESSAGES_NICKNAMES_COLOR%", DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
            replaceColorString( style, "%ODD_MESSAGES_NICKNAMES_COLOR%", DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
            replaceColorString( style, "%ALIASES_MESSAGES_NICKNAMES_COLOR%", DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
            replaceColorString( style, "%SUPPORTERS_MESSAGES_NICKNAMES_COLOR%", DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
            replaceColorString( style, "%BLACK_LIST_MESSAGES_NICKNAMES_COLOR%", DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );


            style.replace( "%EVEN_MESSAGES_NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%ODD_MESSAGES_NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%ALIASES_MESSAGES_NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%SUPPORTERS_MESSAGES_NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%BLACK_LIST_MESSAGES_NICKNAMES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() ) + "px" );


            replaceColorString( style, "%EVEN_MESSAGES_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_EVEN_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
            replaceColorString( style, "%ODD_MESSAGES_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_ODD_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
            replaceColorString( style, "%ALIASES_MESSAGES_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
            replaceColorString( style, "%SUPPORTERS_MESSAGES_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
            replaceColorString( style, "%BLACK_LIST_MESSAGES_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );


            replaceColorString( style, "%EVEN_MESSAGES_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_EVEN_MESSAGES_TEXT_COLOR_SETTING_PATH );
            replaceColorString( style, "%ODD_MESSAGES_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_ODD_MESSAGES_TEXT_COLOR_SETTING_PATH );
            replaceColorString( style, "%ALIASES_MESSAGES_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_TEXT_COLOR_SETTING_PATH );
            replaceColorString( style, "%SUPPORTERS_MESSAGES_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_TEXT_COLOR_SETTING_PATH );
            replaceColorString( style, "%BLACK_LIST_MESSAGES_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_TEXT_COLOR_SETTING_PATH );


            style.replace( "%EVEN_MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_EVEN_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%ODD_MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_ODD_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%ALIASES_MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_ALIASES_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%SUPPORTERS_MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );
            style.replace( "%BLACK_LIST_MESSAGES_FONT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() ) + "px" );


            replaceColorString( style, "%EVEN_MESSAGES_BORDER_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_EVEN_MESSAGES_BORDER_COLOR_SETTING_PATH );
            replaceColorString( style, "%ODD_MESSAGES_BORDER_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_ODD_MESSAGES_BORDER_COLOR_SETTING_PATH );
            replaceColorString( style, "%ALIASES_MESSAGES_BORDER_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_BORDER_COLOR_SETTING_PATH );
            replaceColorString( style, "%SUPPORTERS_MESSAGES_BORDER_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_BORDER_COLOR_SETTING_PATH );
            replaceColorString( style, "%BLACK_LIST_MESSAGES_BORDER_COLOR%", DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_BORDER_COLOR_SETTING_PATH );


            style.replace( "%MESSAGES_LEFT_BORDER_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_LEFT_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGES_RIGHT_BORDER_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_RIGHT_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGES_TOP_BORDER_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_TOP_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGES_BOTTOM_BORDER_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_BOTTOM_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() ) + "px" );


            style.replace( "%MESSAGEBOX_TOP_LEFT_BORDER_RADIUS_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_TOP_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGEBOX_TOP_RIGHT_BORDER_RADIUS_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_TOP_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGEBOX_BOTTOM_LEFT_BORDER_RADIUS_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_BOTTOM_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() ) + "px" );
            style.replace( "%MESSAGEBOX_BOTTOM_RIGHT_BORDER_RADIUS_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_BOTTOM_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() ) + "px" );


            style.replace( "%MESSAGESBOX_MARGIN_BOTTOM%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE ).toInt() ) + "px" );

            style.replace( "%MESSAGES_INDENT_SIZE%", QString::number( settings_.value( GENERATED_STYLE_MESSAGES_INDENT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_INDENT_SIZE ).toInt() ) + "px" );


            int viewersFontSize = settings_.value( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE ).toInt();
            style.replace( "%VIEWERS_FONT_SIZE%", QString::number( viewersFontSize ) + "px" );


            replaceColorString( style, "%VIEWERS_TEXT_COLOR%", DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR, GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH );
            replaceColorString( style, "%VIEWERS_BACKGROUND_COLOR%", DEFAULT_GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR, GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR_SETTING_PATH );
            replaceColorString( style, "%LINKS_COLOR%", DEFAULT_GENERATED_STYLE_LINKS_COLOR, GENERATED_STYLE_LINKS_COLOR_SETTING_PATH );


            style.replace( "%SMILES_SIZE%", QString::number( settings_.value( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SMILES_SIZE ).toInt() ) + "px" );

            int serviceIconsSize = settings_.value( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE ).toInt();
            style.replace( "%SERVICE_ICONS_SIZE%", QString::number( serviceIconsSize ) + "px" );

            style.replace( "%MAX_IMAGES_HEIGHT%", QString::number( settings_.value( GENERATED_STYLE_MAX_IMAGES_HEIGHT_SETTING_PATH, DEFAULT_GENERATED_STYLE_MAX_IMAGES_HEIGHT ).toInt() ) + "px" );

            style.replace( "%ANIMATION_TYPE%", settings_.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE).toString() + "Anim" );

            style.replace( "%ANIMATION_DURATION%", QString::number( settings_.value( GENERATED_STYLE_ANIMATION_DURATION_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_DURATION ).toDouble(), 'f' ) + "s" );

            int viewersHeight = ( viewersFontSize > serviceIconsSize ) ? viewersFontSize : serviceIconsSize;

            viewersHeight += 12;

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
    azubuChat_->setShowSystemMessages( showSystemMessages_ );
    beamproChat_->setShowSystemMessages( showSystemMessages_ );
    cybergameChat_->setShowSystemMessages( showSystemMessages_ );    
    gamerstvChat_->setShowSystemMessages( showSystemMessages_ );
    gipsyteamChat_->setShowSystemMessages( showSystemMessages_ );
    hitboxChat_->setShowSystemMessages( showSystemMessages_ );
    igdcChat_->setShowSystemMessages( showSystemMessages_ );
    livecodingChat_->setShowSystemMessages( showSystemMessages_ );        
    streamcubeChat_->setShowSystemMessages( showSystemMessages_ );


    for(auto& chat:chats_)
    {
        chat->setShowSystemMessages(showSystemMessages_);
    }

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
        if( -1 == updatePictureId_ )
            updatePictureId_ = startTimer( updatePictureInterval_ );
    }
}

void QBroChatView::changeSaveMessagesToLogState()
{
    saveMessagesToLog_ = settings_.value( SAVE_MESSAGES_TO_LOG_FILE_SETTING_PATH, DEFAULT_SAVE_MESSAGES_TO_LOG_FILE ).toBool();
}

void QBroChatView::slotNewMessage( ChatMessage message )
{
    const QChatService * chatService = message.service();

    bool blackListUser = chatService->blackList().contains( message.nickName() );
    bool supportersListUser = chatService->supportersList().contains( message.nickName() );
    bool containsAliases = chatService->isContainsAliases( message.message() );

    if( blackListUser && chatService->isRemoveBlackListUsers() )
        return;


    //if( !chatService->isRemoveBlackListUsers() || !blackListUser )
    //{
    if( blackListUser )
    {
        message.setType( "ignore" );
    }
    else if( supportersListUser )
    {
        message.setType( "supporter" );
    }
    else if( containsAliases )
    {
        message.setType( "alias" );
    }
    else
    {
        message.setType( "" );
    }
    //}

    messagesManager_.add( message );

    addMessage( message.serviceName(), message.nickName(), message.message(), message.type() );
}

void QBroChatView::onNewStatistic( QChatStatistic *statistic )
{
    QString js = "onStatisticReceived( \"" + statistic->service() + "\", \"" + statistic->statistic() + "\");";

    //page()->mainFrame()->evaluateJavaScript( js );
    page()->runJavaScript( js );

    if( chatUpdateServer_ )
    {
        //QString jsonMessage ="{\"service\":\"" + service + "\",\"nick\":\"" + formattedNickName + "\",\"message\":\"" + formattedMessage + "\",\"type\":\"" + type + "\"}";

        QString jsonMessage ="{\"type\":\"statistic\",\"statistic\":{\"service\":\"" + statistic->service() + "\",\"statistic\":\"" + statistic->statistic() + "\"}}";
        chatUpdateServer_->sendMessage( jsonMessage );
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

    connectDialogsToChats(settingsDialog);


    QObject::connect( settingsDialog, SIGNAL( acesChannelChanged() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( azubuChannelChanged() ), azubuChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( beamproChannelChanged() ), beamproChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameChannelChanged() ), cybergameChat_, SLOT( reconnect() ) );    
    QObject::connect( settingsDialog, SIGNAL( gamerstvChannelChanged() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamChannelChanged() ), gipsyteamChat_, SLOT( reconnect() ) );    
    QObject::connect( settingsDialog, SIGNAL( hitboxChannelChanged() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( igdcChannelChanged() ), igdcChat_, SLOT( reconnect() ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingChannelChanged() ), livecodingChat_, SLOT( reconnect() ) );            
    QObject::connect( settingsDialog, SIGNAL( streamcubeChannelChanged() ), streamcubeChat_, SLOT( reconnect() ) );       


    QObject::connect( settingsDialog, SIGNAL( acesAliasesChanged( QString ) ), acesChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( azubuAliasesChanged( QString ) ), azubuChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( beamproAliasesChanged( QString ) ), beamproChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameAliasesChanged( QString ) ), cybergameChat_, SLOT( setAliasesList( QString ) ) );    
    QObject::connect( settingsDialog, SIGNAL( gamerstvAliasesChanged(QString) ), gamerstvChat_, SLOT( setAliasesList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamAliasesChanged(QString) ), gipsyteamChat_, SLOT( setAliasesList(QString) ) );    
    QObject::connect( settingsDialog, SIGNAL( hitboxAliasesChanged( QString ) ), hitboxChat_, SLOT( setAliasesList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( igdcAliasesChanged(QString) ), igdcChat_, SLOT( setAliasesList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingAliasesChanged( QString ) ), livecodingChat_, SLOT( setAliasesList( QString ) ) );        
    QObject::connect( settingsDialog, SIGNAL( streamcubeAliasesChanged( QString ) ), streamcubeChat_, SLOT( setAliasesList( QString ) ) );   

    QObject::connect( settingsDialog, SIGNAL( acesSupportersListChanged( QString ) ), acesChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( azubuSupportersListChanged( QString ) ), azubuChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( beamproSupportersListChanged( QString ) ), beamproChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameSupportersListChanged( QString ) ), cybergameChat_, SLOT( setSupportersList( QString ) ) );    
    QObject::connect( settingsDialog, SIGNAL( gamerstvSupportersListChanged(QString) ), gamerstvChat_, SLOT( setSupportersList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamSupportersListChanged(QString) ), gipsyteamChat_, SLOT( setSupportersList(QString) ) );   
    QObject::connect( settingsDialog, SIGNAL( hitboxSupportersListChanged( QString ) ), hitboxChat_, SLOT( setSupportersList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( igdcSupportersListChanged(QString) ), igdcChat_, SLOT( setSupportersList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingSupportersListChanged( QString ) ), livecodingChat_, SLOT( setSupportersList( QString ) ) );            
    QObject::connect( settingsDialog, SIGNAL( streamcubeSupportersListChanged( QString ) ), streamcubeChat_, SLOT( setSupportersList( QString ) ) );       


    QObject::connect( settingsDialog, SIGNAL( acesBlackListChanged( QString ) ), acesChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( azubuBlackListChanged( QString ) ), azubuChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( beamproBlackListChanged( QString ) ), beamproChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameBlackListChanged( QString ) ), cybergameChat_, SLOT( setBlackList( QString ) ) );    
    QObject::connect( settingsDialog, SIGNAL( gamerstvBlackListChanged(QString) ), gamerstvChat_, SLOT( setBlackList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamBlackListChanged(QString) ), gipsyteamChat_, SLOT( setBlackList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxBlackListChanged( QString ) ), hitboxChat_, SLOT( setBlackList( QString ) ) );
    QObject::connect( settingsDialog, SIGNAL( igdcBlackListChanged(QString) ), igdcChat_, SLOT( setBlackList(QString) ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingBlackListChanged( QString ) ), livecodingChat_, SLOT( setBlackList( QString ) ) );            
    QObject::connect( settingsDialog, SIGNAL( streamcubeBlackListChanged( QString ) ), streamcubeChat_, SLOT( setBlackList( QString ) ) );        


    QObject::connect( settingsDialog, SIGNAL( acesRemoveBlackListUsersChanged( bool ) ), acesChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( azubuRemoveBlackListUsersChanged( bool ) ), azubuChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( beamproRemoveBlackListUsersChanged( bool ) ), beamproChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( cyberGameRemoveBlackListUsersChanged( bool ) ), cybergameChat_, SLOT( setRemoveBlackListUsers(bool ) ) );    
    QObject::connect( settingsDialog, SIGNAL( gamerstvRemoveBlackListUsersChanged(bool) ), gamerstvChat_, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( gipsyteamRemoveBlackListUsersChanged(bool) ), gipsyteamChat_, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( hitboxRemoveBlackListUsersChanged( bool ) ), hitboxChat_, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( settingsDialog, SIGNAL( igdcRemoveBlackListUsersChanged(bool) ), igdcChat_, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingRemoveBlackListUsersChanged( bool ) ), livecodingChat_, SLOT( setRemoveBlackListUsers(bool ) ) );            
    QObject::connect( settingsDialog, SIGNAL( streamcubeRemoveBlackListUsersChanged( bool ) ), streamcubeChat_, SLOT( setRemoveBlackListUsers(bool ) ) );        


    //QObject::connect( settingsDialog, SIGNAL( acesOriginalColorsChanged( bool ) ), acesChat_, SLOT( changeOriginalColors( bool ) ) );    
    QObject::connect( settingsDialog, SIGNAL( hitboxOriginalColorsChanged( bool ) ), hitboxChat_, SLOT( changeOriginalColors( bool ) ) );    
    QObject::connect( settingsDialog, SIGNAL( peka2OriginalColorsChanged( bool ) ), peka2Chat_, SLOT( changeOriginalColors( bool ) ) );    

    //badges    
    QObject::connect( settingsDialog, SIGNAL( gamerstvBadgesChanged(bool) ), gamerstvChat_, SLOT( changeBadges(bool) ) );    
    QObject::connect( settingsDialog, SIGNAL( igdcBadgesChanged(bool) ), igdcChat_, SLOT( changeBadges(bool) ) );
    QObject::connect( settingsDialog, SIGNAL( livecodingBadgesChanged(bool) ), livecodingChat_, SLOT( changeBadges(bool) ) );        


    QObject::connect( settingsDialog, SIGNAL( styleChanged() ), this, SLOT( changeStyle() ) );
    QObject::connect( settingsDialog, SIGNAL( opacityChanged() ), this, SLOT( changeOpacity() ) );
    QObject::connect( settingsDialog, SIGNAL( flagsOrAttributesChanged() ), this, SLOT( loadFlagsAndAttributes() ) );

    QObject::connect( settingsDialog, SIGNAL( showSystemMessagesChanged() ), this, SLOT( changeShowSystemMessagesState() ) );

    QObject::connect( settingsDialog, SIGNAL( showImagesChanged() ), this, SLOT( changeShowImagesState() ) );
    QObject::connect( settingsDialog, SIGNAL( useServerStateChanged() ), this, SLOT( changeUseServerState() ) );
    QObject::connect( settingsDialog, SIGNAL( saveToFileStateChanged() ), this, SLOT( changeSaveToFileState() ) );
    QObject::connect( settingsDialog, SIGNAL( saveMessagesToLogChanged() ), this, SLOT( changeSaveMessagesToLogState() ) );   

    settingsDialog->exec();

    settingsDialog->deleteLater();
}

void QBroChatView::showPollSettings()
{
    QPollSettingsDialog * pollSettingsDialog = new QPollSettingsDialog( this );

    pollSettingsDialog->exec();

    pollSettingsDialog->deleteLater();
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
        ::SetWindowPos( reinterpret_cast< HWND >( winId() ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
    }
    else
    {
        ::SetWindowPos( reinterpret_cast< HWND >( winId() ), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
    }

#endif
}


void QBroChatView::replaceColorString( QString & str, const QString & colorTemplate, unsigned int defaultColor, const QString & settingPath )
{
    const double ALPHA_DIVIDER = 255.0;
    QColor color = QColor::fromRgba( settings_.value( settingPath, defaultColor ).toUInt() );
    QString colorString = "rgba(" + QString::number( color.red() ) + "," + QString::number( color.green() ) + "," + QString::number( color.blue() ) + "," + QString::number( color.alpha() / ALPHA_DIVIDER, 'f' ) + ")";
    str.replace( colorTemplate, colorString );
}

void QBroChatView::connectChatsToReconnectAllSignal(QAction *reconnectAllAction)
{
   for(auto& chat:chats_.values())
   {
        QObject::connect( reconnectAllAction, SIGNAL( triggered() ), chat, SLOT( reconnect() ) );
   }
}

void QBroChatView::connectChatsToLocalSlots()
{
    for(auto& chat:chats_.values())
    {
        QObject::connect( chat, SIGNAL( newMessage ( ChatMessage ) ), this, SLOT( slotNewMessage( ChatMessage ) ) );
        QObject::connect( chat, SIGNAL( newStatistic( QChatStatistic* ) ), this, SLOT( onNewStatistic( QChatStatistic* ) ) );
        loadFinishedConnections_ << QObject::connect( this, SIGNAL( loadFinished( bool ) ), chat, SLOT( reconnect() ) );
    }
}

void QBroChatView::connectDialogsToChats(QSettingsDialog *dialog)
{
    for(auto& chatKey:chats_.keys())
    {
      dialog->connectDialogToChat(chatKey,chats_[chatKey]);
    }
}

void QBroChatView::onNewMessage( QJsonObject json, const QChatService * )
{
    QJsonDocument jsonDoc( json );

    QByteArray jsonData = jsonDoc.toJson();

    qDebug() << "onNewmMessage:" << jsonData;

}


void QBroChatView::testLoadFinished( bool test )
{
    qDebug() << "testLoadFinished:" << test;
}



