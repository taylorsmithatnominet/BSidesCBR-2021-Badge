#ifndef _BADGE_AUDIO_H
#define _BADGE_AUDIO_H

void audio_begin(void);
void audio_teardown(void);
extern std::shared_ptr<DiagApp> audio_post;

#endif//_BADGE_AUDIO_H
