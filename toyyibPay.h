#pragma once
#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "convertArrayToUrlEncoded.h"

namespace ToyyibPay {
    template<typename T>
    std::string ToJSON(const T &array) {
        auto jv = boost::json::value_from(array);
        return boost::json::serialize(jv);
    }

    class Trans {
    public:
        std::string userSecretKey;
        std::string categoryCode;
        std::string billName;
        std::string billDescription;
        std::string billPriceSetting = "1";
        std::string billPayorInfo = "1";
        std::string billAmount;
        std::string billTo;
        std::string billEmail;
        std::string billPhone;
        std::string billReturnUrl;
        std::string billCallbackUrl;
        std::string billExternalReferenceNo;
        std::string billPaymentChannel;
        std::string billContentEmail;
        std::string billChargeToCustomer;
        std::string billExpiryDays;

    public:
        auto GetArray() const {
            return std::unordered_map<std::string, std::string> {
                {"userSecretKey", userSecretKey},
                {"categoryCode", categoryCode},
                {"billName", billName},
                {"billDescription", billDescription},
                {"billPriceSetting", billPriceSetting},
                {"billPayorInfo", billPayorInfo},
                {"billAmount", billAmount},
                {"billTo", billTo},
                {"billEmail", billEmail},
                {"billPhone", billPhone},
                //{"billReturnUrl", billReturnUrl},
                {"billCallbackUrl", billCallbackUrl},
                {"billExternalReferenceNo", billExternalReferenceNo},
                {"billPaymentChannel", billPaymentChannel},
                {"billContentEmail", billContentEmail},
                {"billChargeToCustomer", billChargeToCustomer},
                {"billExpiryDays", billExpiryDays}};
        }
    };

    class Process {
        std::string host;
        std::string secretKey;

    public:
        Process(const std::string &secretKey, const std::string host = "dev.toyyibpay.com") : host(host), secretKey(secretKey) {}

        static std::unordered_map<std::string, std::string> ReceiveNotification(const std::string &contentType, const std::string &body);
        std::string CreateBill(Trans &tran) const;
        std::string CreateCategory(const std::string &categoryCode, const std::string &categoryName) const;

        //std::string ConvertToUrlEncodedBody(const std::vector<std::pair<std::string, std::string>> &array) const;
    };
}