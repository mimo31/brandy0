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

/**
 * Class for exporting a computed simulation to a video file
 */
class VideoExporter
{
private:
	/// FPS of exported videos
	static constexpr uint32_t fps = 60;
	/// GOP size for exported vidoes (number of pictures in a group of pictures as used by the compressing algorithm)
	static constexpr uint32_t gop_size = 32;

	/// Frame drawer used to draw all the frames
	FrameDrawer drawer;
	/// Name of the exported video file
	str filename;
	/// Vector of computed frames to be sampled for video frames
	const vec<SimFrame>& frames;
	/// Video start in simulation time
	double startTime;
	/// Video end in simulation time
	double endTime;
	/// Number of seconds between two consecutive frames stored in the vector
	double sPerFrame;
	/// Simulation time between two consecutive frames stored in the vector
	double dtPerFrame;
	/// Width of the exported video (in pixels)
	uint32_t width;
	/// Height of the exported video (in pixels)
	uint32_t height;
	/// Bitrate of the exported video
	uint32_t bitrate;
	/// GraphicsManager for drawing the frames
	GraphicsManager *graphicsManager;

	/**
	 * Converts time in the video to simulation time
	 * @param videoTime time in the exported video (in seconds)
	 * @return simulation time of the same time point
	 */
	double videoTimeToCompTime(double videoTime) const;
	/**
	 * Converts simulation time to time in the video
	 * @param compTime simulation time of a certain time point
	 * @return time in the exported video (in seconds) of that time point
	 */
	double compTimeToVideoTime(double compTime) const;
	/**
	 * Computes the frame index for a specified time point in the video
	 * @param videoTime time in the exported video (in seconds)
	 * @return index of the frame in the vector of frames that should be displayed at the specified time
	 */
	uint32_t videoTimeToFrame(double videoTime) const;

	/**
	 * Handles / reports an error (not necessarily a fatal one) with a given message
	 * @param message error message
	 */
	void detectError(const str &message);
	/**
	 * Handles / reports a fatal error (leading a fail of the entire export operation) with a given message
	 * @param message error message
	 */
	void detectFatalError(const str &message);

	/// Video format structure
	AVOutputFormat *format;
	/// Video format context structure
	AVFormatContext *formatctx;
	/// Output video stream structure
	AVStream *stream;
	/// Video encoder structure
	AVCodec *encoder;
	/// Video encoder context structure
	AVCodecContext *encoderctx;
	/// Frame structure for sending frames to the encoder
	AVFrame *frame;
	/// Frame structure used for drawing the frames in RGB before they get converted to color format accepted by the encoder
	AVFrame *rgbframe;
	/// SwsContext structure for performing the conversion of frames from RGB to a format accepted by the encoder
	SwsContext *swsctx;
	/// Packet structure for getting encoded frames from the encoder to the output file
	AVPacket packet;
	/// Index of the frame in the frames vector that has been drawn in the last video frame (or -1 if no video frames have been drawn yet)
	uint32_t drawnFramei;
	/// Time in the exported video at which the next frame should be added
	double videoTime;

	/// Last point in time (approximately) when the exporting operation was not blocking the application thread
	std::chrono::steady_clock::time_point lastYield;

	/**
	 * Continues the export operation until it is finished or the time since the last yield reaches a certain limit
	 */
	void doExport();
	/**
	 * Finishes the export operation by flushing the remaining frames out of the encoder and freeing resources
	 */
	void finishExport();
	/**
	 * Handles the event when this video exporter is being called again to continue the export operation
	 */
	void receiveContinueCall();

public:
	/// Number of video frames that need to be processed during the entire export operation
	uint32_t framesToProcess;
	/// Number of video frames that were already processed during the export operation
	uint32_t processedFrames;
	/// True iff the export operation is finishing (flushed frames are being processed and resources freed)
	bool finishing;
	/// True iff the export operation has successfully finished
	bool complete;
	/// True iff the export operation has failed due to an unexpected error
	bool failed;
	/// True iff the export operation is currently running (not failed nor finished yet)
	bool inProgress;
	/// Error message that should be displayed to the user as the cause of a failed export operation
	str errorMessage;

	/// Listeners for the event of a change in the video exporter's state
	ListenerManager updateListeners;

	/**
	 * Constructs a VideoExporter object.
	 * This object can then be used to only export videos with parameters specified in this constructor.
	 * To start the actual video export operation @see exportVideo
	 * @param params simulation parameters of the simulation
	 * @param backDisplayMode background visual mode for the exported video
	 * @param frontDisplayMode foreground visual mode for the exported video
	 * @param filename name of the exported video file (might not work for filename not ending with ".mp4")
	 * @param frames vector of computed frames that will be used to sample for the video frames
	 * @param startTime simulation time of the start of the exported segment
	 * @param endTime simulation time of the end of the exported segment
	 * @param msPerFrame number of video milliseconds between two consecutive frames in the vector
	 * @param dtPerFrame simulation time between two consecutive frames in the vector
	 * @param width width of the exported video (in pixels)
	 * @param height height of the exported video (in pixels)
	 * @param bitrate bitrate of the exported video
	 * @param graphicsManager pointer to the GraphicsManager to use for the drawing of frames
	 */
	VideoExporter(
		const SimulationParams &params,
		uint32_t backDisplayMode,
		uint32_t frontDisplayMode,
		const str &filename,
		const vec<SimFrame> &frames,
		double startTime,
		double endTime,
		double msPerFrame,
		double dtPerFrame,
		uint32_t width,
		uint32_t height,
		uint32_t bitrate,
		GraphicsManager *graphicsManager
	);

	/**
	 * Start the video export operation
	 */
	void exportVideo();
	/**
	 * Cancels the video export operation and deletes the already created video file.
	 * Does nothing if a video export operation is not currently running.
	 */
	void cancel();
};

}

#endif // VIDEO_EXPORTER_HPP