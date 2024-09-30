#ifndef OSAL_ESP_IDF_FILE_SYSTEM
#define OSAL_ESP_IDF_FILE_SYSTEM

#include <AbstractionLayer/IFileSystem.h>
#include <string.h>

namespace FileSystem {

class FileSystem : public IFileSystem
{
  public:
    FileSystem() = default;
    ~FileSystem() override = default;

    FileSystem &operator=(FileSystem &&) = delete;

    bool Open(std::string_view path, OpenFlag flags, FileHandle *handle) override;
    bool Close(FileHandle handle) override;
    bool Exists(std::string_view path) override;
    int32_t Read(FileHandle handle, Buffer &buffer) override;
    bool Seek(FileHandle handle, int32_t offset, SeekOffsetType whence) override;
    int32_t Write(FileHandle handle, const Buffer &buffer) override;
    bool Flush(FileHandle handle) override;
    bool Remove(std::string_view path) override;
    bool Move(std::string_view currentPath, std::string_view newPath) override;

    bool Mount() const;
    bool Unmount() const;

  private:
    void TracePartitionInfo(const char *partitionLabel) const;
    const char *OpenFlagsToMode(OpenFlag flags) const;
    std::string GetFilePathOnPartition(std::string_view filePath) const;

    const char *basePath = "/spiffs";
};

} // namespace FileSystem

#endif /* OSAL_ESP_IDF_FILE_SYSTEM */
