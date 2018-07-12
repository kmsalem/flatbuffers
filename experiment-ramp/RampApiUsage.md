Use in RaMP (in C++) {#flatbufers_gen_ramp_api}
===================

## Introduction
FlatBuffers-RaMP is an application built and based on RDMA-Migration-System (RaMP) and FlatBuffers.

To enable more flexible structures when transferring through RaMP, an object based API specifically for RaMP can be used to create tables inside RaMP segment so that they can be transferred efficiently.

In the following, a full tutorial with simple examples for FlatBuffers-RaMP will be shown, which includes:
- Setup before using FlatBuffers-RaMP
- Write a FlatBuffers-RaMP schema file
- Build the buffer
- Transfer the buffer

## Setup
To get use of FlatBuffers-RaMP, both RDMA-Migration-System and FlatBuffers-RaMP projects need to be downloaded and deployed correctly.

Then you need a configuration file which contains the number of servers in the first line, server id, IP address and port in following lines.

    // config.txt
    2
    0 10.70.0.9 5050
    1 10.70.0.8 5050

This file will be used when you create RDMAMemoryManger. (see later) 

## Write FlatBuffers-RaMP Schema
To start working with FlatBuffers-RaMP, you first need to create a schema file, which defines the format of each data structure you wish to transfer.
Here is a sample that defines the monster and relevant tables:
```cpp
    // Example IDL file for monster’s schema
    namespace MyGame.Sample;
    enum Color:byte { Red = 0, Green, Blue = 2 }

    table Vec3 {
      x:float;
      y:float;
      z:float;
    }

    table Monster {
      pos:Vec3;
      mana:short = 150;
      hp:short = 100;
      name:string;
      inventory:[ubyte];  // Vector of scalars.
      color:Color = Blue; // Enum.
      weapons:[Weapon];   // Vector of tables.
      path:[string];      // Vector of strings.
      properties:unordered_map(string, int); // unordered_map
    }

    table Weapon {
      name:string;
      damage:short;
    }
    
    root_type Monster;
```
#### Tables
Tables are the only way of defining objects in FlatBuffers-RaMP, and consist of a name and a list of fields. Each field has a name and a type; supported scalar-types and enum can also have a default value as shown above.

#### Types
1. All scalar-types supported in FlatBuffers:
   - 8 bit: byte (int8), ubyte (uint8), bool
   - 16 bit: short (int16), ushort (uint16)
   - 32 bit: int (int32), uint (uint32), float (float32)
   - 64 bit: long (int64), ulong (uint64), double (float64)
3. string
2. Pointer to other tables (denoted with a table name)
4. Vector of any other type (denoted with [type])
5. Unordered map with key being built-in types (eg. int, string…) (denoted as unordered_map(type, type))
*Note: Nested containers are not allowed; instead, you can wrap the inner container in a table.*

#### Enum
Define a sequence of named constants, each with a given value, or increasing by one from the previous one. The default first value is 0. You can specify the underlying integral type of the enum with ‘:’, which determines the type of any fields declared with this enum type.

#### Namespaces
These will generate the correspondinf namespace in c++ for all hekoer code. Use ‘.’ to specify nested namespaces.

#### Root type
You have to declare one root type in a schema, which will be the root table of built data.

#### Comments & documentation
Triple comment (///) on a line by itself signals that a comment is documentation for whatever is declared on the line after it (table/field/enum).

*Note: Do not try to use other keywords or additional attributes supported in normal FlatBuffers for RaMP usage.*

## Build the Buffer
Compile your schema using the FlatBuffer compiler:
```
    ./flatc -c --gen-ramp-api monster.fbs
```
and then a header file (eg. monster_generated.h) will be generated. This file will include “flatbuffers/flatbuffers_ramp.h”, which is a library file containing helper methods for building data.

The first step is to import/include the generated file.
```cpp
    #include “monster_generated.h”
    using namespce MyGame::Sample;
```
Now we are ready to start building buffers inside RaMP segment. First, we need to create an instance of RDMAMemoryManager and  RampBuilder with root type:
```cpp
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(config.txt, id); // id is the id of the server you currently run on 
    RampBuilder<struct MonsterT> *mb = new RampBuilder<struct MonsterT>(memory_manager);
```
This builder object can be used to create a new domains (= RaMP segment) and a root object. You can pass an initial size of the buffer, which will cause an “OUT OF MEMORY” error when you exceed space limitation:
```cpp
    struct MonsterT *mt = mb->CreateRoot(1024);
```
Now you can use the MonsterT as a native C++ object and directly modify the fields that do not require initialization of other tables:
```cpp
    mt->mana = 10;
    mt->hp = 99;
    mt->name = “Boss”;
    mt->color = 0;
    mt->inventory.push_back(1);
    mt->inventory.push_back(2);
    mt->path.push_back(“right right left”);
    mt->properties[“property1”] = 99;
```
To create a table, you need to use CreateObj() method on root table. This lets you create a table in the RaMP segment and return a pointer to that table. Don’t forget that you need to use it every time before you first access any table inside a container.
```cpp
    mt->pos = mt->CreateObj<Vec3T>();
    struct WeaponT * w1 = mt->CreateObj<WeaponT>();
    mt->weapons.push_back(w1);
```
CreaterString(), which creates a rString inside the segment, is also offered. You should call it on root table.
```cpp
    rString s1 = mt->CreaterString(“monster”);
```

## Transfer the Buffer
As you have successfully build the buffer, you can send it over machines. 

Send a root table:
```cpp
    mt->Prepare(id); // id is the id of the server you want to send to
    while (!mt->PollForAccept) {} // sending the request to receiving server
    mt->Transfer(); // transfer the buffer

    // If you are want to close the connection
    while (!mt->PollForClose()) {}
```
On the recieving machine, you can receive a root table and modify it as shown in ‘Build the Buffer’ sectoin. Before you do that, you also need to create a RDAMMemoryManager and RampBuilder.
```cpp
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(config.txt, id); // id is the id of the server you currently run on 
    RampBuilder<struct MonsterT> *mb = new RampBuilder<struct MonsterT>(memory_manager);
    struct MonsterT *mt;
    while((mt = mb->PollForRoot) == nullptr) {} // receive the buffer
    
    // If you are ready to close the connection
    mt->Close();
```
