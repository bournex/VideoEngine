// fvs.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pvsdef.h"
#include <stdio.h>
#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib") /*需要加上此行才可以正确link，VC6.0*/
#include <string>
using namespace std;


#ifdef _LINUX
#ifdef FVS_EXPORTS
#	define FVS_API __attribute__((visibility("default")))
#else
#	define FVS_API
#endif
#elif defined _WINDOWS
#ifdef FVS_EXPORTS
#	define FVS_API __declspec(dllexport)
#else
#	define FVS_API __declspec(dllimport)
#endif
#endif


#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
};
#endif
#endif

typedef void * HFVSVID;
PVideoFrameCallback m_vfcb = NULL;



class FilePlayer
{
public:
	FilePlayer():bplaying(false), hworker(NULL){}
	~FilePlayer()
	{
		bplaying = false;
		WaitForSingleObject(hworker, 3000);
		CloseHandle(hworker);
		hworker = NULL;
	}

	/*
	 *	Description : play video ,success return 0, failed return error code
	 */
	int Play(const char *pfilename, void * usr)
	{
		fn = pfilename;
		hworker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FilePlayer::FileDecodeProc, this, 0, NULL);
		return 0;
	}

	void Stop()
	{
		bplaying = false;
	}

	static DWORD FileDecodeProc(LPVOID p){return ((FilePlayer*)p)->FileDecode();}
	DWORD FileDecode()
	{
		pFormatCtx = avformat_alloc_context();

		if(avformat_open_input(&pFormatCtx,fn.c_str(),NULL,NULL)!=0){
			printf("Couldn't open input stream.\n");
			return -1;
		}
		if(avformat_find_stream_info(pFormatCtx,NULL)<0){
			printf("Couldn't find stream information.\n");
			return -1;
		}
		videoindex=-1;
		for(i=0; i<pFormatCtx->nb_streams; i++) 
			if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
				videoindex=i;
				break;
			}

			if(videoindex==-1){
				printf("Didn't find a video stream.\n");
				return -1;
			}

			pCodecCtx = avcodec_alloc_context3(NULL);  
			if (pCodecCtx == NULL)  
			{  
				printf("Could not allocate AVCodecContext\n");  
				return -1;  
			}  
			avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar); 

			pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
			if(pCodec==NULL){
				printf("Codec not found.\n");
				return -1;
			}
			if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
				printf("Could not open codec.\n");
				return -1;
			}

			pFrame=av_frame_alloc();
			pFrameYUV=av_frame_alloc();
			out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
			av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
				AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

			packet=(AVPacket *)av_malloc(sizeof(AVPacket));

			img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
				pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

			while(av_read_frame(pFormatCtx, packet)>=0 && bplaying/* exit flag */){
				if(packet->stream_index==videoindex){
					ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if(ret < 0){
						printf("Decode Error.\n");
						return -1;
					}
					if(got_picture){
						sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
							pFrameYUV->data, pFrameYUV->linesize);

						y_size=pCodecCtx->width*pCodecCtx->height;  
						//fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
						//fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
						//fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
						//printf("Succeed to decode 1 frame!\n");
						if (m_vfcb)
						{
							VideoFrame vf;
							vf.width = pCodecCtx->width;
							vf.height = pCodecCtx->height;
							
							m_vfcb(vf, this, usr, NULL);
						}

					}
				}
				av_free_packet(packet);
			}
			//flush decoder
			//FIX: Flush Frames remained in Codec
			while (1 && bplaying) {
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
				if (ret < 0)
					break;
				if (!got_picture)
					break;
				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);

				int y_size=pCodecCtx->width*pCodecCtx->height;

				printf("Flush Decoder: Succeed to decode 1 frame!\n");
			}

			sws_freeContext(img_convert_ctx);

			av_frame_free(&pFrameYUV);
			av_frame_free(&pFrame);
			avcodec_close(pCodecCtx);
			avformat_close_input(&pFormatCtx);
			return 0;
	}

private:
	string fn;
	HANDLE hworker;
	BOOL bplaying;
	void * usr;

	/*
	 *	Description : ffmpeg parameters
	 */
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodecParameters	*pCodecPara;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
};

FVS_API
int FVS_Init(PVideoFrameCallback vfcb)
{
	
	m_vfcb = vfcb;

	av_register_all();
	avformat_network_init();

	return 0;
}

FVS_API
int FVS_Uninit()
{
	return 0;
}
FVS_API
HFVSVID FVS_PlayVideo(const char *pVideoFile, void *usr)
{
	if (!PathFileExists(pVideoFile))
	{
		return NULL;
	}

	FilePlayer *p = new FilePlayer();
	if (p->Play(pVideoFile, usr) == 0)
		return p;
	return NULL;
}
FVS_API
int FVS_StopVideo(HFVSVID hfvs)
{
	return 0;
}
/*
int main(int argc, char* argv[])
{
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodecParameters	*pCodecPara;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;

	char filepath[]="Titanic.mkv";

	FILE *fp_yuv=fopen("output.yuv","wb+");  

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	
	if(videoindex==-1){
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = avcodec_alloc_context3(NULL);  
    if (pCodecCtx == NULL)  
    {  
        printf("Could not allocate AVCodecContext\n");  
        return -1;  
    }  
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar); 

	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		printf("Codec not found.\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		printf("Could not open codec.\n");
		return -1;
	}
	
	pFrame=av_frame_alloc();
	pFrameYUV=av_frame_alloc();
	out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
		AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);
	
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

	while(av_read_frame(pFormatCtx, packet)>=0){
		if(packet->stream_index==videoindex){
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if(ret < 0){
				printf("Decode Error.\n");
				return -1;
			}
			if(got_picture){
				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);

				y_size=pCodecCtx->width*pCodecCtx->height;  
				fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
				fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
				fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
				printf("Succeed to decode 1 frame!\n");

			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
			pFrameYUV->data, pFrameYUV->linesize);

		int y_size=pCodecCtx->width*pCodecCtx->height;  
		fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y 
		fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
		fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V

		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}


*/