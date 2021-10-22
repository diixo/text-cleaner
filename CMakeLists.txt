
cmake_minimum_required (VERSION 2.8)

project (text-cleaner-prj)

# Set compiler flags and options. 
if (MSVC) # MSVC compiler (Win32 only)
    # Display more warnings
    set (CMAKE_CXX_FLAGS "/W3")
elseif (UNIX OR CMAKE_COMPILER_IS_GNUCXX) # Clang OR Gcc (Linux, Mac OS or Win32 with MingW)
    # Enable C++11 and displays all warnings
    # C++ compiler flags
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")

    if (APPLE) # Clang / Mac OS only
        # Required on OSX to compile c++11
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -mmacosx-version-min=10.7")
    endif (APPLE)
endif ()

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

set (SOURCE_EXE Main.cpp)

add_executable (text-cleaner ${SOURCE_EXE})
