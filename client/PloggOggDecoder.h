// Copyright (C) 2009 Chris Double. All Rights Reserved.
// The original author of this code can be contacted at: chris.double@double.co.nz
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// DEVELOPERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef __PLOG_OGG_DECODER_H__
#define __PLOG_OGG_DECODER_H__

#include<istream>
#include <ogg/ogg.h> // for ogg_int64_t

// Forward References
class OggStream;
class SDL_Surface;
class SDL_Overlay;
class sa_stream_t;
class ogg_packet;
class ogg_sync_state;

typedef map<int, OggStream*> StreamMap; 

class OggDecoder
{
public:
  StreamMap mStreams;  
  SDL_Surface* mSurface;
  SDL_Overlay* mOverlay;
  sa_stream_t* mAudio;
  ogg_int64_t  mGranulepos;

public:
	OggDecoder();
	~OggDecoder();
  	void play(std::istream& stream);

private:
  bool handle_theora_header(OggStream* stream, ogg_packet* packet);
  bool handle_vorbis_header(OggStream* stream, ogg_packet* packet);
  void read_headers(istream& stream, ogg_sync_state* state);

  bool read_page(std::istream& stream, ogg_sync_state* state, ogg_page* page);
  bool read_packet(std::istream& is, ogg_sync_state* state, OggStream* stream, ogg_packet* packet);
  void handle_theora_data(OggStream* stream, ogg_packet* packet);
};


#endif
