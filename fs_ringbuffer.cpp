#include <Arduino.h>

#include "fs_ringbuffer.h"

FS_RingBuffer::FS_RingBuffer(String name, uint32_t max_size_, FS *fs) : isValid(false), filename(name), fs(fs) {
    if (!fs->exists(name)) {
        buffer = fs->open(name, "w+");
        if (!buffer) {
            return;
        }
        isValid = true;
        head = tail = headerSize();
        max_size = max_size_ + headerSize();
        updateHeader();
    } else {
        buffer = fs->open(name, "r+");
        if (!buffer) {
            return;
        }
        buffer.seek(0);
        if (buffer.available() >= headerSize()) {
            buffer.read((byte *)&head, sizeof(uint32_t));
            buffer.read((byte *)&tail, sizeof(uint32_t));
            buffer.read((byte *)&max_size, sizeof(uint32_t));
            // Serial.print("head "); Serial.print(head);
            // Serial.print(" tail "); Serial.print(tail);
            // Serial.print(" max "); Serial.print(max_size);
        } else {
            // Serial.println("header not found");
            head = tail = headerSize();
            max_size = max_size_ + headerSize();
            updateHeader();
        }
        isValid = true;
    }
}

RBStatus FS_RingBuffer::begin(bool overwrite) {
    return RB_OK;
}

RBStatus FS_RingBuffer::end() {
    updateHeader();
    buffer.close();
    return RB_OK;
}

const uint32_t FS_RingBuffer::headerSize() const {
    return 3 * sizeof(uint32_t);
}

RBStatus FS_RingBuffer::write(String data) {
    RBStatus status = RB_OK;
    uint32_t size = data.length();
    if (freeSpace() < data.length()) {
        size = freeSpace();
    }
    if (size == 0) {
        return RB_INSUFFICIENT_SPACE;
    }
    for (uint32_t i=0; i < size; ++i) {
        status = writeByte(data[i]);
        if (status != RB_OK) {
            break;
        }
    }
    if (status == RB_OK) {
        status = updateHeader();
    }
    return status;
}

uint32_t FS_RingBuffer::freeSpace() const {
    if (head >= tail) {
        return max_size - head + tail - headerSize();
    }
    return tail - head - 1;
}

RBStatus FS_RingBuffer::writeByte(byte data) {
    if (!buffer) {
        return RB_INCONSISTENT_STATE;
    }
    if (freeSpace() == 0) {
        return RB_INSUFFICIENT_SPACE;
    }
    buffer.seek(head);
    size_t written = buffer.write(data);
    ++head;
    if (head > max_size) {
        head = headerSize();
    }
    return RB_OK;
}

String FS_RingBuffer::read(uint32_t length) {
    String ret;
    char buf[length];
    buffer.seek(tail);
    while (length > 0 && head != tail) {
        uint32_t readlen = length;
        if (tail == max_size) {
            tail = headerSize();
            buffer.seek(tail);
        }
        if (readlen + tail >= max_size) {
            readlen = max_size - tail;
        }
        if (tail < head && readlen + tail >= head) {
            readlen = head - tail;
        }
        uint32_t bytesread = buffer.readBytes(buf, readlen);
        length -= bytesread;
        tail += bytesread;
        buf[bytesread] = 0;
        ret += String((const char *)buf);
    }
    return ret;
}

String FS_RingBuffer::readUntil(String delim, uint32_t maxLength) {
    String ret;
    while (freeSpace() > 0 && (maxLength == -1 || ret.length() < maxLength)) {
        String b = read(1);
        ret += b;
        if (ret.endsWith(delim)) break;
    }
    return ret;
}

RBStatus FS_RingBuffer::updateHeader() {
    buffer.seek(0);
    buffer.write((byte *)&head, sizeof(uint32_t));
    buffer.write((byte *)&tail, sizeof(uint32_t));
    buffer.write((byte *)&max_size, sizeof(uint32_t));
    return RB_OK;
}
