#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include "qyoutubechatex.h"
#include "../core/notimplementedexception.h"
#include "../core/settingsconsts.h"
#include "../core/qchatmessage.h"
#include "youtubeapi.h"
#include "o2youtube.h"

const QString YOUTUBE_SERVICE = "youtube";
const QString YOUTUBE_USER = "youtube";

QYoutubeChatEx::QYoutubeChatEx( QObject * parent )
    : QChatService( parent )
    , nam_( new QNetworkAccessManager( this ) )
    , youtubeController_(new youtube::YoutubeController(this))
   /*
    , lastMessageTime_()
    , updateMessagesTimerId_( -1 )
    , updateMessagesInterval_( DEFAULT_YOUTUBE_UPDATE_MESSAGES_INTERVAL )
    , updateStatisticTimerId_( -1 )
    , updateStatisticInterval_( DEFAULT_YOUTUBE_UPDATE_STATISTIC_INTERVAL )
    , reconnectTimerId_( -1 )
    , reconnectInterval_( DEFAULT_YOUTUBE_RECONNECT_INTERVAL )
        */
    {
    QObject::connect( youtubeController_, SIGNAL( error(const QString ) ), this, SLOT( error(const QString ) ) );
    QObject::connect( youtubeController_, SIGNAL( message(const QString ,const QString ,const QString ,
                                                          bool , bool ,bool) ),
                      this, SLOT( message(const QString ,const QString ,const QString ,
                                          bool , bool ,bool) ) );
    QObject::connect( youtubeController_, SIGNAL( fundraising(const QString ,const QString ,const QString ,
                                                              ulong ,QString ,QString) ),
            this, SLOT( fundraising(const QString ,const QString ,const QString ,
                                    ulong ,QString ,QString) ) );
    QObject::connect( youtubeController_, SIGNAL( newSponsor(const QString ) ), this, SLOT( newSponsor(const QString ) ) );
    }

QYoutubeChatEx::~QYoutubeChatEx()
{   
   //YoutubeApi y = new O2Facebook();
    disconnect();
}

void QYoutubeChatEx::connect()
{
   if(!youtubeChatProviderEnabled())
       return;
   youtubeController_->doAuth();
}

void QYoutubeChatEx::reconnect()
{
   if(!youtubeChatProviderEnabled())
     return;
   youtubeController_->delogin();
   youtubeController_->doAuth();
}

bool QYoutubeChatEx::youtubeChatProviderEnabled()
{
    QSettings settings;
    return (settings.value( YOUTUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool());
}

void QYoutubeChatEx::disconnect()
{
   //youtubeController_->delogin();
}

//#yotube signals region
void QYoutubeChatEx::error(const QString errorMessage)
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( YOUTUBE_SERVICE, YOUTUBE_USER, "Error: " + errorMessage , "", this ) );
}

void QYoutubeChatEx::message(const QString message, const QString authorName, const QString authorImageLogo, bool isChatModerator, bool isChatOwner, bool isChatSponsor)
{
    bool blackListUser = blackList().contains( authorName );
    bool supportersListUser = supportersList().contains( authorName );

    QString userType = "";

    if (blackListUser)
        userType = "ignore";
    else
    if (supportersListUser||isChatSponsor)
        userType = "supporter";

    emit newMessage( ChatMessage( YOUTUBE_SERVICE, authorName, message , userType, this ) );
}

void QYoutubeChatEx::fundraising(const QString userComment, const QString authorName, const QString authorImageLogo, ulong amountMicros, QString currency, QString amountDisplayString)
{

}

void QYoutubeChatEx::newSponsor(const QString authorName)
{

}

void QYoutubeChatEx::doLogin()
{
    youtubeController_->doAuth();
}

void QYoutubeChatEx::doDelogin()
{
    youtubeController_->delogin();
}

void QYoutubeChatEx::doFreshLogin()
{
    doDelogin();
    doLogin();
}

