#include <QDebug>
#include <QStringList>

#include "qchatservice.h"

#include "qchatmessage.h"

ChatMessage::ChatMessage( const QString & serviceName, const QString & nickName, const QString & message, const QString & type, QChatService * service )
: serviceName_( serviceName )
, nickName_( nickName )
, message_( message )
, type_( type )
, service_( service )
{
}

ChatMessage::ChatMessage( const ChatMessage & chatMessage )
: serviceName_( chatMessage.serviceName_ )
, nickName_( chatMessage.nickName_ )
, message_( chatMessage.message_ )
, type_( chatMessage.type_ )
, service_( chatMessage.service_ )
{
}

ChatMessage::~ChatMessage()
{
}

const ChatMessage & ChatMessage::operator=( const ChatMessage & chatMessage  )
{
    serviceName_ = chatMessage.serviceName_;
    nickName_ = chatMessage.nickName_;
    message_ = chatMessage.message_;
    type_ = chatMessage.type_;
    service_ = chatMessage.service_;

    return * this;
}

QString ChatMessage::serviceName() const
{
    return serviceName_;
}

void ChatMessage::setServiceName( const QString & serviceName )
{
    serviceName_ = serviceName;
}

QString ChatMessage::nickName() const
{
    return nickName_;
}

void ChatMessage::setNickName( const QString& nickName )
{
    nickName_ = nickName;
}

QString ChatMessage::message() const
{
    return message_;
}

void ChatMessage::setMessage( const QString &message )
{
    message_ = message;
}

QString ChatMessage::type() const
{
    return type_;
}

void ChatMessage::setType( const QString &type )
{
    type_ = type;
}

const QChatService * ChatMessage::service() const
{
    return service_;
}

void ChatMessage::setService( QChatService * service )
{
    service_ = service;
}


QString ChatMessage::insertLinks( const QString & message, bool convertImageLinks )
{
    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( isLink( tokens.at( i ) ) )
        {
            //TODO: Try repair links
            //convertedTokens[ i ] = "<a href=\\\'" + tokens.at( i ).toLower() + "\\\'>link</a>";
            if( convertImageLinks && ( ".svg" == tokens.at( i ).right(4).toLower() || ".png" == tokens.at( i ).right(4).toLower() || tokens.at( i ).right( 4 ).toLower() == ".jpg" || ".gif" == tokens.at( i ).right(4).toLower() || ".jpeg" == tokens.at( i ).right( 5 ).toLower() ) )
            {
                //convertedTokens[ i ] = "<a href=\\\'" + tokens.at( i ) + "\\\'><img src=\\\'" + tokens.at( i ) + "\\\'><></a>";
                //convertedTokens[ i ] = "<img src=\\\'" + tokens.at( i ) + "\\\' width=100%></img>";
                convertedTokens[ i ] = "<a href=\"" + tokens.at( i ) + "\"><img class=\"picture\" src=\"" + tokens.at( i ) + "\"></img></a>";
            }
            /*
            else if( convertImageLinks && ".gifv" == tokens.at( i ).right( 5 ).toLower() )
            {
                //class=\"picture\"

                QString test( tokens.at( i ) );

                test.replace( ".gifv", ".webm" );

                convertedTokens[ i ] = "<video preload=\"auto\" autoplay=\"autoplay\" loop=\"loop\"><source src=\"" + tokens.at( i ) + "\" type=\"video/webm\"></source></video>";

            }
            else if( convertImageLinks && "http://coub.com/view/" == tokens.at( i ).left( 21 ) )
            {
                //convertedTokens[ i ] = "<iframe src=\"" + tokens.at( i ) + "?muted=true&autostart=true&originalSize=false&hideTopBar=true&startWithHD=false\" allowfullscreen=\"false\" class = \"picture\"></iframe>";
            }
            */
            else
            {
                //convertedTokens[ i ] = "<a href=\"" + tokens.at( i ) + "\"title=\"" + tokens.at( i ) + "\">link</a>";

                convertedTokens[ i ] = "<a href=\"" + tokens.at( i ) + "\"title=\"" + tokens.at( i ) + "\">" + tokens.at( i ) + "</a>";
            }
        }
    }

    int startPos = 0;

    for( int i = 0; i < tokens.size(); ++i )
    {

        //convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );

        int pos = convertedMessage.indexOf( tokens.at( i ), startPos );
        convertedMessage.remove( pos, tokens.at( i ).length() );
        convertedMessage.insert( pos, convertedTokens.at( i ) );
        startPos = pos + convertedTokens.at( i ).length();

    }

    //qDebug() << convertedMessage;

    return convertedMessage;
}

bool ChatMessage::isLink( const QString& link )
{
    QRegExp urlRegExp(
                        "((?:(?:ht|f)tps?)(?:://))"
                        "(((?:(?:[a-z\\u0430-\\u0451\\d](?:[a-z\\u0430-\\u0451\\d-]*[a-z\\u0430-\\u0451\\d])*)\\.)+(?:[a-z]{2,}|\\u0440\\u0444)"
                        "|(?:(?:\\d{1,3}\\.){3}\\d{1,3}))"
                        "(:\\d+)?"
                        "(/[-a-z\\u0430-\\u0451\\d%_~\\+\\(\\):]*(?:[\\.,][-a-z\\u0430-\\u0451\\d%_~\\+\\(\\):]+)*)*"
                        "(\\?(?:&amp;|&quot;|&#039|[&\"'.:;a-z\\u0430-\\u0451\\d%_~\\+=-])*)?"
                        "(#(?:&amp;|&quot;|&#039|[*!()/&\"'.:;a-z\\u0430-\\u0451\\d%_~\\+=-])*)?)"
                    );



    bool retValue = urlRegExp.exactMatch( link );

    return retValue;
}

QString ChatMessage::replaceEscapeCharecters( const QString &message )
{
    QString convertedMessage = message;

    convertedMessage.replace( '<', "&lt;" );
    convertedMessage.replace( '<', "&gt;" );
    //convertedMessage.replace( '\'', "&#39;" );
    //convertedMessage.replace( '\\', "&#92;" );
    //convertedMessage.replace( '\\', "\\\\" );
    return convertedMessage;
}

QString ChatMessage::deleteTags( const QString &message  )
{
    QString convertedMessage = message;

    convertedMessage.remove( QRegExp( "<[^>]*>" ) );

    //convertedMessage.remove( QRegExp( "<(/?)a\\a((?:[^>\"\']|\"[^\"]*\"|\'[^\']*\')*)>" ) );

    //qDebug() << convertedMessage;

    return convertedMessage;
}
