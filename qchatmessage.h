#ifndef ChatMessage_H
#define ChatMessage_H

#include <QString>

class QChatService;

class ChatMessage
{    
public:
                            ChatMessage( const QString & serviceName, const QString & nickName, const QString & message, const QString & type, QChatService * service = nullptr );
                            ChatMessage( const ChatMessage & chatMessage );
    virtual                 ~ChatMessage();
public:
    const ChatMessage &     operator=( const ChatMessage & chatMessage );
public:
    QString                 serviceName() const;
    void                    setServiceName( const QString & service );

    QString                 nickName() const;
    void                    setNickName( const QString & nickName );

    QString                 message() const;
    void                    setMessage( const QString & message );

    QString                 type() const;
    void                    setType( const QString & type );

    const QChatService *    service() const;
    void                    setService( QChatService * service );

    static QString          deleteTags( const QString & message );

    static QString          replaceEscapeCharecters( const QString & message );

    static bool             isLink( const QString & link );
    static QString          insertLinks( const QString & message, bool convertImageLinks = false );
private:
    QString                 serviceName_;
    QString                 nickName_;
    QString                 message_;
    QString                 type_;
    QChatService *          service_;
};

#endif // ChatMessage_H
