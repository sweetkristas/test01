#pragma once 
#include <cassert>
#include "audio_fwd.hpp"

class locator
{
public:
	static void initialize() {
		audio_service_ = std::make_unique<null_audio>();
	}
	static void release() {
		audio_service_.reset();
	}
	static void provide_audio(audio_ptr&& service) {
		if(service == nullptr) {
			audio_service_ = std::make_unique<null_audio>();
		} else {
			audio_service_ = std::move(service);
		}
	}
	static audio* get_audio() { 
		assert(audio_service_ != nullptr);
		return audio_service_.get(); 
	}

	class manager
	{
	public:
		manager() {
			locator::initialize();
		}
		~manager() {
			locator::release();
		}
	};

private:
	static audio_ptr audio_service_;
};
