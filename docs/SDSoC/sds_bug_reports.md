This contains reports of bugs in SDSoC encountered when implementing Benchmarks for Seashell.

### Bug report 1 
#### SDSoC does wrong code generation for a struct with one field.
Issue #39 identified an erroneous code generation from SDSoC upon encountering a struct with a single data field. The code generation works fine when added a bogus data field.

### Bug report 2
#### SDSoC seems to use some default data size for the first function argument in it's send function when used structs for the arguments.
Issue #34 provides details of this wrong code generation. The code generation is correct when simple arrays are used for function call, or the size of the first argument is explicitly specified using data copy pragma.
