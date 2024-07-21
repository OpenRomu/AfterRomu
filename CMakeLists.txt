cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(
  AfterRomu
  VERSION 1.0
  LANGUAGES CXX
)


set(SOURCES
    "src/blob.cpp"
    "src/bouton.cpp"
    "src/bsp.cpp"
    "src/chat.cpp"
    "src/config.cpp"
    "src/console.cpp"
    "src/cross.cpp"
    "src/dsutil.cpp"
    "src/dxutil.cpp"
    "src/engine.cpp"
    "src/Flag.cpp"
    "src/font.cpp"
    "src/gltga.cpp"
    # "src/glwin.cpp" Disabled for now
    "src/grenade.cpp"
    "src/GrenadeMan.cpp"
    "src/grenadesimple.cpp"
    "src/grenadesmoke.cpp"
    "src/image.cpp"
    "src/input_box.cpp"
    "src/lazer.cpp"
    "src/mark.cpp"
    "src/md5.cpp"
    "src/missile.cpp"
    "src/Model_MDL.cpp"
    "src/mp3.cpp"
    "src/option_bouton.cpp"
    "src/panel.cpp"
    "src/particle.cpp"
    "src/particleBoom.cpp"
    "src/particleBurst.cpp"
    "src/particleHeSmoke.cpp"
    "src/particleMan.cpp"
    "src/particleSimple.cpp"
    "src/particleSmoke.cpp"
    "src/particleSpit.cpp"
    "src/particleTorch.cpp"
    "src/particleTrace.cpp"
    "src/PhysEnv.cpp"
    "src/Pings.cpp"
    "src/player.cpp"
    "src/playerfile.cpp"
    "src/romu.cpp"
    "src/romustrike.cpp"
    "src/stdafx.cpp"
    "src/texMan.cpp"
    "src/texture.cpp"
    "src/tga.cpp"
    "src/ticker.cpp"
    "src/utility.cpp"
    "src/vectormath.cpp"
    "src/vehicle.cpp"
    "src/wad.cpp"
    "src/world.cpp"
    "src/xmlmessaging.cpp"
)

add_executable(AfterRomu ${SOURCES})
target_include_directories(AfterRomu PRIVATE "src/")
target_include_directories(AfterRomu PRIVATE "src/mock")
