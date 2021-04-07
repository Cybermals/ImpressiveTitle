#Minimum CMake version and policy settings
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

#Project Name
project(ImpressiveTitle)

#Include sub-projects
add_subdirectory(LoginServer)
add_subdirectory(GameServer)
add_subdirectory(Game)