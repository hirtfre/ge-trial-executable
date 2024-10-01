#ifndef OSAL_ESP_IDF_JSON
#define OSAL_ESP_IDF_JSON
#include "json_parser.h"
#include <AbstractionLayer/IJson.h>
#include <array>
#include <string>

namespace Json {

static constexpr uint8_t maxTokenCount = 100;

class Json : public IJson
{
  public:
    Json() = default;
    ~Json() override = default;

    bool Open(const JsonString &string) override;
    void Close() override;
    bool GetValue(std::string_view key, uint8_t &value) override;
    bool GetValue(std::string_view key, uint16_t &value) override;
    bool GetValue(std::string_view key, int32_t &value) override;
    bool GetValue(std::string_view key, bool &value) override;

  private:
    template <typename T> bool GetValue(std::string_view key, T &value);
    template <typename T> bool GetValueFromJsonObject(std::string_view key, T &value);
    bool IsKeyWithinObject(std::string_view key) const;
    std::string GetObjectName(std::string_view key) const;
    std::string GetKeyName(std::string_view key) const;
    bool EnterObject(std::string_view object);
    void ExitObject();
    jparse_ctx_t jsonContext;
    std::array<json_tok_t, maxTokenCount> tokenBuffer;
    uint8_t objectDepth = 0;
};

} // namespace Json

#endif /* OSAL_ESP_IDF_JSON */
