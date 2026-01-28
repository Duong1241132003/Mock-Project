cmake_minimum_required(VERSION 3.14)
project(MediaPlayerProject VERSION 1.0.0 LANGUAGES CXX)

# ==============================================================================
# 1. CONFIGURATION
# ==============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- TÌM CÁC THƯ VIỆN (DEPENDENCIES) ---
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# 1. TagLib (Metadata)
pkg_check_modules(TAGLIB REQUIRED taglib)

# 2. FFmpeg (Decoding)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libswresample libavutil)

# 3. SDL2 (Audio Output) - [SỬA LỖI LINKER Ở ĐÂY]
# Dùng PkgConfig tìm sdl2 để đảm bảo lấy đúng cờ linker (-lSDL2)
pkg_check_modules(SDL2 REQUIRED sdl2)

# ==============================================================================
# 2. SOURCE CODE MANAGEMENT
# ==============================================================================
# Tìm tất cả file source
file(GLOB_RECURSE CORE_SOURCES 
    "src/model/*.cpp"
    "src/view/*.cpp"
    "src/controller/*.cpp"
    "src/service/*.cpp"
    "src/utils/*.cpp"
)

# ==============================================================================
# 3. CORE LIBRARY TARGET
# ==============================================================================
add_library(MediaPlayerCore STATIC ${CORE_SOURCES})

# --- INCLUDE DIRECTORIES ---
target_include_directories(MediaPlayerCore PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${TAGLIB_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}  
)

# --- LINK LIBRARIES (QUAN TRỌNG NHẤT) ---
# Từ khóa PUBLIC giúp MediaPlayerApp (người dùng lib này) 
# tự động link với các thư viện con (SDL2, FFmpeg...)
target_link_libraries(MediaPlayerCore PUBLIC 
    Threads::Threads
    ${TAGLIB_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${SDL2_LIBRARIES}     # <--- Dòng này giúp sửa lỗi undefined reference
)

# ==============================================================================
# 4. MAIN EXECUTABLE TARGET
# ==============================================================================
add_executable(MediaPlayerApp src/main.cpp)

# Link App với Core (và nhờ PUBLIC ở trên, nó link luôn SDL2, FFmpeg...)
target_link_libraries(MediaPlayerApp PRIVATE MediaPlayerCore)

# ==============================================================================
# 5. UNIT TESTING (GoogleTest)
# ==============================================================================
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/heads/main.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SOURCES "tests/unit/*.cpp")

    add_executable(UnitTests ${TEST_SOURCES})

    target_link_libraries(UnitTests PRIVATE 
        MediaPlayerCore 
        GTest::gmock_main  
    )
    
    target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/include)

    include(GoogleTest)
    gtest_discover_tests(UnitTests)
endif()