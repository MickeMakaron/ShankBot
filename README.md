# ShankBot

## Linking
The Windows module requires Gdi32.


## Code details
The code is split up by header and source files. These are divided further by namespaces.

### Directory structure
> **incl** - Header files and namespace folders.

> **src** - Source files and namespace folders.

Any header or source files not connected to a namespace are considered as common and must be usable by all platforms and in any C++ code without external libraries.

### Namespaces
A namespace consists of the following:
* A folder with the same name as the namespace in **incl** containing the namespace's header files.
* A folder with the same name as the namespace in **src** containing the namespace's source files.
* A C++ namespace with the same name as the namespace in which all the namespace's code lies under.

The following subsections describe existing namespaces.

#### Windows
Windows-specific code.