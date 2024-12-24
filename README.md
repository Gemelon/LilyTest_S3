# _LilyTest_S3 project_

This is an example of using LVGL 9, ESP-IDF and EEZ Studio with a LILYGO T-Display S3.

So far I have not found a working example of the Lilygo T-Display S3 that works with the new LVGL 9 version and the new ESP-IDF version 5. I then tried to find out the basics of operating the display with the new software versions myself.

Here is the result of my efforts.

This does not claim to be complete and it may contain errors, as I am still a beginner in the area of ​​LVGL and ESP-IDF. But I hope that it can help to put one or another on the right track.

# issues
I haven't found a solution for the problem yet. My quick n dirty solution ist to change this row simply to #include "../../../main/lv_conf.h"

D:/Projekte/Projekte 2025/GitHub/LilyTest_S3/managed_components/lvgl__lvgl/src/lv_conf_internal.h:58:18: fatal error: lv_conf.h: No such file or directory
   58 |         #include "lv_conf.h"
      |                  ^~~~~~~~~~~

      #include "../../../main/lv_conf.h"