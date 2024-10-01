#ifndef OSAL_ESP_IDF_KEY_VALUE_STORE
#define OSAL_ESP_IDF_KEY_VALUE_STORE

#include "nvs_handle.hpp"
#include <AbstractionLayer/IKeyValueStore.h>

namespace KeyValueStore {

class KeyValueStore : public IKeyValueStore
{
  public:
    explicit KeyValueStore(nvs_handle_t pNvsHandle) : nvsHandle(pNvsHandle) {};
    ~KeyValueStore() final = default;

    KeyValueStore &operator=(KeyValueStore &&) = delete;

    bool GetValue(Key key, int32_t &value) final;
    bool SetValue(Key key, const int32_t &value) final;
    bool EraseAll() final;

    nvs_handle_t GetNvsHandle() const;

  private:
    nvs_handle_t nvsHandle;
};

} // namespace KeyValueStore


#endif /* OSAL_ESP_IDF_KEY_VALUE_STORE */
