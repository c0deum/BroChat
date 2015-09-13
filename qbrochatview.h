#ifndef QBROCHATVIEW
#define QBROCHATVIEW

#include <QSettings>

#include <QWebView>

class QChatMessage;
class QChatStatistic;

//class QChatView;
class QAcesChat;
class QCyberGameChat;
class QGamersTvChat;
class QGoodGameChat;
class QSc2tvChat;
class QTwitchChat;

class QFunStreamChat;
class QStreamBoxChat;
class QHitBoxChat;

class QGipsyTeamChat;

class QReallTvChat;

//class QChatServer;
class QChatUpdateServer;

class QBroChatView: public QWebView
{
    Q_OBJECT
public:
    explicit QBroChatView( QWidget *parent = 0 );
    virtual ~QBroChatView();

    //int scrollSpeed() const;
    //void setScrollSpeed( int scrollSpeed );

    void addMessage( const QString &service, const QString &nickName, const QString &message, const QString &type );
protected:
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent *event );
    virtual void mouseReleaseEvent( QMouseEvent *event );

    //virtual void keyPressEvent( QKeyEvent *event );

    //virtual void wheelEvent( QWheelEvent *event );

    virtual void closeEvent( QCloseEvent *event );

    virtual void timerEvent( QTimerEvent *event );
private:
    void loadSettings();
    bool isLink( const QString& str );
public slots:
    void slotNewMessage( QChatMessage *message );
    void onNewStatistic( QChatStatistic *statistic );
    void saveSettings();
    void showSettings();

    void loadFlagsAndAttributes();

    void changeStyle( const QString& styleName );
    //void scrollBottom( const QSize &size );

    void changeShowSystemMessagesState();
    void changeShowImagesState();
    void changeUseServerState();
    void changeSaveToFileState();


private slots:
    void changeStyle();
    void changeOpacity();
    void onLinkClicked( const QUrl &url );
signals:
    void closeWindow();
private:
    //int scrollSpeed_;
    bool moveState_;
    QPoint mouseStartPos_;
    QSettings settings_;
    QAcesChat *acesChat_;
    QCyberGameChat *cybergameChat_;
    QGamersTvChat *gamerstvChat_;
    QGoodGameChat *goodgameChat_;
    QSc2tvChat *sc2tvChat_;
    QTwitchChat *twitchChat_;

    int updatePictureInterval_;
    int updatePictureId_;

    QFunStreamChat *funstreamChat_;
    QStreamBoxChat *streamboxChat_;

    QHitBoxChat *hitboxChat_;

    QGipsyTeamChat *gipsyteamChat_;

    QReallTvChat *realltvChat_;

    QChatUpdateServer *chatUpdateServer_;

    bool showSystemMessages_;
    bool showImages_;
    bool saveToFile_;

};

#endif // QBROCHATVIEW

