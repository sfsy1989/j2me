/*
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */ 


#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <string.h>
#include "javacall_multimedia.h" 
#include "javacall_file.h"
#include "n_lib.h"
#include <sys/resource.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>



#ifdef ANDROID
#include <SLES/OpenSLES.h>
#include <android/log.h>
#define  LOG_TAG    "midp"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGI(...)
#endif



#define fdebug 0
#define TMP_MUSIC_FILENAME_LEN_MAX 64

typedef struct {
	int needBuffer;
	int contentLength;
	void* music;
	int occupied;
	char filename[256];
	javacall_utf16 tmpfilename[64];
	int tmpfilenamelen;
	javacall_handle fp;
	long duration;
	javacall_int64 playerId;
} mmplayer_handle;

struct timeval startPlay;

extern char* javacall_UNICODEsToUtf8(const javacall_utf16* str, int strlen);
#if 0
#define MAX_FILE_NAME_LEN 256

char* javacall_UNICODEsToUtf8(const javacall_utf16* fileName, int fileNameLen) {
    static char result[MAX_FILE_NAME_LEN+1];
    if (fileNameLen >= MAX_FILE_NAME_LEN) {
        return NULL;	
    }

    result[fileNameLen] = 0;
    while(fileNameLen-- > 0) {
        result[fileNameLen] = fileName[fileNameLen];
    }
    return &result[0];
}
#endif

static int music_handle_occupied = 0;
static char music_type_string[32];


void printf_timestamp(void)
{
	struct tm *ltime;
	struct timespec tp;
	int rs;

	static char log_time[] = "YYYYMMDD:HHMMSS.XXX";

	rs = clock_gettime(CLOCK_REALTIME, &tp);

	if (0 != rs) {
		printf("time error\n");
		return;
	}

	ltime = gmtime(&(tp.tv_sec));
	ltime->tm_mon += 1;

	snprintf(log_time, sizeof(log_time) + 1,
		"%4d%02d%02d:%02d%02d%02d.%03d",
		ltime->tm_year + 1900, ltime->tm_mon, ltime->tm_mday,
		ltime->tm_hour, ltime->tm_min, ltime->tm_sec,
		(int)tp.tv_nsec / 1000000);

	if(fdebug) printf("%s\n",log_time);

	return;
	
}

static inline unsigned int 
get_le_32(unsigned char *buf)
{
	return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static int
try_get_wav_duration(FILE *fp)
{
	unsigned char buf[512];
	unsigned int size;
	unsigned int byte_ps;
	//float f;

	if (fread(buf, 1, 12, fp) != 12) {
		return -1;
	}
	/**simple check*/
	if (buf[0] != 'R' || buf[1] != 'I' ||
	    buf[2] != 'F' || buf[3] != 'F') {
		return -1;
	}

	size = get_le_32(buf + 4);

	if (buf[8] != 'W'  || buf[9] != 'A' || 
	    buf[10] != 'V' || buf[11] != 'E') {
		return -1;
	}
	
	for (;;) {
		unsigned int pos;

		if (fread(buf, 1, 4, fp) != 4) {
			return -1;
		}
		if (buf[0] == 'f' && buf[1] == 'm' &&
                    buf[2] == 't' && buf[3] == ' ') {
			/**ok*/
			break;
		} 
		if (fread(buf, 1, 4, fp) != 4) {
			return -1;
		}
		pos = get_le_32(buf);
		fseek(fp, pos, SEEK_CUR);
	}
	/**find the fmt tag*/
	if (fread(buf, 1, 16, fp) != 16) {
		return -1;
	}

	/**get AvgBytesPerSec*/
	byte_ps = get_le_32(buf + 12);
	/**get file size*/
#if 0
	fseeko(fp, 0, SEEK_END);
	size = ftello(fp);
	fseeko(fp, 0, SEEK_SET);
#endif
	return (size + byte_ps - 1) / byte_ps;
}

/**return seconds*/
int
get_wav_duration(char *filename)
{
	FILE *fp;
	int sec;

	fp = fopen(filename, "rb");
	if (!fp) {
		printf("fopen wav file failed %s\n", strerror(errno));
		return -1;
	}

	sec = try_get_wav_duration(fp);

	fclose(fp);
	return sec;
}



/**
 * Call this function when VM starts
 * Perform global initialization operation
 * 
 * @retval JOK      success
 * @retval JFAIL    fail
 */
javacall_result javacall_media_initialize(void) {
    //printf("javacall_media_initialize\n");
    return JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Call this function when VM ends 
 * Perfrom global free operaiton
 * 
 * @retval JOK      success
 * @retval JFAIL    fail 
 */
javacall_result javacall_media_finalize(void) {
    //printf("javacall_media_finalize\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Java MMAPI call this function to create native media handler.
 * This function is called at the first time to initialize native library.
 * You can do your own initializatio job from this function.
 * 
 * @param playerId      Unique player object ID for this playing
 *                      This unique ID is generated by Java MMAPI library.
 * @param mime          Mime unicode string
 * @param mimeLength    String length of mimeType
 * @param uri           URI unicode string to media data
 * @param uriLength     String length of URI
 * @param contentLength Content length in bytes
 *                      If Java MMAPI couldn't determine contenet length, this value should be -1
 * 
 * @return              Handle of native library. if fail return NULL.
 */
javacall_handle javacall_media_create(javacall_int64 playerId, 
                                      const javacall_utf16* mime, 
                                      long mimeLength,
                                      const javacall_utf16* uri, 
                                      long uriLength,
                                      long contentLength)
{
    //printf("javacall_media_create:\n");
    //printf("                      playerId=%ld\n",(long int)playerId);
    //printf("                      mime=%d\n",(int)mime);
    //printf("                      mimeLength=%ld\n",mimeLength);
    //printf("                      uri=%d\n",(int)uri);
    //printf("                      uriLength=%ld\n",uriLength);
    //printf("                      contentLength=%ld\n",contentLength);

    char *pszMime = NULL, *pszUri = NULL;
    int needBuffer;
    mmplayer_handle* handle;

    //TODO:init maplyer

    pszMime = javacall_UNICODEsToUtf8(mime, mimeLength);

    strcpy(music_type_string,pszMime);
    //printf("music_type_string is %s \n",music_type_string);

    //TODO:midi just for test
    if (strcmp(pszMime, JAVACALL_AUDIO_MIDI_MIME) &&
    	 strcmp(pszMime, JAVACALL_AUDIO_MIDI_MIME_2) &&
    	 strcmp(pszMime, JAVACALL_AUDIO_MP3_MIME) &&
    	 strcmp(pszMime, JAVACALL_AUDIO_MP3_MIME_2) &&
    	 strcmp(pszMime, JAVACALL_AUDIO_WAV_MIME)) {
    	 //suppose only support mp3...
    	 return 0;
    }

    if (uri == NULL || uriLength <= 0) {
    	//Read from stream, need buffer,default
    	needBuffer = 1;
    } else {
       //Read from URI,not support
       needBuffer = 0;
    }

    handle = malloc(sizeof(mmplayer_handle));

    handle->needBuffer = needBuffer;
    handle->contentLength = contentLength;
    handle->music = NULL;
    if (pszUri == NULL) {
        handle->filename[0] = '\0';
    } else {
        strcpy(handle->filename, pszUri);
    }
    handle->tmpfilename[0] = (javacall_utf16)0;
    handle->tmpfilenamelen = 0;
    handle->occupied = 1;
    handle->fp = NULL;
	handle->duration = -1;
	handle->playerId = playerId;
    
    return handle;
}

/**
 * Java MMAPI call this function to create native media handler.
 * This function is called at the first time to initialize native library.
 * You can do your own initializatio job from this function.
 * 
 * @param playerId      Unique player object ID for this playing
 * @param mediaType     Media content type
 * @param fileName      File name to play (device's native path format)
 * @return              Handle of native library. if fail return NULL.
 */
javacall_handle javacall_media_create2(int playerId, javacall_media_type mediaType, const char* fileName) {
	//printf("javacall_media_create2\n");
	return 0;
}

/**
 * Close native media player that created by creat or creat2 API call
 * After this call, you can't use any other function in this library
 * 
 * @param handle  Handle to the library.
 * @retval JAVACALL_OK      success
 * @retval JAVACALL_FAIL    fail
 */
javacall_result javacall_media_close(javacall_handle handle) {
    //printf("javacall_media_close\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * finally destroy native media player previously closed by
 * javacall_media_close. intended to be used by finalizer
 * 
 * @param handle  Handle to the library.
 * 
 * @retval JAVACALL_OK      Java VM will proceed as if there is no problem
 * @retval JAVACALL_FAIL    Java VM will raise the media exception
 */
javacall_result javacall_media_destroy(javacall_handle handle){
    (void)handle;
	//printf("javacall_media_destroy\n");
	return JAVACALL_NOT_IMPLEMENTED;
}


/**
 * Request to acquire device resources used to play media data.
 * You could implement this function to control device resource usage.
 * If there is no valid device resource to play media data, return JAVACALL_FAIL.
 * 
 * @param handle  Handle to the library.
 * @retval JAVACALL_OK      Can acquire device
 * @retval JAVACALL_FAIL    There is no valid device resource
 */
javacall_result javacall_media_acquire_device(javacall_handle handle) {
    //printf("javacall_media_acquire_device\n");
#if 0
    if (music_handle_occupied) {
    	//printf("javacall_media_acquire_device: can't create the 2nd music player.\n");
    	return JAVACALL_FAIL;
    } else {
        music_handle_occupied = 1;
        return JAVACALL_OK;
    }
#else
	return JAVACALL_OK;
#endif

    //return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Release device resource. 
 * Java MMAPI call this function to release limited device resources.
 * 
 * @param handle  Handle to the library.
 * @retval JAVACALL_OK      Can release device
 * @retval JAVACALL_FAIL    Can't release device
 */
javacall_result javacall_media_release_device(javacall_handle handle) {
    //printf("javacall_media_release_device\n");
    music_handle_occupied = 0;
    return JAVACALL_OK;
    //return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Ask to the native layer.
 * Is this protocol handled by native layer or Java layer?
 * 
 * @retval JAVACALL_OK      Yes, this protocol handled by device.
 * @retval JAVACALL_FAIL    No, please handle this protocol from Java.
 */
javacall_result javacall_media_protocol_handled_by_device(javacall_handle handle) {
	//printf("javacall_media_protocol_handled_by_device\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Java MMAPI call this function to send media data to this library
 * This function can be called multiple time to send large media data
 * Native library could implement buffering by using any method (for example: file, heap and etc...)
 * And, buffering occured in sequentially. not randomly.
 * 
 * @param handle    Handle to the library
 * @param buffer    Media data buffer pointer
 * @param length    Length of media data
 * @param offset    Offset. If offset value is 0, it means start of buffering
 *                  It'll be incremented as buffering progress
 *                  You can determine your internal buffer's writting position by using this value
 * @return          If success return 'length of buffered data' else return -1
 */
long javacall_media_do_buffering(javacall_handle handle, const void* buffer, long length, long offset) {
    javacall_handle hFile;
    int ret;
    mmplayer_handle* mp = handle;
    static char tmpfilename[64];
	//printf("javacall_media_do_buffering\n");
    if (buffer == NULL || length <= 0) {
    	//end of buffering
    	//printf("endof buffering\n");
    	if (mp->fp) {
    		//printf("close tmp buffer file\n");
    		javacall_file_close(mp->fp);
    		mp->fp = NULL;
        mp->duration=get_wav_duration(javacall_UNICODEsToUtf8(mp->tmpfilename, mp->tmpfilenamelen));
    	}
    	return 0;
    }
    
    if (mp->fp) {
    	 //printf("Continue buffering to tmp file\n");
    	 //ret = javacall_file_write(mp->fp, buffer, length + offset);
    	 ret = javacall_file_write(mp->fp, buffer, length);
    } else {
        int i;
		static char filePath[32] = {0};
		if(!access("/data/jvm",F_OK)){
			sprintf(filePath,"%s","/data/jvm");
		}else{
			sprintf(filePath,"%s","/mnt");	
		}
        if(!strcmp(music_type_string, JAVACALL_AUDIO_WAV_MIME)){
                sprintf(tmpfilename, "%s/appdb/%08x_tmp_mus.wav",filePath,(int)mp); 
        }else if(!strcmp(music_type_string, JAVACALL_AUDIO_MP3_MIME_2)){
                sprintf(tmpfilename, "%s/appdb/%08x_tmp_mus.mp3",filePath,(int)mp); 
        }else{
                sprintf(tmpfilename, "%s/appdb/%08x_tmp_mus.mid",filePath,(int)mp); 
        }
        //s//printf(tmpfilename, "/var/%08x_tmp_mus.mp3", (int)mp);        
        for (i = 0; i < TMP_MUSIC_FILENAME_LEN_MAX; i++) {
        	mp->tmpfilename[i] = (javacall_utf16)tmpfilename[i];
        	if (!tmpfilename[i]) {
        		mp->tmpfilenamelen = i;
        		break;
        	}
        }
        
        if (JAVACALL_OK != javacall_file_open(mp->tmpfilename, mp->tmpfilenamelen, JAVACALL_FILE_O_WRONLY | JAVACALL_FILE_O_CREAT | JAVACALL_FILE_O_TRUNC, &hFile)) {
            //printf("Create file temp music file failed\n");
            return -1;
        } else {
            //printf("Buffering to tmp file\n");
            mp->fp = hFile;
            //ret = javacall_file_write(mp->fp, buffer, length + offset);
            ret = javacall_file_write(mp->fp, buffer, length);
        }
    }

    //printf("%d bytes buffered\n", ret);
	//mp->duration=get_wav_duration(javacall_UNICODEsToUtf8(mp->tmpfilename, mp->tmpfilenamelen))+1;
    return ret;
    //return -1;
}

/**
 * MMAPI call this function to clear(delete) buffered media data
 * You have to clear any resources created from previous buffering
 * 
 * @param handle    Handle to the library
 * @retval JAVACALL_OK      Can clear buffer
 * @retval JAVACALL_FAIL    Can't clear buffer
 */
javacall_result javacall_media_clear_buffer(javacall_handle handle) {
	//printf("javacall_media_clear_buffer\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

#ifdef ANDROID
#define AUDIO_PLAYER_OBJ_NUM 3
static SLObjectItf engineObject = NULL;

static SLEngineItf engineEngine;
/* output mix interfaces */
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
/* aux effect on the output mix */
static const SLEnvironmentalReverbSettings reverbSettings =
  SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
/* uri player interfaces */
static SLObjectItf uriPlayerObject[AUDIO_PLAYER_OBJ_NUM] = {NULL};
static SLPlayItf uriPlayerPlay[AUDIO_PLAYER_OBJ_NUM];
int current_play_obj = 0;
//static SLSeekItf uriPlayerSeek;
enum _bool {
  false = 0,
  true
};
typedef enum _bool bool;


static void createEngine()
{
  SLresult result;
  // create engine
  result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
  // realize the engine
  result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
  // get the engine interface
  result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
  // create output mix
  const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
  const SLboolean req[1] = {SL_BOOLEAN_FALSE};
  result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
  // realize the output mix
  result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
}

static bool find_release_AudioPlayer()
{
        SLuint32 state;
	int i;
	current_play_obj++;
  	if (current_play_obj >= AUDIO_PLAYER_OBJ_NUM){
		current_play_obj = 0;
	}
	for(i = 0; i < AUDIO_PLAYER_OBJ_NUM; i++){
		if(uriPlayerObject[current_play_obj] == NULL)
			break;
		(*uriPlayerPlay[current_play_obj])->GetPlayState(uriPlayerPlay[current_play_obj],&state);
		if(state != SL_PLAYSTATE_PLAYING){
			(*uriPlayerObject[current_play_obj])->Destroy(uriPlayerObject[current_play_obj]); 
			uriPlayerObject[current_play_obj] = NULL;
			break;
		}
  		if (current_play_obj >= AUDIO_PLAYER_OBJ_NUM-1){
			current_play_obj = 0;
		}else{
			current_play_obj++;
  		}
	}
	if(i == AUDIO_PLAYER_OBJ_NUM)
		return false;
	else
		return true;
}

static bool createUriAudioPlayer(char* uri)
{
	  SLresult result;
		
	  // config audio source
	  SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
	  SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
	  SLDataSource audioSrc = {&loc_uri, &format_mime};
	  // config audio sink
	  SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	  SLDataSink audioSnk = {&loc_outmix, NULL};
	  // create audio player
	  if(find_release_AudioPlayer() == true){
	  	const SLInterfaceID ids[1] = {SL_IID_SEEK};
	  	const SLboolean req[1] = {SL_BOOLEAN_TRUE};
	  	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &uriPlayerObject[current_play_obj], &audioSrc, &audioSnk, 1, ids, req);
	  	// realize the player
	  	result = (*uriPlayerObject[current_play_obj])->Realize(uriPlayerObject[current_play_obj], SL_BOOLEAN_FALSE);
	  	if (SL_RESULT_SUCCESS != result) {
			(*uriPlayerObject[current_play_obj])->Destroy(uriPlayerObject[current_play_obj]);
			uriPlayerObject[current_play_obj] = NULL;
			return false;
	  	}
	  	// get the play interface
	  	result = (*uriPlayerObject[current_play_obj])->GetInterface(uriPlayerObject[current_play_obj], SL_IID_PLAY, &uriPlayerPlay[current_play_obj]);
	  	return true;
	}
	return false;
}

static void setPlayingUriAudioPlayer(bool played)
{
  SLresult result;
  if (uriPlayerPlay[current_play_obj] != NULL) {
    result = (*uriPlayerPlay[current_play_obj])->SetPlayState(uriPlayerPlay[current_play_obj], played ?
                        SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
  }
}
#endif
/**
 * Start media playing
 * 
 * @param handle    Handle to the library
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_start(javacall_handle handle) {
	static int control_count = 0;
	if(fdebug) printf("javacall_media_start\n");
    mmplayer_handle* mp = handle;
    char media_url[256];

    sprintf(media_url,"file://%s",javacall_UNICODEsToUtf8(mp->tmpfilename, mp->tmpfilenamelen));
	if(fdebug) printf("f_mplayer_play media_url is %s\n",media_url);
#ifdef ANDROID
	if (control_count == 0){
		control_count++;
		createEngine();
	}
	if(createUriAudioPlayer(media_url)==true){
		setPlayingUriAudioPlayer(true);
		gettimeofday(&startPlay, NULL);
	}

#else
	n_mplayer_play(1,
		   media_url,
		   0,//int iMediaType,
		   0,//int iAudioType,
		   0,//int iVideoType,
		   0,//int iStreamType,
		   0,//int iDrmType,
		   0,//int iFingerPrint,
		   0,//int iCopyProtection,
		   0,//int iAllowTrickmode,
		   "",//char *cMediaCode,
		   "",//char *cEntryID,
		   "",//char *cStartTime, 
		   "",//char *cEndTime, 
		   ""//char *ctm);
		   );
#endif
#if defined FV_USE_QT2 || defined FV_USE_FB
    {
        JavacallMidpEvent mevt;
        mevt.type      = 45;
        mevt.intParam1 = (int)mp->playerId;
        mevt.intParam2 = (int)mp->duration;
        mevt.intParam3 = 0;
        mevt.intParam4 = (int)JAVACALL_EVENT_MEDIA_END_OF_MEDIA;
        mevt.intParam5 = 99;
        javacallMidpStoreEventAndSignalForeground(mevt);
    }
#endif
    return JAVACALL_OK;
    //return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Stop media playing
 * 
 * @param handle      Handle to the library
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_stop(javacall_handle handle) {
    if(fdebug) printf("javacall_media_stop\n");
    //mmplayer_handle* mp = handle;
    if(uriPlayerObject[current_play_obj] != NULL){
    	(*uriPlayerPlay[current_play_obj])->SetPlayState(uriPlayerPlay[current_play_obj],SL_PLAYSTATE_PAUSED);
		(*uriPlayerObject[current_play_obj])->Destroy(uriPlayerObject[current_play_obj]); 
		uriPlayerObject[current_play_obj] = NULL;
    }
    //javacall_file_delete(mp->tmpfilename, mp->tmpfilenamelen);
    return JAVACALL_OK;
    //return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Pause media playing
 * 
 * @param handle      Handle to the library
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_pause(javacall_handle handle) {
	//printf("javacall_media_pause\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Resume media playing
 * 
 * @param handle      Handle to the library
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_resume(javacall_handle handle) {
	//printf("javacall_media_resume\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

static f_mp_rapid_cmd *g_rapid_cmd = 0;

static int rapid_get_playtime(int id)
{
#ifndef ANDROID
        int i;

        if (!g_rapid_cmd) {
                g_rapid_cmd = f_get_dump_addr(DP_MPLAYER_RAPID_CMD,
                                              sizeof(f_mp_rapid_cmd));
                if (!g_rapid_cmd) {
                        return 0;
                }
        }

        for (i = 0; i < sizeof(g_rapid_cmd->ins)/sizeof(g_rapid_cmd->ins[0]); 
	     i++) {
                if (id == g_rapid_cmd->ins[i].id) {
			return g_rapid_cmd->ins[i].playtime; 
                }
        }

        return 0;
#else
	struct timeval end;
	gettimeofday(&end, NULL);
	return (end.tv_sec - startPlay.tv_sec)*1000 + (end.tv_usec - startPlay.tv_usec)/1000;
#endif
}

/**
 * Get current playing media time
 * 
 * @param handle      Handle to the library
 * @return          If success return time in ms else return -1
 */
long javacall_media_get_time(javacall_handle handle) {
	//printf("javacall_media_get_time\n");
	//char p_time[32];
	//n_mplayer_get_current_play_time(1, p_time, sizeof(p_time));
	//printf("javacall_media_get_time time is %s\n",p_time);
	//if(atoi(p_time)==0){
	//	return -1;
	//}
	//return atoi(p_time)*1000;
	return rapid_get_playtime(1);
}

/**
 * Seek to specified time
 * 
 * @param handle      Handle to the library
 * @param ms        Seek position as ms time
 * @return          If success return time in ms else return -1
 */
long javacall_media_set_time(javacall_handle handle, long ms) {
	//printf("javacall_media_set_time\n");
	return -1;
}
 
/**
 * Get whole media time in ms
 * 
 * @param handle      Handle to the library
 * @return          If success return time in ms else return -1
 */
long javacall_media_get_duration(javacall_handle handle) {
 	//printf("javacall_media_get_duration\n");
    mmplayer_handle* mp = handle;
    char filename[256];
	int duration_time;
	//n_mplayer_media_duration(1, &duration_time);
    sprintf(filename,"%s",javacall_UNICODEsToUtf8(mp->tmpfilename, mp->tmpfilenamelen));
    if(fdebug) printf("javacall_media_get_duration filename is %s\n",filename);
	duration_time=get_wav_duration(filename);
	//printf("javacall_media_get_duration time is %d\n",duration_time);
	if(duration_time<0){
		return -1;
	}
	return duration_time*1000;
}

/**
 * Get current audio volume
 * Audio volume range have to be in 0 to 100 inclusive
 * 
 * @param mediaType Media type. 
 *                  It could be used to determine H/W audio path for specific audio data type.
 * @return          Volume value
 */
long javacall_media_get_volume(javacall_handle handle) {
	//printf("javacall_media_get_volume\n");
	return 100;
}

/**
 * Set audio volume
 * Audio volume range have to be in 0 to 100 inclusive
 * 
 * @param mediaType Media type. 
 *                  It could be used to determine H/W audio path for specific data type.
 * @param level     Volume value
 * @return          if success return volume level else return -1
 */
long javacall_media_set_volume(javacall_handle handle, long level) {
	//printf("javacall_media_set_volume\n");
	return -1;
}

/**
 * Is audio muted now?
 * 
 * @param mediaType Media type. 
 *                  It could be used to determine H/W audio path for specific data type.
 * @retval JAVACALL_TRUE    Now in mute state
 * @retval JAVACALL_FALSE   Now in un-mute state
 */
javacall_bool javacall_media_is_mute(javacall_handle handle) {
	//printf("javacall_media_is_mute\n");
	return JAVACALL_FALSE;
}

/**
 * Mute, Unmute audio
 * 
 * @param mediaType Media type. 
 *                  It could be used to determine H/W audio path for specific data type.
 * @param mute      JAVACALL_TRUE to mute, JAVACALL_FALSE to unmute
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */ 
javacall_result javacall_media_set_mute(javacall_handle handle, javacall_bool mute) {
	//printf("javacall_media_set_mute\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Turn on or off video rendering alpha channel.
 * If this is on OEM native layer video renderer SHOULD use this mask color
 * and draw on only the region that is filled with this color value.
 * 
 * @param on    Alpha channel is on?
 * @param color Color of alpha channel
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_video_alpha(javacall_bool on, javacall_pixel color) {
	//printf("javacall_media_set_video_alpha\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get original video width
 * 
 * @param handle      Handle to the library 
 * @param width     Pointer to int variable to get width of video
 *                  Caller can assign NULL value if you don't care about width value.
 * @param height    Pointer to int variable to get height of video
 *                  Caller can assign NULL value if youe don't care about height value.
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_video_size(javacall_handle handle, 
                                              /*OUT*/ long* width, /*OUT*/ long* height) {
	//printf("javacall_media_get_video_size\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set video rendering position in physical screen
 * 
 * @param handle    Handle to the library 
 * @param x         X position of rendering in pixels
 * @param y         Y position of rendering in pixels
 * @param w         Width of rendering
 * @param h         Height of rendering
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_video_location(javacall_handle handle, long x, long y, long w, long h)
{
	//printf("javacall_media_set_video_location\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set video preview visible state to show or hide
 * 
 * @param handle    Handle to the library
 * @param visible   JAVACALL_TRUE to show or JAVACALL_FALSE to hide
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_video_visible(javacall_handle handle, javacall_bool visible)
{
	//printf("javacall_media_set_video_visible\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Start get current snapshot of video data
 * When snapshot operation doen, call callback function to provide snapshot image data to Java.
 *
 * @param handle            Handle to the library
 * @param imageType         Snapshot image type format as unicode string. For example, "jpeg", "png" or "rgb565".
 * @param length            imageType unicode string length
 * 
 * @retval JAVACALL_OK          Success.
 * @retval JAVACALL_WOULD_BLOCK This operation could takes long time. 
 *                              After this operation finished, MUST send - IMPL_NOTE: EVENT NAME - by using 
 *                              "javanotify_on_media_notification" function call
 * @retval JAVACALL_FAIL        Fail. Invalid encodingFormat or some errors.
 */
javacall_result javacall_media_start_video_snapshot(javacall_handle handle, 
                                                    const javacall_utf16* imageType, long length)
{
	//printf("javacall_media_start_video_snapshot\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get snapshot data size
 * 
 * @param handle    Handle to the library
 * @param size      Size of snapshot data
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_video_snapshot_data_size(javacall_handle handle, /*OUT*/ long* size)
{
	//printf("javacall_media_get_video_snapshot_data_size\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get snapshot data
 * 
 * @param handle    Handle to the library
 * @param buffer    Buffer will contains the snapshot data
 * @param size      Size of snapshot data
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_video_snapshot_data(javacall_handle handle, /*OUT*/ char* buffer, long size)
{
	//printf("javacall_media_get_video_snapshot_data\n");
	return JAVACALL_NOT_IMPLEMENTED;
}


/**
 * play simple tone
 *
 * @param note     the note to be played. From 0 to 127 inclusive.
 *                 The frequency of the note can be calculated from the following formula:
 *                    SEMITONE_CONST = 17.31234049066755 = 1/(ln(2^(1/12)))
 *                    note = ln(freq/8.176)*SEMITONE_CONST
 *                    The musical note A = MIDI note 69 (0x45) = 440 Hz.
 * @param duration the duration of the note in ms 
 * @param volume   volume of this play. From 0 to 100 inclusive.
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_play_tone(long note, long duration, long volume) {
	//printf("javacall_media_play_tone\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * stop simple tone
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_stop_tone(void) {
	//printf("javacall_media_stop_tone\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get supported meta data key counts for specefic media type
 * 
 * @param mediaType [in]    Media type
 * @param keyCounts [out]   Return meta data key string counts
 * 
 * @retval JAVACALL_OK              Success
 * @retval JAVACALL_FAIL            Fail
 * @retval JAVACALL_NOT_IMPLEMENTED MetaDataControl is not implemented for this media type
 */
javacall_result javacall_media_get_metadata_key_counts(javacall_handle handle, /*OUT*/ long* keyCounts) {
	//printf("javacall_media_get_metadata_key_counts\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get meta data key strings by using media type and index value
 * 
 * @param mediaType [in]    Media type
 * @param index     [in]    Meta data key string's index value. from 0 to 'key counts - 1'.
 * @param bufLength [in]    keyBuf buffer's size in bytes. 
 * @param keyBuf    [out]   Buffer that used to return key strings. 
 *                          NULL value should be appended to the end of string.
 * 
 * @retval JAVACALL_OK              Success
 * @retval JAVACALL_OUT_OF_MEMORY     keyBuf size is too small
 * @retval JAVACALL_NOT_IMPLEMENTED  MetaDataControl is not implemented for this media type
 * @retval JAVACALL_FAIL            Fail
 */
javacall_result javacall_media_get_metadata_key(javacall_handle handle, 
                                                long index, long bufLength, 
                                                /*OUT*/ char* keyBuf) {
	//printf("javacall_media_get_metadata_key\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get meta data value strings by using meta data key string
 * 
 * @param hLIB      [in]    Handle to the library 
 * @param key       [in]    Meta data key string
 * @param bufLength [in]    dataBuf buffer's size in bytes. 
 * @param dataBuf   [out]   Buffer that used to return meta data strings. 
 *                          NULL value should be appended to the end of string.
 * 
 * @retval JAVACALL_OK              Success
 * @retval JAVACALL_OUT_OF_MEMORY     dataBuf size is too small
 * @retval JAVACALL_NOT_IMPLEMENTED  MetaDataControl is not implemented for this media type
 * @retval JAVACALL_FAIL            Fail
 */
javacall_result javacall_media_get_metadata(javacall_handle handle, 
                                            const char* key, long bufLength, 
                                            /*OUT*/ char* dataBuf) {
	//printf("javacall_media_get_metadata\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get volume for the given channel. 
 * The return value is independent of the master volume, which is set and retrieved with VolumeControl.
 * 
 * @param handle    Handle to the library 
 * @param channel   0-15
 * @param volume    channel volume, 0-127, or -1 if not known
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_channel_volume(javacall_handle handle, 
                                                  long channel, /*OUT*/ long* volume) {
	//printf("javacall_media_get_channel_volume\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set volume for the given channel. To mute, set to 0. 
 * This sets the current volume for the channel and may be overwritten during playback by events in a MIDI sequence.
 * 
 * @param handle    Handle to the library 
 * @param channel   0-15
 * @param volume    channel volume, 0-127
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_channel_volume(javacall_handle handle, 
                                                  long channel, long volume) {
	//printf("javacall_media_set_channel_volume\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set program of a channel. 
 * This sets the current program for the channel and may be overwritten during playback by events in a MIDI sequence.
 * 
 * @param handle    Handle to the library 
 * @param channel   0-15
 * @param bank      0-16383, or -1 for default bank
 * @param program   0-127
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_program(javacall_handle handle, 
                                           long channel, long bank, long program) {
	//printf("javacall_media_set_program\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Sends a short MIDI event to the device.
 * 
 * @param handle    Handle to the library 
 * @param type      0x80..0xFF, excluding 0xF0 and 0xF7, which are reserved for system exclusive
 * @param data1     for 2 and 3-byte events: first data byte, 0..127
 * @param data2     for 3-byte events: second data byte, 0..127
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_short_midi_event(javacall_handle handle,
                                                long type, long data1, long data2) {
	//printf("javacall_media_short_midi_event\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Sends a long MIDI event to the device, typically a system exclusive message.
 * 
 * @param handle    Handle to the library 
 * @param data      array of the bytes to send. 
 *                  This memory buffer will be freed after this function returned.
 *                  So, you should copy this data to the other internal memory buffer
 *                  if this function needs data after return.
 * @param offset    start offset in data array
 * @param length    number of bytes to be sent
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_long_midi_event(javacall_handle handle,
                                               const char* data, long offset, /*INOUT*/ long* length) {
	//printf("javacall_media_long_midi_event\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get maximum rate of media type
 * 
 * @param mediaType [in]    Media type
 * @param maxRate   [out]   Maximum rate value for this media type
 * 
 * @retval JAVACALL_OK              Success
 * @retval JAVACALL_NOT_IMPLEMENTED  RateControl is not implemented for this media type
 * @retval JAVACALL_FAIL            Fail
 */
javacall_result javacall_media_get_max_rate(javacall_handle handle, /*OUT*/ long* maxRate) {
	//printf("javacall_media_get_max_rate\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get minimum rate of media type
 * 
 * @param mediaType [in]    Media type
 * @param minRate   [out]   Minimum rate value for this media type
 * 
 * @retval JAVACALL_OK              Success
 * @retval JAVACALL_FAIL            Fail
 */
javacall_result javacall_media_get_min_rate(javacall_handle handle, /*OUT*/ long* minRate) {
	//printf("javacall_media_get_min_rate\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set media's current playing rate
 * 
 * @param hLIB [in] Handle to the library 
 * @param rate [in] Rate to set
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_rate(javacall_handle handle, long rate) {
	//printf("javacall_media_set_rate\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get media's current playing rate
 * 
 * @param hLIB [in]     Handle to the library 
 * @param rate [out]    Current playing rate
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_get_rate(javacall_handle handle, /*OUT*/ long* rate) {
	//printf("javacall_media_get_rate\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/** 
 * Get media's current playing tempo.
 * 
 * @param hLIB [in]     Handle to the library
 * @param tempo [out]   Current playing tempo
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_get_tempo(javacall_handle handle, /*OUT*/ long* tempo) {
	//printf("javacall_media_get_tempo\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set media's current playing tempo
 * 
 * @param hLIB [in]     Handle to the library
 * @param tempo [in]    Tempo to set
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_set_tempo(javacall_handle handle, long tempo) {
	//printf("javacall_media_set_tempo\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/******************************************************************************/

/**
 * Gets the maximum playback pitch raise supported by the Player
 * 
 * @param handle    Handle to the library 
 * @param maxPitch  The maximum pitch raise in "milli-semitones".
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_max_pitch(javacall_handle handle, /*OUT*/ long* maxPitch) {
	//printf("javacall_media_get_max_pitch\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Gets the minimum playback pitch raise supported by the Player
 * 
 * @param handle    Handle to the library 
 * @param minPitch  The minimum pitch raise in "milli-semitones"
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_get_min_pitch(javacall_handle handle, /*OUT*/ long* minPitch) {
	//printf("javacall_media_get_min_pitch\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Set media's current playing rate
 * 
 * @param handle    Handle to the library 
 * @param pitch     The number of semi tones to raise the playback pitch. It is specified in "milli-semitones"
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_set_pitch(javacall_handle handle, long pitch) {
	//printf("javacall_media_set_pitch\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get media's current playing rate
 * 
 * @param handle    Handle to the library 
 * @param pitch     The current playback pitch raise in "milli-semitones"
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_get_pitch(javacall_handle handle, /*OUT*/ long* pitch) {
	//printf("javacall_media_get_pitch\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/******************************************************************************/

/**
 * Query if recording is supported based on the player's content-type
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_supports_recording(javacall_handle handle) {
	//printf("javacall_media_supports_recording\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Is this recording transaction is handled by native layer or Java layer?
 * 
 * @param handle    Handle to the library 
 * @param locator   URL locator string for recording data (ex: file:///root/test.wav)
 * 
 * @retval JAVACALL_OK      This recording transaction will be handled by native layer
 * @retval JAVACALL_FAIL    This recording transaction should be handled by Java layer
 */
javacall_result javacall_media_recording_handled_by_native(javacall_handle handle, 
                                                           const javacall_utf16* locator,
                                                           long locatorLength) {
	//printf("javacall_media_recording_handled_by_native\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Is javacall_media_set_recordsize_limit function is working for this player?
 * 
 * @retval JAVACALL_TRUE    Yes. Supported.
 * @retval JAVACALL_FALSE   No. Not supported.
 */
javacall_bool javacall_media_set_recordsize_limit_supported(javacall_handle handle) {
	//printf("javacall_media_set_recordsize_limit_supported\n");
	return JAVACALL_FALSE;
}

/**
 * Specify the maximum size of the recording including any headers.
 * If a size of -1 is passed then the record size limit should be removed.
 * 
 * @param handle    Handle to the library 
 * @param size      The maximum size bytes of the recording requested as input parameter.
 *                  The supported maximum size bytes of the recording which is less than or 
 *                  equal to the requested size as output parameter.
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_set_recordsize_limit(javacall_handle handle, 
                                                    /*INOUT*/ long* size) {
	//printf("javacall_media_set_recordsize_limit\n");
	return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Starts the recording. records all the data of the player ( video / audio )
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_start_recording(javacall_handle handle) {
	//printf("javacall_media_start_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Pause the recording. this should enable a future call to javacall_media_start_recording. 
 * Another call to javacall_media_start_recording after pause has been called will result 
 * in recording the new data and concatanating it to the previously recorded data.
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_pause_recording(javacall_handle handle) {
	//printf("javacall_media_pause_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Stop the recording.
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_stop_recording(javacall_handle handle) {
	//printf("javacall_media_stop_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * The recording that has been done so far should be discarded. (deleted)
 * Recording will be paused before this method is called. 
 * If javacall_media_start_recording is called after this method is called, recording should resume.
 * Calling reset after javacall_media_finish_recording will have no effect on the current recording.
 * If the Player that is associated with this RecordControl is closed, 
 * javacall_media_reset_recording will be called implicitly. 
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_reset_recording(javacall_handle handle) {
	//printf("javacall_media_reset_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * The recording should be completed; 
 * this may involve updating the header,flushing buffers and closing the temporary file if it is used
 * by the implementation.
 * javacall_media_pause_recording will be called before this method is called.
 * 
 * @param handle  Handle to the library 
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_commit_recording(javacall_handle handle) {
	//printf("javacall_media_commit_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get how much data was returned. 
 * This function can be called after a successful call to javacall_media_finish_recording.
 * 
 * @param handle    Handle to the library 
 * @param size      How much data was recorded
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_get_recorded_data_size(javacall_handle handle, 
                                                      /*OUT*/ long* size) {
	//printf("javacall_media_get_recorded_data_size\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Gets the recorded data.
 * This function can be called after a successful call to javacall_media_finish_recording.
 * It receives the data recorded from offset till the size.
 * 
 * @param handle    Handle to the library 
 * @param buffer    Buffer will contains the recorded data
 * @param offset    An offset to the start of the required recorded data
 * @param size      How much data will be copied to buffer
 * 
 * @retval JAVACALL_OK          Success
 * @retval JAVACALL_FAIL        Fail
 */
javacall_result javacall_media_get_recorded_data(javacall_handle handle, 
                                                 /*OUT*/ char* buffer, long offset, long size) {
	//printf("javacall_media_get_recorded_data\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Get the current recording data content type mime string length
 *
 * @return  If success return length of string else return 0
 */
int javacall_media_get_record_content_type_length(javacall_handle handle) {
	//printf("javacall_media_get_record_content_type_length\n");
    return 0;
}

/**
 * Get the current recording data content type mime string length
 * For example : 'audio/x-wav' for audio recording
 *
 * @param handle                Handle of native player
 * @param contentTypeBuf        Buffer to return content type unicode string
 * @param contentTypeBufLength  Lenght of contentTypeBuf buffer (in unicode metrics)
 *
 * @return  Length of content type string stored in contentTypeBuf
 */
int javacall_media_get_record_content_type(javacall_handle handle, 
                                           /*OUT*/ javacall_utf16* contentTypeBuf,
                                           int contentTypeBufLength) {
	//printf("javacall_media_get_record_content_type\n");
    return 0;
}

/**
 * Close the recording. Delete all resources related with this recording.
 * 
 * @param handle    Handle to the library 
 * 
 * @retval JAVACALL_OK      Success
 * @retval JAVACALL_FAIL    Fail
 */
javacall_result javacall_media_close_recording(javacall_handle handle) {
	//printf("javacall_media_close_recording\n");
    return JAVACALL_NOT_IMPLEMENTED;
}


/**
 * This function called by JVM when this player goes to foreground.
 * There is only one foreground midlets but, multiple player can be exits at this midlets.
 * So, there could be multiple players from JVM.
 * Device resource handling policy is not part of Java implementation. It is totally depends on
 * native layer's implementation.
 * 
 * @param handle    Handle to the native player
 * @param option    MVM options. Check about javacall_media_mvm_option type definition.
 * 
 * @retval JAVACALL_OK  Somthing happened
 * @retval JAVACALL_OK  Nothing happened
 */
javacall_result javacall_media_to_foreground(javacall_handle handle,
                                             javacall_media_mvm_option option) {
	//printf("javacall_media_to_foreground\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * This function called by JVM when this player goes to background.
 * There could be multiple background midlets. Also, multiple player can be exits at this midlets.
 * Device resource handling policy is not part of Java implementation. It is totally depends on
 * native layer's implementation.
 * 
 * @param handle    Handle to the native player
 * @param option    MVM options. Check about javacall_media_mvm_option type definition.
 * 
 * @retval JAVACALL_OK  Somthing happened
 * @retval JAVACALL_OK  Nothing happened
 */
javacall_result javacall_media_to_background(javacall_handle handle,
                                             javacall_media_mvm_option option) {
	//printf("javacall_media_to_background\n");
    return JAVACALL_NOT_IMPLEMENTED;
}


/**
 * This function is used to ascertain the availability of MIDI bank support
 *
 * @param handle     Handle to the native player
 * @param supported  return of support availability
 *
 * @retval JAVACALL_OK      MIDI Bank Query support is available
 * @retval JAVACALL_FAIL    NO MIDI Bank Query support is available
 */
javacall_result javacall_media_is_midibank_query_supported(javacall_handle handle,
                                             long* supported) {
	//printf("javacall_media_is_midibank_query_supported\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * This function is used to get a list of installed banks. If the custom
 * parameter is true, a list of custom banks is returned. Otherwise, a list of
 * all banks (custom and internal) is returned. This function can be left empty.
 *
 * @param handle    Handle to the native player
 * @param custom    a flag indicating whether to return just custom banks, or
 *                  all banks.
 * @param banklist  an array which will be filled out with the banklist
 * @param numlist   the length of the array to be filled out, and on return
 *                  contains the number of values written to the array.
 *
 * @retval JAVACALL_OK      Bank List is available
 * @retval JAVACALL_FAIL    Bank List is NOT available
 */
javacall_result javacall_media_get_midibank_list(javacall_handle handle,
                                             long custom,
                                             /*OUT*/short* banklist,
                                             /*INOUT*/long* numlist) {
	//printf("javacall_media_get_midibank_list\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Given a bank, program and key, get name of key. This function applies to
 * key-mapped banks (i.e. percussive banks or effect banks) only. If the returned
 * keyname length is 0, then the key is not mapped to a sound. For melodic banks,
 * where each key (=note) produces the same sound at different pitch, this
 * function always returns a 0 length string. For space saving reasons an
 * implementation may return a 0 length string instead of the keyname. This
 * can be left empty.
 *
 * @param handle    Handle to the native player
 * @param bank      The bank to query
 * @param program   The program to query
 * @param key       The key to query
 * @param keyname   The name of the key returned.
 * @param keynameLen    The length of the keyname array, and on return the
 *                      length of the keyname.
 *
 * @retval JAVACALL_OK      Keyname available
 * @retval JAVACALL_FAIL    Keyname not supported
 */
javacall_result javacall_media_get_midibank_key_name(javacall_handle handle,
                                            long bank,
                                            long program,
                                            long key,
                                            /*OUT*/char* keyname,
                                            /*INOUT*/long* keynameLen) {
	//printf("javacall_media_get_midibank_key_name\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Given the bank and program, get name of program. For space-saving reasons
 * a 0 length string may be returned.
 *
 * @param handle    Handle to the native player
 * @param bank      The bank being queried
 * @param program   The program being queried
 * @param progname  The name of the program returned
 * @param prognameLen    The length of the progname array, and on return the
 *                       length of the progname
 *
 * @retval JAVACALL_OK      Program name available
 * @retval JAVACALL_FAIL    Program name not supported
 */
javacall_result javacall_media_get_midibank_program_name(javacall_handle handle,
                                                long bank,
                                                long program,
                                                /*OUT*/char* progname,
                                                /*INOUT*/long* prognameLen) {
	//printf("javacall_media_get_midibank_program_name\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Given bank, get list of program numbers. If and only if this bank is not
 * installed, an empty array is returned.
 *
 * @param handle    Handle to the native player
 * @param bank      The bank being queried
 * @param proglist  The Program List being returned
 * @param proglistLen     The length of the proglist, and on return the number
 *                        of program numbers in the list
 *
 * @retval JAVACALL_OK     Program list available
 * @retval JAVACALL_FAIL   Program list unsupported
 */
javacall_result javacall_media_get_midibank_program_list(javacall_handle handle,
                                                long bank,
                                                /*OUT*/char* proglist,
                                                /*INOUT*/long* proglistLen) {
	//printf("javacall_media_get_midibank_program_list\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

/**
 * Returns the program assigned to the channel. It represents the current state
 * of the channel. During playbank of the MIDI file, the program may change due
 * to program change events in the MIDI file. The returned array is represented
 * by an array {bank, program}. The support of this function is optional.
 *
 * @param handle    Handle to the native player
 * @param channel   The channel being queried
 * @param prog      The return array (size 2) in the form {bank, program}
 *
 * @retval JAVACALL_OK    Program available
 * @retval JAVACALL_FAIL  Get Program unsupported
 */
javacall_result javacall_media_get_midibank_program(javacall_handle handle,
                                                long channel,
                                                /*OUT*/long* prog) {
	//printf("javacall_media_get_midibank_program\n");
    return JAVACALL_NOT_IMPLEMENTED;
}

#ifdef __cplusplus
}
#endif 
 
