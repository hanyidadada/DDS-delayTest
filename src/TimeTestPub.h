// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file TimeTestPub.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifndef _FAST_DDS_GENERATED_TIMETESTPUB_H_
#define _FAST_DDS_GENERATED_TIMETESTPUB_H_

#include <array>
#include <bitset>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <fastcdr/cdr/fixed_size_string.hpp>
#include <fastcdr/xcdr/external.hpp>
#include <fastcdr/xcdr/optional.hpp>



#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(TIMETESTPUB_SOURCE)
#define TIMETESTPUB_DllAPI __declspec( dllexport )
#else
#define TIMETESTPUB_DllAPI __declspec( dllimport )
#endif // TIMETESTPUB_SOURCE
#else
#define TIMETESTPUB_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define TIMETESTPUB_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
class CdrSizeCalculator;
} // namespace fastcdr
} // namespace eprosima





/*!
 * @brief This class represents the structure TimeTestPub defined by the user in the IDL file.
 * @ingroup TimeTestPub
 */
class TimeTestPub
{
public:

    /*!
     * @brief Default constructor.
     */
    eProsima_user_DllExport TimeTestPub();

    /*!
     * @brief Default destructor.
     */
    eProsima_user_DllExport ~TimeTestPub();

    /*!
     * @brief Copy constructor.
     * @param x Reference to the object TimeTestPub that will be copied.
     */
    eProsima_user_DllExport TimeTestPub(
            const TimeTestPub& x);

    /*!
     * @brief Move constructor.
     * @param x Reference to the object TimeTestPub that will be copied.
     */
    eProsima_user_DllExport TimeTestPub(
            TimeTestPub&& x) noexcept;

    /*!
     * @brief Copy assignment.
     * @param x Reference to the object TimeTestPub that will be copied.
     */
    eProsima_user_DllExport TimeTestPub& operator =(
            const TimeTestPub& x);

    /*!
     * @brief Move assignment.
     * @param x Reference to the object TimeTestPub that will be copied.
     */
    eProsima_user_DllExport TimeTestPub& operator =(
            TimeTestPub&& x) noexcept;

    /*!
     * @brief Comparison operator.
     * @param x TimeTestPub object to compare.
     */
    eProsima_user_DllExport bool operator ==(
            const TimeTestPub& x) const;

    /*!
     * @brief Comparison operator.
     * @param x TimeTestPub object to compare.
     */
    eProsima_user_DllExport bool operator !=(
            const TimeTestPub& x) const;

    /*!
     * @brief This function sets a value in member timestamp
     * @param _timestamp New value for member timestamp
     */
    eProsima_user_DllExport void timestamp(
            uint64_t _timestamp);

    /*!
     * @brief This function returns the value of member timestamp
     * @return Value of member timestamp
     */
    eProsima_user_DllExport uint64_t timestamp() const;

    /*!
     * @brief This function returns a reference to member timestamp
     * @return Reference to member timestamp
     */
    eProsima_user_DllExport uint64_t& timestamp();


    /*!
     * @brief This function copies the value in member message
     * @param _message New value to be copied in member message
     */
    eProsima_user_DllExport void message(
            const std::string& _message);

    /*!
     * @brief This function moves the value in member message
     * @param _message New value to be moved in member message
     */
    eProsima_user_DllExport void message(
            std::string&& _message);

    /*!
     * @brief This function returns a constant reference to member message
     * @return Constant reference to member message
     */
    eProsima_user_DllExport const std::string& message() const;

    /*!
     * @brief This function returns a reference to member message
     * @return Reference to member message
     */
    eProsima_user_DllExport std::string& message();

private:

    uint64_t m_timestamp{0};
    std::string m_message;

};

#endif // _FAST_DDS_GENERATED_TIMETESTPUB_H_



