#ifndef OSAL_ESP_IDF_CLOCK_OUTPUT
#define OSAL_ESP_IDF_CLOCK_OUTPUT

#include "ClockOutputConfig.hpp"
#include "driver/ledc.h"
#include <AbstractionLayer/IClockOutput.h>

namespace ClockOutput {

class ClockOutput : public IClockOutput
{
  public:
    ClockOutput() = default;
    ~ClockOutput() override;

    ClockOutput &operator=(ClockOutput &&) = delete;

    bool Init(const ClockOutputConfig &config);
    bool Start() final;
    bool Stop() final;

  private:
    void DeInit() const;
    ledc_channel_t channel;
    ledc_mode_t speedMode;
    ledc_timer_t timerNum;
};

} // namespace ClockOutput

#endif /* OSAL_ESP_IDF_CLOCK_OUTPUT */
