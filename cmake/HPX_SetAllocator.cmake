# Copyright (c) 2007-2012 Hartmut Kaiser
# Copyright (c) 2011-2013 Thomas Heller
# Copyright (c) 2007-2008 Chirag Dekate
# Copyright (c)      2011 Bryce Lelbach
# Copyright (c)      2011 Vinay C Amatya
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package(HPX_TCMalloc)
find_package(HPX_Jemalloc)
find_package(HPX_TBBmalloc)

if("${HPX_MALLOC}" MATCHES "tcmalloc|TCMalloc|TCMALLOC" AND NOT TCMALLOC_FOUND)
  hpx_warn("malloc" "tcmalloc allocator not found.")
endif()

if("${HPX_MALLOC}" MATCHES "jemalloc|Jemalloc|JEMALLOC" AND NOT JEMALLOC_FOUND)
  hpx_warn("malloc" "jemalloc allocator not found.")
endif()

if("${HPX_MALLOC}" MATCHES "tbbmalloc|TBBMalloc|TBBMALLOC|TBBmalloc" AND NOT
        TBBMALLOC_FOUND)
  hpx_warn("malloc" "tbbmalloc allocator not found.")
endif()

if(MSVC)
  if(NOT "${HPX_MALLOC}" MATCHES "system|System|SYSTEM")
    if(TCMALLOC_FOUND AND "${HPX_MALLOC}" MATCHES "tcmalloc|TCMalloc|TCMALLOC")
      hpx_info("malloc" "Using tcmalloc allocator.")
      set(hpx_MALLOC_LIBRARY ${TCMALLOC_LIBRARY})
      set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TCMALLOC_LIBRARY})
      set(HPX_LINK_FLAG_TARGET_PROPERTIES "/INCLUDE:__tcmalloc")
      hpx_include_sys_directories(${TCMALLOC_INCLUDE_DIR})
      hpx_link_sys_directories(${TCMALLOC_LIBRARY_DIR})
      hpx_add_definitions(-DHPX_TCMALLOC)
    elseif(TBBMALLOC_FOUND AND "${HPX_MALLOC}" MATCHES
        "tbbmalloc|TBBMalloc|TBBMALLOC|TBBmalloc")
      hpx_info("malloc" "Using tbbmalloc allocator.")
      set(hpx_MALLOC_LIBRARY "${TBBMALLOC_LIBRARY};${TBBMALLOC_PROXY_LIBRARY}")
      set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TBBMALLOC_LIBRARY})
      set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TBBMALLOC_PROXY_LIBRARY})
      set(HPX_LINK_FLAG_TARGET_PROPERTIES "/INCLUDE:__TBB_malloc_proxy")
      hpx_include_sys_directories(${TBBMALLOC_INCLUDE_DIR})
      hpx_link_sys_directories(${TBBMALLOC_LIBRARY_DIR})
      hpx_add_definitions(-DHPX_TBBMALLOC)
    endif()
  else()
    hpx_info("malloc" "Using system allocator.")
  endif()
else()
  if(NOT "${HPX_MALLOC}" MATCHES "system|System|SYSTEM")
    if(TCMALLOC_FOUND)
      if("${HPX_MALLOC}" MATCHES "tcmalloc|TCMalloc|TCMALLOC")
        hpx_info("malloc" "Using tcmalloc allocator.")
        set(hpx_MALLOC_LIBRARY ${TCMALLOC_LIBRARY})
        set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TCMALLOC_LIBRARY})
        hpx_include_sys_directories(${TCMALLOC_INCLUDE_DIR})
        hpx_link_sys_directories(${TCMALLOC_LIBRARY_DIR})
        hpx_add_definitions(-DHPX_TCMALLOC)
        set(HPX_USE_CUSTOM_ALLOCATOR On)
      endif()
    elseif(JEMALLOC_FOUND)
      if("${HPX_MALLOC}" MATCHES "jemalloc|Jemalloc|JEMALLOC")
        hpx_info("malloc" "Using jemalloc allocator.")
        set(hpx_MALLOC_LIBRARY ${JEMALLOC_LIBRARY})
        set(hpx_LIBRARIES ${hpx_LIBRARIES} ${JEMALLOC_LIBRARY})
        hpx_include_sys_directories(${JEMALLOC_INCLUDE_DIR})
        hpx_link_sys_directories(${JEMALLOC_LIBRARY_DIR})
        hpx_add_definitions(-DHPX_JEMALLOC)
        set(HPX_USE_CUSTOM_ALLOCATOR On)
      endif()
    elseif(TBBMALLOC_FOUND)
      if("${HPX_MALLOC}" MATCHES "tbbmalloc|TBBMalloc|TBBMALLOC|TBBmalloc")
        hpx_info("malloc" "Using tbbmalloc allocator.")
        set(hpx_MALLOC_LIBRARY "${TBBMALLOC_LIBRARY};${TBBMALLOC_PROXY_LIBRARY}")
        set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TBBMALLOC_LIBRARY})
        set(hpx_LIBRARIES ${hpx_LIBRARIES} ${TBBMALLOC_PROXY_LIBRARY})
        hpx_include_sys_directories(${TBBMALLOC_INCLUDE_DIR})
        hpx_link_sys_directories(${TBBMALLOC_LIBRARY_DIR})
        hpx_add_definitions(-DHPX_TBBMALLOC)
        set(HPX_USE_CUSTOM_ALLOCATOR On)
      endif()
    endif()

    if(HPX_USE_CUSTOM_ALLOCATOR)
      hpx_use_flag_if_available(-fno-builtin-cfree LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-pvalloc LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-malloc LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-free LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-calloc LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-realloc LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-valloc LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-memalign LANGUAGES CXX C)
      hpx_use_flag_if_available(-fno-builtin-posix_memalign LANGUAGES CXX C)
    endif()
  else()
    hpx_info("malloc" "Using system allocator.")

    hpx_warn("malloc"
      "HPX will perform poorly without tcmalloc or jemalloc. See docs for more info.")
  endif()
endif()