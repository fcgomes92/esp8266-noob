#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)

#ifndef DEBUG
#define DEBUG false
#endif

#ifndef APP_TOPIC_NAME
#define APP_TOPIC_NAME "mainStrip"
#endif

#ifndef APP_TOPIC_PATH
#define APP_TOPIC_PATH "mainStrip"
#endif

#ifndef APP_HOST
#define APP_HOST "berry.local"
#endif

#ifndef APP_PORT
#define APP_PORT 1883
#endif

#ifndef APP_STRIP_PIXELS
#define APP_STRIP_PIXELS 144
#endif

#ifndef APP_ENABLE_PORTAL
#define APP_ENABLE_PORTAL false
#endif

#ifndef APP_OTA_PASSWORD
#define APP_OTA_PASSWORD ""
#endif
