#ifndef QCHATSMILE_H
#define QCHATSMILE_H

#include <QString>

class QChatSmile
{
public:
    explicit    QChatSmile( const QString & name, const QString & link );
                ~QChatSmile();
public:
    QString     name() const;
    void        setName( const QString & name );

    QString     link() const;
    void        setLink( const QString & link );

private:
    QString     name_;
    QString     link_;
};

#endif // QCHATSMILE_H
