#include "OnTimer.hpp"

std::unique_ptr<OnTimer> OnTimer::m_instance;
std::mutex OnTimer::m_mtx; 