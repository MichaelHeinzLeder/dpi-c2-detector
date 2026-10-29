#include "ingestion/PcapReader.hpp"

#include <Packet.h>
#include <RawPacket.h>
#include <spdlog/spdlog.h>

#include <utility>

namespace dpi::ingestion {

    PcapReader::PcapReader(std::filesystem::path pcapPath)
        : filePath_(std::move(pcapPath)) {}

    PcapReader::~PcapReader() noexcept {
        close();
    }

    bool PcapReader::open() {
        if (isOpen()) {
            return true;
        }

        if (!std::filesystem::exists(filePath_)) {
            spdlog::error("PCAP file does not exist: {}", filePath_.string());
            return false;
        }

        reader_.reset(pcpp::IFileReaderDevice::getReader(filePath_.string()));
        if (!reader_) {
            spdlog::error("Unsupported PCAP file format: {}", filePath_.string());
            return false;
        }

        if (!reader_->open()) {
            spdlog::error("Failed to open PCAP reader device: {}", filePath_.string());
            reader_.reset();
            return false;
        }

        spdlog::info("Opened PCAP file: {}", filePath_.string());
        return true;
    }

    void PcapReader::close() noexcept {
        if (reader_ && reader_->isOpened()) {
            reader_->close();
            spdlog::debug("Closed PCAP reader device: {}", filePath_.string());
        }
        reader_.reset();
    }

    bool PcapReader::isOpen() const noexcept {
        return reader_ != nullptr && reader_->isOpened();
    }

    std::uint64_t PcapReader::processPackets() {
        if (!isOpen() && !open()) {
            return 0;
        }

        std::uint64_t packetCount{0};
        pcpp::RawPacket rawPacket;

        // Reuse stack-allocated RawPacket container across iterations
        while (reader_->getNextPacket(rawPacket)) {
            // Zero-copy parsing wrapper around rawPacket buffer
            pcpp::Packet parsedPacket(&rawPacket, false);
            ++packetCount;
        }

        spdlog::info("Finished processing '{}'. Total packets ingested: {}",
                     filePath_.string(), packetCount);
        return packetCount;
    }

} // namespace dpi::ingestion
