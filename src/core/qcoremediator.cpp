#include "qchatservice.h"

#include "qsettingsdialog.h"

#include "qcoremediator.h"


QCoreMediator * QCoreMediator::self_ = nullptr;

QCoreMediator::QCoreMediator( QObject *parent )
    : QObject( parent )    
{
}

QCoreMediator & QCoreMediator::instance()
{
    if( !self_ )
        self_ = new QCoreMediator();
    return *self_;
}

void QCoreMediator::release()
{
    delete self_;
    self_ = nullptr;
}

void QCoreMediator::registerChatService( QSharedPointer<QChatService> service )
{
    chatServices_.push_back( service );

    QObject::connect( service.data(), SIGNAL( newMessage( ChatMessage ) ), &model_, SLOT( add( ChatMessage ) ) );
    QObject::connect( this, SIGNAL( reconnect() ), service.data(), SLOT( reconnect() ) );

}

QMessagesModel * QCoreMediator::messagesModel()
{
    return &model_;
}

void QCoreMediator::showSettings()
{
    QSharedPointer<QSettingsDialog> settingsDialog( new QSettingsDialog() );
    settingsDialog.data()->show();
}

void QCoreMediator::onReconnect()
{
    emit reconnect();
}
