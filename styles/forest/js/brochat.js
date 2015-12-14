var MESSAGE = "message",
    STATISTIC = "statistic",
    userScroll = false,
    wsUri = "ws://localhost:15619",
    websocket = null;

function onWheel(event) {
    event = event || window.event;

    var delta = -( event.deltaY || event.detail || event.wheelDelta );
    userScroll = document.getElementById('messagesList').scrollTop + window.innerHeight + delta < document.getElementById('messagesList').scrollHeight;

    document.getElementById('messagesList').scrollTop = document.body.scrollTop + delta;

    event.preventDefault ? event.preventDefault() : (event.returnValue = false);
}

function scrollToBottom() {
    if (userScroll)
        return;

    document.getElementById('messagesList').scrollTop = document.getElementById('messagesList').scrollHeight;
}
function onStatisticReceived(service, value) {
}
function onNewMessage(service, nickName, message, type) {
    var serviceElem = document.createElement("img");
    serviceElem.className = service;

    var serviceBlock = document.createElement("div");
    serviceBlock.className = "service";
    serviceBlock.appendChild(serviceElem);

    var nickBlock = document.createElement("div");
    nickBlock.className = "nick";
    nickBlock.innerHTML = nickName + "<span class='separator'>:&nbsp;</span>";

    var headerkBlock = document.createElement("div");
    headerkBlock.className = "header";
    headerkBlock.appendChild(serviceBlock);
    headerkBlock.appendChild(nickBlock);

    var messageBlock = document.createElement("div");
    messageBlock.className = "message";
    messageBlock.innerHTML = message;

    var messageBox = document.createElement("div");
    messageBox.className = "messagebox";

    messageBox.appendChild(headerkBlock);
    messageBox.appendChild(messageBlock);

    var messageType = document.createElement("div");
    messageType.className = type;
    messageType.appendChild(messageBox);

    var messagesList = document.getElementById("messagesList");

    if (type != "") {
        messagesList.appendChild(messageType);
    }
    else {
        messagesList.appendChild(messageBox);
    }
    scrollToBottom();
}
function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            webSocket = MozWebSocket;
        if (websocket && websocket.readyState == 1)
            websocket.close();
        websocket = new WebSocket(wsUri);
        websocket.onopen = function (event) {
            console.log('initWebSocket:open');
        };
        websocket.onclose = function (event) {
            setTimeout(initWebSocket, 3000);
        };
        websocket.onerror = function (event) {
            websocket.close();
        };
        websocket.onmessage = function (event) {
            var message = JSON.parse(event.data);

            if (MESSAGE == message.type) {
                onNewMessage(message.message.service, message.message.nick, message.message.message, message.message.type);
                document.body.scrollTop = document.body.scrollHeight;
            }
            else if (STATISTIC == message.type) {
                onStatisticReceived(message.statistic.service, message.statistic.statistic);
            }
        };
    }
    catch (exception) {
    }
}
initWebSocket();