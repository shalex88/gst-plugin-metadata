# gst-plugin-metadata
[![Release](https://img.shields.io/github/v/release/shalex88/gst-plugin-metadata.svg)](https://github.com/shalex88/gst-plugin-metadata/releases/latest)

## Build

```bash
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

## Run
```bash
gst-inspect-1.0 metadata
gst-launch-1.0 videotestsrc num-buffers=5 ! metadatainjector ! metadataextractor ! fakesink
```