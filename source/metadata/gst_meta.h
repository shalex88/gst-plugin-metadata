// Based on https://github.com/jackersson/gst-python-hacks

#ifndef __GST_BUFFER_INFO_META_H__
#define __GST_BUFFER_INFO_META_H__

#include <gst/gst.h>

G_BEGIN_DECLS

// Api Type
// 1-st field of GstMetaInfo
#define GST_BUFFER_INFO_META_API_TYPE (gst_buffer_info_meta_api_get_type())
#define GST_BUFFER_INFO_META_INFO (gst_buffer_info_meta_get_info())

typedef struct _GstBufferInfoMeta GstBufferInfoMeta;
typedef struct _GstBufferInfo GstBufferInfo;

struct _GstBufferInfo {
    gchar* description;
};

struct _GstBufferInfoMeta {
    // Required as it is base structure for metadata
    // https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstMeta.html
    GstMeta meta;

    // Custom fields
    GstBufferInfo info;
};

GType gst_buffer_info_meta_api_get_type(void);

GST_EXPORT
const GstMetaInfo* gst_buffer_info_meta_get_info(void);

GST_EXPORT
GstBufferInfoMeta* gst_buffer_add_buffer_info_meta(GstBuffer *buffer, GstBufferInfo* metadata);

GST_EXPORT
gboolean gst_buffer_remove_buffer_info_meta(GstBuffer *buffer);

GST_EXPORT
GstBufferInfoMeta* gst_buffer_get_buffer_info_meta(GstBuffer* b);

G_END_DECLS

#endif /* __GST_BUFFER_INFO_META_H__ */