#pragma once
#include "head.h"
#include "libcurl/libcurl.h"
namespace Server {
CURLcode HttpPost(const std::string& strUrl, std::string header,
                  std::string szJson, std::string& strResponse, int nTimeout);
CURLcode HttpGet(const std::string& strUrl, std::string& strResponse,
                 std::string header, int nTimeout);
}  // namespace Server
