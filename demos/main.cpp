// Copyright 2024 Khalil Estell
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-exceptions/control.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/error.hpp>

// Application function must be implemented by one of the compilation units
// (.cpp) files.
extern void application();

[[noreturn]] void terminate_handler() noexcept
{
  hal::cortex_m::dwt_counter steady_clock(
    hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu));

  hal::stm32f1::output_pin led('C', 13);

  while (true) {
    using namespace std::chrono_literals;
    led.level(false);
    hal::delay(steady_clock, 100ms);
    led.level(true);
    hal::delay(steady_clock, 100ms);
    led.level(false);
    hal::delay(steady_clock, 100ms);
    led.level(true);
    hal::delay(steady_clock, 1000ms);
  }
}

int main()
{
  hal::set_terminate(terminate_handler);
  application();
  return 0;
}
