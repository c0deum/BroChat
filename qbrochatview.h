#ifndef QBROCHATVIEW
#define QBROCHATVIEW

#include <QSettings>

#include <QWebEngineView>

#include <QJsonObject>

#include "qmessagesmanager.h"

class ChatMessage;
class QChatStatistic;

class QChatUpdateServer;

class QAcesChat;
class QAzubuChat;
class QBeamProChat;
class QCyberGameChat;
class QGamersTvChat;
class QGipsyTeamChat;
class QGoodGameChat;
class QHitBoxChat;
class QIgdcChat;
class QLivecodingChat;
class QPeka2Chat;
class QStreamCubeChat;
class QTwitchChat;
class QVidiChat;

class QBroChatView: public QWebEngineView
{
    Q_OBJECT
public:
    explicit            QBroChatView( QWidget * parent = nullptr );
    virtual             ~QBroChatView();

    void                addMessage( const QString & service, const QString & nickName, const QString & message, const QString & type );
protected:
    virtual void        mousePressEvent( QMouseEvent * event );
    virtual void        mouseMoveEvent( QMouseEvent * event );
    virtual void        mouseReleaseEvent( QMouseEvent * event );

    /*
    virtual void        keyPressEvent( QKeyEvent *event );
    virtual void        keyReleaseEvent( QKeyEvent * event );
    */

    virtual void        closeEvent( QCloseEvent * event );

    virtual void        timerEvent( QTimerEvent * event );

private:
    void                loadSettings();
    bool                isLink( const QString & str );

    void                replaceColorString( QString & str, const QString & colorTemplate, unsigned int defaultColor, const QString & settingPath );

public slots:
    void                onNewMessage( QJsonObject json, const QChatService * service );
    void                slotNewMessage( ChatMessage message );
    void                onNewStatistic( QChatStatistic * statistic );
    void                saveSettings();
    void                showSettings();

    void                loadFlagsAndAttributes();

    void                changeStyle( const QString & styleName );

    void                changeShowSystemMessagesState();
    void                changeShowImagesState();
    void                changeUseServerState();
    void                changeSaveToFileState();
    void                changeSaveMessagesToLogState();

private slots:
    void                changeStyle();
    void                changeOpacity();
    void                onLinkClicked( const QUrl & url );

    void                showPollSettings();

    void                testLoadFinished( bool test );
signals:
    void                closeWindow();
private:
    QAcesChat *         acesChat_;
    QAzubuChat *        azubuChat_;
    QBeamProChat *      beamproChat_;
    QCyberGameChat *    cybergameChat_;
    QGamersTvChat *     gamerstvChat_;
    QGipsyTeamChat *    gipsyteamChat_;
    QGoodGameChat *     goodgameChat_;
    QHitBoxChat *       hitboxChat_;
    QIgdcChat *         igdcChat_;
    QLivecodingChat *   livecodingChat_;        
    QPeka2Chat *        peka2Chat_;
    QStreamCubeChat *   streamcubeChat_;
    QTwitchChat *       twitchChat_;    
    QVidiChat *         vidiChat_;

    QChatUpdateServer * chatUpdateServer_;

    QSettings           settings_;

    bool                moveState_;
    QPoint              mouseStartPos_;

    int                 updatePictureId_;
    int                 updatePictureInterval_;

    bool                showSystemMessages_;
    bool                showImages_;
    bool                saveToFile_;

    QMessagesManager    messagesManager_;
    bool                saveMessagesToLog_;
};

#endif // QBROCHATVIEW

