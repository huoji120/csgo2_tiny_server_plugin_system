#include "libcurl.h"

CRITICAL_SECTION curl_critical;

size_t writer(char* data, size_t size, size_t nmemb, std::string* writer_data)
{
    size_t sizes = size * nmemb;

    if (NULL == writer_data) {
        return 0;
    }

    writer_data->append(data, sizes);
    return sizes;
}

bool curl_get(std::string url, std::string* pbuffer, curl_slist* headers, bool tls, bool proxy)
{
    curl_code code;
    curl_handle handle;

    handle = curl_easy_init();
    if (NULL == handle) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_WRITEDATA, pbuffer);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    if (tls) {
        code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, false);
        if (code != CURLE_OK) {
            curl_easy_cleanup(handle);
            return false;
        }
        code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, true);
        if (code != CURLE_OK) {
            curl_easy_cleanup(handle);
            return false;
        }
    }
    code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }

    if (proxy) {
        curl_easy_setopt(handle, CURLOPT_PROXY, "127.0.0.1");
        curl_easy_setopt(handle, CURLOPT_PROXYPORT, 7890);
        curl_easy_setopt(handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
    }

    EnterCriticalSection(&curl_critical);
    code = curl_easy_perform(handle);
    LeaveCriticalSection(&curl_critical);

    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }

    curl_easy_cleanup(handle);
    return true;
}

bool curl_post(std::string url, std::string* pbuffer, std::string data, curl_slist* headers, bool tls, bool proxy)
{
    curl_code code;
    curl_handle handle;

    handle = curl_easy_init();
    if (NULL == handle) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_WRITEDATA, pbuffer);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    if (tls) {
        code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, false);
        if (code != CURLE_OK) {
            curl_easy_cleanup(handle);
            return false;
        }
        code = curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, true);
        if (code != CURLE_OK) {
            curl_easy_cleanup(handle);
            return false;
        }
    }
    code = curl_easy_setopt(handle, CURLOPT_POST, true);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }

    if (proxy) {
        curl_easy_setopt(handle, CURLOPT_PROXY, "127.0.0.1");
        curl_easy_setopt(handle, CURLOPT_PROXYPORT, 7890);
        curl_easy_setopt(handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
    }

    EnterCriticalSection(&curl_critical);
    code = curl_easy_perform(handle);
    LeaveCriticalSection(&curl_critical);

    if (code != CURLE_OK) {
        curl_easy_cleanup(handle);
        return false;
    }

    curl_easy_cleanup(handle);
    return true;
}

void curl_init()
{
    InitializeCriticalSection(&curl_critical);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void curl_cleanup()
{
    curl_global_cleanup();
    DeleteCriticalSection(&curl_critical);
}