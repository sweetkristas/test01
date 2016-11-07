#pragma once

#include <memory>

struct audio
{
	virtual ~audio() {}
	virtual void play_sound(int sound_id) = 0;
	virtual void stop_sound(int sound_id) = 0;
	virtual void stop_all_sounds() = 0;
};

struct null_audio : public audio
{
	void play_sound(int sound_id) override final {}
	void stop_sound(int sound_id)  override final {}
	void stop_all_sounds() override final {}
};

typedef std::unique_ptr<audio> audio_ptr;