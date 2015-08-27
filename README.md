# ShankBot

## Linking
The Windows module requires Gdi32.


## Code details
The code is split up into modules.

### Directory structure
> **incl** - Header files and namespace folders.

> **src** - Source files and namespace folders.

Any header or source files not connected to a namespace are considered as common and must be usable by all platforms and in any C++-code without external libraries.

### Namespaces
A namespace consists of the following:
* A folder with the same name as the namespace in **incl** containing the namespace's header files.
* A folder with the same name as the namespace in **src** containing the namespace's source files.
* A C++ namespace with the same name as the module in which all the module's code lies under.


#### Windows
Windows-specific code.