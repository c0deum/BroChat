#include <QDebug>
#include <QStringList>

#include "qchatmessage.h"

QChatMessage::QChatMessage( const QString &service, const QString &nickName, const QString &message, const QString &type, QObject *parent )
: QObject( parent )
, service_( service )
, nickName_( nickName )
, message_( message )
, type_( type )
{
}

QChatMessage::~QChatMessage()
{
}

QString QChatMessage::service() const
{
    return service_;
}

void QChatMessage::setService( const QString& service )
{
    service_ = service;
}

QString QChatMessage::nickName() const
{
    return nickName_;
}

void QChatMessage::setNickName( const QString& nickName )
{
    nickName_ = nickName;
}

QString QChatMessage::message() const
{
    return message_;
}

void QChatMessage::setMessage( const QString &message )
{
    message_ = message;
}

QString QChatMessage::type() const
{
    return type_;
}

void QChatMessage::setType( const QString &type )
{
    type_ = type;
}

QString QChatMessage::insertLinks( const QString& message, bool convertImageLinks )
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
            if( convertImageLinks && ( tokens.at( i ).right(4).toLower() == ".svg" || tokens.at( i ).right(4).toLower() == ".png" || tokens.at( i ).right( 4 ).toLower() == ".jpg" || tokens.at( i ).right(4).toLower() == ".gif" || tokens.at( i ).right( 5 ).toLower() == ".jpeg" ) )
            {
                //convertedTokens[ i ] = "<a href=\\\'" + tokens.at( i ) + "\\\'><img src=\\\'" + tokens.at( i ) + "\\\'><></a>";
                //convertedTokens[ i ] = "<img src=\\\'" + tokens.at( i ) + "\\\' width=100%></img>";
                convertedTokens[ i ] = "<a href=\"" + tokens.at( i ) + "\"><img class=\"picture\" src=\"" + tokens.at( i ) + "\"></img></a>";
            }
            else
            {
                convertedTokens[ i ] = "<a href=\"" + tokens.at( i ) + "\"title=\"" + tokens.at( i ) + "\">link</a>";
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

bool QChatMessage::isLink( const QString& link )
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

QString QChatMessage::replaceEscapeCharecters( const QString &message )
{
    QString convertedMessage = message;

    convertedMessage.replace( '<', "&lt;" );
    convertedMessage.replace( '<', "&gt;" );
    //convertedMessage.replace( '\'', "&#39;" );
    //convertedMessage.replace( '\\', "&#92;" );
    //convertedMessage.replace( '\\', "\\\\" );
    return convertedMessage;
}

QString QChatMessage::deleteTags( const QString &message  )
{
    QString convertedMessage = message;

    convertedMessage.remove( QRegExp( "<[^>]*>" ) );

    //convertedMessage.remove( QRegExp( "<(/?)a\\a((?:[^>\"\']|\"[^\"]*\"|\'[^\']*\')*)>" ) );

    //qDebug() << convertedMessage;

    return convertedMessage;
}
