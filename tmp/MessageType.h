// MessageType.h
#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

enum class MessageType {
    Text = 0,
    Image,
    Video,
    Audio,
    Mixed
};

enum class MessageRole {
    Sender = 0,
    Receiver
};

#endif // MESSAGETYPE_H
