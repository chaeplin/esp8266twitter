esp8266twitter
===============

direct text post using esp8266/Arduino without 3rd party api.


https://dev.twitter.com/rest/reference/post/statuses/update

##### Based on

- tweeting_silicon : http://harizanov.com/2015/06/tweeting-silicon/
- https://github.com/igrr/axtls-8266/blob/master/crypto/hmac.c
- http://hardwarefun.com/tutorials/url-encoding-in-arduino


##### 

 - Use ntp or rtc to sync time
 - constructors : esp8266Twitter esp8266Twitter(consumer_key, consumer_secret, access_token, access_secret);
 - to tweet : esp8266Twitter.tweet(message, value_timestamp, value_nonce)


##### Limitations

 - no rate limit checking
 - no 140 limit checking

##### To use oauth :

 - make an app at https://apps.twitter.com
 - generate Access Token / Secret. 
 - Use 4 keys to sketch


##### for image on spiffs
 - post with media upload (if image is big, timout will occur) : https://github.com/chaeplin/esp8266_and_arduino/blob/master/_56-gopro-control/a-09-twitter-image-post/a-09-twitter-image-post.ino 
 - post with APPEND(chunked) : https://github.com/chaeplin/esp8266_and_arduino/blob/master/_56-gopro-control/a-09-twitter-image-post/a-09-twitter-image-post.ino
 - upload image to twitter is slow(3 ~ 4KB/s)

Enjoy.

// chaeplin