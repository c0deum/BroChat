QT += core network widgets webenginewidgets websockets svg

CONFIG += c++14

QMAKE_CXXFLAGS += -DQXMPP_STATIC

HEADERS += \
    src\twitch\qtwitchchat.h \
    src\core\qchatservice.h \
    src\core\qchatmessage.h \
    src\core\qsettingsdialog.h \
    src\core\settingsconsts.h \
    src\goodgame\qgoodgamechat.h \
    src\goodgame\qgoodgamesettingsdialog.h \
    src\cybergame\qcybergamechat.h \
    src\aces\qaceschat.h \
    src\gamerstv\qgamerstvchat.h \
    src\core\qbrochatview.h \
    src\core\qchatupdateserver.h \
    src\core\qchatstatistic.h \
    src\hitbox\qhitboxchat.h \
    src\gipsyteam\qgipsyteamchat.h \
    src\livecoding\qlivecodingchat.h \
    src\igdc\qigdcchat.h \
    src\peka2tv\qpeka2chat.h \
    src\core\qpoll.h \
    src\core\qpollview.h \
    src\core\qmessagesmanager.h \
    src\core\qpollsettingsdialog.h \
    src\vidi\qvidichat.h \
    src\azubu\qazubuchat.h \
    src\streamcube\qstreamcubechat.h \
    src\beampro\qbeamprochat.h \
    src\core\qbrochatwebpage.h \
    src\core\notimplementedexception.h \
    src\youtube\o2youtube.h \
    src\youtube\commentmodel.h \
    src\youtube\youtubeapi.h \
    src\youtube\livebroadcastmodel.h \
    src\youtube\qyoutubesettingsdialog.h \
    src\youtube\youtubecontroller.h \
    src\youtube\qyoutubechatEx.h \
    src/core/chattypeenum.h


SOURCES += \
    src\core\main.cpp \
    src\twitch\qtwitchchat.cpp \
    src\core\qchatservice.cpp \
    src\core\qchatmessage.cpp \
    src\core\qsettingsdialog.cpp \
    src\goodgame\qgoodgamechat.cpp \
    src\goodgame\qgoodgamesettingsdialog.cpp \
    src\cybergame\qcybergamechat.cpp \
    src\aces\qaceschat.cpp \
    src\gamerstv\qgamerstvchat.cpp \
    src\core\qbrochatview.cpp \
    src\core\qchatupdateserver.cpp \
    src\core\qchatstatistic.cpp \
    src\hitbox\qhitboxchat.cpp \
    src\gipsyteam\qgipsyteamchat.cpp \
    src\livecoding\qlivecodingchat.cpp \
    src\igdc\qigdcchat.cpp \
    src\core\qmessagesmanager.cpp \
    src\peka2tv\qpeka2chat.cpp \
    src\core\qpoll.cpp \
    src\core\qpollsettingsdialog.cpp \
    src\core\qpollview.cpp \
    src\vidi\qvidichat.cpp \
    src\azubu\qazubuchat.cpp \
    src\streamcube\qstreamcubechat.cpp \
    src\beampro\qbeamprochat.cpp \
    src\core\qbrochatwebpage.cpp \
    src\youtube\o2youtube.cpp \
    src\youtube\commentmodel.cpp \
    src\youtube\youtubeapi.cpp \
    src\youtube\livebroadcastmodel.cpp \
    src\youtube\qyoutubesettingsdialog.cpp \
    src\youtube\youtubecontroller.cpp \
    src\youtube\qyoutubechatEx.cpp \


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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/external/o2/src/ -lo2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/external/o2/src/ -lo2d
else:unix: LIBS += -L$$PWD/external/o2/src/ -lo2

INCLUDEPATH += $$PWD/external/o2/src
DEPENDPATH += $$PWD/external/o2/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/o2/src/libo2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/o2/src/libo2d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/o2/src/o2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/o2/src/o2d.lib
else:unix: PRE_TARGETDEPS += $$PWD/external/o2/src/libo2.a


