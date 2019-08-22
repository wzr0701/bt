/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: media.proto */
#include "config.h"
#if(AMA_ENABLE==1)
#ifndef PROTOBUF_C_media_2eproto__INCLUDED
#define PROTOBUF_C_media_2eproto__INCLUDED

#include <protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _IssueMediaControl IssueMediaControl;


/* --- enums --- */

typedef enum _MediaControl {
  MEDIA_CONTROL__PLAY = 0,
  MEDIA_CONTROL__PAUSE = 1,
  MEDIA_CONTROL__NEXT = 2,
  MEDIA_CONTROL__PREVIOUS = 3,
  MEDIA_CONTROL__PLAY_PAUSE = 4
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(MEDIA_CONTROL)
} MediaControl;

/* --- messages --- */

struct  _IssueMediaControl
{
  ProtobufCMessage base;
  MediaControl control;
};
#define ISSUE_MEDIA_CONTROL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&issue_media_control__descriptor) \
    , MEDIA_CONTROL__PLAY }


/* IssueMediaControl methods */
void   issue_media_control__init
                     (IssueMediaControl         *message);
size_t issue_media_control__get_packed_size
                     (const IssueMediaControl   *message);
size_t issue_media_control__pack
                     (const IssueMediaControl   *message,
                      uint8_t             *out);
size_t issue_media_control__pack_to_buffer
                     (const IssueMediaControl   *message,
                      ProtobufCBuffer     *buffer);
IssueMediaControl *
       issue_media_control__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   issue_media_control__free_unpacked
                     (IssueMediaControl *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*IssueMediaControl_Closure)
                 (const IssueMediaControl *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    media_control__descriptor;
extern const ProtobufCMessageDescriptor issue_media_control__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_media_2eproto__INCLUDED */
#endif