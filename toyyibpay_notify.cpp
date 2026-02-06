#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <fmt/format.h>
#include <unordered_map>
#include "authDB.h"
#include "toyyibPay.h"
/*
refno : Payment reference no - external ref; our reference to how to update the payment (eg: 84fb96c5/41f3b8c9)
status : Payment status. 1= success, 2=pending, 3=fail
reason : Reason for the status received
billcode : Your billcode / permanent link
order_id : Your external payment reference no, if specified
amount : Payment amount received
transaction_time : Datetime of the transaction status received.

eg:

        refno: TP2407300069894486
        status: 1
        reason: Payment Approved
        billcode: 5sc2s1z8
        order_id: 84fb96c5/41f3b8c9/693d2a6b/billId --- folder/db/chat/billid
        amount: 3.35
        transaction_time: 2024-07-30 12:24:19

        fpx_transaction_id: 2407301224192625
        hash: be0303bbf86f38c84453e32676a72f63
        msg: ok
        status_id: 1
        transaction_id: TP2407300069894486

*/
std::unordered_map<std::string, std::string> ToyyibPay::Process::ReceiveNotification(const std::string &contentType, const std::string &body) {
    boost::tokenizer<boost::char_separator<char>> tok(contentType, boost::char_separator<char>(";", "", boost::drop_empty_tokens));
    auto formatOk = false;
    std::string sep;
    for (auto &x : tok) {
        auto val = boost::trim_copy(x);
        if (boost::iequals(x, "multipart/form-data")) {
            formatOk = true;
        } else {
            if (val.find("boundary=") != std::string::npos) {
                sep = val.substr(9);
            }
        }
    }
    if (formatOk && !sep.empty()) {
        boost::tokenizer<boost::char_separator<char>> tok(body, boost::char_separator<char>("\n\r", "", boost::drop_empty_tokens));
        std::unordered_map<std::string, std::string> keyVal;
        std::string key, value;
        for (auto &l : tok) {
            if (boost::contains(l, sep)) {
                if (!key.empty()) {
                    keyVal[key] = value;
                }
                key.clear();
                value.clear();
                continue;
            }
            if (boost::icontains(l, "Content-Disposition")) {
                boost::tokenizer<boost::char_separator<char>> tok(l, boost::char_separator<char>(";", "", boost::drop_empty_tokens));
                int i = 0;
                for (auto x = tok.begin(); x != tok.end(); x++, i++) {
                    if (i == 1) {
                        boost::tokenizer<boost::char_separator<char>> tok2(*x, boost::char_separator<char>("=\"", "", boost::drop_empty_tokens));
                        int j = 0;
                        for (auto y = tok2.begin(); y != tok2.end(); y++, j++) {
                            if (j == 1) {
                                key = boost::trim_copy(*y);
                            }
                        }
                    }
                }
            }
            if (l.empty())
                continue;
            else
                value = boost::trim_copy(l);
        }
        if (!key.empty()) {
            keyVal[key] = value;
        }
        return keyVal;
    } else {
        LOG_INFO("Incorrect format or separator empty: sep: {}", sep);
        //record missing trans:
        auto authDb = std::make_unique<AuthorizationDB>();
        authDb->Open();
        auto stt = authDb->GetSession().PrepareStatement("insert into failedToyyibMessages(timeCreated, contentType, message) values(@date, @contentType, @body)");
        stt->Bind("@date", std::chrono::system_clock::now());
        stt->Bind("@contentType", contentType);
        stt->Bind("@body", body);
        stt->ExecuteUpdate();
    }
    return std::unordered_map<std::string, std::string>{};
}

// void SplitTime() {
//     std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
//     auto duration = now.time_since_epoch();
//
//     typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days; /* UTC: +8:00 */
//
//     Days days = std::chrono::duration_cast<Days>(duration);
//     duration -= days;
//     auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
//     duration -= hours;
//     auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
//     duration -= minutes;
//     auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
//     duration -= seconds;
//     auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
//     duration -= milliseconds;
//     auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
//     duration -= microseconds;
//     auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
//
//     days, hours, minutes, seconds, milliseconds, nanoseconds;
// }
// }
