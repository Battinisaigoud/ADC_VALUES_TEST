# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/ESP_IDF_5_3_1/v5.3.1/esp-idf/components/bootloader/subproject"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/tmp"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/src/bootloader-stamp"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/src"
  "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/SAI_WORKSPACE/ADC_Values/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
