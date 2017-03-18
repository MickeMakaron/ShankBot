// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
#ifndef GRAPHICS_LAYER_SEMAPHORE_HPP
#define GRAPHICS_LAYER_SEMAPHORE_HPP


///////////////////////////////////
// STD C++
#include <cstring>
#include <chrono>
#include <condition_variable>
#include <mutex>
///////////////////////////////////


namespace GraphicsLayer
{
// Modified version of
// http://stackoverflow.com/a/27852868
class Semaphore
{
    public:
        Semaphore(size_t count = 0);
        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

        void signal();
        void wait();
        bool try_wait();

        template<class Rep, class Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& d);

        template<class Clock, class Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

    private:
        std::mutex   mMutex;
        std::condition_variable mCv;
        size_t  mCount;
};

#include "monitor/Semaphore.inl"
}



#endif // GRAPHICS_LAYER_SEMAPHORE_HPP
