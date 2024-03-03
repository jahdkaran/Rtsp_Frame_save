#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <fstream>

struct CustomData {
    GstElement *pipeline;
    GMainLoop *loop;
    bool frame_saved;
};

GstFlowReturn new_sample (GstElement *sink, gpointer data) {
    GstSample *sample;
    GstBuffer *buffer;
    GstMapInfo map;
    CustomData *customData = (CustomData *)data;

    /* Retrieve the buffer */
    g_signal_emit_by_name (sink, "pull-sample", &sample);
    if (sample) {
        /* The only thing we do in this example is print a * to indicate a received buffer */
        g_print ("*");

        /* Write buffer data to a file */
        buffer = gst_sample_get_buffer(sample);
        if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            std::ofstream out("output.jpg", std::ios::binary);
            out.write((char*)map.data, map.size);
            out.close();
            gst_buffer_unmap(buffer, &map);
        }

        gst_sample_unref (sample);

        /* Send an EOS event to the pipeline */
        gst_element_send_event(customData->pipeline, gst_event_new_eos());

        /* Set frame_saved to true */
        customData->frame_saved = true;

        /* Quit the main loop */
        g_main_loop_quit(customData->loop);

        return GST_FLOW_EOS;
    }

    return GST_FLOW_OK;
}

int main(int argc, char *argv[]) {
    CustomData data;
    GstElement *sink;

    /* Check input arguments */
    if (argc != 2) {
        g_printerr("Usage: %s <RTSP URL>\n", argv[0]);
        return -1;
    }

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Build the pipeline */
    gchar *pipeline_desc = g_strdup_printf("rtspsrc location=%s latency=0 sync=false qos=false ! decodebin async=false ! videoconvert async=false ! jpegenc async=false ! appsink name=sink max-buffers=1 drop=true", argv[1]);
    data.pipeline = gst_parse_launch(pipeline_desc, NULL);
    g_free(pipeline_desc);

    /* Set up appsink */
    sink = gst_bin_get_by_name(GST_BIN(data.pipeline), "sink");
    g_object_set (G_OBJECT (sink), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect (sink, "new-sample", G_CALLBACK (new_sample), &data);
    gst_object_unref (sink);

    /* Start playing */
    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    data.loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run(data.loop);

    /* Free resources */
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);

    return 0;
}