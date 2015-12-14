var MESSAGE = "message",
    STATISTIC = "statistic",
    userScroll = false;

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
