/*

 sftools - Copyright (c) 2012-2013 Marco Antognini <antognini.marco@gmail.com>

 This software is provided 'as-is', without any express or implied warranty. In
 no event will the authors be held liable for any damages arising from the use
 of this software.

 Permission is granted to anyone to use this software for any purpose, including
 commercial applications, and to alter it and redistribute it freely, subject to
 the following restrictions:

 1. The origin of this software must not be misrepresented; you must not claim
 that you wrote the original software. If you use this software in a product,
 an acknowledgment in the product documentation would be appreciated but is
 not required.

 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.

 3. This notice may not be removed or altered from any source distribution.

 */

/*!
 @file sftools/Chronometer.hpp
 @brief Defines Chronometer
 */

#ifndef __SFTOOLS_BASE_CHRONOMETER_HPP__
#define __SFTOOLS_BASE_CHRONOMETER_HPP__

#include <SFML/System/Clock.hpp>

// Slightly modified by G. Parolini

/*!
 @namespace sftools
 @brief Simple and Fast Tools
 */
namespace sftools
{
    /*!
     @class Chronometer
     @brief Provide functionalities of a chronometer, aka stop watch
     */
    class Chronometer
    {
    public:
        /*!
         @brief Constructor

         @param initialTime Initial time elapsed
         */
        Chronometer(sf::Time initialTime = sf::Time::Zero)
        {
            reset(true);
            add(initialTime);
        }

        /*!
         @brief Add some time

         @param time Time to be added to the time elapsed
         @return Time elapsed
         */
        sf::Time add(sf::Time time)
        {
            m_time += time;

            if (m_state == STOPPED) m_state = PAUSED;

            return getElapsedTime();
        }

        /*!
         @brief Reset the chronometer

         @param start if true the chronometer automatically starts
         @return Time elapsed on the chronometer before the reset
         */
        sf::Time reset(bool start = false)
        {
            sf::Time time = getElapsedTime();

            m_time = sf::Time::Zero;
            m_state = STOPPED;

            if (start) resume();

            return time;
        }

	sf::Time restart() {
		return reset(isRunning());
	}

        /*!
         @brief Pause the chronometer

         @return Time elapsed

         @see toggle
         */
        sf::Time pause()
        {
            if (isRunning())
            {
                m_state = PAUSED;
                m_time += m_clock.getElapsedTime();
            }
            return getElapsedTime();
        }

        /*!
         @brief Resume the chronometer

         @return Time elapsed

         @see toggle
         */
        sf::Time resume()
        {
            if (!isRunning())
            {
                m_state = RUNNING;
                m_clock.restart();
            }
            return getElapsedTime();
        }

        /*!
         @brief Pause or resume the chronometer

         If the chronometer is running the it is paused;
         otherwise it is resumes.

         @return Time elapsed

         @see pause
         @see resume
         */
        sf::Time toggle()
        {
            if (isRunning())    pause();
            else                resume();

            return getElapsedTime();
        }

        /*!
         @brief Tell the chronometer is running or not

         @brief chronometer's status
         */
        bool isRunning() const
        {
            return m_state == RUNNING;
        }

        /*!
         @brief Give the amount of time elapsed since the chronometer was started

         @return Time elapsed
         */
        sf::Time getElapsedTime() const
        {
            switch (m_state) {
                case STOPPED:
                    return sf::Time::Zero;

                case RUNNING:
                    return m_time + m_timeScale * m_clock.getElapsedTime();

                case PAUSED:
                    return m_time;
            }
	    return m_time;
        }

	void setTimeScale(float scale)
	{
	    m_timeScale = scale;
	}

	float getTimeScale() const
	{
		return m_timeScale;
	}

        /*!
         @brief Implicit conversion to sf::Time

         @return Time elapsed

         @see getElapsedTime
         */
        operator sf::Time() const
        {
            return getElapsedTime();
        }

    private:
        enum { STOPPED, RUNNING, PAUSED } m_state = STOPPED;  //!< state
        sf::Time m_time;                            //!< time counter
        sf::Clock m_clock;                          //!< clock
	float m_timeScale = 1.f;
    };
}


#endif // __SFTOOLS_BASE_CHRONOMETER_HPP__
