#include "Json.h"
#include <AbstractionLayer/IJsonFactory.h>
#include <AbstractionLayer/IMemory.h>
#include <esp_log.h>
#include <new>

namespace Json {

static constexpr char objectDelimiter = '.';
static constexpr uint8_t maxObjectDepth = 1;

IJson *JsonFactory::Create()
{
    auto memory = MEM_Calloc(1, sizeof(Json));

    if (memory == nullptr)
    {
        return nullptr;
    }

    return new (memory) Json();
}

void JsonFactory::Destroy(IJson **json)
{
    if (json == nullptr || *json == nullptr)
    {
        return;
    }

    auto *instance = static_cast<Json *>(*json);

    instance->Close();

    instance->~Json();
    MEM_Free(instance);

    *json = nullptr;
}

bool Json::Open(const JsonString &string)
{
    bool success = json_parse_start_static(&jsonContext, string.data, string.size,
                                           tokenBuffer.data(), maxTokenCount) == OS_SUCCESS;
    return success;
}

void Json::Close()
{
    json_parse_end_static(&jsonContext);
}

template <typename T> bool Json::GetValue(std::string_view key, T &value)
{
    bool success = true;

    if (IsKeyWithinObject(key))
    {
        success = EnterObject(GetObjectName(key));
    }

    success = success && GetValueFromJsonObject(key, value);

    if (IsKeyWithinObject(key))
    {
        ExitObject();
    }

    return success;
}

template <typename T> bool Json::GetValueFromJsonObject(std::string_view key, T &value)
{
    static_assert(std::is_same<T, int32_t>::value || std::is_same<T, uint16_t>::value ||
                      std::is_same<T, uint8_t>::value,
                  "Type not supported");
    int jsonValue;

    auto success =
        json_obj_get_int(&jsonContext, GetKeyName(key).c_str(), &jsonValue) == OS_SUCCESS;

    value = static_cast<T>(jsonValue);

    return success;
}

template <> bool Json::GetValueFromJsonObject<bool>(std::string_view key, bool &value)
{
    return json_obj_get_bool(&jsonContext, GetKeyName(key).c_str(), &value) == OS_SUCCESS;
}

bool Json::GetValue(std::string_view key, uint8_t &value)
{
    return GetValue<>(key, value);
}

bool Json::GetValue(std::string_view key, uint16_t &value)
{
    return GetValue<>(key, value);
}

bool Json::GetValue(std::string_view key, int32_t &value)
{
    return GetValue<>(key, value);
}

bool Json::GetValue(std::string_view key, bool &value)
{
    return GetValue<>(key, value);
}

bool Json::IsKeyWithinObject(std::string_view key) const
{
    return key.find(".") != std::string::npos;
}

std::string Json::GetObjectName(std::string_view key) const
{
    size_t objectStringEndIndex = key.find(objectDelimiter);

    if (objectStringEndIndex == std::string::npos)
    {
        return "";
    }

    return std::string(key).substr(0, objectStringEndIndex);
}

std::string Json::GetKeyName(std::string_view key) const
{
    size_t objectStringEndIndex = key.find(objectDelimiter);

    if (objectStringEndIndex == std::string::npos)
    {
        return std::string(key);
    }
    else
    {
        return std::string(key).substr(objectStringEndIndex + 1, key.length() - 1);
    }
}

bool Json::EnterObject(std::string_view object)
{
    if (objectDepth >= maxObjectDepth)
    {
        return false;
    }

    if (json_obj_get_object(&jsonContext, object.data()) != 0)
    {
        return false;
    }

    objectDepth++;

    return true;
}

void Json::ExitObject()
{
    if (objectDepth > 0)
    {
        json_obj_leave_object(&jsonContext);
        objectDepth--;
    }
}

} // namespace Json
