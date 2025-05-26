# cmake_minimum_required(VERSION 3.20)
# project(redis_redistribution_lock LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)

# add_library(redis_redistribution_lock-lib src/App.cpp)

# ## find packages
# find_path(HIREDIS_INCLUDE_DIR
#   NAMES hiredis/hiredis.h
# )
# find_library(HIREDIS_LIBRARY
#   NAMES hiredis
# )

# find_path(REDISPLUSPLUS_INCLUDE_DIR
#   NAMES sw/redis++/redis++.h
# )
# find_library(REDISPLUSPLUS_LIBRARY
#   NAMES redis++
# )

# ## include directories
# find_package(oatpp          1.4.0 REQUIRED)
# find_package(oatpp-swagger  1.4.0 REQUIRED)

# target_link_directories(redis_redistribution_lock-lib 
#             PUBLIC 
#                 oatpp::oatpp
#                 oatpp::oatpp-swagger
#                 ${HIREDIS_LIBRARY}
#                 ${REDISPLUSPLUS_LIBRARY}
#             )

# target_include_directories(redis_redistribution_lock-lib
#     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
#     PUBLIC ${oatpp_INCLUDE_DIRS}
#     PUBLIC ${HIREDIS_INCLUDE_DIR}
#     PUBLIC ${REDISPLUSPLUS_INCLUDE_DIR}
# )

# add_definitions(
#         ## define path to swagger-ui static resources folder
#         -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"
# )

# if(CMAKE_SYSTEM_NAME MATCHES Linux)
#     find_package(Threads REQUIRED)
#     target_link_libraries(redis_redistribution_lock-lib INTERFACE Threads::Threads ${CMAKE_DL_LIBS})
# endif()

# ## add executables
# add_executable(redis_redistribution_lock-exe src/App.cpp
#                         src/RedisConnectionManager.cpp
#                         src/RedisInventoryManager.cpp
# )
# target_link_libraries(redis_redistribution_lock-exe 
# redis_redistribution_lock-lib
# )
# target_include_directories(redis_redistribution_lock-exe 
#             PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
#             PUBLIC ${oatpp_INCLUDE_DIRS}
# )

cmake_minimum_required(VERSION 3.10)

project(redis_redistribution_lock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_path(HIREDIS_INCLUDE_DIR
  NAMES hiredis/hiredis.h
)
find_library(HIREDIS_LIBRARY
  NAMES hiredis
)
find_path(REDISPLUSPLUS_INCLUDE_DIR
  NAMES sw/redis++/redis++.h
)
find_library(REDISPLUSPLUS_LIBRARY
  NAMES redis++
)

## include directories
find_package(oatpp          1.4.0 REQUIRED)
find_package(oatpp-swagger  1.4.0 REQUIRED)

add_definitions(
        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"
)

add_executable(redis_redistribution_lock
  src/App.cpp
  src/OnTimer.cpp
  include/service/UserService.cpp
  include/ErrorHandler.cpp
  src/RedisInventoryManager.cpp
  src/RedisConnectionManager.cpp
)

target_include_directories(redis_redistribution_lock
  PUBLIC
    ${HIREDIS_INCLUDE_DIR}
    ${REDISPLUSPLUS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${oatpp_INCLUDE_DIRS}
)

target_link_libraries(redis_redistribution_lock
  PUBLIC
    ${HIREDIS_LIBRARY}
    ${REDISPLUSPLUS_LIBRARY}
    oatpp::oatpp
    oatpp::oatpp-swagger
)