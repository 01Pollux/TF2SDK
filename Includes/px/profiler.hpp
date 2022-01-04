#pragma once

#include <px/profiler/defines.hpp>
#include <px/profiler/context.hpp>

PX_NAMESPACE_BEGIN(::profiler);

// Macro used for profiling a section, optionally sets color
#define PX_PROFILE_SECTION(SECTION, ...)            px::profiler::entry profile_##__LINE__{ false, SECTION, __VA_ARGS__ }
#define PX_PROFILE_SECTION_BACKTRACE(SECTION, ...)  px::profiler::entry profile_##__LINE__{ true, SECTION, __VA_ARGS__ }

// Macro used setting/reseting default color
#define PX_PROFILER_PUSH_COLOR(COLOR, IDX)                                  \
    do {                                                                    \
    auto profiler_backup_clr##IDX = px::profiler::manager::Get()->Color;    \
    px::profiler::manager::Get()->Color = COLOR

#define PX_PROFILER_POP_COLOR(IDX)                                          \
    px::profiler::manager::Get()->Color = profiler_backup_clr##IDX;         \
} while (false)

class entry
{
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;

    entry(bool backtrace, const std::string_view& section_name, const std::string_view& entry_name, const types::color_type& color = { }) :
        m_SectionIter{ manager::Get()->IsEnabled() ? manager::Get()->BeginSection(backtrace, section_name, entry_name, color) : types::sae_iterator{ manager::Get()->InvalidSection(), { } } }
    { }

    ~entry()
    {
        if (is_active())
            manager::Get()->EndSection(m_SectionIter);
    }

    bool is_active() const noexcept
    {
        return m_SectionIter.first != manager::Get()->InvalidSection();
    }

private:
    types::sae_iterator m_SectionIter{ };
};

PX_NAMESPACE_END();
