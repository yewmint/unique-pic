# Unique Pic

Unique Pic helps you clean your album, get rid of thumbnails and duplicates.

## Build

1. cd ./compare
2. modify CMakeLists.txt to fit your own path to opencv library.
3. use cmake to build and install
4. enter 'node index.js path/to/your/album'

## Compare

Compare-Program scans image to extract a fingerprint, which is compared with
other fingerprints using Hamming-Distance to classify thumbnails and duplicates.

## License

MIT
