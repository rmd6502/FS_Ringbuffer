# FS_Ringbuffer

An ESP32 library to create a ringbuffer backed by a SPIFFS file. By default, when the buffer is filled, the earliest entries will be
overwritten. Detect and prevent this condition by checking that buffer.free >= size.
