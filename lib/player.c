#include <unistd.h>

#include <vlc/vlc.h>

#include "common.h"

static ssize_t media_read_cb(void *opaque, unsigned char *buf, size_t len) {
  MemAudioData *mVid = (MemAudioData *)opaque; // cast and give context

  size_t copyLen =
      (mVid->size - mVid->pos < len) ? mVid->size - mVid->pos : len;
  char *start = mVid->audio + mVid->pos;
  memmove(buf, start, copyLen); // copy size requested to buffer.
  mVid->pos += copyLen;

  return copyLen;
}

static int media_open_cb(void *opaque, void **datap, uint64_t *sizep) {
  // cast opaque to our audio state struct
  MemAudioData *mVid = (MemAudioData *)opaque;
  *sizep = mVid->size; // set stream length
  *datap = mVid;

  return 0;
}

static int media_seek_cb(void *opaque, uint64_t offset) {
  MemAudioData *mVid = (MemAudioData *)opaque;
  mVid->pos = offset;
  return 0;
}

void play_audio(MemAudioData mem, float speed) {

  libvlc_instance_t *vlc;

  // add Verbose option to instance
  const char *options[] = {"--quiet"};

  vlc = libvlc_new(1, options);
  // vlc = libvlc_new(0, NULL);
  libvlc_media_t *media = libvlc_media_new_callbacks(
      vlc, media_open_cb, media_read_cb, media_seek_cb, NULL, (void *)&mem);

  // Create a media player playing environment
  libvlc_media_player_t *mediaPlayer =
      libvlc_media_player_new_from_media(media);

  // Set the playback rate to 2x
  libvlc_media_player_set_rate(mediaPlayer, speed);

  // play the media_player
  libvlc_media_player_play(mediaPlayer);

  sleep(1);
  // Main event loop
  while (libvlc_media_player_is_playing(mediaPlayer)) {
  }

  libvlc_media_player_stop(mediaPlayer);
  libvlc_media_player_release(mediaPlayer);

  // Free vlc
  libvlc_release(vlc);
}
