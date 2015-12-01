#include "qchatsmile.h"

QChatSmile::QChatSmile( const QString& name, const QString &link )
: name_( name )
, link_( link )
{
}

QChatSmile::~QChatSmile()
{
}

QString QChatSmile::name() const
{
    return name_;
}

void QChatSmile::setName( const QString &name )
{
    name_ = name;
}

QString QChatSmile::link() const
{
    return link_;
}

void QChatSmile::setLink( const QString &link )
{
    link_ = link;
}

