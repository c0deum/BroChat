#include "qchatsmile.h"

QChatSmile::QChatSmile( const QString& name, const QString &link, int width, int height )
: name_( name )
, link_( link )
, width_( width )
, height_( height )
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

int QChatSmile::width() const
{
    return width_;
}

void QChatSmile::setWidth( int width )
{
    width_ = width;
}

int QChatSmile::height() const
{
    return height_;
}

void QChatSmile::setHeight( int height )
{
    height_ = height;
}

