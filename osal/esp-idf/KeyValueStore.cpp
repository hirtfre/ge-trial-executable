#include "KeyValueStore.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <AbstractionLayer/IKeyValueStoreFactory.h>
#include <AbstractionLayer/IMemory.h>
#include <array>
#include <new>
#include <string.h>

namespace KeyValueStore {

constexpr std::array<const char *, static_cast<size_t>(Key::Count)> keyString = {
    "Runtime",
};

constexpr const char *GetKeyString(Key key)
{
    return keyString[static_cast<size_t>(key)];
}

template <Key K> struct KeyStringValidator
{
    static_assert(strlen(GetKeyString(K)) <= 15,
                  "Key string is too long, maximum length is 15 characters!");
    static_assert(GetKeyString(K)[strlen(GetKeyString(K))] == '\0',
                  "Key string is not zero-terminated!");
};

template struct KeyStringValidator<Key::Runtime>;

bool KeyValueStore::GetValue(Key key, int32_t &value)
{
    if (nvs_get_i32(nvsHandle, GetKeyString(key), &value) != ESP_OK)
    {
        return false;
    }

    return true;
}

bool KeyValueStore::SetValue(Key key, const int32_t &value)
{
    if (nvs_set_i32(nvsHandle, GetKeyString(key), value) != ESP_OK)
    {
        return false;
    }

    return true;
}

bool KeyValueStore::EraseAll()
{
    if (nvs_erase_all(nvsHandle) != ESP_OK)
    {
        return false;
    }

    return true;
}

nvs_handle_t KeyValueStore::GetNvsHandle() const
{
    return nvsHandle;
}

KeyValueStore *singleton = nullptr;

IKeyValueStore *KeyValueStoreFactory::Create()
{
    if (singleton != nullptr)
    {
        return singleton;
    }

    auto memory = MEM_Calloc(1, sizeof(KeyValueStore));

    if (memory == nullptr)
    {
        goto unable_to_allocate_memory;
    }

    nvs_handle_t nvsHandle;

    if (nvs_open("GE", NVS_READWRITE, &nvsHandle) != ESP_OK)
    {
        goto unable_to_open_nvs;
    }

    singleton = new (memory) KeyValueStore(nvsHandle);

    return singleton;

    nvs_close(nvsHandle);

unable_to_open_nvs:
    MEM_Free(memory);

unable_to_allocate_memory:
    return nullptr;
}

void KeyValueStoreFactory::Destroy(IKeyValueStore **store)
{
    if (store == nullptr || *store == nullptr || singleton != *store)
    {
        return;
    }

    auto nvsHandle = singleton->GetNvsHandle();

    singleton->~KeyValueStore();
    MEM_Free(singleton);

    nvs_close(nvsHandle);

    *store = nullptr;
    singleton = nullptr;
}

} // namespace KeyValueStore