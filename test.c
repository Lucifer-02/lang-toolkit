#include <stdio.h>
#include <curl/curl.h>
#include <jansson.h>

int main() {
    CURL *curl;
    CURLcode res;
    char *data = NULL;
    long response_code;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.example.com/data");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (res == CURLE_OK && response_code == 200) {
            json_error_t error;
            json_t *root = json_loads(data, 0, &error);
            if (root) {
                // Process the JSON data
                json_decref(root);
            } else {
                fprintf(stderr, "JSON error: %s\n", error.text);
            }
        } else {
            fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
        free(data);
    }

    return 0;
}
