/*******************************************************************************
 * Copyright (c) 2023 Orbbec 3D Technology, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#include "orbbec_camera/ob_camera_node.h"
#include <rclcpp/rclcpp.hpp>
#include <nlohmann/json.hpp>
#include <thread>

#include "orbbec_camera/utils.h"
namespace orbbec_camera {

void OBCameraNode::setupCameraCtrlServices() {
  using std_srvs::srv::SetBool;
  for (auto stream_index : IMAGE_STREAMS) {
    if (!enable_stream_[stream_index]) {
      continue;
    }
    auto stream_name = stream_name_[stream_index];
    std::string service_name = "get_" + stream_name + "_exposure";
    get_exposure_srv_[stream_index] = node_->create_service<GetInt32>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<GetInt32::Request> request,
                                            std::shared_ptr<GetInt32::Response> response) {
          getExposureCallback(request, response, stream_index);
        });

    service_name = "set_" + stream_name + "_exposure";
    set_exposure_srv_[stream_index] = node_->create_service<SetInt32>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<SetInt32::Request> request,
                                            std::shared_ptr<SetInt32::Response> response) {
          setExposureCallback(request, response, stream_index);
        });
    service_name = "get_" + stream_name + "_gain";
    get_gain_srv_[stream_index] = node_->create_service<GetInt32>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<GetInt32::Request> request,
                                            std::shared_ptr<GetInt32::Response> response) {
          getGainCallback(request, response, stream_index);
        });

    service_name = "set_" + stream_name + "_gain";
    set_gain_srv_[stream_index] = node_->create_service<SetInt32>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<SetInt32::Request> request,
                                            std::shared_ptr<SetInt32::Response> response) {
          setGainCallback(request, response, stream_index);
        });
    service_name = "set_" + stream_name + "_auto_exposure";
    set_auto_exposure_srv_[stream_index] = node_->create_service<SetBool>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<SetBool::Request> request,
                                            std::shared_ptr<SetBool::Response> response) {
          setAutoExposureCallback(request, response, stream_index);
        });

    service_name = "toggle_" + stream_name;

    toggle_sensor_srv_[stream_index] = node_->create_service<SetBool>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<SetBool::Request> request,
                                            std::shared_ptr<SetBool::Response> response) {
          toggleSensorCallback(request, response, stream_index);
        });
    service_name = "set_" + stream_name + "_mirror";
    set_mirror_srv_[stream_index] = node_->create_service<SetBool>(
        service_name,
        [this, stream_index = stream_index](const std::shared_ptr<SetBool::Request> request,
                                            std::shared_ptr<SetBool::Response> response) {
          setMirrorCallback(request, response, stream_index);
        });
  }
  set_fan_work_mode_srv_ = node_->create_service<SetInt32>(
      "set_fan_work_mode", [this](const std::shared_ptr<SetInt32::Request> request,
                                  std::shared_ptr<SetInt32::Response> response) {
        setFanWorkModeCallback(request, response);
      });
  set_floor_enable_srv_ = node_->create_service<SetBool>(
      "set_floor_enable", [this](const std::shared_ptr<rmw_request_id_t> request_header,
                                 const std::shared_ptr<SetBool::Request> request,
                                 std::shared_ptr<SetBool::Response> response) {
        setFloorEnableCallback(request_header, request, response);
      });
  set_laser_enable_srv_ = node_->create_service<SetBool>(
      "set_laser_enable", [this](const std::shared_ptr<rmw_request_id_t> request_header,
                                 const std::shared_ptr<SetBool::Request> request,
                                 std::shared_ptr<SetBool::Response> response) {
        setLaserEnableCallback(request_header, request, response);
      });
  set_lrm_enable_srv_ = node_->create_service<SetBool>(
      "set_lrm_enable", [this](const std::shared_ptr<rmw_request_id_t> request_header,
                               const std::shared_ptr<SetBool::Request> request,
                               std::shared_ptr<SetBool::Response> response) {
        setLrmEnableCallback(request_header, request, response);
      });
  get_lrm_status_srv_ = node_->create_service<GetBool>(
      "get_lrm_status", [this](const std::shared_ptr<rmw_request_id_t> request_header,
                               const std::shared_ptr<GetBool::Request> request,
                               std::shared_ptr<GetBool::Response> response) {
        (void)request_header;
        getLrmStatusCallback(request, response);
      });

  get_white_balance_srv_ = node_->create_service<GetInt32>(
      "get_white_balance", [this](const std::shared_ptr<GetInt32::Request> request,
                                  std::shared_ptr<GetInt32::Response> response) {
        getWhiteBalanceCallback(request, response);
      });

  set_white_balance_srv_ = node_->create_service<SetInt32>(
      "set_white_balance", [this](const std::shared_ptr<SetInt32::Request> request,
                                  std::shared_ptr<SetInt32::Response> response) {
        setWhiteBalanceCallback(request, response);
      });
  get_auto_white_balance_srv_ = node_->create_service<GetInt32>(
      "get_auto_white_balance", [this](const std::shared_ptr<GetInt32::Request> request,
                                       std::shared_ptr<GetInt32::Response> response) {
        getAutoWhiteBalanceCallback(request, response);
      });
  set_auto_white_balance_srv_ = node_->create_service<SetBool>(
      "set_auto_white_balance", [this](const std::shared_ptr<SetBool::Request> request,
                                       std::shared_ptr<SetBool::Response> response) {
        setAutoWhiteBalanceCallback(request, response);
      });
  get_device_srv_ = node_->create_service<GetDeviceInfo>(
      "get_device_info", [this](const std::shared_ptr<GetDeviceInfo::Request> request,
                                std::shared_ptr<GetDeviceInfo::Response> response) {
        getDeviceInfoCallback(request, response);
      });
  get_sdk_version_srv_ = node_->create_service<GetString>(
      "get_sdk_version",
      [this](const std::shared_ptr<GetString::Request> request,
             std::shared_ptr<GetString::Response> response) { getSDKVersion(request, response); });
  save_images_srv_ = node_->create_service<std_srvs::srv::Empty>(
      "save_images", [this](const std::shared_ptr<std_srvs::srv::Empty::Request> request,
                            std::shared_ptr<std_srvs::srv::Empty::Response> response) {
        saveImageCallback(request, response);
      });
  save_point_cloud_srv_ = node_->create_service<std_srvs::srv::Empty>(
      "save_point_cloud", [this](const std::shared_ptr<std_srvs::srv::Empty::Request> request,
                                 std::shared_ptr<std_srvs::srv::Empty::Response> response) {
        savePointCloudCallback(request, response);
      });
  switch_ir_camera_srv_ = node_->create_service<SetString>(
      "switch_ir", [this](const std::shared_ptr<SetString::Request> request,
                          std::shared_ptr<SetString::Response> response) {
        switchIRCameraCallback(request, response);
      });
  set_ir_long_exposure_srv_ = node_->create_service<SetBool>(
      "set_ir_long_exposure", [this](const std::shared_ptr<SetBool::Request> request,
                                     std::shared_ptr<SetBool::Response> response) {
        setIRLongExposureCallback(request, response);
      });
  get_lrm_measure_distance_srv_ = node_->create_service<GetInt32>(
      "get_lrm_measure_distance", [this](const std::shared_ptr<GetInt32::Request> request,
                                         std::shared_ptr<GetInt32::Response> response) {
        getLrmMeasureDistanceCallback(request, response);
      });
  set_reset_timestamp_srv_ = node_->create_service<SetBool>(
      "set_reset_timestamp", [this](const std::shared_ptr<SetBool::Request> request,
                                    std::shared_ptr<SetBool::Response> response) {
        setRESETTimestampCallback(request, response);
      });
  set_interleaver_laser_sync_srv_ = node_->create_service<SetInt32>(
      "set_sync_interleaverlaser", [this](const std::shared_ptr<SetInt32::Request> request,
                                          std::shared_ptr<SetInt32::Response> response) {
        setSYNCInterleaveLaserCallback(request, response);
      });
  set_sync_host_time_srv_ = node_->create_service<SetBool>(
      "set_sync_hosttime", [this](const std::shared_ptr<SetBool::Request> request,
                                  std::shared_ptr<SetBool::Response> response) {
        setSYNCHostimeCallback(request, response);
      });
}

void OBCameraNode::setExposureCallback(const std::shared_ptr<SetInt32::Request>& request,
                                       std::shared_ptr<SetInt32::Response>& response,
                                       const stream_index_pair& stream_index) {
  auto stream = stream_index.first;
  try {
    switch (stream) {
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR_RIGHT:
      case OB_STREAM_IR:
        device_->setIntProperty(OB_PROP_IR_EXPOSURE_INT, request->data);
        break;
      case OB_STREAM_DEPTH:
        device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, request->data);
        break;
      case OB_STREAM_COLOR:
        device_->setIntProperty(OB_PROP_COLOR_EXPOSURE_INT, request->data);
        break;
      default:
        RCLCPP_ERROR(logger_, "%s NOT a video stream", __FUNCTION__);
        break;
    }
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    RCLCPP_ERROR(logger_, "%s unknown error %d", __FUNCTION__, __LINE__);
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::getGainCallback(const std::shared_ptr<GetInt32::Request>& request,
                                   std::shared_ptr<GetInt32::Response>& response,
                                   const stream_index_pair& stream_index) {
  (void)request;
  auto stream = stream_index.first;
  try {
    switch (stream) {
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR_RIGHT:
      case OB_STREAM_IR:
        response->data = device_->getIntProperty(OB_PROP_IR_GAIN_INT);
        break;
      case OB_STREAM_DEPTH:
        response->data = device_->getIntProperty(OB_PROP_DEPTH_GAIN_INT);
        break;
      case OB_STREAM_COLOR:
        response->data = device_->getIntProperty(OB_PROP_COLOR_GAIN_INT);
        break;
      default:
        RCLCPP_ERROR(logger_, " %s NOT a video stream", __FUNCTION__);
        break;
    }
    response->success = true;
  } catch (ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setGainCallback(const std::shared_ptr<SetInt32 ::Request>& request,
                                   std::shared_ptr<SetInt32::Response>& response,
                                   const stream_index_pair& stream_index) {
  auto stream = stream_index.first;
  OBPropertyID prop_id = OB_PROP_IR_GAIN_INT;
  try {
    switch (stream) {
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR_RIGHT:
      case OB_STREAM_IR:
        prop_id = OB_PROP_IR_GAIN_INT;
        break;
      case OB_STREAM_DEPTH:
        prop_id = OB_PROP_DEPTH_GAIN_INT;
        break;
      case OB_STREAM_COLOR:
        prop_id = OB_PROP_COLOR_GAIN_INT;
        break;
      default:
        RCLCPP_ERROR(logger_, "%s NOT a video stream", __FUNCTION__);
        response->success = false;
        response->message = "NOT a video stream";
        return;
    }
    auto range = device_->getIntPropertyRange(prop_id);
    if (request->data < range.min || request->data > range.max) {
      response->success = false;
      RCLCPP_INFO_STREAM(logger_, "set gain value out of range");
      response->message = "value out of range";
      return;
    }
    device_->setIntProperty(prop_id, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::getWhiteBalanceCallback(const std::shared_ptr<GetInt32::Request>& request,
                                           std::shared_ptr<GetInt32::Response>& response) {
  (void)request;
  try {
    response->data = device_->getIntProperty(OB_PROP_COLOR_WHITE_BALANCE_INT);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setWhiteBalanceCallback(const std::shared_ptr<SetInt32 ::Request>& request,
                                           std::shared_ptr<SetInt32 ::Response>& response) {
  try {
    auto range = device_->getIntPropertyRange(OB_PROP_COLOR_WHITE_BALANCE_INT);
    if (request->data < range.min || request->data > range.max) {
      response->success = false;
      RCLCPP_INFO_STREAM(logger_, "set white balance value out of range");
      response->message = "value out of range";
      return;
    }
    bool auto_white_balance = device_->getBoolProperty(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL);
    if (auto_white_balance) {
      RCLCPP_WARN(logger_, "auto white balance is enabled, set white balance will be ignored");
      response->success = false;
      response->message = "auto white balance is enabled";
      return;
    }
    device_->setIntProperty(OB_PROP_COLOR_WHITE_BALANCE_INT, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::getAutoWhiteBalanceCallback(const std::shared_ptr<GetInt32::Request>& request,
                                               std::shared_ptr<GetInt32::Response>& response) {
  (void)request;
  try {
    response->data = device_->getBoolProperty(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setAutoWhiteBalanceCallback(const std::shared_ptr<SetBool::Request>& request,
                                               std::shared_ptr<SetBool::Response>& response) {
  try {
    device_->setBoolProperty(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setAutoExposureCallback(
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response,
    const stream_index_pair& stream_index) {
  auto stream = stream_index.first;
  OBPropertyID prop_id = OB_PROP_IR_AUTO_EXPOSURE_BOOL;
  try {
    switch (stream) {
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR_RIGHT:
      case OB_STREAM_IR:
        prop_id = OB_PROP_IR_AUTO_EXPOSURE_BOOL;
        break;
      case OB_STREAM_DEPTH:
        prop_id = OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL;
        break;
      case OB_STREAM_COLOR:
        prop_id = OB_PROP_COLOR_AUTO_EXPOSURE_BOOL;
        break;
      default:
        RCLCPP_ERROR(logger_, "%s NOT a video stream", __FUNCTION__);
        response->success = false;
        response->message = "NOT a video stream";
        return;
    }
    auto range = device_->getIntPropertyRange(prop_id);
    if (request->data < range.min || request->data > range.max) {
      response->success = false;
      RCLCPP_INFO_STREAM(logger_, "set auto exposure value out of range");
      response->message = "value out of range";
      return;
    }
    device_->setIntProperty(prop_id, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setFanWorkModeCallback(const std::shared_ptr<SetInt32::Request>& request,
                                          std::shared_ptr<SetInt32::Response>& response) {
  (void)response;
  bool fan_mode = request->data;
  try {
    device_->setBoolProperty(OB_PROP_FAN_WORK_MODE_INT, fan_mode);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setFloorEnableCallback(
    const std::shared_ptr<rmw_request_id_t>& request_header,
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  (void)request_header;
  (void)response;
  bool floor_enable = request->data;
  try {
    device_->setBoolProperty(OB_PROP_FLOOD_BOOL, floor_enable);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setLaserEnableCallback(
    const std::shared_ptr<rmw_request_id_t>& request_header,
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  (void)request_header;
  (void)response;
  int laser_enable = request->data ? 1 : 0;
  try {
    if (device_->isPropertySupported(OB_PROP_LASER_CONTROL_INT, OB_PERMISSION_READ_WRITE)) {
      device_->setIntProperty(OB_PROP_LASER_CONTROL_INT, laser_enable);
    } else if (device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE)) {
      device_->setIntProperty(OB_PROP_LASER_BOOL, laser_enable);
    }
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::setLrmEnableCallback(
    const std::shared_ptr<rmw_request_id_t>& request_header,
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  (void)request_header;
  (void)response;
  bool lrm_enable = request->data;
  try {
    if (device_->isPropertySupported(OB_PROP_LASER_CONTROL_INT, OB_PERMISSION_READ_WRITE)) {
      auto laser_enable = device_->getIntProperty(OB_PROP_LASER_CONTROL_INT);
      device_->setBoolProperty(OB_PROP_LDP_BOOL, lrm_enable);
      device_->setIntProperty(OB_PROP_LASER_CONTROL_INT, laser_enable);
    } else if (device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE)) {
      auto laser_enable = device_->getIntProperty(OB_PROP_LASER_BOOL);
      device_->setBoolProperty(OB_PROP_LDP_BOOL, lrm_enable);
      device_->setIntProperty(OB_PROP_LASER_BOOL, laser_enable);
    }
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::getExposureCallback(const std::shared_ptr<GetInt32::Request>& request,
                                       std::shared_ptr<GetInt32 ::Response>& response,
                                       const stream_index_pair& stream_index) {
  (void)request;
  auto stream = stream_index.first;
  try {
    switch (stream) {
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR_RIGHT:
      case OB_STREAM_IR:
        response->data = device_->getIntProperty(OB_PROP_IR_EXPOSURE_INT);
        break;
      case OB_STREAM_DEPTH:
        response->data = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
        break;
      case OB_STREAM_COLOR:
        response->data = device_->getIntProperty(OB_PROP_COLOR_EXPOSURE_INT);
        break;
      default:
        RCLCPP_ERROR(logger_, " %s NOT a video stream", __FUNCTION__);
        break;
    }
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::getDeviceInfoCallback(const std::shared_ptr<GetDeviceInfo::Request>& request,
                                         std::shared_ptr<GetDeviceInfo::Response>& response) {
  (void)request;
  try {
    auto device_info = device_->getDeviceInfo();
    response->info.name = device_info->getName();
    response->info.serial_number = device_info->getSerialNumber();
    response->info.firmware_version = device_info->getFirmwareVersion();
    response->info.supported_min_sdk_version = device_info->getSupportedMinSdkVersion();
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::getSDKVersion(const std::shared_ptr<GetString::Request>& request,
                                 std::shared_ptr<GetString::Response>& response) {
  (void)request;
  try {
    auto device_info = device_->getDeviceInfo();
    nlohmann::json data;
    data["firmware_version"] = device_info->getFirmwareVersion();
    data["supported_min_sdk_version"] = device_info->getSupportedMinSdkVersion();
    data["ros_sdk_version"] = OB_ROS_VERSION_STR;
    data["ob_sdk_version"] = getObSDKVersion();
    response->data = data.dump(2);
    response->success = true;
  } catch (const ob::Error& e) {
    response->success = false;
    response->message = e.getMessage();
  } catch (const std::exception& e) {
    response->success = false;
    response->message = e.what();
  } catch (...) {
    response->success = false;
    response->message = "unknown error";
  }
}

void OBCameraNode::setMirrorCallback(const std::shared_ptr<SetBool::Request>& request,
                                     std::shared_ptr<SetBool::Response>& response,
                                     const stream_index_pair& stream_index) {
  (void)request;
  auto stream = stream_index.first;
  try {
    switch (stream) {
      case OB_STREAM_IR_RIGHT:
        device_->setBoolProperty(OB_PROP_IR_RIGHT_MIRROR_BOOL, request->data);
        break;
      case OB_STREAM_IR_LEFT:
      case OB_STREAM_IR:
        device_->setBoolProperty(OB_PROP_IR_MIRROR_BOOL, request->data);
        break;
      case OB_STREAM_DEPTH:
        device_->setBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL, request->data);
        break;
      case OB_STREAM_COLOR:
        device_->setBoolProperty(OB_PROP_COLOR_MIRROR_BOOL, request->data);
        break;
      default:
        RCLCPP_ERROR(logger_, " %s NOT a video stream", __FUNCTION__);
        break;
    }
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::getLrmStatusCallback(const std::shared_ptr<GetBool::Request>& request,
                                        std::shared_ptr<GetBool::Response>& response) {
  (void)request;
  try {
    response->data = device_->getBoolProperty(OB_PROP_LDP_STATUS_BOOL);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::getLrmMeasureDistanceCallback(const std::shared_ptr<GetInt32::Request>& request,
                                                 std::shared_ptr<GetInt32::Response>& response) {
  (void)request;
  try {
    response->data = device_->getIntProperty(OB_PROP_LDP_MEASURE_DISTANCE_INT);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::toggleSensorCallback(const std::shared_ptr<SetBool::Request>& request,
                                        std::shared_ptr<SetBool::Response>& response,
                                        const stream_index_pair& stream_index) {
  std::string msg;
  if (request->data) {
    if (enable_stream_[stream_index]) {
      msg = stream_name_[stream_index] + " Already ON";
    }
    RCLCPP_INFO_STREAM(logger_, "toggling sensor " << stream_name_[stream_index] << " ON");

  } else {
    if (!enable_stream_[stream_index]) {
      msg = stream_name_[stream_index] + " Already OFF";
    }
    RCLCPP_INFO_STREAM(logger_, "toggling sensor " << stream_name_[stream_index] << " OFF");
  }
  if (!msg.empty()) {
    RCLCPP_ERROR_STREAM(logger_, msg);
    response->success = false;
    response->message = msg;
    return;
  }
  response->success = toggleSensor(stream_index, request->data, response->message);
}

bool OBCameraNode::toggleSensor(const stream_index_pair& stream_index, bool enabled,
                                std::string& msg) {
  try {
    pipeline_->stop();
    enable_stream_[stream_index] = enabled;
    setupProfiles();
    startStreams();
    return true;
  } catch (const ob::Error& e) {
    msg = e.getMessage();
    return false;
  } catch (const std::exception& e) {
    msg = e.what();
    return false;
  } catch (...) {
    msg = "unknown error";
    return false;
  }
}

void OBCameraNode::saveImageCallback(const std::shared_ptr<std_srvs::srv::Empty::Request>& request,
                                     std::shared_ptr<std_srvs::srv::Empty::Response>& response) {
  (void)request;
  (void)response;
  for (const auto& stream_index : IMAGE_STREAMS) {
    if (enable_stream_[stream_index]) {
      save_images_[stream_index] = true;
      save_images_count_[stream_index] = 0;
    }
  }
}

void OBCameraNode::savePointCloudCallback(
    const std::shared_ptr<std_srvs::srv::Empty::Request>& request,
    std::shared_ptr<std_srvs::srv::Empty::Response>& response) {
  (void)request;
  (void)response;
  if (enable_point_cloud_) {
    save_point_cloud_ = true;
  }
  if (enable_colored_point_cloud_) {
    save_colored_point_cloud_ = true;
  }
}

void OBCameraNode::switchIRCameraCallback(const std::shared_ptr<SetString::Request>& request,
                                          std::shared_ptr<SetString::Response>& response) {
  if (request->data != "left" && request->data != "right") {
    response->success = false;
    response->message = "invalid ir camera name";
    return;
  }
  try {
    int data = request->data == "left" ? 0 : 1;
    device_->setIntProperty(OB_PROP_IR_CHANNEL_DATA_SOURCE_INT, data);
    response->success = true;
    return;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}
void OBCameraNode::setIRLongExposureCallback(
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  try {
    device_->setBoolProperty(OB_PROP_IR_LONG_EXPOSURE_BOOL, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

void OBCameraNode::setRESETTimestampCallback(
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  (void)request;
  try {
    device_->setBoolProperty(OB_PROP_TIMER_RESET_TRIGGER_OUT_ENABLE_BOOL, true);
    device_->setBoolProperty(OB_PROP_TIMER_RESET_SIGNAL_BOOL, true);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}
void OBCameraNode::setSYNCInterleaveLaserCallback(
    const std::shared_ptr<SetInt32 ::Request>& request,
    std::shared_ptr<SetInt32 ::Response>& response) {
  (void)request;
  try {
    device_->setIntProperty(OB_PROP_FRAME_INTERLEAVE_LASER_PATTERN_SYNC_DELAY_INT, request->data);
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}
void OBCameraNode::setSYNCHostimeCallback(
    const std::shared_ptr<std_srvs::srv::SetBool::Request>& request,
    std::shared_ptr<std_srvs::srv::SetBool::Response>& response) {
  (void)request;
  try {
    device_->timerSyncWithHost();
    response->success = true;
  } catch (const ob::Error& e) {
    response->message = e.getMessage();
    response->success = false;
  } catch (const std::exception& e) {
    response->message = e.what();
    response->success = false;
  } catch (...) {
    response->message = "unknown error";
    response->success = false;
  }
}

}  // namespace orbbec_camera
