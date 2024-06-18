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
#include <libhal-stm32f1/can.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-stm32f1/pin.hpp>
#include <libhal-stm32f1/uart.hpp>
#include <libhal-util/can.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/error.hpp>
#include <libhal/initializers.hpp>

void print_message(hal::serial& p_serial, const hal::can::message_t& p_message)
{
  hal::print<64>(p_serial, "{ ");
  hal::print<64>(p_serial, "id = %lu, ", p_message.id);
  hal::print<64>(p_serial, "length = %lu, ", p_message.length);
  hal::print<64>(p_serial,
                 "payload = { 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, "
                 "0x%02X, 0x%02X },",
                 p_message.payload[0],
                 p_message.payload[1],
                 p_message.payload[2],
                 p_message.payload[3],
                 p_message.payload[4],
                 p_message.payload[5],
                 p_message.payload[6],
                 p_message.payload[7]);
  hal::print<64>(p_serial, "}\n");
}

void application()
{
  using namespace hal::literals;
  using namespace std::chrono_literals;

  hal::cortex_m::dwt_counter steady_clock(
    hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu));

  hal::stm32f1::uart uart1(hal::port<1>, hal::buffer<128>);
  hal::stm32f1::can can({ .baud_rate = 100'000 },
                        hal::stm32f1::can_pins::pb9_pb8);

#if 0  // set to 1 to enable self test, 0 to disable
  can.enable_self_test(true);
#endif

  can.on_receive([&uart1](const hal::can::message_t& p_message) {
    hal::print(uart1, "Received: ");
    print_message(uart1, p_message);
  });

  while (true) {
    hal::can::message_t message{
      .id = 0x123,
      .payload = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 },
      .length = 8,
      .is_remote_request = false,
    };

    hal::print(uart1, "Sending Can message: ");
    print_message(uart1, message);

    try {
      can.send(message);
    } catch (const hal::operation_not_permitted& p_error) {
      // hal::operation_not_permitted indicates that the device is in
      // "bus-off" mode. Use `bus_on()` to turn the bus back on.
      can.bus_on();
    } catch (const hal::resource_unavailable_try_again& p_error) {
      hal::print(uart1, "CAN outgoing mailbox is full, trying again...\n");
    }

    hal::delay(steady_clock, 1000ms);
  }
}
