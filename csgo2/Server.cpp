#include "server.h"
namespace Server {
size_t receive_data(void* contents, size_t size, size_t nmemb, void* stream) {
    std::string* str = (std::string*)stream;
    (*str).append((char*)contents, size * nmemb);
    return size * nmemb;
}
CURLcode HttpGet(const std::string& strUrl, std::string& strResponse,
                 std::string header, int nTimeout) {
    CURLcode res;
    CURL* pCURL = curl_easy_init();
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }
    struct curl_slist* headers = NULL;

    if (header.empty() == false) {
        headers = curl_slist_append(headers, (char*)header.c_str());
    }
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers,
                                "Content-Type: application/json");  // text/html
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, nTimeout);
    curl_easy_setopt(pCURL, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, receive_data);
    curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
    res = curl_easy_perform(pCURL);
    curl_slist_free_all(headers);
    curl_easy_cleanup(pCURL);

    return res;
}

CURLcode HttpPost(const std::string& strUrl, std::string header,
                  std::string szJson, std::string& strResponse, int nTimeout) {
    CURLcode res;
    CURL* pCURL = curl_easy_init();
    struct curl_slist* headers = NULL;
    if (pCURL == NULL) {
        return CURLE_FAILED_INIT;
    }
    if (header.empty() == false) {
        headers = curl_slist_append(headers, (char*)header.c_str());
    }
    CURLcode ret;
    ret = curl_easy_setopt(pCURL, CURLOPT_URL, strUrl.c_str());
    // std::string data = curl_easy_escape(pCURL, szJson.c_str(),
    // szJson.size());
    std::string data = szJson;
    ret = curl_easy_setopt(pCURL, CURLOPT_POST, 1L);
    // headers = curl_slist_append(headers, "expect: ");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers,
                                "Content-Type: application/json");  // text/html
    headers = curl_slist_append(headers, "charsets: utf-8");
    ret = curl_easy_setopt(pCURL, CURLOPT_HTTPHEADER, headers);
    ret = curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, data.c_str());
    ret = curl_easy_setopt(pCURL, CURLOPT_TIMEOUT, 60);
    ret = curl_easy_setopt(pCURL, CURLOPT_NOSIGNAL, 1);
    ret = curl_easy_setopt(pCURL, CURLOPT_TIMEOUT_MS, 60000);
    ret = curl_easy_setopt(pCURL, CURLOPT_SSL_VERIFYPEER, false);
    ret = curl_easy_setopt(pCURL, CURLOPT_NOPROGRESS, 1L);
    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, receive_data);
    ret = curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, (void*)&strResponse);
    res = curl_easy_perform(pCURL);
    curl_slist_free_all(headers);
    curl_easy_cleanup(pCURL);
    return res;
}

}  // namespace Server
