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
#include <libhal-armcortex/system_control.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/uart.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/initializers.hpp>

void application()
{
  auto cpu_frequency = hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu);
  hal::cortex_m::dwt_counter steady_clock(cpu_frequency);
  hal::stm32f1::uart uart1(hal::port<1>, hal::buffer<128>);
  hal::print(uart1, "[stm32f1] Starting UART demo...\n");

  int counter = 0;

  while (true) {
    using namespace std::chrono_literals;
    std::array<hal::byte, 64> read_buffer{};
    hal::print<32>(uart1, "Hello, World %d\n", counter++);
    hal::print(uart1, uart1.read(read_buffer).data);
    hal::delay(steady_clock, 500ms);

    if (counter > 10) {
      hal::print(uart1, "Resetting board...\n\n\n\n");
      hal::cortex_m::reset();
    }
  }
}
