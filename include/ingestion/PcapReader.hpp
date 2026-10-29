#pragma once

#include <PcapFileDevice.h>

#include <cstdint>
#include <filesystem>
#include <memory>

namespace dpi::ingestion {

    class PcapReader final {
    public:
        explicit PcapReader(std::filesystem::path pcapPath);
        ~PcapReader() noexcept;

        // Strict RAII: Disable copying, allow moving
        PcapReader(const PcapReader&) = delete;
        PcapReader& operator=(const PcapReader&) = delete;
        PcapReader(PcapReader&&) noexcept = default;
        PcapReader& operator=(PcapReader&&) noexcept = default;

        [[nodiscard]] bool open();
        void close() noexcept;
        [[nodiscard]] bool isOpen() const noexcept;
        [[nodiscard]] std::uint64_t processPackets();

    private:
        std::filesystem::path filePath_;
        std::unique_ptr<pcpp::IFileReaderDevice> reader_{nullptr};
    };

} // namespace dpi::ingestion
