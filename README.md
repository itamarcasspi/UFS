# **Unix File System Implementation in C**  

## **Overview**  

This project is a simplified Unix-like file system implemented entirely in the program's memory using C. It provides basic file management functionality, including file creation, reading, writing, seeking, and directory management. The file system is simulated in memory, meaning no actual file storage is used.  

## **Features**  

- File creation, opening, and closing.  
- Reading and writing data to files.  
- File seeking (`myfseek`, `mylseek`).  
- File format reading/writing (`myfscanf`, `myfprintf`).  
- Directory management (`myopendir`, `myreaddir`, `myclosedir`).  
- File system mounting and formatting.  
- Full in-memory file system structure with inodes, blocks, and directories.  

## **UFS File System Structure**  

### **Core Data Structures breakdown**  

1. **`superblock`**: Manages the number and size of inodes and blocks.
2. **`inode`**: Describes file metadata (size, name, type). In the UFS system 10% of the system's space is allocated to inodes.
3. **`block`**: Stores file data, linked as a list.  
4. **`file_pair`**: Maps file names to inode IDs in directories.  
5. **`myopenfile`**: Tracks open file information (inode, offset, and access flags).  
6. **`myDIR` / `mydirent`**: Directory management structures.

___

### The Superblock

 The superblock is the central control structure of UFS. It contains essential metadata about the file system, including:

    File system size
    Number of inodes
    Number of data blocks
    Block size (usually 512 bytes or 4KB)
    File system state (clean/dirty)
    Mount status
It is stored at the beggining of the file system for easy access.

### Inodes (index nodes)

Inodes store file metadata but do not store file names or contents. Each file or directory has an inode, containing:

    File size
    Pointers to data blocks 
    Name (For ease of implementation, usually is not stored in inodes)
    File type (regular file, directory, symbolic link)
    Root folder flag
    End of file

### Data blocks

Data blocks store the actual file contents. Each file's data is stored across one or more blocks, depending on its size.

- Block Size: 512 bytes.
- Allocation Strategy: Files are allocated using the "cylinder group" mechanism to reduce fragmentation and enhance access performance.

## **Functionality Overview**  

### **File Operations (`myfile.h`)**  

- `myfopen(const char *pathname, const char *mode)` – Opens a file with specified mode.  
- `myfclose(myFILE *stream)` – Closes an opened file.  
- `myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream)` – Reads data from a file.  
- `myfwrite(const void *ptr, size_t size, size_t nmemb, myFILE *stream)` – Writes data to a file.  
- `myfseek(myFILE *stream, long offset, int whence)` – Sets file position.  
- `myfscanf` / `myfprintf` – File format-based reading/writing.  

---

### **File System Management (`myfs.h`)**  

- **Initialization**:  
  - `mymount()` – Mounts the file system.  
  - `mymkfs(int size)` – Formats the file system.  

- **File Management**:  
  - `myopen()`, `myclose()` – Opens and closes files.  
  - `myread()`, `mywrite()` – Reads and writes file data.  
  - `mylseek()` – Repositions file offsets.  

- **Directory Management**:  
  - `myopendir()`, `myreaddir()`, `myclosedir()` – Manages directories.  

- **Utility Functions**:  
  - `find_empty_inode()`, `find_empty_block()` – Finds available space.  
  - `allocate_file()`, `set_filesize()`, `shorten_file()` – Manages files internally.  
  - `print_open_files()`, `print_fs()`, `print_directories()` – Displays file system structure.  

## File System Constants

    File Name Length: 32 characters
    Block Data Size: 512 bytes (standard Unix block size)
    Max files: as allocated in mymkfs() function.

Access Flags

    O_RDONLY – Read-only
    O_WRONLY – Write-only
    O_RDWR – Read and write
    O_CREAT – Create a new file if it doesn’t exist
