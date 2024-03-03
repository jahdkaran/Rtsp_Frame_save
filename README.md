# RTSP Frame Saver

This application captures a single frame from an RTSP stream and saves it as a JPEG image.

## Prerequisites

The application is built using GStreamer, so you need to have GStreamer installed on your system.

## Building

To build the application, use the following command:

```bash
g++ -o rtsp_frame_saver main.cpp `pkg-config --cflags --libs gstreamer-1.0`
```

Replace `main.cpp` with the actual name of your source file.

## Usage

To run the application, use the following command:

```bash
./rtsp_frame_saver rtsp://your_rtsp_stream_url
```

Replace `rtsp://your_rtsp_stream_url` with the actual RTSP stream URL.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
