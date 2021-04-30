#include <curl/curl.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <nlohmann/json.hpp>
#include <malloc.h>
#define UNIX_DAY 86400

#define LAT 55.75222
#define LON 37.61556
auto appid = "3508d3548f43b75e74312bb8b5ba4668";
auto api_call_template =
"https://api.openweathermap.org/data/2.5/forecast/daily?lat=%f&lon=%f&units=metric&cnt=5&appid=%s";


using json = nlohmann::json;

struct MemoryStruct {
	char* memory;
	size_t size;
};
static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr) {
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}
char *get_response(char* url){
	struct MemoryStruct chunk;
	chunk.memory = (char*)malloc(1);
	chunk.size = 0;
	auto handler = curl_easy_init();
	if (handler) {
		CURLcode res;
		curl_easy_setopt(handler, CURLOPT_URL, url);
		curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(handler, CURLOPT_WRITEDATA, (void*)&chunk);
		res = curl_easy_perform(handler);
		curl_easy_cleanup(handler);
		if(res == CURLE_OK)
			return chunk.memory;
		else return nullptr;
	}
}
int main() {
	auto api_call = new char[256];
	sprintf(api_call, api_call_template, LAT, LON, appid);

	auto response = get_response(api_call);
	if(response == nullptr)
		return 1;
	
	auto response_json = json::parse(response);
	int maxP = 0, min_diff_indx = 0 ; 
	float  min_diff = float(response_json["list"][0]["temp"]["night"])-float(response_json["list"][0]["temp"]["morn"]);
	for(int i =0; i < 5; i++){
		if(response_json["list"][i]["pressure"] > maxP) maxP = response_json["list"][i]["pressure"];
		if(float(response_json["list"][0]["temp"]["night"])-float(response_json["list"][0]["temp"]["morn"]) < min_diff){
			min_diff_indx = i;
			min_diff =float(response_json["list"][0]["temp"]["night"])-float(response_json["list"][0]["temp"]["morn"]);
		}
	}
	printf("lat: %f lon: %f\nmax pressure: %d\nmin temp difference btwn morn and night: day: %d val:%f\n",
		LAT,LON, maxP, int(response_json["list"][min_diff_indx]["dt"]), min_diff);
	
	delete api_call;
	delete response;
}