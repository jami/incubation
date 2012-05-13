#
#
# Tell CMake to install the header file
INSTALL_FILES(/include/lib/ FILES 
    log.h
    configuration.h
    graphic.h
    event_manager.h
    state.h
    timer.h
    json.h
    ressource.h
    image.h
    gui.h
)

# sdl depend
#INSTALL_FILES(/include/lib/ FILES SDLEventDispatcher.hpp SDLCore.hpp)

#add_subdirectory(Window)
#add_subdirectory(InputEvent)
#add_subdirectory(System)
#add_subdirectory(Graphic)
#add_subdirectory(Storage)
#add_subdirectory(State)
