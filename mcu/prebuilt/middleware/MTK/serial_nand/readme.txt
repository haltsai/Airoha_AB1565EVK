Serial nor flash library

Brief:          This is the serial NOR flash library. It provides a generic serial NOR flash control interface over different flash devices, and emulates a sector based storage device for the FAT32 file system.


Usage:          You do not directly call this library. You call the file system interface and the file system wraps the library to be compatible for the the specific platform. The Airoha service is transparent and you can always see the processes that are occurring, regardless of the platform.    

Dependency:     It is dependent on the flash_manager module. 

Notice:         None

Relative doc:   Please refer to middileware/flash_manager/snor/readme.txt.

Example project:None