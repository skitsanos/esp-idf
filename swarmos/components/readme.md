### Utils


#### Accessing embedded content 

- EXT_EMBED_FILE(_name_)
- EXT_EMBED_FILE_SIZE(_name_)
- EXT_EMBED_FILE_CONTENT(_name_)

**Example**

```c
#include <stdio.h>
#include "esp_log.h"

#define TAG "esp32"

#include "../components/utils/embed_file.h"

EXT_EMBED_FILE(index_html)

void app_main()
{
    size_t len = EXT_EMBED_FILE_SIZE(index_html);
    uint8_t *content = EXT_EMBED_FILE_CONTENT(index_html);

    ESP_LOGI(TAG, "index.html size: %d bytes", len);
    ESP_LOGI(TAG, "index.html content: \n%s", (char *) content);
}
```
