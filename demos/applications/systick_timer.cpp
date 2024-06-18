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

#include <libhal-armcortex/systick_timer.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/interrupt.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-util/enum.hpp>
#include <libhal-util/steady_clock.hpp>

using namespace hal::literals;
using namespace std::literals;

volatile int poll_counter = 0;

void application()
{
  hal::stm32f1::output_pin led('C', 13);

  hal::stm32f1::initialize_interrupts();
  static hal::cortex_m::systick_timer timer(
    hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu));

  timer.schedule(
    [&led]() {
      // Invert the pin level with each call of this function.
      led.level(!led.level());
    },
    500ms);

  while (true) {
    // Use a debugger to inspect this value to confirm its updates. This
    // approach aids in affirming that the timer interrupt is indeed being
    // executed.
    poll_counter = poll_counter + 1;
  }
}
