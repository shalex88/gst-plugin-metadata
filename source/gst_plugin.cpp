#include <gst/gst.h>
#include "config.h"
#include "injector/gst_metadata_injector.h"
#include "extractor/gst_metadata_extractor.h"

static gboolean metadata_init(GstPlugin* metadata) {
    // Register each element with GStreamer
    if (!gst_element_register(metadata, "metadataextractor", GST_RANK_NONE, GST_TYPE_METADATAEXTRACTOR))
        return FALSE;
    if (!gst_element_register(metadata, "metadatainjector", GST_RANK_NONE, GST_TYPE_METADATAINJECTOR))
        return FALSE;

    return TRUE;
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  metadata,
                  "Metadata manipulation plugin",
                  metadata_init,
                  PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
