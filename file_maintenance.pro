TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        action_detail_row.cpp \
        action_dialog.cpp \  # must be reconsided to "menu_system" version of this class
        action_io_row.cpp \
        actions.cpp \
    field_navigation_interaction_map.cpp \
        global_entities.cpp \
        interaction_result.cpp \
        io_field.cpp \
    io_field_specs_variant.cpp \
        io_row.cpp \
        io_table.cpp \
        main.cpp \
        menu.cpp \
        menu_actions.cpp \
        menu_option.cpp \
        process_menu.cpp \
    print_functions.cpp \
        state_application.cpp \
        state_menu.cpp \
        valid_values.cpp \
        window_panel.cpp

HEADERS += \
    action_detail_row.h \
    action_dialog.h \
    action_io_row.h \
    actions.h \
    field_navigation_interaction_map.h \
    global_entities.h \
    interaction_result.h \
    io_field.h \
    io_field_specs_variant.h \
    io_row.h \
    io_table.h \
    menu.h \
    menu_actions.h \
    menu_option.h \
    process_menu.h \
    print_functions.h \
    state_application.h \
    state_menu.h \
    valid_values.h \
    window_panel.h

INCLUDEPATH +=../../cereal/include/
INCLUDEPATH +=/home/grostig/libs/include/

QMAKE_CXXFLAGS += \
        -std=c++2a 		\
        -Wno-comment 		\
        -Wno-uninitialized 	\
        -Wno-reorder 		\
        -Wno-unused-parameter 	\

LIBS += \
        -lpthread 		\
        -lrt 			\
        -L /home/grostig/libs/lib \

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/release/ -llib_tty
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/debug/ -llib_tty
#else:unix: LIBS += -L$$PWD/../build-lib_tty-Desktop_Qt_*_GCC_64bit-Debug/ -llib_tty
else:unix: LIBS += -L$$PWD/../build-lib_tty-Desktop_Qt_6_0_0_GCC_64bit-Debug/ -llib_tty

INCLUDEPATH += $$PWD/../lib_tty
DEPENDPATH += $$PWD/../lib_tty

# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/release/liblib_tty.a
# else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/debug/liblib_tty.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/release/lib_tty.lib
# else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/debug/lib_tty.lib
# else:unix: PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_5_13_1_GCC_64bit-Debug/liblib_tty.a

#unix: PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_*_GCC_64bit-Debug/liblib_tty.a
unix: PRE_TARGETDEPS += $$PWD/../build-lib_tty-Desktop_Qt_6_0_0_GCC_64bit-Debug/liblib_tty.a

DISTFILES += \
    model_test.qmodel
