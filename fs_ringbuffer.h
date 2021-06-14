#include <FS.h>
#include <SPIFFS.h>

enum RBStatus {
    RB_OK = 0,
    RB_INSUFFICIENT_SPACE,
    RB_WRITE_ERROR,
    RB_READ_ERROR,
    RB_INCONSISTENT_STATE,
};

class FS_RingBuffer {
    public:
        // Initializes the file if it doesn't exist.
        FS_RingBuffer(String name, uint32_t max_size, FS *fs = &SPIFFS);

        virtual ~FS_RingBuffer() { end(); }

        // Opens the file.
        // @param overwrite If true, clears the file and reinitializes it.
        RBStatus begin(bool overwrite = false);

        // Close the file (called by the destructor).
        RBStatus end();

        // Write data to the file.
        // First overload takes a String.
        RBStatus write(String data);
        // Second overload takes a byte array. If you want to write a c string and include
        // the null terminator, make sure to pass length as strlen(str) + 1.
        RBStatus write(byte *data, uint32_t length);

        // Read exactly `length` bytes from the file.
        String read(uint32_t length);
        // Read until we see the terminator, including it in the string, or we read maxLength bytes.
        String readUntil(String terminator="\n", uint32_t maxLength = -1);

        // Remaining space in file.
        uint32_t freeSpace() const;

        operator bool() const { return isValid; }
    private:
        uint32_t head, tail;
        uint32_t max_size;
        File buffer;
        bool isValid;
        String filename;
        FS *fs;

        RBStatus writeByte(byte data);
        RBStatus updateHeader();
        const uint32_t headerSize() const;
};
