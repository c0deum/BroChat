#include "qmessagesmodel.h"

/*
namespace {
    QmlHelpers::QmlTypeRegistrator<QMessagesModel> r("BroChat.Models", 1, 0, "QMessagesModel");
}
*/


QMessagesModel::QMessagesModel( QObject *parent )
    : QAbstractListModel( parent )
{
    //roleNames_[ static_cast< int >( ERole::ERoleService ) ] = "service";
    roleNames_[ static_cast< int >( ERole::ERoleNickname ) ] = "nickname";
    roleNames_[ static_cast< int >( ERole::ERoleMessage ) ] = "message";
    //roleNames_[ static_cast< int >( ERole::EroleType ) ] = "type";
    //roleNames_[ static_cast< int >( ERole::EroleIcons ) ] = "icons";
}

QVariant QMessagesModel::data(const QModelIndex &index, int role) const
{
    auto& message = messages_.at( index.row() );

    switch( static_cast< ERole >( role ) )
    {
    /*
    case ERole::ERoleService:
        return message.serviceName();
        break;
        */
    case ERole::ERoleNickname:
        return message.nickName();
        break;
    case ERole::ERoleMessage:
        return message.message();
        break;
        /*
    case ERole::EroleType:
        return message.type();
        break;
        */
    }
    return QVariant();
}

int QMessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent );
    return messages_.count();
}

QHash<int, QByteArray> QMessagesModel::roleNames() const
{
    return roleNames_;
}

void QMessagesModel::add( ChatMessage message )
{
    beginInsertRows( QModelIndex(), messages_.size(), messages_.size());
    messages_.push_back( message );
    endInsertRows();   
}
