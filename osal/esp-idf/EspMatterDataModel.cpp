#include "EspMatterDataModel.h"
#include "../ota/ble/ota_ble.h"
#include <AbstractionLayer/IMemory.h>
#include <AbstractionLayer/MatterDataModel/IMatterDataModelFactory.h>
#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_cluster.h>
#include <stdint.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static const char *TAG = "OSAL_EspMatterDataModel";

namespace esp_matter::attribute {

extern esp_err_t get_data_from_attr_val(esp_matter_attr_val_t *val,
                                        EmberAfAttributeType *attribute_type,
                                        uint16_t *attribute_size, uint8_t *value);

} // namespace esp_matter::attribute

static uint16_t CreateMatterNode(MatterDataModel::EndpointType endpointType);
static void AddLiftSupport(cluster_t *windowcovering_cluster);
static void AddTiltSupport(cluster_t *windowcovering_cluster);
static attribute_t *GetAttribute(const uint16_t endpointId, const uint32_t clusterId,
                                 const uint32_t attributeId);
static bool GetAttributeValue(const uint16_t endpointId, const uint32_t clusterId,
                              const uint32_t attributeId, void *value);
static bool SetAttributeValue(const uint16_t endpointId, const uint32_t clusterId,
                              const uint32_t attributeId, void *value);

namespace MatterDataModel {

static MatterDataModel *matterDataModel = nullptr;
static uint8_t matterDataModelInstanceCount = 0;

IMatterDataModel *MatterDataModelFactory::Create()
{
    if (matterDataModel != nullptr)
    {
        return matterDataModel;
    }

    auto memory = MEM_Calloc(1, sizeof(MatterDataModel));

    if (!memory)
    {
        goto memory_allocation_failed;
    }

    matterDataModel = new (memory) MatterDataModel();

    matterDataModelInstanceCount++;

    return matterDataModel;

    matterDataModel->~MatterDataModel();
    MEM_Free(memory);

memory_allocation_failed:
    return nullptr;
}

void MatterDataModelFactory::Destroy(IMatterDataModel **pMatterDataModel)
{
    if (pMatterDataModel == nullptr || *pMatterDataModel == nullptr ||
        *pMatterDataModel == matterDataModel)
    {
        return;
    }

    matterDataModelInstanceCount--;

    if (matterDataModelInstanceCount > 0)
    {
        return;
    }

    auto *me = static_cast<MatterDataModel *>(*pMatterDataModel);
    me->~MatterDataModel();
    MEM_Free(me);
    matterDataModel = nullptr;
    *pMatterDataModel = nullptr;
}

uint16_t MatterDataModel::CreateEndpoint(EndpointType endpointType)
{
    return CreateMatterNode(endpointType);
}

bool MatterDataModel::SetAttribute(const AttributeIdentifier &attributeIdentifier, void *value)
{
    // TODO run this in a separate task
    return SetAttributeValue(attributeIdentifier.endpointId, attributeIdentifier.clusterId,
                             attributeIdentifier.attributeId, value);
}

bool MatterDataModel::GetAttribute(const AttributeIdentifier &attributeIdentifier, void *value)
{
    // TODO run this in a separate task
    return GetAttributeValue(attributeIdentifier.endpointId, attributeIdentifier.clusterId,
                             attributeIdentifier.attributeId, value);
}

bool MatterDataModel::NotifyOnIdentificationRequest(IdentificationCallbackType callbackType,
                                                    uint16_t endpointId, uint8_t effectId,
                                                    uint8_t effectVariant)
{
    ObserversNotify(IdentificationObserverNotification, callbackType, endpointId, effectId,
                    effectVariant);

    return true; // TODO change GeLib/Observer to return bool
}

bool MatterDataModel::NotifyOnAttributeUpdate(AttributeUpdateCallbackType callbackType,
                                              const AttributeIdentifier attributeIdentifier,
                                              void *attributeValue)
{
    ObserversNotify(AttributeUpdatedObserverNotification, callbackType, attributeIdentifier,
                    attributeValue);

    return true; // TODO change GeLib/Observer to return bool
}

void MatterDataModel::IdentificationObserverNotification(IMatterDataModelObserver *observer,
                                                         IdentificationCallbackType callbackType,
                                                         uint16_t endpointId, uint8_t effectId,
                                                         uint8_t effectVariant)
{
    observer->OnIdentificationRequest(callbackType, endpointId, effectId, effectVariant);
}

void MatterDataModel::AttributeUpdatedObserverNotification(
    IMatterDataModelObserver *observer, AttributeUpdateCallbackType callbackType,
    const AttributeIdentifier attributeIdentifier, void *attributeValue)
{
    observer->OnAttributeUpdate(callbackType, attributeIdentifier, attributeValue);
}

} // namespace MatterDataModel

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id,
                                       uint8_t effect_id, uint8_t effect_variant,
                                       [[maybe_unused]] void *priv_data)
{
    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "Identification callback: type: %u, effect: %u, variant: %u", type, effect_id,
             effect_variant);

    if (MatterDataModel::matterDataModel != nullptr)
    {
        bool successful = MatterDataModel::matterDataModel->NotifyOnIdentificationRequest(
            (MatterDataModel::IdentificationCallbackType)type, endpoint_id, effect_id,
            effect_variant);

        err = successful ? ESP_OK : ESP_FAIL;
    }

    return err;
}

// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id,
                                         uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val,
                                         [[maybe_unused]] void *priv_data)
{
    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "App update callback: type: %u, endpoint: %u, cluster: %lu, attribute: %lu", type,
             endpoint_id, cluster_id, attribute_id);

    if (MatterDataModel::matterDataModel != nullptr)
    {
        MatterDataModel::AttributeIdentifier attributeIdentifier = {
            .endpointId = endpoint_id, .clusterId = cluster_id, .attributeId = attribute_id};

        bool successful = MatterDataModel::matterDataModel->NotifyOnAttributeUpdate(
            (MatterDataModel::AttributeUpdateCallbackType)type, attributeIdentifier, &val->val);

        err = successful ? ESP_OK : ESP_FAIL;
    }

    return err;
}

static attribute_t *GetAttribute(const uint16_t endpointId, const uint32_t clusterId,
                                 const uint32_t attributeId)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpointId);
    cluster_t *cluster = cluster::get(endpoint, clusterId);
    return attribute::get(cluster, attributeId);
}

static bool GetAttributeValue(const uint16_t endpointId, const uint32_t clusterId,
                              const uint32_t attributeId, void *value)
{
    uint16_t attributeSize = 0;
    esp_matter_attr_val_t attributeValue = esp_matter_invalid(nullptr);
    attribute_t *attribute = GetAttribute(endpointId, clusterId, attributeId);

    if (get_val(attribute, &attributeValue) != ESP_OK)
    {
        return false;
    }

    // attribute size is required because otherwise it references to a null pointer during some calculations
    if (get_data_from_attr_val(&attributeValue, nullptr, &attributeSize, (uint8_t *)value) !=
        ESP_OK)
    {
        return false;
    }

    return true;
}

static bool SetAttributeValue(const uint16_t endpointId, const uint32_t clusterId,
                              const uint32_t attributeId, void *value)
{
    uint16_t attributeSize = 0;
    esp_matter_attr_val_t attributeValue = esp_matter_invalid(nullptr);
    attribute_t *attribute = GetAttribute(endpointId, clusterId, attributeId);

    if (get_val(attribute, &attributeValue) != ESP_OK)
    {
        return false;
    }

    if (get_data_from_attr_val(&attributeValue, nullptr, &attributeSize, nullptr) != ESP_OK)
    {
        return false;
    }

    memcpy(&attributeValue.val, value, attributeSize);

    // sets attribute value in matter data model without sending to subscribers
    if (set_val(attribute, &attributeValue) != ESP_OK)
    {
        return false;
    }

    // sends attribute to all subscribers (matter clients)
    if (update(endpointId, clusterId, attributeId, &attributeValue) != ESP_OK)
    {
        return false;
    }

    return true;
}

static uint16_t CreateMatterNode(MatterDataModel::EndpointType endpointType)
{
    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;

    // node handle can be used to add/modify other endpoints.
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    // ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    // add diagnostic logs cluster on root endpoint
    cluster::diagnostic_logs::config_t diag_logs_config;
    endpoint_t *root_ep = endpoint::get(node, 0); // get the root node ep
    cluster::diagnostic_logs::create(root_ep, &diag_logs_config, CLUSTER_FLAG_SERVER);

    // add serial number attribute
    // data is fetched from factory partition, no need to provide here.
    cluster_t *basicInformationCluster = cluster::get(root_ep, BasicInformation::Id);
    cluster::basic_information::attribute::create_serial_number(basicInformationCluster, nullptr,
                                                                0);

    if (endpointType >= MatterDataModel::EndpointType::NumberOfModels)
    {
        return 0xFFFF;
    }

    // 0x0D: EndProductType = Exterior Venetian Blind; Indoor/Outdoor = Outdoor; Indicative Dimension = 2D; Recommended Type = 0x08 Tilt Blind;
    // 0x11: EndProductType = Roller Shutter; Indoor/Outdoor = Outdoor; Indicative Dimension = 1D; Recommended Type = 0x02 Rollershade;
    // 0x12: EndProductType = Exterior Vertical Screen; Indoor/Outdoor = Outdoor; Indicative Dimension = 1D; Recommended Type = 0x02 Rollershade;
    // 0x13: EndProductType = Awning Terrace (Patio); Indoor/Outdoor = Outdoor; Indicative Dimension = 1D; Recommended Type = 0x05 Awning;
    // 0x14: EndProductType = Awning Vertical Screen; Indoor/Outdoor = Outdoor; Indicative Dimension = 1D; Recommended Type = 0x05 Awning;
    // 0x15: EndProductType = Tilt Only Pergola; Indoor/Outdoor = Outdoor; Indicative Dimension = +D; Recommended Type = 0x06 Shutter;
    uint8_t end_product_type =
        (endpointType == MatterDataModel::EndpointType::VenetianBlind) ? 0x0D : 0x11;

    // Rollladen = 0x02: Type = Rollershade Exterior; Supported Features = Lift;
    // Markise = 0x05: Type = Awning; Supported Features = Lift;
    // Jalousie = 0x06: Type = Shutter; Supported Features = Tilt, Lift;
    // Jalousie = 0x08: Type = Tilt Blind; Supported Features = Tilt, Lift;
    uint8_t window_covering_type =
        (endpointType == MatterDataModel::EndpointType::VenetianBlind) ? 0x08 : 0x02;

    // create window covering device
    window_covering_device::config_t window_covering_device_config(end_product_type);
    window_covering_device_config.window_covering.type = window_covering_type;

    endpoint_t *endpoint = window_covering_device::create(node, &window_covering_device_config,
                                                          ENDPOINT_FLAG_NONE, nullptr);
    // ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create window covering endpoint"));
    uint16_t windowcovering_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "window covering created with endpoint_id %d", windowcovering_endpoint_id);

    cluster::identify::config_t identify_cluster_config;
    identify_cluster_config.identify_type = 0x05; // Actuator;
    cluster::identify::create(endpoint, &identify_cluster_config, CLUSTER_FLAG_SERVER);

    cluster_t *windowcovering_cluster =
        cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);

    cluster::window_covering::feature::absolute_position::config_t absolute_position;
    cluster::window_covering::feature::absolute_position::add(windowcovering_cluster,
                                                              &absolute_position);

    AddLiftSupport(windowcovering_cluster);

    if (endpointType == MatterDataModel::EndpointType::VenetianBlind)
    {
        AddTiltSupport(windowcovering_cluster);
    }

    return windowcovering_endpoint_id;
}

static void AddLiftSupport(cluster_t *windowcovering_cluster)
{
    cluster::window_covering::feature::lift::config_t lift;
    cluster::window_covering::feature::position_aware_lift::config_t position_aware_lift;
    auto percentage = nullable<uint8_t>(0);
    auto percentage_100ths = nullable<uint16_t>(0);
    position_aware_lift.current_position_lift_percentage = percentage;
    position_aware_lift.target_position_lift_percent_100ths = percentage_100ths;
    position_aware_lift.current_position_lift_percent_100ths = percentage_100ths;
    cluster::window_covering::feature::lift::add(windowcovering_cluster, &lift);
    cluster::window_covering::feature::position_aware_lift::add(windowcovering_cluster,
                                                                &position_aware_lift);
}

static void AddTiltSupport(cluster_t *windowcovering_cluster)
{
    cluster::window_covering::feature::tilt::config_t tilt;
    cluster::window_covering::feature::position_aware_tilt::config_t position_aware_tilt;
    auto percentage_tilt = nullable<uint8_t>(0);
    auto percentage_100ths_tilt = nullable<uint16_t>(0);
    position_aware_tilt.current_position_tilt_percentage = percentage_tilt;
    position_aware_tilt.target_position_tilt_percent_100ths = percentage_100ths_tilt;
    position_aware_tilt.current_position_tilt_percent_100ths = percentage_100ths_tilt;
    cluster::window_covering::feature::tilt::add(windowcovering_cluster, &tilt);
    cluster::window_covering::feature::position_aware_tilt::add(windowcovering_cluster,
                                                                &position_aware_tilt);
}
