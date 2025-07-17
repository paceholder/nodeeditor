file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.h )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(demo ${CPPS} ${HPPS})

target_include_directories(demo
  PUBLIC
    include
)

target_link_libraries(demo PRIVATE  Qt6PropertyBrowser QtNodes Qt6::Core
    Qt6::Widgets)
