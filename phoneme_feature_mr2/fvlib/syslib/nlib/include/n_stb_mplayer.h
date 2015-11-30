#ifndef __N_STB_MPLAYER_H__
#define __N_STB_MPLAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define cmd_mp_Forward                  "mp.Forward"
#define cmd_mp_Pause                    "mp.Pause"
#define cmd_mp_Resume                   "mp.Resume"
#define cmd_mp_Play                     "mp.Play"
#define cmd_mp_Seek                     "mp.Seek"
#define cmd_mp_Rewind                   "mp.Rewind"
#define cmd_mp_Stop                     "mp.Stop"
#define cmd_mp_SetVolume                "mp.SetVolume"
#define cmd_mp_GetVolume                "mp.GetVolume"
#define cmd_mp_SetMute                  "mp.SetMute"
#define cmd_mp_GetMute                  "mp.GetMute"
#define cmd_mp_VideoLocation            "mp.VideoLocation"
#define cmd_mp_JoinChannel              "mp.JoinChannel"
#define cmd_mp_LeaveChannel             "mp.LeaveChannel"
#define cmd_mp_GotoEnd                  "mp.GotoEnd"
#define cmd_mp_GotoStart                "mp.GotoStart"
#define cmd_mp_SwitchMode               "mp.SwitchMode"
#define cmd_mp_GetAudioPids             "mp.GetAudioPids"
#define cmd_mp_GetSubtitlePids          "mp.GetSubtitlePids"
#define cmd_mp_MediaDuration            "mp.MediaDuration"
#define cmd_mp_SetVideoAlpha            "mp.SetVideoAlpha"
#define cmd_mp_GetVideoAlpha            "mp.GetVideoAlpha"
#define cmd_mp_GetCurrentPlayTime       "mp.GetCurrentPlayTime"
#define cmd_mp_SoundInit                "mp.SoundInit"
#define cmd_mp_SoundExit                "mp.SoundExit"
#define cmd_mp_SoundOpen                "mp.SoundOpen"
#define cmd_mp_SoundClose               "mp.SoundClose"
#define cmd_mp_reg_stream_stat_listener "mp.RegisterStreamStatusListener"
#define cmd_mp_stream_stat              "mp.StreamStat"
#define cmd_mp_enable_retry             "mp.EnableRetry"
#define cmd_mp_switchAudioChannel       "mp.switchAudioChannel"
#define cmd_mp_switchAudioTrack         "mp.switchAudioTrack"
#define cmd_mp_switchSubtitle           "mp.switchSubtitle"
#define cmd_mp_getAudioChannel          "mp.getCurrentAudioChannel"
#define cmd_mp_setAudioChannel          "mp.setCurrentAudioChannel"
#define cmd_mp_getAudioTrack            "mp.getAudioTrack"
#define cmd_mp_getSubtitle              "mp.getSubtitle"
#define cmd_mp_setContType              "mp.setContType"

int n_mplayer_forward(int id, int speed);
int n_mplayer_pause(int id);
int n_mplayer_resume(int id);
int n_mplayer_play(int id,
	const char *cMediaUrl,
	int iMediaType,
	int iAudioType,
	int iVideoType,
	int iStreamType,
	int iDrmType,
	int iFingerPrint,
	int iCopyProtection,
	int iAllowTrickmode,
	char *cMediaCode,
	char *cEntryID,
	char *cStartTime,
	char *cEndTime,
	char *ctm);
int n_mplayer_seek(int id, int mode, char *pos);
int n_mplayer_stop(int id);
int n_mplayer_set_volume(int id, int vol);
int n_mplayer_get_volume(int id, int *ret);
int n_mplayer_set_mute(int id, int flag);
int n_mplayer_get_mute(int id, int *ret);
int n_mplayer_video_location(int id, int x, int y, int w, int h, int mode);
int n_mplayer_join_channel(int id, int chan);
int n_mplayer_leave_channel(int id);
int n_mplayer_goto_end(int id);
int n_mplayer_goto_start(int id);
int n_mplayer_switch_mode(int id, int mode);
int n_mplayer_get_audio_pids(int id, int num, int* ret);
int n_mplayer_get_subtitle_pids(int id, int num, int* ret);
int n_mplayer_media_duration(int id, int *ret);
int n_mplayer_set_video_alpha(int id, int alpha);
int n_mplayer_get_video_alpha(int id, int *ret);
int n_mplayer_get_current_play_time(int id, char *ret, unsigned int size);
int n_mplayer_sound_init();
int n_mplayer_sound_exit();
int n_mplayer_sound_open(int format,
          int sample_rate,
          int bit_per_sample,
          int channels,
          int option_flag,
          int *ret);
int n_mplayer_sound_close(int handle, int *ret);
int n_mplayer_reg_stream_stat_listener(int id);
int n_mplayer_get_stream_stat();
int n_mplayer_enable_mplayer_retry(int flag);
int n_mplayer_switch_audio_channel(int id);
int n_mplayer_switch_audio_track(int id);
int n_mplayer_switch_subtitle(int id);
int n_mplayer_get_audio_channel(int id, char *ret);
int n_mplayer_set_audio_channel(int id, char *chan);
int n_mplayer_get_audio_track(int id, char *ret);
int n_mplayer_get_subtitle(int id, char *ret);
int n_mplayer_set_cont_type(int type);

typedef enum {
	F_MP_CMD = 0,	///< changed because of command
	F_MP_INTERNAL,	///< changed because of network event or error
	F_MP_SERVER	///< changed because of network event or error
} f_mplayer_reason_e;

typedef enum {
	F_MP_STATE_IDLE = 0,
	F_MP_STATE_PLAY,
	F_MP_STATE_PAUSE,
	F_MP_STATE_TRICK,
	F_MP_STATE_JOIN,
	F_MP_STATE_TIMESHIFT_PLAY,
	F_MP_STATE_TIMESHIFT_TRICK,
	F_MP_STATE_TIMESHIFT_PAUSE,
	F_MP_STATE_STOP
} f_mplayer_status_e;

typedef enum {
	F_MP_RTSP_CONNECT_FAILED = 1,
	F_MP_RTSP_SEND_FAILED,
	F_MP_RTSP_READ_FAILED,
	F_MP_RTSP_CONNECTION_DIED,
	F_MP_RTSP_STATUS_LINE,
	F_MP_RTSP_STATUS_PLAYING,
	F_MP_RTSP_STATUS_ANNOUNCE,
	F_MP_RTSP_STATUS_END_OF_STREAM,
	F_MP_RTSP_STATUS_NOT_FOUND,
	F_MP_RTSP_STATUS_NO_VIDEO,
	F_MP_RTSP_CONNECT_STOPPED,
	F_MP_RTSP_STATUS_AUTHENTICATE,
	F_MP_RTSP_STATUS_REDIRECTED,
	F_MP_RTSP_STATUS_START_OF_STREAM,
	F_MP_RTSP_STATUS_ANNOUNCE_PARTIAL,
	F_MP_RTSP_STATUS_SET_PARAMETER,
	F_MP_RTSP_STATUS_INVALID_RANGE,

	F_MP_IGMP_STATUS_PLAYING,
	F_MP_IGMP_STATUS_END_OF_STREAM,

	F_MP_UDP_STATUS_END_OF_STREAM,
	F_MP_UDP_STATUS_PLAYING,
	F_MP_MP3S_STATUS_END_OF_STREAM,


	F_MP_MP3S_STATUS_START_OF_STREAM,

	F_MP_AUD_DESCRIP_READY,

	F_MP_STATUS_PMT_CHANGED,
	F_MP_PVRS_STATUS_PLAYING,
	F_MP_PVRS_STATUS_END_OF_STREAM
} f_mplayer_event_e;

#ifdef __cplusplus
}
#endif

#endif

