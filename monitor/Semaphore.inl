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


template<class Rep, class Period>
bool Semaphore::wait_for(const std::chrono::duration<Rep, Period>& d)
{
    std::unique_lock<std::mutex> lock{mMutex};
    auto finished = mCv.wait_for(lock, d, [&]{ return mCount > 0; });

    if (finished)
        --mCount;

    return finished;
}


template<class Clock, class Duration>
bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& t)
{
    std::unique_lock<std::mutex> lock{mMutex};
    auto finished = mCv.wait_until(lock, t, [&]{ return mCount > 0; });

    if (finished)
        --mCount;

    return finished;
}

