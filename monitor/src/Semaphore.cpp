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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Semaphore.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


Semaphore::Semaphore(size_t count)
: mCount{count}
{

}

void Semaphore::signal()
{
    std::lock_guard<std::mutex> lock{mMutex};
    ++mCount;
    mCv.notify_one();
}


void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock{mMutex};
    mCv.wait(lock, [&]{ return mCount > 0; });
    --mCount;
}


bool Semaphore::try_wait()
{
    std::lock_guard<std::mutex> lock{mMutex};

    if (mCount > 0) {
        --mCount;
        return true;
    }

    return false;
}


