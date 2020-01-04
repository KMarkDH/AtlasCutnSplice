cmake_minimum_required(VERSION 3.10)
project(PngCP)
set(CMAKE_CXX_STANDARD 11)
include_directories(include)

add_executable(pngcut src/PngCut.cpp)
add_executable(pngsplice src/PngSplice.cpp)

install(TARGETS pngcut DESTINATION bin)
install(TARGETS pngsplice DESTINATION bin)

set(IMG_PATH ${PROJECT_SOURCE_DIR}/test/images.png)
set(DIR_PATH ${PROJECT_SOURCE_DIR}/test/images)

enable_testing()
add_test(pngcut_usage pngcut)
set_tests_properties(pngcut_usage PROPERTIES PASS_REGULAR_EXPRESSION "usage: pngcut <filepath> <width> <height>")

add_test(pngcut_1p pngcut ${IMG_PATH})
set_tests_properties(pngcut_1p PROPERTIES PASS_REGULAR_EXPRESSION "usage: pngcut <filepath> <width> <height>")

add_test(pngcut_2p pngcut ${IMG_PATH} 100)
set_tests_properties(pngcut_2p PROPERTIES PASS_REGULAR_EXPRESSION "usage: pngcut <filepath> <width> <height>")

add_test(pngcut_300_200 pngcut ${IMG_PATH} 100 100)

add_test(pngcut_100_100 pngcut ${IMG_PATH} 100 100)

add_test(pngcut_50_50 pngcut ${IMG_PATH} 50 50)

add_test(pngcut_10_10 pngcut ${IMG_PATH} 10 10)

add_test(pngsplice_usage pngsplice)
set_tests_properties(pngsplice_usage PROPERTIES PASS_REGULAR_EXPRESSION "usage: pngsplice <dirpath> <margin>")

add_test(pngsplice_1p pngsplice)
set_tests_properties(pngsplice_1p PROPERTIES PASS_REGULAR_EXPRESSION "usage: pngsplice <dirpath> <margin>")

add_test(pngsplice_0 pngsplice ${DIR_PATH} 0)

add_test(pngsplice_10 pngsplice ${DIR_PATH} 10)

add_test(pngsplice_50 pngsplice ${DIR_PATH} 50)

add_test(pngsplice_100 pngsplice ${DIR_PATH} 100)