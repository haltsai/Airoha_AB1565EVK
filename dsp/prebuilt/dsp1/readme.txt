DSP1 standard library

Brief:          This module include some DSP1 standard libraries.
                    libc.a: C library from either newlib, uClibc, or xclib, alternate implementations of the C standard library, often used in embedded systems.
                    libgcc.a: Compiler-specific library which implements functions that may be called by compiler-generated code. 
                    libgloss.a: System call library is the OS-specific "system call" library layer (e.g., _open_r, _close_r, etc.)
                    libhal.a: HAL library is the processor configuration description and abstraction layer.
                               You do not directly call this library. You call the HAL interface and the HAL driver wraps the library to be compatible for the the specific platform.
                               The Airoha service is transparent and you can always see the processes that are occurring, regardless of the platform.
Usage:

Dependency:     None

Notice:         None

Relative doc:   None

Example project:None
