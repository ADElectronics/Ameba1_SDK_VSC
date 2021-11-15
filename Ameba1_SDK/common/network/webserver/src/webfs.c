/*********************************************************************
 * webfs.c
 * RTL871x Flash WEB File System v1.0
 ********************************************************************/
#include <autoconf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include <osdep_service.h>
#include "device_lock.h"
#include "flash_utils.h"
#include "web_utils.h"
#include "webfs.h"

uint32_t disk_base_addr;

/*
 *
 * Structure:
 *
 *     [F][W][E][B][uint8 Ver Hi][uint8 Ver Lo] // заголовок диска
 *     [uint16 Number of Files] // кол-во файлов на диске
 *     [Name Hash 0]...[Name Hash N] // uint16 типа хеш на каждое имя файла
 *     [File Record 0]...[File Record N] // uint32 указатели на адреса структур файлов, относительно начала диска
 *
 * File Record Structure:
 *     [uint32 Len] размер файла с заголовком
 *     [uint16 HeadLen] длина заголовка, включая размер, флаг, имя (адрес данных - адрес позиции len)
 *     [uint16 Flags] бит 0 =1 - файл сжат GZIP, бит 1 = 1 - парсится - имеет динамические переменные
 *     [File Name, 0] Имя файла с "СИ" терминатором
 *     [File Data] данные файла
 *
 * Name hash (2 uint8s) is calculated as follows:
 *     hash = 0
 *     for each(uint8 in name)
 *	 hash += uint8
 *	 hash <<= 1
 *
 *     Technically this means the hash only includes the
 *     final 15 characters of a name.
 *
 * String FileNmae Structure (1 to 64 uint8s):
 *     ["path/to/file.ext"][0x00]
 *
 *
 * Current version is 1.0
 */

// Lock WEBFS access during the upgrade
volatile bool isWEBFSLocked;
// Track the WEBFS File Handles
// WEBFSStubs[0] is reserved for internal use (FAT access)
webfs_stub_t WEBFSStubs[WEBFS_OPENFILES_MAX + 1]; // + HANDLE = 0
// FAT record cache
webfs_fat_record_t fatCache;
// ID of currently loaded fatCache
static uint32_t fatCacheID;
// Number of files in this WEBFS image
uint16_t numFiles;

static void webfs_GetFATRecord(uint16_t fatID);
static void webfs_Update(void);

/*****************************************************************************
   Function:
      void webfs_Init(void)
   Description:
      Web Disk Init
 *****************************************************************************/
void webfs_Init(void)
{
   disk_base_addr = webfs_BaseAddr();
   memset((char *) &WEBFSStubs, 0xff, sizeof(WEBFSStubs));
   // Validate the image and load numFiles
   webfs_Update();
#if WEBSERVER_DEBUG_EN > 0
   os_printf("\nDisk init: %d files, addr = %p\n", numFiles, disk_base_addr);
#endif
   // тут надо расчет контрольки тела диска или другой контроль...
   isWEBFSLocked = (numFiles == 0) ? TRUE : FALSE;
}

/*****************************************************************************
  Function:
	webfs_handle_t webfs_Open(uint8_t* cFile)

  Description:
	Opens a file in the WEBFS2 file system.

  Precondition:
	None

  Parameters:
	cFile - a null terminated file name to open

  Returns:
	An webfs_handle_t to the opened file if found, or WEBFS_INVALID_HANDLE
	if the file could not be found or no free handles exist.
  ***************************************************************************/
webfs_handle_t webfs_Open(uint8_t* cFile)
{
	webfs_handle_t hWEBFS;
	uint16_t nameHash;
	int i, len = 0; 
	uint16_t hashCache[16];
	uint8_t bufname[WEBFS_FILENAME_LEN_MAX];
	uint8_t *ptr;

	// Make sure WEBFS is unlocked and we got a filename
	if(*cFile == '\0' || isWEBFSLocked == TRUE)
		return WEBFS_INVALID_HANDLE;

	// Calculate the name hash to speed up searching
	for(nameHash = 0, ptr = cFile; *ptr != '\0'; ptr++)
	{
		nameHash += *ptr;
		nameHash <<= 1;
		len++;
	}
	// Find a free file handle to use
	for(hWEBFS = 1; hWEBFS <= WEBFS_OPENFILES_MAX; hWEBFS++)
		if(WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) break;
	if(hWEBFS == WEBFS_OPENFILES_MAX)
		return WEBFS_INVALID_HANDLE;
	// Read in hashes, and check remainder on a match.  Store 8 in cache for performance
	for(i = 0; i < numFiles; i++) {
		// For new block of 8, read in data
		if((i & 0x0F) == 0)	{
			WEBFSStubs[0].addr = 12 + i*2;
			WEBFSStubs[0].bytesRem = 32;
			webfs_GetArray(0, (uint8_t*)hashCache, 32);
		}
		// If the hash matches, compare the full filename
		if(hashCache[i&0x0F] == nameHash)
		{
			webfs_GetFATRecord(i);
			// filename comparison
			WEBFSStubs[0].addr = fatCache.string;
			WEBFSStubs[0].bytesRem = WEBFS_FILENAME_LEN_MAX;
			webfs_GetArray(0, bufname, WEBFS_FILENAME_LEN_MAX);
			if(strncmp(cFile, bufname, len) == 0) { // Filename matches, so return TRUE
				WEBFSStubs[hWEBFS].addr = fatCache.data;
				WEBFSStubs[hWEBFS].bytesRem = fatCache.len;
				WEBFSStubs[hWEBFS].fatID = i;
				return hWEBFS;
			}
		}
	}
	// No file name matched, so return nothing
	return WEBFS_INVALID_HANDLE;
}

/*****************************************************************************
  Function:	void webfs_Close(webfs_handle_t hWEBFS)
  Summary: Closes a file.
  Returns:	None
  ***************************************************************************/
void webfs_Close(webfs_handle_t hWEBFS)
{
   if(hWEBFS != 0 && hWEBFS <= WEBFS_OPENFILES_MAX)
      WEBFSStubs[hWEBFS].addr = WEBFS_INVALID;
}

/*****************************************************************************
  Function: uint16_t webfs_GetArray(webfs_handle_t hWEBFS, uint8_t* cData, uint16_t wLen)
  Description: Reads a series of uint8s from a file.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters:
	hWEBFS - the file handle from which to read
	cData - where to store the uint8_ts that were read
	wLen - how many uint8_ts to read
  Returns:
	The number of uint8s successfully read.  If this is less than wLen,
	an EOF occurred while attempting to read.
  ***************************************************************************/
uint16_t  webfs_GetArray(webfs_handle_t hWEBFS, uint8_t* cData, uint16_t wLen)
{
   // Make sure we're reading a valid address
   if(hWEBFS > WEBFS_OPENFILES_MAX) return 0;
   // Determine how many we can actually read
   if(wLen > WEBFSStubs[hWEBFS].bytesRem) wLen = WEBFSStubs[hWEBFS].bytesRem;
   // Make sure we're reading a valid address
   if(WEBFSStubs[hWEBFS].addr == WEBFS_INVALID || wLen == 0) return 0;

   if(cData != NULL) // Read the data
   {
      _flash_mutex_lock();
      //_flash_read(WEBFSStubs[hWEBFS].addr + disk_base_addr, wLen, cData);
      flash_stream_read(&flashobj, WEBFSStubs[hWEBFS].addr + disk_base_addr, wLen, cData);
      _flash_mutex_unlock();
   };
   WEBFSStubs[hWEBFS].addr += wLen;
   WEBFSStubs[hWEBFS].bytesRem -= wLen;
   return wLen;
}

/*****************************************************************************
  Function:	
   bool webfs_Seek(webfs_handle_t hWEBFS, uint32 dwOffset, WEBFS_SEEK_MODE tMode)
  Description: Moves the current read pointer to a new location.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters:
	hWEBFS - the file handle to seek with
	dwOffset - offset from the specified position in the specified direction
	tMode - one of the WEBFS_SEEK_MODE constants
  Returns:
	TRUE - the seek was successful
	FALSE - either the new location or the handle itself was invalid
  ***************************************************************************/
bool webfs_Seek(webfs_handle_t hWEBFS, uint32_t dwOffset, WEBFS_SEEK_MODE tMode)
{
	uint32_t temp;

	// Make sure a valid file is open
	if(hWEBFS > WEBFS_OPENFILES_MAX || WEBFSStubs[hWEBFS].addr == WEBFS_INVALID)
		return FALSE;

	switch(tMode)
	{
		// Seek offset uint8s from start
		case WEBFS_SEEK_START:
			temp = webfs_GetSize(hWEBFS);
			if(dwOffset > temp)
				return FALSE;

			WEBFSStubs[hWEBFS].addr = webfs_GetStartAddr(hWEBFS) + dwOffset;
			WEBFSStubs[hWEBFS].bytesRem = temp - dwOffset;
			return TRUE;

		// Seek forwards offset uint8s
		case WEBFS_SEEK_FORWARD:
			if(dwOffset > WEBFSStubs[hWEBFS].bytesRem)
				return FALSE;

			WEBFSStubs[hWEBFS].addr += dwOffset;
			WEBFSStubs[hWEBFS].bytesRem -= dwOffset;
			return TRUE;

		// Seek backwards offset uint8s
		case WEBFS_SEEK_REWIND:
			temp = webfs_GetStartAddr(hWEBFS);
			if(WEBFSStubs[hWEBFS].addr < temp + dwOffset)
				return FALSE;

			WEBFSStubs[hWEBFS].addr -= dwOffset;
			WEBFSStubs[hWEBFS].bytesRem += dwOffset;
			return TRUE;

		// Seek so that offset uint8s remain in file
		case WEBFS_SEEK_END:
			temp = webfs_GetSize(hWEBFS);
			if(dwOffset > temp)
				return FALSE;

			WEBFSStubs[hWEBFS].addr = webfs_GetEndAddr(hWEBFS) - dwOffset;
			WEBFSStubs[hWEBFS].bytesRem = dwOffset;
			return TRUE;

		default:
			return FALSE;
	}
}

/*****************************************************************************
  Function:	static void webfs_GetFATRecord(uint16_t fatID)
  Description: 	Loads the FAT record for a specified handle.
  Precondition:	None
  Parameters:	fatID - the ID of the file whose FAT is to be loaded
  Returns:	None
  Remarks:	The FAT record will be stored in fatCache.
  ***************************************************************************/
static void webfs_GetFATRecord(uint16_t fatID)
{
   webfs_file_header_t fhead;
   if(fatID == fatCacheID || fatID >= numFiles)
      return;
   // Read the FAT record to the cache
   WEBFSStubs[0].bytesRem = sizeof(fhead) + 4;
   WEBFSStubs[0].addr = 12 + numFiles*2 + fatID *4;
   webfs_GetArray(0, (uint8_t *)&fatCache.data, 4);
   WEBFSStubs[0].addr = fatCache.data;
   webfs_GetArray(0, (uint8_t *)&fhead, sizeof(fhead));
   fatCache.len = fhead.blksize - fhead.headlen;
   fatCache.string = fatCache.data + 8;
   fatCache.flags = fhead.flags;
   fatCache.data = fatCache.data + fhead.headlen;
   fatCacheID = fatID;
}

/*****************************************************************************
  Function:	uint16_t webfs_GetFlags(webfs_handle_t hWEBFS)
  Description: Reads a file's flags.
  Precondition:	The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle from which to read the metadata
  Returns: The flags that were associated with the file
  ***************************************************************************/
uint16_t webfs_GetFlags(webfs_handle_t hWEBFS)
{
   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX	|| WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) 
      return 0;

   //move to the point for reading
   webfs_GetFATRecord(WEBFSStubs[hWEBFS].fatID);
   return fatCache.flags;
}

/*****************************************************************************
  Function: uint32 webfs_GetSize(webfs_handle_t hWEBFS)
  Description: Reads the size of a file.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle from which to read the metadata
  Returns: The size that was read as a uint32
  ***************************************************************************/
uint32_t webfs_GetSize(webfs_handle_t hWEBFS)
{
   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX	|| WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) 
      return 0;

   // Move to the point for reading
   webfs_GetFATRecord(WEBFSStubs[hWEBFS].fatID);
   return fatCache.len;
}

/*****************************************************************************
  Function: uint32 webfs_GetBytesRem(webfs_handle_t hWEBFS)
  Description: Determines how many uint8s remain to be read.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle from which to read the metadata
  Returns: The number of uint8s remaining in the file as a uint32
  ***************************************************************************/
uint32_t webfs_GetBytesRem(webfs_handle_t hWEBFS)
{
   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX || WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) 
      return 0;
   return WEBFSStubs[hWEBFS].bytesRem;
}

/*****************************************************************************
  Function:	webfs_pntr_t webfs_GetStartAddr(webfs_handle_t hWEBFS)
  Description: Reads the starting address of a file.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle from which to read the metadata
  Returns: The starting address of the file in the WEBFS image
  ***************************************************************************/
webfs_pntr_t webfs_GetStartAddr(webfs_handle_t hWEBFS)
{
   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX || WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) 
      return 0;
   // Move to the point for reading
   webfs_GetFATRecord(WEBFSStubs[hWEBFS].fatID);
   return fatCache.data;
}

/*****************************************************************************
  Function: webfs_pntr_t webfs_GetEndAddr(webfs_handle_t hWEBFS)
  Description: Determines the ending address of a file.
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle from which to read the metadata
  Returns:	The address just after the file ends (start address of next file)
  ***************************************************************************/
webfs_pntr_t webfs_GetEndAddr(webfs_handle_t hWEBFS)
{
   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX || WEBFSStubs[hWEBFS].addr == WEBFS_INVALID) 
      return WEBFS_INVALID;
   // Move to the point for reading
   webfs_GetFATRecord(WEBFSStubs[hWEBFS].fatID);
   return fatCache.data + fatCache.len;
}

/*****************************************************************************
  Function: bool webfs_GetFilename(webfs_handle_t hWEBFS, uint8* cName, uint16_t wLen)
  Description: Reads the file name of a file that is already open.
  Precondition:	The file handle referenced by hWEBFS is already open.
  Parameters: 
    hWEBFS - the file handle from which to determine the file name
	cName - where to store the name of the file
	wLen - the maximum length of data to store in cName
  Returns:
	TRUE - the file name was successfully located
	FALSE - the file handle provided is not currently open
  ***************************************************************************/
bool webfs_GetFilename(webfs_handle_t hWEBFS, uint8_t* cName, uint16_t wLen)
{
   uint32_t addr;

   // Make sure a valid file is open
   if(hWEBFS > WEBFS_OPENFILES_MAX	|| WEBFSStubs[hWEBFS].addr == WEBFS_INVALID)
      return FALSE;

   // Move to the point for reading
   webfs_GetFATRecord(WEBFSStubs[hWEBFS].fatID);
   addr = fatCache.string;
   WEBFSStubs[0].addr = addr;
   WEBFSStubs[0].bytesRem = 255;

   // Read the value and return
   webfs_GetArray(0, cName, wLen);
   return TRUE;
}

/*****************************************************************************
  Function: uint32 webfs_GetPosition(webfs_handle_t hWEBFS)
  Description: Determines the current position in the file
  Precondition: The file handle referenced by hWEBFS is already open.
  Parameters: hWEBFS - the file handle for which to determine position
  Returns: The position in the file as a uint32 (or webfs_pntr_t)
  ***************************************************************************/
uint32_t  webfs_GetPosition(webfs_handle_t hWEBFS)
{
   return WEBFSStubs[hWEBFS].addr - webfs_GetStartAddr(hWEBFS);
}

/*****************************************************************************
  Function:	void webfs_Update(void)
  Summary: Validates the WEBFS Image
  Description: Verifies that the WEBFS image is valid, and reads the number of
	available files from the image header.  This function is called on
	boot, and again after any image is written.
  Parameters: None
  Returns: None
  ***************************************************************************/
static void webfs_Update(void)
{
   webfs_disk_header_t dhead;
   WEBFSStubs[0].addr = 0;
   WEBFSStubs[0].bytesRem = sizeof(dhead);
   webfs_GetArray(0, (uint8_t*)&dhead, sizeof(dhead));
   if(dhead.id == WEBFS_DISK_ID && dhead.ver == WEBFS_DISK_VER)
   {
      numFiles = dhead.numFiles;
   }
   else
      numFiles = 0;
   fatCacheID = WEBFS_INVALID_FAT;
}

/****************************************************************************
 * webfs_MaxSize()
 ***************************************************************************/
uint32_t webfs_MaxSize(void)
{
   return flash_get_size() - WEBFS_DISK_FADDR;
}

/****************************************************************************
 * WEBFS_size()
 ***************************************************************************/
uint32_t webfs_CurentSize(void)
{
   uint32_t size = 0;
   flash_mutex_lock();
   if(numFiles)
      size = _flash_read_dword(disk_base_addr + 8);
   flash_mutex_unlock();
   return size;
}

/****************************************************************************
 * WEBFS_size()
 ***************************************************************************/
uint32_t webfs_BaseAddr(void)
{
   return WEBFS_DISK_FADDR;
}
