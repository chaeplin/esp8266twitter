/*
 esp8266twitter.h - direct text posting to twitter
  chaeplin at gmail.com
*/

#ifndef esp8266Twitter_h
#define esp8266Twitter_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <base64.h>

#define SHA1_SIZE 20

#define BASE_HOST              "api.twitter.com"
#define BASE_URL               "https://api.twitter.com/1.1/statuses/update.json"
#define BASE_URI               "/1.1/statuses/update.json"
#define HTTPSPORT              443

#define KEY_HTTP_METHOD        "POST"
#define KEY_CONSUMER_KEY       "oauth_consumer_key"
#define KEY_NONCE              "oauth_nonce"
#define KEY_SIGNATURE_METHOD   "oauth_signature_method"
#define KEY_TIMESTAMP          "oauth_timestamp"
#define KEY_TOKEN              "oauth_token"
#define KEY_VERSION            "oauth_version"
#define KEY_SIGNATURE          "oauth_signature"
#define KEY_MEDIA_ID           "media_id"
#define KEY_MEDIA_TYPE         "media_type"
#define KEY_MEDIA_IDS          "media_ids"

#define VALUE_SIGNATURE_METHOD "HMAC-SHA1"
#define VALUE_VERSION          "1.0"

#define KEY_STATUS             "status"
#define UPLOAD_COMMAND         "command"

#define UPLOAD_BASE_HOST       "upload.twitter.com"
#define UPLOAD_BASE_URL        "https://upload.twitter.com/1.1/media/upload.json"
#define UPLOAD_BASE_URI        "/1.1/media/upload.json"
#define UPLOAD_OAUTH_KEY       "oauth_body_hash"

#define UPLOAD_CMD_INIT        "INIT"
#define UPLOAD_CMD_APPEND      "APPEND"
#define UPLOAD_CMD_FINALIZE    "FINALIZE"

#define UPLOAD_MEDIA_TYPE      "image/jpeg"
#define UPLOAD_MEDIA_SIZE      "total_bytes"
#define UPLOAD_MEDIA_SEG_INDX  "segment_index"

class esp8266Twitter {

private:
	uint32_t value_timestamp;
	uint32_t value_nonce;
	const char* value_status;
	void ssl_hmac_sha1(const uint8_t *msg, int length, const uint8_t *key, int key_len, uint8_t *digest);
	String URLEncode(const char* msg);
	String make_signature(const char* secret_one, const char* secret_two, String base_string);
	bool do_http_text_post(String OAuth_header);
	String make_OAuth_header(String oauth_signature);
	String make_signature(String base_string);
	String make_base_string(String para_string);
	String make_para_string();
	esp8266Twitter& setKeys(const char * CONSUMER_KEY, const char * CONSUMER_SECRET, const char * ACCESS_TOKEN, const char * ACCESS_SECRET);
	const char* consumerkey;
	const char* consumersecret;
	const char* accesstoken;
	const char* accessscecret;
	const char* api_fingerprint    = "D8 01 5B F4 6D FB 91 C6 E4 B1 B6 AB 9A 72 C1 68 93 3D C2 D9";
	const char* upload_fingerprint = "95 00 10 59 C8 27 FD 2C D0 76 12 F7 88 35 64 21 F5 60 D3 E9";

public:
	esp8266Twitter(const char *, const char *, const char *, const char *);
	boolean tweet(const char* message, uint32_t timestamp, uint32_t nonce);
};

#endif