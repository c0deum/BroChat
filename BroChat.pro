QT += core network xml widgets webkitwidgets websockets svg

QMAKE_CXXFLAGS += -DQXMPP_STATIC

HEADERS += \
    qtwitchchat.h \
    qchatservice.h \
    qchatmessage.h \
    qsettingsdialog.h \
    qchatsmile.h \
    qsc2tvchat.h \
    settingsconsts.h \
    qgoodgamechat.h \
    qcybergamechat.h \
    qaceschat.h \
    qgamerstvchat.h \
    qbrochatview.h \    
    qfunstreamchat.h \
    qchatupdateserver.h \
    qstreamboxchat.h \
    qchatstatistic.h \
    qhitboxchat.h \
    qgipsyteamchat.h \
    qrealltvchat.h \
    qyoutubechat.h \
    qlivecodingchat.h \
    qigdcchat.h

SOURCES += \
    main.cpp \
    qtwitchchat.cpp \
    qchatservice.cpp \
    qchatmessage.cpp \
    qsettingsdialog.cpp \
    qchatsmile.cpp \
    qsc2tvchat.cpp \
    qgoodgamechat.cpp \
    qcybergamechat.cpp \
    qaceschat.cpp \
    qgamerstvchat.cpp \
    qbrochatview.cpp \
    qfunstreamchat.cpp \
    qchatupdateserver.cpp \
    qstreamboxchat.cpp \
    qchatstatistic.cpp \
    qhitboxchat.cpp \
    qgipsyteamchat.cpp \
    qrealltvchat.cpp \
    qyoutubechat.cpp \
    qlivecodingchat.cpp \
    qigdcchat.cpp

RESOURCES += \
    resources.rc

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
