Use in RaMP (in C++) {#flatbufers_gen_ramp_api}
===================

## Introduction
To enable more flexible structures when transferring through RaMP,  an object based API specifically for RaMP can be used (through --gen-ramp-api) to create tables inside RaMP segment so that they can be transferred efficiently.

## Types
1. All scalar-types supported in FlatBuffers
2. Pointer to other tables (no structs)
3. string
4. Vector of any other type (denoted with [type])  
5. Unordered map with key being built-in types (eg. int, string…) (denoted as unordered_map(type, type))
Note: Nested containers are not allowed; instead, you can wrap the inner container in a table.

#### Enum/Namespaces/Root type
These keywords can be used (required) as in normal FlatBuffers.

## IMPORTANT
#### Schema:
- Only use the keywords and types that are supported in above 
- Do not try to use additional attributes for tables or fields unless they are specified to be abled for ramp-api
#### Compiler:
- Use ‘-c --gen-ramp-api’ for a header specific to RaMP usage. Combination with other flags may not work
#### Generated header:
- rString is the generated type name for string
- rMap is the generated type name for map
- All structures will be created with a constructor which you *should not* touch directly in programming 

## To use:
- Use CreateRoot() function, which returns a pointer to root type, to create the root object
- Use CreaterString(), CreateObj<T>() to create local object in current RaMP segment
- All fields in an object can be accessed directly once its created except pointer to other tables, for which CreateObj<T>() should be called to initialize it first
- For strings in container, you can either use string like “...” directly or create an rString first and then assign it.
- Refer to experiment-ramp for examples. 
