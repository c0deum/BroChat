#!/bin/bash
git clone https://github.com/c0deum/BroChat.git && cd BroChat && git checkout dev && git submodule init && git submodule update && cd external && cd qxmpp-master && qmake-qt5 QXMPP_LIBRARY_TYPE=staticlib && make -j5 && cd .. && cd .. && mkdir build && cd build && qmake-qt5 .. && make -j5 && cp -rf ../styles/ styles
