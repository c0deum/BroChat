#include "qchatstatistic.h"

QChatStatistic::QChatStatistic( const QString &service, const QString &statistic, QObject *parent )
: QObject( parent )
, service_( service )
, statistic_( statistic )
{
}

QChatStatistic::~QChatStatistic()
{
}

QString QChatStatistic::service() const
{
    return service_;
}

void QChatStatistic::setService( const QString &service )
{
    service_ = service;
}

QString QChatStatistic::statistic() const
{
    return statistic_;
}

void QChatStatistic::setStatistic( const QString &statistic )
{
    statistic_ = statistic;
}
