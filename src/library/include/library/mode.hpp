#ifndef LIBRARY_MODE_HPP
#define LIBRARY_MODE_HPP

namespace library {

enum class mode {
    // Resolve all undefined symbols in the library
    resolve_all,

    // The symbols defined by this library will be made available
    // for symbol resolution of subsequently loaded libraries
    global,

    // Do not unload the library during destruction.
    // Consequently, the library's static variables are not reinitialized
    // if the library is reloaded at a later time
    no_delete,

    // Don't load the library. This can be used to test if the library is already resident
    no_load,

    // Place the lookup scope of the symbols in this library ahead of the global scope
    deep_bind
};

}

#endif
