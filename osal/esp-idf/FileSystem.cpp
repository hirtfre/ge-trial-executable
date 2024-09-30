#include "FileSystem.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <AbstractionLayer/IFileSystemFactory.h>
#include <AbstractionLayer/IMemory.h>
#include <new>
#include <sys/stat.h>
#include <sys/unistd.h>

static const char *TAG = "OSAL_Filesystem";

namespace FileSystem {

FileSystem *fileSystemSingleton = nullptr;

IFileSystem *FileSystemFactory::Create()
{
    if (fileSystemSingleton != nullptr)
    {
        return nullptr;
    }

    auto memory = MEM_Calloc(1, sizeof(FileSystem));

    if (memory == nullptr)
    {
        return nullptr;
    }

    fileSystemSingleton = new (memory) FileSystem();

    if (fileSystemSingleton->Mount() == false)
    {
        goto mounting_file_system_failed;
    }

    return fileSystemSingleton;

mounting_file_system_failed:
    fileSystemSingleton->~FileSystem();
    MEM_Free(fileSystemSingleton);
    fileSystemSingleton = nullptr;
    return nullptr;
}

void FileSystemFactory::Destroy(IFileSystem **fileSystem)
{
    if (fileSystem == nullptr || *fileSystem == nullptr || fileSystemSingleton != *fileSystem)
    {
        return;
    }

    fileSystemSingleton->Unmount();

    fileSystemSingleton->~FileSystem();
    MEM_Free(fileSystemSingleton);

    *fileSystem = nullptr;
    fileSystemSingleton = nullptr;
}

bool FileSystem::Mount() const
{
    esp_vfs_spiffs_conf_t config = {
        .base_path = basePath,
        .partition_label = nullptr,
        .max_files = 2,
        .format_if_mount_failed = false,
    };

    esp_err_t err = esp_vfs_spiffs_register(&config);

    if (err == ESP_OK)
    {
        err = esp_spiffs_check(config.partition_label);
    }

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Successfully mounted SPIFFS file system.");
        TracePartitionInfo(config.partition_label);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to mount SPIFFS file system. Error: %s", esp_err_to_name(err));
        // @todo: define actin in case of error. should fs be formatted without notice?
        // ESP_LOGI(TAG, "Formatting SPIFFS file system.");
        // esp_spiffs_format(config.partition_label);
    }

    // @todo: periodically run the garbage collector 'esp_spiffs_gc()' to free up space

    return err == ESP_OK;
}

void FileSystem::TracePartitionInfo(const char *partitionLabel) const
{
    size_t total = 0;
    size_t used = 0;
    esp_err_t ret = esp_spiffs_info(partitionLabel, &total, &used);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Partition size: total: %d bytes, used: %d bytes", total, used);
    }
    else
    {
        ESP_LOGI(TAG, "Failed to read partition info");
    }
}

bool FileSystem::Unmount() const
{
    esp_err_t err = esp_vfs_spiffs_unregister(nullptr);

    return err == ESP_OK;
}

bool FileSystem::Open(std::string_view path, OpenFlag flags, FileHandle *handle)
{
    std::string filePathOnPartition = GetFilePathOnPartition(path);

    FILE *fileHandle = fopen(filePathOnPartition.c_str(), OpenFlagsToMode(flags));

    if (fileHandle == nullptr)
    {
        ESP_LOGE(TAG, "Failed to open file %s. %s (%d)", path.data(), strerror(errno), errno);
        handle = nullptr;
        return false;
    }

    *handle = static_cast<FileHandle>(fileHandle);

    return true;
}

bool FileSystem::Close(FileHandle handle)
{
    auto *fileHandle = static_cast<FILE *>(handle);

    bool success = fclose(fileHandle) == ESP_OK;

    if (!success)
    {
        ESP_LOGE(TAG, "Failed to close file. %s (%d)", strerror(errno), errno);
    }

    return success;
}

int32_t FileSystem::Read(FileHandle handle, Buffer &buffer)
{
    auto *fileHandle = static_cast<FILE *>(handle);

    size_t numBytes = fread(buffer.data, sizeof(char), buffer.length, fileHandle);

    if (ferror(fileHandle))
    {
        ESP_LOGE(TAG, "Failed to read. %s (%d)", strerror(errno), errno);
    }

    return (int32_t)numBytes;
}

bool FileSystem::Exists(std::string_view path)
{
    std::string filePathOnPartition = GetFilePathOnPartition(path);

    struct stat statusStruct;
    return stat(filePathOnPartition.c_str(), &statusStruct) == 0;
}

int32_t FileSystem::Write(FileHandle handle, const Buffer &buffer)
{
    auto *fileHandle = static_cast<FILE *>(handle);

    size_t numBytes = fwrite(buffer.data, sizeof(char), buffer.length, fileHandle);

    if (ferror(fileHandle))
    {
        ESP_LOGE(TAG, "Failed to write. %s (%d)", strerror(errno), errno);
    }

    return (int32_t)numBytes;
}

bool FileSystem::Flush(FileHandle handle)
{
    auto *fileHandle = static_cast<FILE *>(handle);

    bool success = fflush(fileHandle) == ESP_OK;

    if (!success)
    {
        ESP_LOGE(TAG, "Failed to flush. %s (%d)", strerror(errno), errno);
    }

    return success;
}

bool FileSystem::Remove(std::string_view path)
{
    std::string filePathOnPartition = GetFilePathOnPartition(path);

    bool success = unlink(filePathOnPartition.c_str()) == 0;

    if (!success)
    {
        ESP_LOGE(TAG, "Failed to remove file %s. %s (%d)", path.data(), strerror(errno), errno);
    }

    return success;
}

bool FileSystem::Move(std::string_view currentPath, std::string_view newPath)
{
    std::string currentFilePathOnPartition = GetFilePathOnPartition(currentPath);
    std::string newFilePathOnPartition = GetFilePathOnPartition(newPath);

    bool success =
        rename(currentFilePathOnPartition.c_str(), newFilePathOnPartition.c_str()) == ESP_OK;

    if (!success)
    {
        ESP_LOGE(TAG, "Failed to move file %s. %s (%d)", currentPath.data(), strerror(errno),
                 errno);
    }

    return success;
}

bool FileSystem::Seek(FileHandle handle, int32_t offset, SeekOffsetType whence)
{
    auto *fileHandle = static_cast<FILE *>(handle);

    int wrappedWhence = SEEK_CUR;

    switch (whence)
    {
        case SeekOffsetType::Set:
            wrappedWhence = SEEK_SET;
            break;

        case SeekOffsetType::Cursor:
            wrappedWhence = SEEK_CUR;
            break;

        case SeekOffsetType::End:
            wrappedWhence = SEEK_END;
            break;
    }

    bool success = fseek(fileHandle, offset, wrappedWhence) == ESP_OK;

    if (!success)
    {
        ESP_LOGE(TAG, "Seek failed. %s (%d)", strerror(errno), errno);
    }

    return success;
}

namespace FileMode {
constexpr const char *Read = "r";
constexpr const char *Write = "w";
constexpr const char *Append = "a";
constexpr const char *ReadWrite = "r+";
constexpr const char *WriteRead = "w+";
constexpr const char *AppendRead = "a+";
} // namespace FileMode

const char *FileSystem::OpenFlagsToMode(OpenFlag flags) const
{
    const char *mode = FileMode::Read;

    if (flags == OpenFlag::Read)
    {
        mode = FileMode::Read;
    }
    else if (flags == OpenFlag::Write)
    {
        mode = FileMode::Write;
    }
    else if (flags == OpenFlag::Append)
    {
        mode = FileMode::Append;
    }
    else if ((flags & OpenFlag::Append) && (flags & OpenFlag::Read))
    {
        mode = FileMode::AppendRead;
    }
    else if ((flags & OpenFlag::Write) && (flags & OpenFlag::Read) && (flags & OpenFlag::Truncate))
    {
        mode = FileMode::WriteRead;
    }
    else if ((flags & OpenFlag::Write) && (flags & OpenFlag::Read))
    {
        mode = FileMode::ReadWrite;
    }

    return mode;
}

std::string FileSystem::GetFilePathOnPartition(std::string_view filePath) const
{
    std::string_view::size_type firstNonSeparatorPosition = filePath.find_first_not_of('/');
    if (firstNonSeparatorPosition == std::string_view::npos)
    {
        return std::string(basePath) + "/";
    }

    return std::string(basePath) + "/" + std::string(filePath.substr(firstNonSeparatorPosition));
}

} // namespace FileSystem
