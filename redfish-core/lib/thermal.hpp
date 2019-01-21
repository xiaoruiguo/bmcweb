/*
// Copyright (c) 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#pragma once

#include "node.hpp"
#include "sensors.hpp"

namespace redfish
{

class Thermal : public Node
{
  public:
    Thermal(CrowApp& app) :
        Node((app), "/redfish/v1/Chassis/<str>/Thermal/", std::string())
    {
        entityPrivileges = {
            {boost::beast::http::verb::get, {{"Login"}}},
            {boost::beast::http::verb::head, {{"Login"}}},
            {boost::beast::http::verb::patch, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::put, {{"ConfigureManager"}}},
            {boost::beast::http::verb::delete_, {{"ConfigureManager"}}},
            {boost::beast::http::verb::post, {{"ConfigureManager"}}}};
    }

  private:
    std::initializer_list<const char*> typeList = {
        "/xyz/openbmc_project/sensors/fan_tach",
        "/xyz/openbmc_project/sensors/temperature",
        "/xyz/openbmc_project/sensors/fan_pwm"};
    void doGet(crow::Response& res, const crow::Request& req,
               const std::vector<std::string>& params) override
    {
        if (params.size() != 1)
        {
            messages::internalError(res);
            res.end();
            return;
        }
        const std::string& chassisName = params[0];
#ifdef BMCWEB_ENABLE_REDFISH_ONE_CHASSIS
        // In a one chassis system the only supported name is "chassis"
        if (chassisName != "chassis")
        {
            messages::resourceNotFound(res, "#Chassis.v1_4_0.Chassis",
                                       chassisName);
            res.end();
            return;
        }
#endif

        res.jsonValue["@odata.type"] = "#Thermal.v1_4_0.Thermal";
        res.jsonValue["@odata.context"] =
            "/redfish/v1/$metadata#Thermal.Thermal";
        res.jsonValue["Id"] = "Thermal";
        res.jsonValue["Name"] = "Thermal";

        res.jsonValue["@odata.id"] =
            "/redfish/v1/Chassis/" + chassisName + "/Thermal";
#ifdef BMCWEB_ENABLE_REDFISH_ONE_CHASSIS
        res.end();
        return;
#endif
        auto sensorAsyncResp = std::make_shared<SensorsAsyncResp>(
            res, chassisName, typeList, "Thermal");

        // TODO Need to get Chassis Redundancy information.
        getChassisData(sensorAsyncResp);
    }
    void doPatch(crow::Response& res, const crow::Request& req,
                 const std::vector<std::string>& params) override
    {
        setSensorOverride(res, req, params, typeList, "Thermal");
    }
};

} // namespace redfish
