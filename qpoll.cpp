#include <QDateTime>
#include <QTimerEvent>

#include <QSettings>

#include <QStringList>

#include "qchatmessage.h"

#include "settingsconsts.h"

#include "qpoll.h"

QPoll::QPoll( QObject * parent )
: QObject( parent )
, question_()
, entries_()
, time_( 0 )
, elapsedTime_( 0 )
, startTime_( 0 )
, updateTimerId_( -1 )
, updateTimerInterval_( 1000 )
, multiChoice_( false )
{
}

QPoll::~QPoll()
{
}

const QString & QPoll::question() const
{
    return question_;
}

void QPoll::question( const QString & newQuestion )
{
    question_ = newQuestion;
}

const QMap<QString, QPoll::PollEntry> & QPoll::entries() const
{
    return entries_;
}

void QPoll::entries( const QMap< QString, PollEntry> & newEntries )
{
    entries_ = newEntries;
}

int QPoll::time() const
{
    return time_;
}

void QPoll::time( int newTime )
{
    time_ = newTime;
}

bool QPoll::isMultiChoice() const
{
    return multiChoice_;
}

void QPoll::multiChoice( bool newMultiChoce )
{
    multiChoice_ = newMultiChoce;
}

void QPoll::timerEvent( QTimerEvent * event )
{
    if( updateTimerId_ == event->timerId() )
    {

    }
}

void QPoll::start()
{
    stop();
    init();
}

void QPoll::stop()
{
    if( -1 != updateTimerId_ )
    {
        killTimer( updateTimerId_ );
        updateTimerId_ = 0;
    }
}

void QPoll::finish()
{
    emit finished();
}

void QPoll::onNewMessage( const ChatMessage * message )
{
    if( elapsedTime_ < time_ )
    {
        if( entries_.contains( message->message() ) )
        {
            PollEntry & entry = entries_[ message->message() ];
            if( multiChoice_ )
            {
                QPair<QString, QString> serviceUser = qMakePair( message->nickName(), message->message() );
                if( !entry.servicesUsers.contains( serviceUser ) )
                {
                    entry.servicesUsers.insert( serviceUser );
                    entry.votes++;
                }

            }
            else
            {
                if( !entry.users.contains( message->nickName() ) )
                {
                    entry.users.insert( message->nickName() );
                    entry.votes++;
                }
            }
        }
    }
}

void QPoll::init()
{
    elapsedTime_ = 0;
    startTime_ = QDateTime::currentMSecsSinceEpoch();
    foreach( const PollEntry & keyEntry, entries_ )
    {
        PollEntry & entry = entries_[ keyEntry.answer ];
        entry.votes = 0;
        entry.users.clear();
        entry.servicesUsers.clear();
    }
}

void QPoll::loadSettings()
{
    QSettings settings;

    question_ = settings.value( LAST_POLL_QUESTION_SETTING_PATH, QString() ).toString();

    multiChoice_ = settings.value( LAST_POLL_MULTI_CHOICE_SETTING_PATH, false ).toBool();

    QStringList entriesAnswers = settings.value( LAST_POLL_ENTRIES_ANSWERS_SETTING_PATH, QStringList() ).toStringList();
    QStringList entriesVotes = settings.value( LAST_POLL_ENTRIES_VOTES_SETTING_PATH, QStringList() ).toStringList();

    entries_.clear();

    for( int i = 0; i < entriesAnswers.size(); ++i )
    {
        PollEntry entry;
        entry.answer = entriesAnswers[ i ];
        entry.votes = entriesVotes[ i ].toInt();

        entries_.insert( entry.answer, entry );
    }
}

void QPoll::saveSettings()
{
    QSettings settings;

    settings.setValue( LAST_POLL_QUESTION_SETTING_PATH, question_ );
    settings.setValue( LAST_POLL_MULTI_CHOICE_SETTING_PATH, multiChoice_ );

    QStringList entriesAnswers;
    QStringList entriesVotes;

    foreach( const PollEntry & entry, entries_ )
    {
        entriesAnswers << entry.answer;
        entriesVotes << QString::number( entry.votes );
    }

    settings.setValue( LAST_POLL_ENTRIES_ANSWERS_SETTING_PATH, entriesAnswers );
    settings.setValue( LAST_POLL_ENTRIES_VOTES_SETTING_PATH, entriesVotes );
}

