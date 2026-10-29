#include "ingestion/PcapReader.hpp"

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <memory>

namespace {
    void initAsyncLogger() {
        constexpr std::size_t kQueueSize = 8192;
        constexpr std::size_t kBackingThreads = 1;

        spdlog::init_thread_pool(kQueueSize, kBackingThreads);
        auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto asyncLogger = std::make_shared<spdlog::async_logger>(
            "dpi_logger",
            stdoutSink,
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );

        asyncLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
        asyncLogger->set_level(spdlog::level::info);
        spdlog::set_default_logger(asyncLogger);
    }
} // namespace

int main(int argc, char* argv[]) {
    try {
        initAsyncLogger();

        if (argc < 2) {
            spdlog::error("Usage: {} <path_to_pcap_file>", argv[0]);
            spdlog::shutdown();
            return EXIT_FAILURE;
        }

        const std::filesystem::path pcapPath{argv[1]};
        dpi::ingestion::PcapReader reader{pcapPath};

        if (!reader.open()) {
            spdlog::shutdown();
            return EXIT_FAILURE;
        }

        [[maybe_unused]] const auto totalPackets = reader.processPackets();
        reader.close();

        spdlog::shutdown();
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        spdlog::critical("Unhandled exception: {}", ex.what());
        spdlog::shutdown();
        return EXIT_FAILURE;
    }
}
