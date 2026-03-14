# KanoopProtocolQt

Qt library providing threaded HTTP operations and an auto-reconnecting MQTT client with SSL/TLS support.

**[API Documentation](https://StevePunak.github.io/KanoopProtocolQt/)** | [Class List](https://StevePunak.github.io/KanoopProtocolQt/annotated.html) | [Class Hierarchy](https://StevePunak.github.io/KanoopProtocolQt/hierarchy.html) | [Files](https://StevePunak.github.io/KanoopProtocolQt/files.html)

## Requirements

- C++11
- Qt 6.7.0+ (Core, Network, Mqtt, SerialBus)
- CMake 3.16+

### Dependencies

| Library | Description |
|---------|-------------|
| [KanoopCommonQt](https://github.com/StevePunak/KanoopCommonQt) | Threading, logging, time utilities |

## Building

KanoopProtocolQt is typically built as part of the [meta-qt-mains](https://github.com/epcpower/meta-qt-mains) superproject. To build standalone:

```bash
# Clone both repos side by side
git clone git@github.com:StevePunak/KanoopCommonQt.git
git clone git@github.com:StevePunak/KanoopProtocolQt.git

# Create a wrapper CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(KanoopProtocol-Standalone)
add_subdirectory(KanoopCommonQt)
add_subdirectory(KanoopProtocolQt)
EOF

# Build
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake --build build --parallel
```

## Modules

### HTTP

Threaded HTTP client built on `QNetworkAccessManager`. Each request runs in a dedicated `QThread` and emits `operationComplete()` when done.

| Class | Description |
|-------|-------------|
| `HttpOperation` | Abstract base — URL, headers, cookies, SSL, timeout, status code |
| `HttpGet` | GET with query parameters and optional streaming response |
| `HttpPost` | POST with raw or JSON body |
| `HttpPut` | PUT with raw or JSON body |
| `HttpDelete` | DELETE with optional body |
| `HttpUpload` | Multipart form upload with file and parameters |

```cpp
HttpGet get("https://api.example.com/data");
get.addParameter("page", "1");
connect(&get, &HttpOperation::operationComplete, [&]() {
    qDebug() << get.statusCode() << get.responseBody();
});
get.start();
```

### MQTT

Auto-reconnecting MQTT client supporting MQTT 5.0 and 3.1.1 with optional TLS.

| Class | Description |
|-------|-------------|
| `MqttParameters` | Connection config — host, port, auth, SSL certs, protocol version |
| `MqttClient` | Wrapper around `QMqttClient` with auto-reconnect and SSL |

```cpp
MqttParameters params("broker.example.com", 8883);
params.setUseSsl(true);
params.setUsername("device");

MqttClient client(params);
connect(&client, &MqttClient::clientConnected, [&]() {
    client.subscribe("sensors/#");
});
client.start();
```

## API Documentation

Full Doxygen documentation is generated automatically on CI and published to GitHub Pages:

**https://StevePunak.github.io/KanoopProtocolQt/**

To generate locally:

```bash
# Via CMake (requires Doxygen + Graphviz)
cmake --build build --target doc_KanoopProtocolQt

# Or directly
cd KanoopProtocolQt
doxygen Doxyfile
```

Open `docs/html/index.html` in a browser to browse the local copy.

## Testing

```bash
# Run unit tests
ctest --test-dir build/KanoopProtocolQt --output-on-failure --exclude-regex integration

# Run integration tests (requires internet — hits httpbin.org)
ctest --test-dir build/KanoopProtocolQt --output-on-failure --tests-regex integration
```

| Suite | Tests | Type |
|-------|-------|------|
| `tst_mqtt_parameters` | 17 | Unit — MqttParameters construction, getters/setters, copy |
| `tst_http` | 30 | Unit — headers, accessors, RequestMethod mapping, HttpStatus |
| `tst_http_integration` | 19 | Integration — GET/POST/PUT/DELETE against httpbin.org |

## CI

[![CI](https://github.com/StevePunak/KanoopProtocolQt/actions/workflows/ci.yaml/badge.svg)](https://github.com/StevePunak/KanoopProtocolQt/actions/workflows/ci.yaml)

Builds on every push using GitHub Actions (`ubuntu-latest`, Qt 6.10.1, Ninja). Unit and integration tests run separately.

## Project Structure

```
KanoopProtocolQt/
  include/Kanoop/
    http/               HTTP operation classes
    mqtt/               MQTT client and parameters
    kanoopprotocol.h    Export macro
  src/                  Implementation files
  tests/                Unit and integration tests
  docs/                 Generated documentation (not committed)
  Doxyfile              Doxygen configuration
  CMakeLists.txt        Build configuration
```

## License

MIT
