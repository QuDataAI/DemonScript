QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += "$$PWD/../../../Common"
INCLUDEPATH += "$$PWD/../../src/Values"
INCLUDEPATH += "$$PWD/../../src/Graph"
INCLUDEPATH += "$$PWD/../../src/Script"
INCLUDEPATH += "$$PWD/../../src/Sys"
INCLUDEPATH += "$$PWD/../../src/"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../src/main.cpp \
    ../../src/Application.cpp \
    ../../src/Compiler.cpp \
    ../../src/Parser.cpp \
    ../../src/Graph/Edge.cpp \
    ../../src/Graph/Graph.cpp \
    ../../src/Graph/GraphClass.cpp \
    ../../src/Graph/GraphManager.cpp \
    ../../src/Graph/Node.cpp \
    ../../src/Script/Modules/MathModule.cpp \
    ../../src/Script/Modules/FileModule.cpp \
    ../../src/Script/Modules/Mind.cpp \
    ../../src/Script/Modules/MindModule.cpp \
    ../../src/Script/Modules/Module.cpp \
    ../../src/Script/Modules/ModuleFunction.cpp \
    ../../src/Script/Modules/ModuleManager.cpp \
    ../../src/Script/Modules/System.cpp \
    ../../src/Script/Stack/StackFrame.cpp \
    ../../src/Script/Code.cpp \
    ../../src/Script/Command.cpp \
    ../../src/Script/Expr.cpp \
    ../../src/Script/Function.cpp \
    ../../src/Script/Script.cpp \
    ../../src/Script/Source.cpp \
    ../../src/Script/Variable.cpp \
    ../../src/Sys/Debugger/Debugger.cpp \
    ../../src/Sys/Debugger/DebuggerClient.cpp \
    ../../src/Sys/Debugger/DebuggerMessenger.cpp \
    ../../src/Sys/Debugger/DebuggerVSCode.cpp \
    ../../src/Sys/Net/TCPServer.cpp \
    ../../src/Sys/Net/TCPServerDummy.cpp \
    ../../src/Sys/ThirdParty/Jsoncpp/json_reader.cpp \
    ../../src/Sys/ThirdParty/Jsoncpp/json_value.cpp \
    ../../src/Sys/ThirdParty/Jsoncpp/json_writer.cpp \
    ../../src/Sys/ErrorManager.cpp \
    ../../src/Sys/Trace.cpp \
    ../../src/Sys/Utils.cpp \
    ../../src/Values/Value.cpp \
    ../../src/Values/ValueArr.cpp \
    ../../src/Values/ValueBase.cpp \
    ../../src/Values/ValueEdge.cpp \
    ../../src/Values/ValueFloat.cpp \
    ../../src/Values/ValueGraph.cpp \
    ../../src/Values/ValueIndexRef.cpp \
    ../../src/Values/ValueLogic.cpp \
    ../../src/Values/ValueManager.cpp \
    ../../src/Values/ValueNode.cpp \
    ../../src/Values/ValueNone.cpp \
    ../../src/Values/ValueRef.cpp \
    ../../src/Values/ValueStr.cpp \
    ../../src/Values/ValueFile.cpp \
    ../../src/Values/ValueFus.cpp \
    ../../src/Values/ValueMap.cpp \
    ../../../Common/Logic.cpp \
    ../../../Common/FusNumber.cpp \
    ../../../Common/Senses.cpp 


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

