QT      -= core gui
TEMPLATE = lib
CONFIG  += staticlib
CONFIG  -= c++11

QMAKE_CXXFLAGS += -std=c++14
QMAKE_LFLAGS   += -std=c++14
LIBS           += -ltinyxml2 -lsfml-graphics -lsfml-window -lsfml-system -lz \
                  -L/usr/lib/x86_64-linux-gnu

linux {
    QMAKE_CXXFLAGS += -DMACRO_PLATFORM_LINUX
    contains(QT_ARCH, i386) {
        LIBS += -L../../bin/linux/g++-x86
    } else:contains(QT_ARCH, x86_64) {
        LIBS += -L../../bin/linux/g++-x86_64 \
                -L/usr/lib/x86_64-linux-gnu
    }
}

debug {
    TARGET  = ksg-d
    LIBS   += -lcommon-d
}
release {
    TARGET  = ksg
    LIBS   += -lcommon
}

SOURCES += \
    ../src/ArrowButton.cpp   \
    ../src/Button.cpp        \
    ../src/Draggable.cpp     \
    ../src/DrawCharacter.cpp \
    ../src/DrawTriangle.cpp  \
    ../src/Frame.cpp         \
    ../src/ImageWidget.cpp   \
    ../src/OptionsSlider.cpp \
    ../src/ProgressBar.cpp   \
    ../src/StyleMap.cpp      \
    ../src/TextArea.cpp      \
    ../src/TextButton.cpp    \
    ../src/Text.cpp          \
    ../src/Widget.cpp

HEADERS += \
    \ # private headers
    ../src/ksg/DrawCharacter.hpp \
    \ # public headers
    ../inc/ksg/ArrowButton.hpp   \
    ../inc/ksg/Button.hpp        \
    ../inc/ksg/Draggable.hpp     \
    ../inc/ksg/DrawTriangle.hpp  \
    ../inc/ksg/Frame.hpp         \
    ../inc/ksg/ImageWidget.hpp   \
    ../inc/ksg/OptionsSlider.hpp \
    ../inc/ksg/ProgressBar.hpp   \
    ../inc/ksg/StorageUnion.hpp  \
    ../inc/ksg/StyleMap.hpp      \
    ../inc/ksg/TextArea.hpp      \
    ../inc/ksg/TextButton.hpp    \
    ../inc/ksg/Text.hpp          \
    ../inc/ksg/Widget.hpp        \
    ../inc/ksg/Visitor.hpp       \
    ../inc/ksg/ForwardWidgets.hpp

INCLUDEPATH += \
    ../inc           \
    ../../common/inc
