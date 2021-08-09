set(TiRPC_PATH ${CMAKE_CURRENT_LIST_DIR})

include_directories(${TiRPC_PATH}/include/3rd/cereal)
include_directories(${TiRPC_PATH}/include/3rd/zeromq)
include_directories(${TiRPC_PATH}/include/3rd/cppzmq)
include_directories(${TiRPC_PATH}/include/core)
link_directories(${TiRPC_PATH}/libraries)
if(WIN32)
  link_libraries(libzmq)
else()
  link_libraries(pthread zmq)
endif()

function(PrecompileTiRPC target)
  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(${target} PRIVATE
	  ${TiRPC_PATH}/include/core/tmss/RpcProcessRequest.hpp)
    target_precompile_headers(${target} PRIVATE
	  ${TiRPC_PATH}/include/core/tmss/RpcProcessResponse.hpp)
    target_precompile_headers(${target} PRIVATE
	  ${TiRPC_PATH}/include/core/tmss/RpcAsyncBroadcast.hpp)
  endif()
endfunction()
