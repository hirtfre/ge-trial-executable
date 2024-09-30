#ifndef OSAL_ESP_IDF_ESP_MATTER_DATA_MODEL_H
#define OSAL_ESP_IDF_ESP_MATTER_DATA_MODEL_H

#include <AbstractionLayer/MatterDataModel/IMatterDataModel.h>
#include <stdint.h>

namespace MatterDataModel {

class MatterDataModel : public IMatterDataModel
{
  public:
    MatterDataModel() = default;
    ~MatterDataModel() override = default;

    MatterDataModel &operator=(MatterDataModel &&) = delete;

    uint16_t CreateEndpoint(EndpointType endpointType) override;
    bool SetAttribute(const AttributeIdentifier &attributeIdentifier, void *value) override;
    bool GetAttribute(const AttributeIdentifier &attributeIdentifier, void *value) override;

    bool NotifyOnIdentificationRequest(IdentificationCallbackType callbackType, uint16_t endpointId,
                                       uint8_t effectId, uint8_t effectVariant);
    bool NotifyOnAttributeUpdate(AttributeUpdateCallbackType callbackType,
                                 const AttributeIdentifier attributeIdentifier,
                                 void *attributeValue);

  private:
    static void IdentificationObserverNotification(IMatterDataModelObserver *observer,
                                                   IdentificationCallbackType callbackType,
                                                   uint16_t endpointId, uint8_t effectId,
                                                   uint8_t effectVariant);
    static void AttributeUpdatedObserverNotification(IMatterDataModelObserver *observer,
                                                     AttributeUpdateCallbackType callbackType,
                                                     const AttributeIdentifier attributeIdentifier,
                                                     void *attributeValue);
};

} // namespace MatterDataModel

#endif /* OSAL_ESP_IDF_ESP_MATTER_DATA_MODEL_H */
