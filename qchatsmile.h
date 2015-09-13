#ifndef QCHATSMILE_H
#define QCHATSMILE_H

#include <QString>

class QChatSmile
{
public:
    explicit QChatSmile( const QString &name, const QString &link, int widt, int height );
    ~QChatSmile();

    QString name() const;
    void setName( const QString &name );

    QString link() const;
    void setLink( const QString &link );

    int width() const;
    void setWidth( int width );

    int height() const;
    void setHeight( int height );

private:
    QString name_;
    QString link_;
    int width_;
    int height_;
};

#endif // QCHATSMILE_H
