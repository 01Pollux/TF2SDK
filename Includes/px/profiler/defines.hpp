#pragma once

#include <chrono>
#include <list>
#include <map>
#include <memory>
#include <px/defines.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <boost/stacktrace.hpp>

PX_NAMESPACE_BEGIN(::profiler::types);

struct color_type
{
    using type = char8_t;
    type rgba[4]{  };

    constexpr color_type(type r, type g, type b, type a) noexcept :
        rgba{ r, g, b, a }
    { }

    constexpr color_type(const type rgba[4]) noexcept :
        rgba{ rgba[0], rgba[1], rgba[2], rgba[3] }
    { }

    constexpr color_type() = default;

    constexpr operator uint32_t() const noexcept
    {
        return {
            static_cast<uint32_t>(rgba[0]) << 0x00 | // r
            static_cast<uint32_t>(rgba[1]) << 0x08 | // g
            static_cast<uint32_t>(rgba[2]) << 0x10 | // b
            static_cast<uint32_t>(rgba[3]) << 0x18   // a
        };
    }
};

using clock_type = std::chrono::steady_clock;
using time_point = clock_type::time_point;
using clock_duration = clock_type::duration;

using section_id = uint32_t;
static constexpr section_id invalid_section_id = std::numeric_limits<section_id>::max();

using stacktrace = boost::stacktrace::stacktrace;

struct entry_info
{
    time_point begin_time, end_time;
    std::unique_ptr<stacktrace> stack_info;

    std::string name;
    size_t stackoffset;

    color_type color;

    bool is_valid() const noexcept
    {
        return end_time.time_since_epoch() != clock_duration::zero();
    }

    bool has_backtrace() const noexcept
    {
        return stack_info != nullptr;
    }

    entry_info(const time_point& begin_time, std::unique_ptr<stacktrace> stack_info, const std::string_view& name, size_t stackoffset, const color_type& color) noexcept :
        begin_time{ begin_time }, end_time{ },
        stack_info{ std::move(stack_info) },
        name{ name },
        stackoffset{ stackoffset },
        color{ color }
    { }

    entry_info(const entry_info& o) :
        begin_time{ o.begin_time }, end_time{ o.end_time },
        stack_info{ o.stack_info ? std::make_unique<stacktrace>(*o.stack_info) : nullptr },
        name{ o.name },
        stackoffset{ o.stackoffset },
        color{ o.color }
    { }

    entry_info& operator=(const entry_info&) = delete;
    entry_info& operator=(entry_info&&) = default;
    entry_info(entry_info&&) = default;
    ~entry_info() = default;
};

using entry_container = std::list<entry_info>;
using section_container = std::map<std::string, entry_container>;

using sae_iterator = std::pair<section_container::iterator, entry_container::iterator>;

PX_NAMESPACE_END();
