# hide-imports

### Features 
- Written in C
- Supports both 32-bit and 64-bit
- Supports both Unicode and Multi-Byte Character Set
- Manual mode that allows achieve zero imports
- Function address is fetched only once, then retrieved from the list thereafter

### Usage
```bash
CALL("DbgPrint", 1, "DbgPrint Test\n");
```
Driver [example](example/main.c)

### IDA imports view

(**MANUAL_MODE** *disabled*):

<img src="https://i.imgur.com/GrpOwJt.png" width="500">

(**MANUAL_MODE** *enabled*):

<img src="https://i.imgur.com/fxnbI2j.png" width="400">

Manual mode requires you to obtain the address of MmGetSystemRoutineAddress manually, for example, by passing the address in mapping parameters from user mode.
