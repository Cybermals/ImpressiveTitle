#include "GameConfig.h"
#include "MagixEncryption.h"
#include "OgreLogManager.h"
 
#include <zzip/zzip.h>
#include <zzip/plugin.h>
 
// Change this magic number to a value of your choosing.
static const char xor_values[10] = ENCRYPTION_XORS;
static zzip_plugin_io_handlers xor_handlers = { };
// Change "OBFUSZIP" to match the file extension of your choosing.
static zzip_strings_t xor_fileext[] = { ENCRYPTION_EXTENSION, 0 };
 
// Static method that un-obfuscates an obfuscated file.
static zzip_ssize_t xor_read (int fd, void* buf, zzip_size_t len)
{
	zzip_off_t y = tell(fd);
    const zzip_ssize_t bytes = read(fd, buf, len);
    zzip_ssize_t i;
    char* pch = (char*)buf;
    for (i=0; i<bytes; ++i)
    {
        pch[i] ^= xor_values[(y+i)&9];
    }
    return bytes;
}
 
/// Utility method to format out zzip errors
Ogre::String getZzipErrorDescription(zzip_error_t zzipError)
{
    Ogre::String errorMsg;
    switch (zzipError)
    {
    case ZZIP_NO_ERROR:
        break;
    case ZZIP_OUTOFMEM:
        errorMsg = "Out of memory.";
        break;
    case ZZIP_DIR_OPEN:
    case ZZIP_DIR_STAT:
    case ZZIP_DIR_SEEK:
    case ZZIP_DIR_READ:
        errorMsg = "Unable to read zip file.";
        break;
	case ZZIP_DIR_EDH_MISSING:
		errorMsg = "EDH Missing";
		break;
    case ZZIP_UNSUPP_COMPR:
        errorMsg = "Unsupported compression format.";
        break;
    case ZZIP_CORRUPTED:
        errorMsg = "Corrupted archive.";
        break;
    default:
        errorMsg = "Unknown error.";
        break;
    };
 
    return errorMsg;
}
 
//-----------------------------------------------------------------------
MagixEncryptionZip::MagixEncryptionZip(const Ogre::String& name, const Ogre::String& archType )
    : Archive(name, archType), mZzipDir(0)
{
    zzip_init_io(&xor_handlers, 0);
    xor_handlers.fd.read = &xor_read;
}
//-----------------------------------------------------------------------
MagixEncryptionZip::~MagixEncryptionZip()
{
    unload();
}
//-----------------------------------------------------------------------
void MagixEncryptionZip::load()
{
    if (!mZzipDir)
    {
        zzip_error_t zzipError;
        mZzipDir = zzip_dir_open_ext_io(mName.c_str(), &zzipError, xor_fileext, &xor_handlers);
        checkZzipError(zzipError, "opening encrypted file");
 
        // Cache names
        ZZIP_DIRENT zzipEntry;
        while (zzip_dir_read(mZzipDir, &zzipEntry))
        {
            Ogre::FileInfo info;
            info.archive = this;
            // Get basename / path
            Ogre::StringUtil::splitFilename(zzipEntry.d_name, info.basename, info.path);
            info.filename = zzipEntry.d_name;
            // Get sizes
            info.compressedSize = static_cast<size_t>(zzipEntry.d_csize);
            info.uncompressedSize = static_cast<size_t>(zzipEntry.st_size);
            // folder entries
            if (info.basename.empty())
            {
                info.filename = info.filename.substr (0, info.filename.length () - 1);
                Ogre::StringUtil::splitFilename(info.filename, info.basename, info.path);
                // Set compressed size to -1 for folders; anyway nobody will check
                // the compressed size of a folder, and if he does, its useless anyway
                info.compressedSize = size_t (-1);
            }
 
            mFileList.push_back(info);
 
        }
 
    }
}
//-----------------------------------------------------------------------
void MagixEncryptionZip::unload()
{
    if (mZzipDir)
    {
        zzip_dir_close(mZzipDir);
        mZzipDir = 0;
        mFileList.clear();
    }
}
//-----------------------------------------------------------------------
Ogre::DataStreamPtr MagixEncryptionZip::open(const Ogre::String& filename, bool readOnly) const
{
 
    // Format not used here (always binary)
    ZZIP_FILE* zzipFile =
        zzip_file_open(mZzipDir, filename.c_str(), ZZIP_ONLYZIP | ZZIP_CASELESS);
    if (!zzipFile)
    {
        int zerr = zzip_error(mZzipDir);
        Ogre::String zzDesc = getZzipErrorDescription((zzip_error_t)zerr);
		
        Ogre::LogManager::getSingleton().logMessage(
            mName + " - Unable to open file " + filename + ", error was '" + zzDesc + "'");
 
        // return null pointer
        return Ogre::DataStreamPtr();
    }
 
    // Get uncompressed size too
    ZZIP_STAT zstat;
    zzip_dir_stat(mZzipDir, filename.c_str(), &zstat, ZZIP_CASEINSENSITIVE);
 
    // Construct & return stream
    return Ogre::DataStreamPtr(OGRE_NEW MagixEncryptionZipDataStream(filename, zzipFile,  static_cast<size_t>(zstat.st_size)));
}
//-----------------------------------------------------------------------
Ogre::StringVectorPtr MagixEncryptionZip::list(bool recursive, bool dirs)
{
    Ogre::StringVectorPtr ret = Ogre::StringVectorPtr(OGRE_NEW_T(Ogre::StringVector, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);
 
    Ogre::FileInfoList::iterator i, iend;
    iend = mFileList.end();
    for (i = mFileList.begin(); i != iend; ++i)
        if ((dirs == (i->compressedSize == size_t (-1))) &&
            (recursive || i->path.empty()))
            ret->push_back(i->filename);
 
    return ret;
}
//-----------------------------------------------------------------------
Ogre::FileInfoListPtr MagixEncryptionZip::listFileInfo(bool recursive, bool dirs)
{
    Ogre::FileInfoList* fil = OGRE_NEW_T(Ogre::FileInfoList, Ogre::MEMCATEGORY_GENERAL)();
    Ogre::FileInfoList::const_iterator i, iend;
    iend = mFileList.end();
    for (i = mFileList.begin(); i != iend; ++i)
        if ((dirs == (i->compressedSize == size_t (-1))) &&
            (recursive || i->path.empty()))
            fil->push_back(*i);
 
    return Ogre::FileInfoListPtr(fil, Ogre::SPFM_DELETE_T);
}
//-----------------------------------------------------------------------
Ogre::StringVectorPtr MagixEncryptionZip::find(const Ogre::String& pattern, bool recursive, bool dirs)
{
    Ogre::StringVectorPtr ret = Ogre::StringVectorPtr(OGRE_NEW_T(Ogre::StringVector, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);
    // If pattern contains a directory name, do a full match
    bool full_match = (pattern.find ('/') != Ogre::String::npos) ||
                        (pattern.find ('\\') != Ogre::String::npos);
 
    Ogre::FileInfoList::iterator i, iend;
    iend = mFileList.end();
    for (i = mFileList.begin(); i != iend; ++i)
        if ((dirs == (i->compressedSize == size_t (-1))) &&
            (recursive || full_match || i->path.empty()))
            // Check basename matches pattern (zip is case insensitive)
            if (Ogre::StringUtil::match(full_match ? i->filename : i->basename, pattern, false))
                ret->push_back(i->filename);
 
    return ret;
}
//-----------------------------------------------------------------------
Ogre::FileInfoListPtr MagixEncryptionZip::findFileInfo(const Ogre::String& pattern,
    bool recursive, bool dirs)
{
    Ogre::FileInfoListPtr ret = Ogre::FileInfoListPtr(OGRE_NEW_T(Ogre::FileInfoList, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);
    // If pattern contains a directory name, do a full match
    bool full_match = (pattern.find ('/') != Ogre::String::npos) ||
                        (pattern.find ('\\') != Ogre::String::npos);
 
    Ogre::FileInfoList::iterator i, iend;
    iend = mFileList.end();
    for (i = mFileList.begin(); i != iend; ++i)
        if ((dirs == (i->compressedSize == size_t (-1))) &&
            (recursive || full_match || i->path.empty()))
            // Check name matches pattern (zip is case insensitive)
            if (Ogre::StringUtil::match(full_match ? i->filename : i->basename, pattern, false))
                ret->push_back(*i);
 
    return ret;
}
//-----------------------------------------------------------------------
bool MagixEncryptionZip::exists(const Ogre::String& filename)
{
    ZZIP_STAT zstat;
    int res = zzip_dir_stat(mZzipDir, filename.c_str(), &zstat, ZZIP_CASEINSENSITIVE);
 
    return (res == ZZIP_NO_ERROR);
 
}
//---------------------------------------------------------------------
time_t MagixEncryptionZip::getModifiedTime(const Ogre::String& filename)
{
    // Zziplib doesn't yet support getting the modification time of individual files
    // so just check the mod time of the zip itself
    struct stat tagStat;
    bool ret = (stat(mName.c_str(), &tagStat) == 0);
 
    if (ret)
    {
        return tagStat.st_mtime;
    }
    else
    {
        return 0;
    }
 
}
//-----------------------------------------------------------------------
void MagixEncryptionZip::checkZzipError(int zzipError, const Ogre::String& operation) const
{
    if (zzipError != ZZIP_NO_ERROR)
    {
        Ogre::String errorMsg = getZzipErrorDescription(static_cast<zzip_error_t>(zzipError));
 
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
            mName + " - error whilst " + operation + ": " + errorMsg,
            "MagixEncryptionZip::checkZzipError");
    }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
MagixEncryptionZipDataStream::MagixEncryptionZipDataStream(ZZIP_FILE* zzipFile, size_t uncompressedSize)
    : mZzipFile(zzipFile)
{
    mSize = uncompressedSize;
}
//-----------------------------------------------------------------------
MagixEncryptionZipDataStream::MagixEncryptionZipDataStream(const Ogre::String& name, ZZIP_FILE* zzipFile, size_t uncompressedSize)
    :DataStream(name), mZzipFile(zzipFile)
{
    mSize = uncompressedSize;
}
//-----------------------------------------------------------------------
MagixEncryptionZipDataStream::~MagixEncryptionZipDataStream()
{
    close();
}
//-----------------------------------------------------------------------
size_t MagixEncryptionZipDataStream::read(void* buf, size_t count)
{
    zzip_ssize_t r = zzip_file_read(mZzipFile, (char*)buf, count);
    if (r<0) {
        ZZIP_DIR *dir = zzip_dirhandle(mZzipFile);
        Ogre::String msg = zzip_strerror_of(dir);
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
                    mName+" - error from zziplib: "+msg,
                    "MagixEncryptionZipDataStream::read");
    }
    return (size_t) r;
}
//-----------------------------------------------------------------------
void MagixEncryptionZipDataStream::skip(long count)
{
    zzip_seek(mZzipFile, static_cast<zzip_off_t>(count), SEEK_CUR);
}
//-----------------------------------------------------------------------
void MagixEncryptionZipDataStream::seek( size_t pos )
{
    zzip_seek(mZzipFile, static_cast<zzip_off_t>(pos), SEEK_SET);
}
//-----------------------------------------------------------------------
size_t MagixEncryptionZipDataStream::tell(void) const
{
    return zzip_tell(mZzipFile);
}
//-----------------------------------------------------------------------
bool MagixEncryptionZipDataStream::eof(void) const
{
    return (zzip_tell(mZzipFile) >= static_cast<zzip_off_t>(mSize));
}
//-----------------------------------------------------------------------
void MagixEncryptionZipDataStream::close(void)
{
    if (mZzipFile != 0)
    {
        zzip_file_close(mZzipFile);
        mZzipFile = 0;
    }
}
//-----------------------------------------------------------------------
const Ogre::String& MagixEncryptionZipFactory::getType(void) const
{
    static Ogre::String name = "KITOZIP";
    return name;
}