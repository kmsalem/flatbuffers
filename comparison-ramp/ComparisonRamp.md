# Introduction
This folder includes three different versions of FlatBuffer data migration. They are used to compare and evaluate behaviour of FlatBuffer-RaMP usage.

Before the experiment starts, basic connection between two servers is already settled and targeted native object is set up. Currently, taget object only carries a vector of strings. 

## Version 1 (ramp_api_experiment)
  An object API specific for RaMP is used. Target object can be created and used as native C++ object and then be transferred by RaMP.
  Evaluate the time cost of procedure:
  - server 0 prepare to send object #1 to server 1
  - server 0 sends request to server 1
  - server 0 sends object #1
  - server 1 receives object #1
  - server 1 prepares to send object #2 to server 0
  - server 1 sends request to server 0
  - server 1 sends object #2
  - server 0 receives object #2
  

## Version 2 (flatbuffer_ramp_experiment)
  A normal FlatBuffer C++ generated header is used. Target object needs to be Packed into FlatBuffer (using RaMP allocator) before it is sent by RaMP. On receiving side, UnPack needs to be called so that data can be obtained as a native object.
  Evaluate the time cost of procedure:
  - server 0 packs target object #1 into flatbuffer m
  - server 0 prepares to send flatbuffer m to server 1
  - server 0 sends request to server 1
  - server 0 sends flatbuffer m
  - server 1 receives flatbuffer m
  - server 1 unpacks flatbuffer m to get object #1
  - server 1 packs target object #2 into flatbuffer n
  - server 1 prepares to send flatbuffer n to server 0
  - server 1 sends request to server 0
  - server 1 sends flatbuffer n
  - server 0 receives flatbuffer n
  - server 0 unpacks flatbuffer n to get object #2
  
## Version 3 (api_socket_experiment)
  This experiment is similar to version 2. However, instead of being Packed into RaMP segment, default allocator is used in FlatBuffer on target object. And then, send the buffer through tcp socket.
  It is not working right now because of socket problem.
