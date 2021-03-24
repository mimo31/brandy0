/**
 * video-exporter.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/21
 */
#ifndef VIDEO_EXPORTER_HPP
#define VIDEO_EXPORTER_HPP

#include <memory>
#include <string>
#include <vector>

extern "C"
{

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}

#include "graphics.hpp"
#include "listener-manager.hpp"
#include "sim-frame.hpp"
#include "str.hpp"

namespace brandy0
{

class VideoExporter
{
private:
	static constexpr uint32_t fps = 60;
	static constexpr uint64_t bitrate = 320'000;
	static constexpr uint32_t gop_size = 32;

	FrameDrawer drawer;
	str filename;
	const vec<SimFrame>& frames;
	double startTime;
	double endTime;
	double sPerFrame;
	double dtPerFrame;
	uint32_t width;
	uint32_t height;
	GraphicsManager *graphicsManager;

	double videoTimeToCompTime(double videoTime) const;
	double compTimeToVideoTime(double compTime) const;
	uint32_t videoTimeToFrame(double videoTime) const;

	void detectError(const str &message);
	void detectFatalError(const str &message);

	AVOutputFormat *format;
	AVFormatContext *formatctx;
	AVStream *stream;
	AVCodec *encoder;
	AVCodecContext *encoderctx;
	AVFrame *frame, *rgbframe;
	SwsContext *swsctx;
	AVPacket packet;
	uint32_t drawnFramei;
	uint8_t *drawndata;
	double videoTime;

	std::chrono::steady_clock::time_point lastYield;

	void doExport();
	void finishExport();
	void receiveContinueCall();

public:
	uint32_t framesToProcess, processedFrames;
	bool finishing, complete, failed, inProgress;
	str errorMessage;

	ListenerManager updateListeners;

	VideoExporter(
		const SimulationParams& params,
		uint32_t backDisplayMode,
		uint32_t frontDisplayMode,
		const str& filename,
		const vec<SimFrame>& frames,
		double startTime,
		double endTime,
		double msPerFrame,
		double dtPerFrame,
		uint32_t width,
		uint32_t height,
		GraphicsManager *graphicsManager
	);

	void exportVideo();
	void cancel();
};

}

#endif // VIDEO_EXPORTER_HPP