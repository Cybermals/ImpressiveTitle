#ifndef MagixEncryption_h
#define MagixEncryption_h
 
#include "OgreArchive.h"
#include "OgreArchiveFactory.h"
 
// Forward declaration for zziplib to avoid header file dependency.
typedef struct zzip_dir ZZIP_DIR;
typedef struct zzip_file ZZIP_FILE;
 
/** Specialisation of the Archive class to allow reading of files from an
    obfuscated zip format source archive.
@remarks
    This archive format supports obfuscated zip archives.
*/
class MagixEncryptionZip : public Ogre::Archive 
{
protected:
    /// Handle to root zip file
    ZZIP_DIR* mZzipDir;
    /// Handle any errors from zzip
    void checkZzipError(int zzipError, const Ogre::String& operation) const;
    /// File list (since zziplib seems to only allow scanning of dir tree once)
    Ogre::FileInfoList mFileList;
 
	OGRE_AUTO_MUTEX
public:
    MagixEncryptionZip(const Ogre::String& name, const Ogre::String& archType );
    ~MagixEncryptionZip();
    /// @copydoc Archive::isCaseSensitive
    bool isCaseSensitive(void) const { return false; }
 
    /// @copydoc Archive::load
    void load();
    /// @copydoc Archive::unload
    void unload();
 
    /// @copydoc Archive::open
    Ogre::DataStreamPtr open(const Ogre::String& filename, bool readOnly = true) const;

	/// @copydoc Archive::create
	Ogre::DataStreamPtr create(const Ogre::String& filename) const;

	/// @copydoc Archive remove
	void remove(const Ogre::String& filename) const;
 
    /// @copydoc Archive::list
    Ogre::StringVectorPtr list(bool recursive = true, bool dirs = false);
 
    /// @copydoc Archive::listFileInfo
    Ogre::FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);
 
    /// @copydoc Archive::find
    Ogre::StringVectorPtr find(const Ogre::String& pattern, bool recursive = true,
        bool dirs = false);
 
    /// @copydoc Archive::findFileInfo
    Ogre::FileInfoListPtr findFileInfo(const Ogre::String& pattern, bool recursive = true,
        bool dirs = false) const;
 
    /// @copydoc Archive::exists
    bool exists(const Ogre::String& filename);
 
    /// @copydoc Archive::getModifiedTime
    time_t getModifiedTime(const Ogre::String& filename);
};
 
/** Specialisation of ArchiveFactory for Obfuscated Zip files. */
class MagixEncryptionZipFactory : public Ogre::ArchiveFactory
{
public:
    virtual ~MagixEncryptionZipFactory() {}
    /// @copydoc FactoryObj::getType
    const Ogre::String& getType(void) const;
    /// @copydoc FactoryObj::createInstance
    Ogre::Archive *createInstance( const Ogre::String& name ) 
    {
        // Change "OBFUSZIP" to match the file extension of your choosing.
        return new MagixEncryptionZip(name, "KITOZIP");
    }
    /// @copydoc FactoryObj::destroyInstance
    void destroyInstance( Ogre::Archive* arch) { OGRE_DELETE arch; }
};

/** Template version of cache based on static array.
'cacheSize' defines size of cache in bytes. */
template <size_t cacheSize>
class StaticCache
{
protected:
	/// Static buffer
	char mBuffer[cacheSize];

	/// Number of bytes valid in cache (written from the beginning of static buffer)
	size_t mValidBytes;
	/// Current read position
	size_t mPos;

public:
	/// Constructor
	StaticCache()
	{
		mValidBytes = 0;
		mPos = 0;
		memset(mBuffer, 0, cacheSize);
	}

	/** Cache data pointed by 'buf'. If 'count' is greater than cache size, we cache only last bytes.
	Returns number of bytes written to cache. */
	size_t cacheData(const void* buf, size_t count)
	{
		assert(avail() == 0 && "It is assumed that you cache data only after you have read everything.");

		if (count < cacheSize)
		{
			// number of bytes written is less than total size of cache
			if (count + mValidBytes <= cacheSize)
			{
				// just append
				memcpy(mBuffer + mValidBytes, buf, count);
				mValidBytes += count;
			}
			else
			{
				size_t begOff = count - (cacheSize - mValidBytes);
				// override old cache content in the beginning
				memmove(mBuffer, mBuffer + begOff, mValidBytes - begOff);
				// append new data
				memcpy(mBuffer + cacheSize - count, buf, count);
				mValidBytes = cacheSize;
			}
			mPos = mValidBytes;
			return count;
		}
		else
		{
			// discard all
			memcpy(mBuffer, (const char*)buf + count - cacheSize, cacheSize);
			mValidBytes = mPos = cacheSize;
			return cacheSize;
		}
	}
	/** Read data from cache to 'buf' (maximum 'count' bytes). Returns number of bytes read from cache. */
	size_t read(void* buf, size_t count)
	{
		size_t rb = avail();
		rb = (rb < count) ? rb : count;
		memcpy(buf, mBuffer + mPos, rb);
		mPos += rb;
		return rb;
	}

	/** Step back in cached stream by 'count' bytes. Returns 'true' if cache contains resulting position. */
	bool rewind(size_t count)
	{
		if (mPos < count)
		{
			clear();
			return false;
		}
		else
		{
			mPos -= count;
			return true;
		}
	}
	/** Step forward in cached stream by 'count' bytes. Returns 'true' if cache contains resulting position. */
	bool ff(size_t count)
	{
		if (avail() < count)
		{
			clear();
			return false;
		}
		else
		{
			mPos += count;
			return true;
		}
	}

	/** Returns number of bytes available for reading in cache after rewinding. */
	size_t avail() const
	{
		return mValidBytes - mPos;
	}

	/** Clear the cache */
	void clear()
	{
		mValidBytes = 0;
		mPos = 0;
	}
};

 
/** Specialisation of DataStream to handle streaming data from zip archives. */
class MagixEncryptionZipDataStream : public Ogre::DataStream
{
protected:
    ZZIP_FILE* mZzipFile;
	/// We need caching because sometimes serializers step back in data stream and zziplib behaves slow
	StaticCache<2 * OGRE_STREAM_TEMP_SIZE> mCache;
public:
    /// Unnamed constructor
    MagixEncryptionZipDataStream(ZZIP_FILE* zzipFile, size_t uncompressedSize);
    /// Constructor for creating named streams
    MagixEncryptionZipDataStream(const Ogre::String& name, ZZIP_FILE* zzipFile, size_t uncompressedSize);
    ~MagixEncryptionZipDataStream();
    /// @copydoc DataStream::read
    size_t read(void* buf, size_t count);
	/// @copydoc DataStream::write
	size_t write(void* buf, size_t count);
    /// @copydoc DataStream::skip
    void skip(long count);
    /// @copydoc DataStream::seek
    void seek( size_t pos );
    /// @copydoc DataStream::seek
    size_t tell(void) const;
    /// @copydoc DataStream::eof
    bool eof(void) const;
    /// @copydoc DataStream::close
    void close(void);
};
#endif