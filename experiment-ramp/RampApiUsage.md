Use in RaMP (in C++) {#flatbufers_gen_ramp_api}
===================

## Introduction
FlatBuffers-RaMP is an application built and based on RDMA-Migration-System (RaMP) and FlatBuffers.

To enable more flexible structures when transferring through RaMP, an object based API specifically for RaMP can be used to create tables inside RaMP segment so that they can be transferred efficiently.

In the following, a full tutorial with simple examples for FlatBuffers-RaMP will be shown, which includes:
- Setup before you started
- Write a FlatBuffers-RaMP schema file
- Build the buffer
- Transfer the buffer

## Before you started
To get use of FlatBuffers-RaMP, both RDMA-Migration-System and FlatBuffers-RaMP projects need to be deployed correctly. To do this, you need to follow:
1. Download RDMA-Migration-System
2. Set up RaMP normally as it requires
3. Download flatBuffer-RAMP
4. Build flatBuffer-RAMP project as normal FlatBuffers requires
5. Create your configuration file and program as shown in later sections
6. Compile your program, specifically requires flag '-std=c++11', '-I<path to RaMP project /include folder>' and '-I<path to RaMP project /src folder>' (check Makefile in this folder as a sample)

Configuration file contains the number of servers in the first line; in following lines, they all should have server id, IP address and port number sequentially.

    // Example of configuration file: config.txt
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
Tables are the only way of defining objects in FlatBuffers-RaMP, and consist of a name and a list of fields. Each field has a name and a type; supported scalar-types and enum can also have a default value as shown above, which will be default value of that field when you create a new table.

#### Types
1. All scalar-types supported in FlatBuffers:
   - 8 bit: byte (int8), ubyte (uint8), bool
   - 16 bit: short (int16), ushort (uint16)
   - 32 bit: int (int32), uint (uint32), float (float32)
   - 64 bit: long (int64), ulong (uint64), double (float64)
2. string
    Example: the name field in Weapon table
3. Pointer to sub-tables (denoted with the sub-table name)
    Example: the pos field in Monster table, which is a pointer Vec3 table
4. Vector of any other type including table types (denoted with [type])
    Example: the weapons field in Monster table, which is a vector of Weapon tables
5. Unordered map with key being supported scalar-types or string (denoted as unordered_map(type, type))
    Example: the properties field in Monster table, which is an unordered map with key being string and value type being integer

*Note: Nested containers are not allowed; instead, you can wrap the inner container in a table.*

#### Enum
Define a sequence of named constants, each with a given value, or increasing by one from the previous one. The default first value is 0. You can specify the underlying integral type of the enum with ‘:’, which determines the type of any fields declared with this enum type.

#### Namespaces
These will generate the corresponding namespace in c++ for all helper code. Use ‘.’ to specify nested namespaces.

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
CreaterString(), which creates a rString inside the segment. You should call it on root table.
```cpp
    rString s1 = mt->CreaterString(“left right”);
    mt->path.push_back(s1);
    
```

## Transfer the Buffer
As you have successfully build the buffer, you can send it over machines. 

Send a root table:
```cpp
    mt->Prepare(id); // id is the id of the server you want to send to
    while (!mt->PollForAccept) {} // wait for the receiver to agree to take the segment
    mt->Transfer(); // transfer the root table and everything it refers

    // If you are want to close the connection
    while (!mt->PollForClose()) {}
```
On the recieving machine, you can receive a root table and modify it as shown in ‘Build the Buffer’ sectoin. Before you do that, you also need to create a RDAMMemoryManager and RampBuilder. Don't forget to include the header file for program on receiving side as well!
```cpp
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(config.txt, id); // id is the id of the server you currently run on 
    RampBuilder<struct MonsterT> *mb = new RampBuilder<struct MonsterT>(memory_manager);
    struct MonsterT *mt;
    while((mt = mb->PollForRoot) == nullptr) {} // receive the transferred root object
    
    // If you are ready to close the connection
    mt->Close();
```
