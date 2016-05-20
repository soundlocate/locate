#include "PostProcessor.h"

// Note(robin): only valid values for dissimilarity are allowed (), no validity
// checks performed!
PostProcessor::PostProcessor(std::vector<Microfone> mics,
                             const f64 maxClusterSize, const f64 maxDist,
                             const u64 dissimilarity, const u64 meanWindow,
                             const u64 maxKeep, const f64 keepTime)
    : clusterer(
          dissimilarity == 0
              ? (Dissimilarity *)new DissimilarityMeanDirection(mics,
                                                                maxClusterSize)
              : (Dissimilarity *)new DissimilarityMeanDist(maxClusterSize)),
      positionBuffer(meanWindow, maxKeep), mics(mics), maxDist(maxDist),
      numMics(mics.size()), meanWindow(meanWindow), maxKeep(maxKeep),
      keepTime(keepTime) {}

u64 PostProcessor::add(FFTPacket::Sinus * p, v3 pos) {
    if(pos.norm() > maxDist || std::isnan(pos.x) || std::isnan(pos.y) ||
       std::isnan(pos.z) || std::isinf(pos.x) || std::isinf(pos.y) ||
       std::isinf(pos.z))
        return 1;

    Position position;
    position.frequency = p[0].freq;
    position.amplitude = FFTPacket::meanAmplitude(p, numMics);
    position.pos       = pos;

    positionBuffer.add(position);

    // recluster all positions
    if(counter++ % 10 == 0) {
        positionBuffer.deleteOlderThan(
            std::chrono::duration_cast<
                std::chrono::high_resolution_clock::duration>(
                std::chrono::milliseconds((int)(keepTime * 1000))));

        positions = clusterer.cluster(positionBuffer.getPositions());
    }

    return 0;
}

std::vector<Position> & PostProcessor::getPositions() { return positions; }
