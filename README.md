# dpi-c2-detector

C++20 Deep Packet Inspection (DPI) engine for analyzing network captures (`.pcap` / `.pcapng`) to identify Command-and-Control (C2) traffic patterns.

## Architecture

- **Ingestion (`dpi::ingestion::PcapReader`)**: RAII wrapper around `PcapPlusPlus` file reader devices (`pcpp::IFileReaderDevice`). Reuses stack-allocated `pcpp::RawPacket` buffers across iterations with non-owning `pcpp::Packet` views to avoid per-packet heap allocations.
- **Logging**: Asynchronous, non-blocking logging backed by `spdlog` thread pools to keep I/O off the packet processing path.
- **Alerting**: Structured JSON output via `nlohmann/json`.

```text
.
├── CMakeLists.txt
├── include/
│   └── ingestion/
│       └── PcapReader.hpp
├── src/
│   ├── ingestion/
│   │   └── PcapReader.cpp
│   └── main.cpp
└── data/
    └── sample.pcap
```

## Dependencies

System requirements:
- C++20 compiler (`gcc >= 11` or `clang >= 14`)
- `cmake >= 3.20`
- `libpcap-dev`

Fetched automatically via CMake `FetchContent`:
- [`PcapPlusPlus`](https://github.com/seladb/PcapPlusPlus) (`v24.09`)
- [`spdlog`](https://github.com/gabime/spdlog) (`v1.15.3`)
- [`nlohmann/json`](https://github.com/nlohmann/json) (`v3.11.3`)

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Usage

```bash
./build/dpi_c2_detector <path_to_pcap_file>
```

Example:

```bash
./build/dpi_c2_detector data/sample.pcap
```

> Note: All other `*.pcap` and `*.pcapng` files are ignored via `.gitignore` to prevent committing large or sensitive packet captures.

