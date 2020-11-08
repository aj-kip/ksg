QT      -= core gui
TEMPLATE = app
# CONFIG  += staticlib
CONFIG  -= c++11

QMAKE_CXXFLAGS += -std=c++17
QMAKE_LFLAGS   += -std=c++17
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
    LIBS += "-L$$PWD/../lib/cul"
    LIBS   += -lcommon-d
}

SOURCES += \
    ../src/Widget.cpp        \
    ../src/ImageWidget.cpp   \
    ../src/Button.cpp        \
    ../src/StyleMap.cpp      \
    ../src/OptionsSlider.cpp \
    ../src/EditableText.cpp  \
    ../src/ProgressBar.cpp   \
    ../src/TextArea.cpp      \
    ../src/Text.cpp          \
    ../src/FocusWidget.cpp   \
    ../src/DrawTriangle.cpp  \
    ../src/FrameBorder.cpp   \
    ../src/DrawCharacter.cpp \
    ../src/Draggable.cpp     \
    ../src/ArrowButton.cpp   \
    ../src/TextButton.cpp    \
    ../src/Frame.cpp         \
    ../src/SelectionMenu.cpp \
    ../demos/demo.cpp

HEADERS += \
    \ # private headers
    # <none>
    \ # public headers
    ../inc/ksg/ArrowButton.hpp    \
    ../inc/ksg/ForwardWidgets.hpp \
    ../inc/ksg/Draggable.hpp      \
    ../inc/ksg/Widget.hpp         \
    ../inc/ksg/FocusWidget.hpp    \
    ../inc/ksg/TextButton.hpp     \
    ../inc/ksg/StyleMap.hpp       \
    \#../inc/ksg/SelectionList.hpp  \
    ../inc/ksg/ProgressBar.hpp    \
    ../inc/ksg/OptionsSlider.hpp  \
    ../inc/ksg/ImageWidget.hpp    \
    ../inc/ksg/Text.hpp           \
    ../inc/ksg/Frame.hpp          \
    ../inc/ksg/Button.hpp         \
    ../inc/ksg/DrawCharacter.hpp  \
    ../inc/ksg/DrawTriangle.hpp   \
    ../inc/ksg/FrameBorder.hpp    \
    ../inc/ksg/EditableText.hpp   \
    ../inc/ksg/TextArea.hpp       \
    ../inc/ksg/SelectionMenu.hpp

INCLUDEPATH += \
    ../inc           \
    ../lib/cul/inc
