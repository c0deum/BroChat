import QtQuick 2.5
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: root

    minimumWidth: 128
    minimumHeight: 256

    width: 256
    height: 512    

    visible: true    

    //color: "transparent"

    signal showSettingsTriggered
    signal reconnectAllChatsTriggered
    signal reconnectAcesChatTriggered
    signal reconnectAzubuChatTriggered
    signal reconnectBeamproChatTriggered
    signal reconnectCybergameChatTriggered
    signal reconnectGamerstvChatTriggered
    signal reconnectGipsyteamChatTriggered
    signal reconnectGoodgameChatTriggered
    signal reconnectHitboxChatTriggered
    signal reconnectIgdcChatTriggered
    signal reconnectLivecodingChatTriggered
    signal reconnectPeka2ChatTriggered
    signal reconnectStreamcubeChatTriggered
    signal reconnectTwitchChatTriggered
    signal reconnectVidiChatTriggered
    signal reconnectYoutubeChatTriggered
    signal exitTriggered

    header: ToolBar{
        ToolButton{
            text: qsTr("Menu")
            onClicked: mainMenu.open()
            Menu{
                id: mainMenu
                MenuItem{
                    text: qsTr( "Settings..." )
                    onTriggered: showSettingsTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect All Chats" )
                    onTriggered: reconnectAllChatsTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Aces Chat" )
                    onTriggered: reconnectAcesChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Azubu Chat" )
                    onTriggered: reconnectAzubuChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Beampro Chat" )
                    onTriggered: reconnectBeamproChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Cybergame Chat" )
                    onTriggered: reconnectCybergameChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Gamerstv Chat" )
                    onTriggered: reconnectGamerstvChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Gipsyteam Chat" )
                    onTriggered: reconnectGipsyteamChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Goodgame Chat" )
                    onTriggered: reconnectGoodgameChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Hitbox Chat" )
                    onTriggered: reconnectHitboxChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Igdc Chat" )
                    onTriggered: reconnectIgdcChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Livecoding Chat" )
                    onTriggered: reconnectLivecodingChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Peka2 Chat" )
                    onTriggered: reconnectPeka2ChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Streamcube Chat" )
                    onTriggered: reconnectStreamcubeChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Twitch Chat" )
                    onTriggered: reconnectTwitchChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Vidi Chat" )
                    onTriggered: reconnectVidiChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Reconnect Youtube Chat" )
                    onTriggered: reconnectYoutubeChatTriggered()
                }
                MenuItem{
                    text: qsTr( "Exit" )
                    onTriggered: exitTriggered()
                }
            }
        }
    }

    footer: ToolBar
    {
    }

    ListView {
        id: messagesView

        property bool autoScroll: true

        anchors.fill: parent
        spacing: 10
        model: coreMediator.messagesModel
        delegate:
        Rectangle {
            anchors.margins: 10
            anchors.left: parent.left
            anchors.right: parent.right
            height: idHeader.height + idMessage.height
            Rectangle {
                id: idHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 24
                color: "#3F51B5"
                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: model.nickname
                    font.pixelSize: 16
                    color: "#FFFFFF"
                }
            }

            Rectangle {
                id: idMessage
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: idHeader.bottom
                height: idMessageText.contentHeight + 24
                color: "#FFFFFF"
                Text {
                    id: idMessageText
                    anchors.margins: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //anchors.verticalCenter: parent.verticalCenter
                    anchors.top: idHeader.bottom
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    elide: Text.ElideRight
                    text: model.message
                    font.pixelSize: 14
                }
            }
        }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }

        focus: true

        onCountChanged:
        {
            currentIndex = count - 1
        }
    }

    Settings
    {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }
}
