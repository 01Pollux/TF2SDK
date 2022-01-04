#pragma once

#include <chrono>

using namespace std::chrono_literals;
namespace px
{
	class stopwatch
	{
	public:
		using clock_type = std::chrono::steady_clock;

		stopwatch(bool auto_start = true) noexcept
		{
			if (auto_start)
				start();
		}

		void start() noexcept
		{
			reset();
		}

		void reset() noexcept
		{
			m_StartTick = clock_type::now();
		}

		auto elapsed_nano() const noexcept
		{
			using namespace std::chrono;
			const auto delta = elapsed();
			return duration_cast<nanoseconds>(delta);
		}

		auto elapsed_micro() const noexcept
		{
			using namespace std::chrono;
			const auto delta = elapsed();
			return duration_cast<microseconds>(delta);
		}

		auto elapsed_milli() const noexcept
		{
			using namespace std::chrono;
			const auto delta = elapsed();
			return duration_cast<milliseconds>(delta);
		}

		auto elapsed_sec() const noexcept
		{
			using namespace std::chrono;
			const auto delta = elapsed();
			return duration_cast<seconds>(delta);
		}

		clock_type::duration elapsed() const noexcept
		{
			const auto now = clock_type::now();
			return now - m_StartTick;
		}

	private:
		clock_type::time_point	m_StartTick;
	};

	class Collectiblestopwatch
	{
	private:
		friend class stopwatchWrapper;
		class stopwatchWrapper
		{
		public:
			stopwatchWrapper(Collectiblestopwatch* owner) noexcept : m_Handler(owner) { }

			~stopwatchWrapper()
			{
				const auto elapsed = m_Timer.elapsed();
				m_Handler->m_Durations.emplace_back(elapsed);
			}

			stopwatchWrapper(const stopwatchWrapper&) = delete; stopwatchWrapper& operator=(const stopwatchWrapper&) = delete;
			stopwatchWrapper(stopwatchWrapper&&) = delete; stopwatchWrapper& operator=(stopwatchWrapper&&) = delete;

		private:
			stopwatch m_Timer;
			Collectiblestopwatch* m_Handler{ };
		};
		
	public:
		using clock_type = std::chrono::steady_clock;

		Collectiblestopwatch(size_t capacity = 0) { m_Durations.reserve(capacity); }

		stopwatchWrapper start() noexcept
		{
			return stopwatchWrapper{ this };
		}

		clock_type::duration avg() const noexcept
		{
			clock_type::duration avg{ };

			for (auto& x : m_Durations)
				avg += x;

			return avg / m_Durations.size();
		}

		clock_type::duration min() const noexcept
		{
			clock_type::duration min = clock_type::duration::max();

			for (auto& x : m_Durations)
				if (min > x)
					min = x;

			return min;
		}

		clock_type::duration max() const noexcept
		{
			clock_type::duration max = clock_type::duration::min();

			for (auto& x : m_Durations)
				if (max < x)
					max = x;

			return max;
		}

		void reset() noexcept { m_Durations.clear(); }

		auto& get() noexcept { return m_Durations; }
		auto& get() const noexcept { return m_Durations; }

	private:
		std::vector<clock_type::duration> m_Durations;
	};
}