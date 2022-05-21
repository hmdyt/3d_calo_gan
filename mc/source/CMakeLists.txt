cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set( NAME_MAIN_PROGRAM Application_Main)


project( ${NAME_MAIN_PROGRAM} )

# Find Geant4 package
  option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
  if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
  else()
    find_package(Geant4 REQUIRED)
  endif()

# Setting ROOT
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
  find_package(ROOT CONFIG REQUIRED)
  include(${ROOT_USE_FILE})

# Set up Geant4 include directories and compile definition
  include(${Geant4_USE_FILE})
  include_directories(${PROJECT_SOURCE_DIR}/include)

# Locate sources and headers for this project
  file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
  file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Compile and link your application
  add_definitions(${ROOT_CXX_FLAGS})
  add_executable(${NAME_MAIN_PROGRAM} ${NAME_MAIN_PROGRAM}.cc ${sources} ${headers})
  target_link_libraries(${NAME_MAIN_PROGRAM} ${Geant4_LIBRARIES} ROOT::MathCore ROOT::Hist ROOT::Tree)

# Install the executable
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)
  install(TARGETS ${NAME_MAIN_PROGRAM} DESTINATION bin)
