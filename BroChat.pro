QT += core network widgets webenginewidgets websockets svg

CONFIG += c++14

QMAKE_CXXFLAGS += -DQXMPP_STATIC

HEADERS += \
    qtwitchchat.h \
    qchatservice.h \
    qchatmessage.h \
    qsettingsdialog.h \    
    settingsconsts.h \
    qgoodgamechat.h \
    qcybergamechat.h \
    qaceschat.h \
    qgamerstvchat.h \
    qbrochatview.h \        
    qchatupdateserver.h \
    qchatstatistic.h \
    qhitboxchat.h \
    qgipsyteamchat.h \    
    qlivecodingchat.h \
    qigdcchat.h \
    qpeka2chat.h \
    qpoll.h \
    qpollview.h \
    qmessagesmanager.h \
    qpollsettingsdialog.h \
    qvidichat.h \
    qazubuchat.h \
    qstreamcubechat.h \
    qbeamprochat.h \
    qbrochatwebpage.h

SOURCES += \
    main.cpp \
    qtwitchchat.cpp \
    qchatservice.cpp \
    qchatmessage.cpp \
    qsettingsdialog.cpp \    
    qgoodgamechat.cpp \
    qcybergamechat.cpp \
    qaceschat.cpp \
    qgamerstvchat.cpp \
    qbrochatview.cpp \    
    qchatupdateserver.cpp \
    qchatstatistic.cpp \
    qhitboxchat.cpp \
    qgipsyteamchat.cpp \    
    qlivecodingchat.cpp \
    qigdcchat.cpp \
    qmessagesmanager.cpp \
    qpeka2chat.cpp \
    qpoll.cpp \
    qpollsettingsdialog.cpp \
    qpollview.cpp \
    qvidichat.cpp \
    qazubuchat.cpp \
    qstreamcubechat.cpp \
    qbeamprochat.cpp \
    qbrochatwebpage.cpp

RESOURCES += \
    resources.qrc

win32:RC_FILE = ico.rc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/external/qxmpp-master/src/ -lqxmpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/external/qxmpp-master/src/ -lqxmppd

unix:!macx: LIBS += -L$$PWD/external/qxmpp-master/src/ -lqxmpp

INCLUDEPATH += \
    $$PWD/external/qxmpp-master/src/base \
    $$PWD/external/qxmpp-master/src/client \
    $$PWD/external/qxmpp-master/src/server
DEPENDPATH += \
    $$PWD/external/qxmpp-master/src/base \
    $$PWD/external/qxmpp-master/src/client \
    $$PWD/external/qxmpp-master/src/server

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/qxmpp-master/src/libqxmpp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/qxmpp-master/src/libqxmppd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/qxmpp-master/src/qxmpp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/qxmpp-master/src/qxmppd.lib

unix:!macx: PRE_TARGETDEPS += $$PWD/external/qxmpp-master/src/libqxmpp.a
