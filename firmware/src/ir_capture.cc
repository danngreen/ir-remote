#include "ir_capture.hh"

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *) {
	RemoteVolume::IRCapture::_instance->process();
}
