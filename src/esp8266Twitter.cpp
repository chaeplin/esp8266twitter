/*
 esp8266twitter.cpp - direct text posting to twitter
  chaeplin at gmail.com
*/

#include "esp8266twitter.h"
#include "Arduino.h"

extern "C" 
{
  typedef struct
  {
    uint32_t Intermediate_Hash[SHA1_SIZE / 4]; /* Message Digest */
    uint32_t Length_Low;            /* Message length in bits */
    uint32_t Length_High;           /* Message length in bits */
    uint16_t Message_Block_Index;   /* Index into message block array   */
    uint8_t Message_Block[64];      /* 512-bit message blocks */
  } SHA1_CTX;

  void SHA1_Init(SHA1_CTX *);
  void SHA1_Update(SHA1_CTX *, const uint8_t * msg, int len);
  void SHA1_Final(uint8_t *digest, SHA1_CTX *);
}

esp8266Twitter::esp8266Twitter(const char * CONSUMER_KEY, const char * CONSUMER_SECRET, const char * ACCESS_TOKEN, const char * ACCESS_SECRET) 
{
	setKeys(CONSUMER_KEY, CONSUMER_SECRET, ACCESS_TOKEN, ACCESS_SECRET);
}

boolean esp8266Twitter::tweet(const char* message, uint32_t timestamp, uint32_t nonce) 
{
  bool rtn = false;

  this->value_timestamp  = timestamp;
  this->value_nonce      = nonce;
  this->value_status     = message;

  String para_string     = make_para_string();
  String base_string     = make_base_string(para_string);
  String oauth_signature = make_signature(base_string);
  String OAuth_header    = make_OAuth_header(oauth_signature);
  rtn                    = do_http_text_post(OAuth_header);

  return rtn;
}

// https://github.com/igrr/axtls-8266/blob/master/crypto/hmac.c
void esp8266Twitter::ssl_hmac_sha1(const uint8_t *msg, int length, const uint8_t *key, int key_len, uint8_t *digest) 
{
  SHA1_CTX context;
  uint8_t k_ipad[64];
  uint8_t k_opad[64];
  int i;

  memset(k_ipad, 0, sizeof k_ipad);
  memset(k_opad, 0, sizeof k_opad);
  memcpy(k_ipad, key, key_len);
  memcpy(k_opad, key, key_len);

  for (i = 0; i < 64; i++)
  {
    k_ipad[i] ^= 0x36;
    k_opad[i] ^= 0x5c;
  }

  SHA1_Init(&context);
  SHA1_Update(&context, k_ipad, 64);
  SHA1_Update(&context, msg, length);
  SHA1_Final(digest, &context);
  SHA1_Init(&context);
  SHA1_Update(&context, k_opad, 64);
  SHA1_Update(&context, digest, SHA1_SIZE);
  SHA1_Final(digest, &context);
}

// from http://hardwarefun.com/tutorials/url-encoding-in-arduino
// modified by chaeplin
String esp8266Twitter::URLEncode(const char* msg) 
{
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";

  while (*msg != '\0') 
  {
    if ( ('a' <= *msg && *msg <= 'z')
         || ('A' <= *msg && *msg <= 'Z')
         || ('0' <= *msg && *msg <= '9')
         || *msg  == '-' || *msg == '_' || *msg == '.' || *msg == '~' ) 
    {
      encodedMsg += *msg;
    } 
    else 
    {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 0xf];
    }
    msg++;
  }
  return encodedMsg;
}

String esp8266Twitter::make_signature(const char* secret_one, const char* secret_two, String base_string) 
{
  String signing_key = URLEncode(secret_one);
  signing_key += "&";
  signing_key += URLEncode(secret_two);

  uint8_t digestkey[32];
  SHA1_CTX context;
  SHA1_Init(&context);
  SHA1_Update(&context, (uint8_t*) signing_key.c_str(), (int)signing_key.length());
  SHA1_Final(digestkey, &context);

  uint8_t digest[32];
  ssl_hmac_sha1((uint8_t*) base_string.c_str(), (int)base_string.length(), digestkey, SHA1_SIZE, digest);

  String oauth_signature = URLEncode(base64::encode(digest, SHA1_SIZE).c_str());

  return oauth_signature;
}

bool esp8266Twitter::do_http_text_post(String OAuth_header) 
{
  String payload = "";
  String uri_to_post = BASE_URI;
  String req_body_to_post;

  req_body_to_post = "status=";
  req_body_to_post += String(URLEncode(this->value_status));

  HTTPClient http;
  http.begin(BASE_HOST, HTTPSPORT, uri_to_post, api_fingerprint);
  http.addHeader("Authorization", OAuth_header);
  http.addHeader("Content-Length", String(req_body_to_post.length()));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(req_body_to_post);

  // debug 
  Serial.print("[DEBUG] do_http_text_post httpCode : ");
  Serial.println(httpCode);


  if (httpCode > 0) 
  {
    //if (httpCode >= 200 && httpCode < 400) {
      payload = http.getString();
      Serial.print("[DEBUG] do_http_text_post payload : ");
      Serial.println(payload);      
    //}
  } 
  else 
  {
    http.end();
    return false;
  }

  http.end();

  if (httpCode >= 200 && httpCode < 400) 
  {
  	return true;
  } 
  else 
  {
    return false;
  }
}

String esp8266Twitter::make_OAuth_header(String oauth_signature) 
{
  String OAuth_header = "OAuth ";
  OAuth_header += KEY_CONSUMER_KEY;
  OAuth_header += "=\"";
  OAuth_header += this->consumerkey;
  OAuth_header += "\", ";
  OAuth_header += KEY_NONCE;
  OAuth_header += "=\"";
  OAuth_header += this->value_nonce;
  OAuth_header += "\", ";
  OAuth_header += KEY_SIGNATURE;
  OAuth_header += "=\"";
  OAuth_header += oauth_signature;
  OAuth_header += "\", ";
  OAuth_header += KEY_SIGNATURE_METHOD;
  OAuth_header += "=\"";
  OAuth_header += VALUE_SIGNATURE_METHOD;
  OAuth_header += "\", ";
  OAuth_header += KEY_TIMESTAMP;
  OAuth_header += "=\"";
  OAuth_header += this->value_timestamp;
  OAuth_header += "\", ";
  OAuth_header += KEY_TOKEN;
  OAuth_header += "=\"";
  OAuth_header += this->accesstoken;
  OAuth_header += "\", ";
  OAuth_header += KEY_VERSION;
  OAuth_header += "=\"";
  OAuth_header += VALUE_VERSION;
  OAuth_header += "\"";

  return OAuth_header;
}

String esp8266Twitter::make_signature(String base_string) 
{
  String signing_key = URLEncode(this->consumersecret);
  signing_key += "&";
  signing_key += URLEncode(this->accessscecret);

  uint8_t digestkey[32];
  SHA1_CTX context;
  SHA1_Init(&context);
  SHA1_Update(&context, (uint8_t*) signing_key.c_str(), (int)signing_key.length());
  SHA1_Final(digestkey, &context);

  uint8_t digest[32];
  ssl_hmac_sha1((uint8_t*) base_string.c_str(), (int)base_string.length(), digestkey, SHA1_SIZE, digest);

  String oauth_signature = URLEncode(base64::encode(digest, SHA1_SIZE).c_str());

  return oauth_signature;
}

String esp8266Twitter::make_base_string(String para_string) 
{
  String base_string = KEY_HTTP_METHOD;
  base_string += "&";
  base_string += URLEncode(BASE_URL);
  base_string += "&";
  base_string += URLEncode(para_string.c_str());

  return base_string;
}

String esp8266Twitter::make_para_string() 
{
  String para_string;
  para_string += KEY_CONSUMER_KEY;
  para_string += "=" ;
  para_string += this->consumerkey;
  para_string += "&";
  para_string += KEY_NONCE;
  para_string += "=";
  para_string += this->value_nonce;
  para_string += "&";
  para_string += KEY_SIGNATURE_METHOD;
  para_string += "=";
  para_string += VALUE_SIGNATURE_METHOD;
  para_string += "&";
  para_string += KEY_TIMESTAMP;
  para_string += "=";
  para_string += this->value_timestamp;
  para_string += "&";
  para_string += KEY_TOKEN;
  para_string += "=";
  para_string += this->accesstoken;
  para_string += "&";
  para_string += KEY_VERSION;
  para_string += "=";
  para_string += VALUE_VERSION;
  para_string += "&";
  para_string += KEY_STATUS;
  para_string += "=";
  para_string += URLEncode(this->value_status);

  return para_string;
}

esp8266Twitter& esp8266Twitter::setKeys(const char * CONSUMER_KEY, const char * CONSUMER_SECRET, const char * ACCESS_TOKEN, const char * ACCESS_SECRET) 
{
  this->consumerkey    = CONSUMER_KEY;
  this->consumersecret = CONSUMER_SECRET;
  this->accesstoken    = ACCESS_TOKEN;
  this->accessscecret  = ACCESS_SECRET;
  return *this;
}

// end of esp8266Twitter.cpp
