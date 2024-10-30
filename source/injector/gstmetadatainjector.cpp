#include "gstmetadatainjector.h"
#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC(gst_metadata_injector_debug);
#define GST_CAT_DEFAULT gst_metadata_injector_debug

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
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS ("ANY")
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS ("ANY")
);

#define gst_metadata_injector_parent_class parent_class
G_DEFINE_TYPE(GstMetadataInjector, gst_metadata_injector, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(metadata_injector, "metadata_injector", GST_RANK_NONE,
                            GST_TYPE_METADATAINJECTOR);

static void gst_metadata_injector_set_property(GObject* object,
                                               guint prop_id, const GValue* value, GParamSpec* pspec);

static void gst_metadata_injector_get_property(GObject* object,
                                               guint prop_id, GValue* value, GParamSpec* pspec);

static gboolean gst_metadata_injector_sink_event(GstPad* pad,
                                                 GstObject* parent, GstEvent* event);

static GstFlowReturn gst_metadata_injector_chain(GstPad* pad,
                                                 GstObject* parent, GstBuffer* buf);

/* GObject vmethod implementations */

/* initialize the metadatainjector's class */
static void gst_metadata_injector_class_init(GstMetadataInjectorClass* klass) {
    auto gobject_class = reinterpret_cast<GObjectClass*>(klass);
    auto gstelement_class = reinterpret_cast<GstElementClass*>(klass);

    gobject_class->set_property = gst_metadata_injector_set_property;
    gobject_class->get_property = gst_metadata_injector_get_property;

    g_object_class_install_property(gobject_class, PROP_SILENT,
                                    g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                         FALSE, G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class,
                                         "Injector for metadata",
                                         "Filter/Video",
                                         "Injector for metadata",
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
static void gst_metadata_injector_init(GstMetadataInjector* filter) {
    filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_event_function(filter->sinkpad,
                               GST_DEBUG_FUNCPTR (gst_metadata_injector_sink_event));
    gst_pad_set_chain_function(filter->sinkpad,
                               GST_DEBUG_FUNCPTR (gst_metadata_injector_chain));
    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    filter->silent = FALSE;
}

static void gst_metadata_injector_set_property(GObject* object, guint prop_id,
                                               const GValue* value, GParamSpec* pspec) {
    GstMetadataInjector* filter = GST_METADATAINJECTOR(object);

    switch (prop_id) {
        case PROP_SILENT:
            filter->silent = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void gst_metadata_injector_get_property(GObject* object, guint prop_id,
                                               GValue* value, GParamSpec* pspec) {
    GstMetadataInjector* filter = GST_METADATAINJECTOR(object);

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
static gboolean gst_metadata_injector_sink_event(GstPad* pad, GstObject* parent,
                                                 GstEvent* event) {
    GstMetadataInjector* filter = GST_METADATAINJECTOR(parent);
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
static GstFlowReturn gst_metadata_injector_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
    GstMetadataInjector* filter = GST_METADATAINJECTOR(parent);
    static int frame_counter{};

    if (filter->silent == FALSE)
        g_print("%s: frame %d\n", gst_element_get_name(GST_ELEMENT(filter)), frame_counter++);


    /* just push out the incoming buffer without touching it */
    return gst_pad_push(filter->srcpad, buf);
}
