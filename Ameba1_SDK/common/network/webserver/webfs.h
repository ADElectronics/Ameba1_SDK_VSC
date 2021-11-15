/*********************************************************************
 *
 * FileName:        webfs.h
 * Basis of MPFS2 (Microchip File System).
 * WEBFS has its differences Based.
 *
 ********************************************************************/
#ifndef _WEBFS_H_
#define _WEBFS_H_

#ifndef WEBFS_DISK_FADDR
#define WEBFS_DISK_FADDR         0x0D0000
#endif

// заголовок WEBFiles.bin
#define WEBFS_DISK_ID            0x42455746
#define WEBFS_DISK_VER           0x0001

#ifndef WEBFS_FILENAME_LEN_MAX
#define WEBFS_FILENAME_LEN_MAX   64
#endif

#ifndef WEBFS_OPENFILES_MAX
#define WEBFS_OPENFILES_MAX      31
#endif

#define WEBFS_FLAG_ISZIPPED      0x0001  // Indicates a file is compressed with GZIP compression
#define WEBFS_FLAG_HASINDEX      0x0002  // Indicates a file has an associated index of dynamic variables
#define WEBFS_INVALID            0xffffffff
#define WEBFS_INVALID_FAT        0xffff
#define WEBFS_INVALID_HANDLE     0xff

typedef uint32_t webfs_pntr_t;
typedef uint8_t webfs_handle_t;

typedef struct
{
   webfs_pntr_t addr;
   uint32_t bytesRem;
#ifdef USE_MAX_IRAM
   uint32_t fatID;
#else
   uint16_t fatID;
#endif
} webfs_stub_t;

typedef enum
{
   WEBFS_SEEK_START = 0,   // Seek forwards from the front of the file
   WEBFS_SEEK_END,         // Seek backwards from the end of the file
   WEBFS_SEEK_FORWARD,     // Seek forward from the current position
   WEBFS_SEEK_REWIND       // See backwards from the current position
} WEBFS_SEEK_MODE;

typedef struct __attribute__((packed))
{
   uint32_t id;
   uint16_t ver;
   uint16_t numFiles;
   uint32_t disksize;
} webfs_disk_header_t;

typedef struct  __attribute__((packed))
{
   uint32_t blksize;
   uint16_t headlen;
   uint16_t flags;
} webfs_file_header_t;

typedef struct
{
   uint32_t string;     // Pointer to the file name
   uint32_t data;       // Address of the file data
   uint32_t len;        // Length of file data
#ifdef USE_MAX_IRAM
   uint32_t  flags;
#else
   uint16_t  flags;
#endif
} webfs_fat_record_t;

extern volatile bool isWEBFSLocked;
extern uint16_t numFiles;
extern webfs_fat_record_t fatCache;
extern webfs_stub_t WEBFSStubs[WEBFS_OPENFILES_MAX + 1];
extern uint32_t disk_base_addr;

void webfs_Init(void);
webfs_handle_t webfs_Open(uint8_t* cFile);
void webfs_Close(webfs_handle_t hWEBFS);
uint16_t webfs_GetArray(webfs_handle_t hWEBFS, uint8_t* cData, uint16_t wLen);
bool webfs_Seek(webfs_handle_t hWEBFS, uint32_t dwOffset, WEBFS_SEEK_MODE tMode);
uint16_t webfs_GetFlags(webfs_handle_t hWEBFS);
uint32_t webfs_GetSize(webfs_handle_t hWEBFS);
uint32_t webfs_GetBytesRem(webfs_handle_t hWEBFS);
webfs_pntr_t webfs_GetStartAddr(webfs_handle_t hWEBFS);
webfs_pntr_t webfs_GetEndAddr(webfs_handle_t hWEBFS);
bool webfs_GetFilename(webfs_handle_t hWEBFS, uint8_t* cName, uint16_t wLen);
uint32_t webfs_GetPosition(webfs_handle_t hWEBFS);
uint32_t webfs_MaxSize(void);
uint32_t webfs_CurentSize(void);
uint32_t webfs_BaseAddr(void);

#endif // _WEBFS_H_
