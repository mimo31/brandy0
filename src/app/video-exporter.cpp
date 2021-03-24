/**
 * video-exporter.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/27
 */
#include "video-exporter.hpp"

#include <filesystem>

#include <glibmm.h>

namespace brandy0
{

double VideoExporter::videoTimeToCompTime(const double videoTime) const
{
	return startTime + videoTime / sPerFrame * dtPerFrame;
}

double VideoExporter::compTimeToVideoTime(const double compTime) const
{
	return (compTime - startTime) * sPerFrame / dtPerFrame;
}

uint32_t VideoExporter::videoTimeToFrame(const double videoTime) const
{
	const uint32_t res = round(startTime / dtPerFrame + videoTime / sPerFrame);
	return res < frames.size() ? res : frames.size() - 1;
}

void VideoExporter::detectError(const str& /*message*/)
{
	// TODO implement
}

void VideoExporter::detectFatalError(const str& message)
{
	failed = true;
	detectError(message);
}

VideoExporter::VideoExporter(
		const SimulationParams& params,
		const uint32_t backDisplayMode,
		const uint32_t frontDisplayMode,
		const str& filename,
		const vec<SimFrame>& frames,
		const double startTime,
		const double endTime,
		const double msPerFrame,
		const double dtPerFrame,
		const uint32_t width,
		const uint32_t height,
		GraphicsManager *const graphicsManager
	) :
		drawer(params),
		filename(filename),
		frames(frames),
		startTime(startTime),
		endTime(endTime),
		sPerFrame(msPerFrame / 1000),
		dtPerFrame(dtPerFrame),
		width(width),
		height(height),
		graphicsManager(graphicsManager)
{
	drawer.setFrontDisplayMode(frontDisplayMode);
	drawer.setBackDisplayMode(backDisplayMode);

	framesToProcess = 0;
	processedFrames = 0;
	finishing = complete = failed = inProgress = false;
	errorMessage = "";
}

void VideoExporter::exportVideo()
{
	lastYield = std::chrono::steady_clock::now();

	processedFrames = 0;
	finishing = complete = failed = false;
	inProgress = true;
	errorMessage = "";

	const double vidTimeBound = compTimeToVideoTime(endTime);
	framesToProcess = uint32_t(vidTimeBound * fps);

	av_log_set_level(AV_LOG_FATAL);

	format = av_guess_format(NULL, filename.c_str(), NULL);
	if (!format)
	{
		detectFatalError("error in av_guess_format");
		return;
	}
	
	if (avformat_alloc_output_context2(&formatctx, NULL, NULL, filename.c_str()) < 0)
	{
		detectFatalError("error in avformat_alloc_output_context2");
		return;
	}

	stream = avformat_new_stream(formatctx, 0);
	if (!stream)
	{
		detectFatalError("error in avformat_new_stream");
		return;
	}

	stream->codecpar->codec_id = format->video_codec;
	stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	stream->codecpar->width = width;
	stream->codecpar->height = height;
	stream->time_base.num = 1;
	stream->time_base.den = fps;

	encoder = avcodec_find_encoder(stream->codecpar->codec_id);
	if (!encoder)
	{
		detectFatalError("error in avcodec_find_encoder");
		return;
	}

	encoderctx = avcodec_alloc_context3(encoder);
	if (!encoderctx)
	{
		detectFatalError("error in avcodec_alloc_context3");
		return;
	}

	if (avcodec_parameters_to_context(encoderctx, stream->codecpar) < 0)
		detectError("error in avcodec_parameters_to_context");

	encoderctx->bit_rate = bitrate;
	encoderctx->width = width;
	encoderctx->height = height;
	encoderctx->time_base.num = 1;
	encoderctx->time_base.den = fps;
	encoderctx->gop_size = gop_size;
	encoderctx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (stream->codecpar->codec_id == AV_CODEC_ID_H264)
	{
		if (av_opt_set(encoderctx->priv_data, "preset", "ultrafast", 0))
			detectError("error in av_opt_set");
	}

	if (avcodec_parameters_from_context(stream->codecpar, encoderctx) < 0)
		detectError("error in avcodec_parameters_from_context");

	if (avcodec_open2(encoderctx, encoder, NULL) < 0)
		detectError("error in avcodec_open2");
	
	if (!(format->flags & AVFMT_NOFILE))
		if (avio_open(&formatctx->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0)
			detectError("error in avio_open");
	
	if (avformat_write_header(formatctx, NULL) < 0)
		detectError("error in avformat_write_header");

	rgbframe = av_frame_alloc();
	if (!rgbframe)
	{
		detectFatalError("error in av_frame_alloc");
		return;
	}
	rgbframe->format = AV_PIX_FMT_RGB24;
	rgbframe->width = width;
	rgbframe->height = height;
	if (av_image_alloc(rgbframe->data, rgbframe->linesize, width, height, AV_PIX_FMT_RGB24, 32) < 0)
	{
		detectFatalError("error in av_image_alloc");
		return;
	}

	swsctx = sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	frame = av_frame_alloc();
	if (!frame)
	{
		detectFatalError("error in av_frame_alloc");
		return;
	}
	frame->format = encoderctx->pix_fmt;
	frame->width = encoderctx->width;
	frame->height = encoderctx->height;
	if (av_image_alloc(frame->data, frame->linesize, encoderctx->width, encoderctx->height, encoderctx->pix_fmt, 32) < 0)
	{
		detectFatalError("error in av_image_alloc");
		return;
	}

	drawnFramei = -1;
	drawndata = new uint8_t[width * height * 3];

	Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &VideoExporter::receiveContinueCall), 1);
	updateListeners.invoke();
}

void VideoExporter::doExport()
{
	if (!inProgress)
		return;
	
	constexpr uint32_t yieldIntervalMs = 40;

	for (; videoTimeToCompTime(videoTime) < endTime; videoTime += 1 / double(fps), processedFrames++)
	{
		const auto ctime = std::chrono::steady_clock::now();
		const uint32_t mselapsed = std::chrono::duration_cast<std::chrono::milliseconds>(ctime - lastYield).count();
		if (mselapsed >= yieldIntervalMs)
		{
			Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &VideoExporter::receiveContinueCall), 1);
			updateListeners.invoke();
			return;
		}
		const double pts = av_rescale_q(processedFrames, encoderctx->time_base, stream->time_base);
		const uint32_t framei = videoTimeToFrame(videoTime);
		if (framei != drawnFramei)
		{
			drawer.drawFrame(frames[framei], width, height, rgbframe->data[0], rgbframe->linesize[0], graphicsManager);
			sws_scale(swsctx, rgbframe->data, rgbframe->linesize, 0, height, frame->data, frame->linesize);
			drawnFramei = framei;
		}
		av_init_packet(&packet);
		packet.flags |= AV_PKT_FLAG_KEY;
		packet.pts = frame->pts = pts;
		packet.data = NULL;
		packet.size = 0;
		packet.stream_index = stream->index;

		if (avcodec_send_frame(encoderctx, frame) < 0)
			detectError("error in avcodec_send_frame");

		if (avcodec_receive_packet(encoderctx, &packet) == 0)
		{
			if (av_interleaved_write_frame(formatctx, &packet))
				detectError("error in av_interleaved_write_frame");
			av_packet_unref(&packet);
		}
	}
	finishExport();
	updateListeners.invoke();
}

void VideoExporter::finishExport()
{
	finishing = true;
	delete [] drawndata;
	av_frame_free(&frame);
	av_frame_free(&rgbframe);
	sws_freeContext(swsctx);

	while (true)
	{
		if (avcodec_send_frame(encoderctx, NULL) < 0)
			detectError("error in avcodec_send_frame");
		
		if (avcodec_receive_packet(encoderctx, &packet) == 0)
		{
			if (av_interleaved_write_frame(formatctx, &packet))
				detectError("error in av_interleaved_write_frame");
			av_packet_unref(&packet);
		}
		else
			break;
	}

	if (av_write_trailer(formatctx))
		detectError("error in av_write_trailer");
	if (!(format->flags & AVFMT_NOFILE))
		if (avio_close(formatctx->pb) < 0)
			detectError("error in avio_close");
	
	avcodec_free_context(&encoderctx);
	avformat_free_context(formatctx);
	finishing = false;
	complete = true;
	inProgress = false;
}

void VideoExporter::receiveContinueCall()
{
	lastYield = std::chrono::steady_clock::now();
	doExport();
}

void VideoExporter::cancel()
{
	if (inProgress)
	{
		delete[] drawndata;
		av_frame_free(&frame);
		av_frame_free(&rgbframe);
		sws_freeContext(swsctx);
		avcodec_free_context(&encoderctx);
		avformat_free_context(formatctx);
		std::filesystem::remove(filename);
		inProgress = false;
	}
}

}