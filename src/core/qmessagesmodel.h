#ifndef QMESSAGESMODEL_H
#define QMESSAGESMODEL_H

#include <QAbstractListModel>
#include "qchatmessage.h"

/*
#include <QtQml>

namespace QmlHelpers {

    template<class T>
    class QmlTypeRegistrator
    {
    public:
        explicit QmlTypeRegistrator(int qmlTypeId)
        {
            Q_UNUSED(qmlTypeId);
        }
        explicit QmlTypeRegistrator(const char *module, int majorVersion, int minorVersion, const char *name)
        {
            qmlRegisterType<T>(module, majorVersion, minorVersion, name);
        }
    };
}
*/

class QMessagesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum class ERole: int
    {
        //ERoleService = Qt::UserRole,
        ERoleNickname = Qt::UserRole,
        ERoleMessage,
        //EroleType,
        //EroleIcons,
    };
public:
    explicit QMessagesModel( QObject * parent = nullptr );
    virtual ~QMessagesModel() = default;
public:
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex &parent ) const;
    virtual QHash< int, QByteArray > roleNames() const;
public slots:
    void add( ChatMessage message );
signals:
    void changed();
private:
    QList< ChatMessage > messages_;
    QHash< int, QByteArray > roleNames_;
};

#endif // QMESSAGESMODEL_H
