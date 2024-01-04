#ifndef LOGGER_HEADER_
#define LOGGER_HEADER_

#define COGNIT_LOG_LEVEL_DEBUG 0
#define COGNIT_LOG_LEVEL_INFO  1
#define COGNIT_LOG_LEVEL_ERROR 2

#define COGNIT_LOG_LEVEL COGNIT_LOG_LEVEL_DEBUG

#define COGNIT_LOG_DEBUG(...)                       \
    if (COGNIT_LOG_LEVEL <= COGNIT_LOG_LEVEL_DEBUG) \
    printf("[DEBUG] %s", __VA_ARGS__)
#define COGNIT_LOG_INFO(...)                       \
    if (COGNIT_LOG_LEVEL <= COGNIT_LOG_LEVEL_INFO) \
    printf("[INFO] %s", __VA_ARGS__)
#define COGNIT_LOG_ERROR(...)                       \
    if (COGNIT_LOG_LEVEL <= COGNIT_LOG_LEVEL_ERROR) \
    printf("[ERROR] %s", __VA_ARGS__)

#endif // LOGGER_HEADER_
