#include <gst/gst.h>
#include "metadata/gst_meta.h"
#include "gst_metadata_extractor.h"

GST_DEBUG_CATEGORY_STATIC(gst_metadata_extractor_debug);
#define GST_CAT_DEFAULT gst_metadata_extractor_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("video_sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS ("ANY")
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS ("ANY")
);

#define gst_metadata_extractor_parent_class parent_class
G_DEFINE_TYPE(GstMetadataExtractor, gst_metadata_extractor, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(metadata_extractor, "metadata_extractor", GST_RANK_NONE,
                            GST_TYPE_METADATAEXTRACTOR);

static void gst_metadata_extractor_set_property(GObject* object,
                                                guint prop_id, const GValue* value, GParamSpec* pspec);

static void gst_metadata_extractor_get_property(GObject* object,
                                                guint prop_id, GValue* value, GParamSpec* pspec);

static gboolean gst_metadata_extractor_sink_event(GstPad* pad,
                                                  GstObject* parent, GstEvent* event);

static GstFlowReturn gst_metadata_extractor_chain(GstPad* pad,
                                                  GstObject* parent, GstBuffer* buf);

/* GObject vmethod implementations */

/* initialize the metadataextractor's class */
static void gst_metadata_extractor_class_init(GstMetadataExtractorClass* klass) {
    auto gobject_class = reinterpret_cast<GObjectClass*>(klass);
    auto gstelement_class = reinterpret_cast<GstElementClass*>(klass);

    gobject_class->set_property = gst_metadata_extractor_set_property;
    gobject_class->get_property = gst_metadata_extractor_get_property;

    g_object_class_install_property(gobject_class, PROP_SILENT,
                                    g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                         FALSE, G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class,
                                         "Extractor for metadata",
                                         "Filter/Video",
                                         "Extractor for metadata",
                                         "shalex <<user@hostname.org>>");

    gst_element_class_add_pad_template(gstelement_class,
                                       gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(gstelement_class,
                                       gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_metadata_extractor_init(GstMetadataExtractor* filter) {
    filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_event_function(filter->sinkpad,
                               GST_DEBUG_FUNCPTR (gst_metadata_extractor_sink_event));
    gst_pad_set_chain_function(filter->sinkpad,
                               GST_DEBUG_FUNCPTR (gst_metadata_extractor_chain));
    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    filter->silent = FALSE;
}

static void gst_metadata_extractor_set_property(GObject* object, guint prop_id,
                                                const GValue* value, GParamSpec* pspec) {
    GstMetadataExtractor* filter = GST_METADATAEXTRACTOR(object);

    switch (prop_id) {
        case PROP_SILENT:
            filter->silent = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void gst_metadata_extractor_get_property(GObject* object, guint prop_id,
                                                GValue* value, GParamSpec* pspec) {
    GstMetadataExtractor* filter = GST_METADATAEXTRACTOR(object);

    switch (prop_id) {
        case PROP_SILENT:
            g_value_set_boolean(value, filter->silent);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_metadata_extractor_sink_event(GstPad* pad, GstObject* parent,
                                                  GstEvent* event) {
    GstMetadataExtractor* filter = GST_METADATAEXTRACTOR(parent);
    gboolean ret;

    GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT,
                   GST_EVENT_TYPE_NAME (event), event);

    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_CAPS: {
            GstCaps* caps;

            gst_event_parse_caps(event, &caps);
            /* do something with the caps */

            /* and forward */
            ret = gst_pad_event_default(pad, parent, event);
            break;
        }
        default:
            ret = gst_pad_event_default(pad, parent, event);
            break;
    }
    return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_metadata_extractor_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
    GstMetadataExtractor* filter = GST_METADATAEXTRACTOR(parent);
    static int frame_counter{};

    // if (filter->silent == FALSE)
    //     g_print("%s: frame %d\n", gst_element_get_name(GST_ELEMENT(filter)), frame_counter++);

    // Get metadata from the buffer
    auto metadata = gst_buffer_get_buffer_info_meta(buf);
    if(metadata) {
        g_print("%s: [%s]\n", gst_element_get_name(GST_ELEMENT(filter)), metadata->info.description);
    }

    // Just push out the incoming buffer without touching it
    return gst_pad_push(filter->srcpad, buf);
}
