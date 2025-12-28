#include "webclient.h"
#include "toyyibPay.h"
#include "logger.h"

std::string ToyyibPay::Process::CreateBill(ToyyibPay::Trans &rec) const {
    std::string res;
    try {
        rec.userSecretKey = secretKey;
        auto body = ConvertToUrlEncodedBody(rec.GetArray());
        auto [response, responseCode] = WebClient::Post(fmt::format("https://{host}/index.php/api/createBill", fmt::arg("host", host)), body);
        res = response;
        ShowLog(fmt::format("Return from toyyibpay::createBill: {}: {}", responseCode, response));
        auto jv = boost::json::parse(response);
        if (jv.if_array()) {
            for (auto &x : *jv.if_array()) {
                if (x.if_object()) {
                    auto str = (*x.if_object())["BillCode"].if_string();
                    if (str) return str->c_str();
                }
            }
        } else if (jv.if_object()) {
            auto str = (*jv.if_object())["BillCode"].if_string();
            if (str) return str->c_str();
            str = (*jv.if_object())["status"].if_string();
            if (str && boost::iequals(*str, "error")) {
                str = (*jv.if_object())["msg"].if_string();
                return std::string("error:") + str->c_str();
            }
        }
    } catch (std::exception const &e) {
        ShowLog(fmt::format("CreateBill exception: {}", e.what()));
    } catch (...) {
        ShowLog(fmt::format("CreateBill unknown exception"));
    }
    auto msg = boost::replace_all_copy(res, "\t", "");
    return "error:" + msg;
}

/*
boost::json::value jv0 = {
    {"userSecretKey", secretKey},
    {"catname", code},
    {"catdescription", name}};

auto jv2 = boost::json::serialize(jv0);
ShowLog("\njv2 = " << jv2 << std::endl;
*/

std::string ToyyibPay::Process::CreateCategory(const std::string &code, const std::string &name) const {
    try {
        std::vector<std::pair<std::string, std::string>> array = {
            {"catname", code},
            {"catdescription", name},
            {"userSecretKey", secretKey},
        };

        auto jsonString = ConvertToUrlEncodedBody(array);

        auto url = fmt::format("https://{host}/index.php/api/createCategory", fmt::arg("host", host));
        auto [response, responseCode] = WebClient::Post(url, jsonString);
        ShowLog(fmt::format("from toyyibpay::createCategory, RESPONSE: {}: {}", responseCode, response));
        auto jv = boost::json::parse(response);
        if (jv.if_array()) {
            auto jo = jv.if_array();
            for (auto x : *jo) {
                if (x.if_object()) {
                    auto str = (*x.if_object())["CategoryCode"].if_string();
                    if (str) return str->c_str();
                }
            }
        } else if (jv.if_object()) {
            auto str = (*jv.if_object())["CategoryCode"].if_string();
            if (str) return str->c_str();
        }
    } catch (std::exception &e) {
        ShowLog(fmt::format("Return error exception: {}", e.what()));
    } catch (...) {
        ShowLog("Return error ");
    }
    ShowLog("parse response: Nil");
    return "";
}

// std::string ToyyibPay::Process::ConvertToUrlEncodedBody(const std::unordered_map<std::string, std::string> &array) const {
//     std::ostringstream out;

//     std::string delim;
//     for (const auto &x : array) {
//         out << delim << x.first << "=" << x.second;
//         delim = "&";
//     }
//     return out.str();
// }

// std::string ToyyibPay::Process::ConvertToUrlEncodedBody(const std::vector<std::pair<std::string, std::string>> &array) const {
//     std::ostringstream out;

//     std::string delim;
//     for (const auto &x : array) {
//         out << delim << x.first << "=" << x.second;
//         delim = "&";
//     }
//     return out.str();
// }
